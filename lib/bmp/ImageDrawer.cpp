#include "ImageDrawer.h"

ImageDrawer::ImageDrawer(GxEPD2_GFX &display) : display(display)
{
}

void ImageDrawer::drawBitmapFromSD_Buffered(
    File& file, int16_t x, int16_t y, bool with_color, bool partial_update)
{
    bool valid = false;  // valid format to be handled
    bool flip  = true;   // bitmap is stored bottom-to-top
    bool has_multicolors = (display.epd2.panel == GxEPD2::ACeP565) || (display.epd2.panel == GxEPD2::GDEY073D46);
    uint32_t startTime = millis();
    if ((x >= display.width()) || (y >= display.height()))
    {
        return;
    }
    Serial.println();
    Serial.print("Loading image '");
    Serial.print(file.name());
    Serial.println('\'');
    if (!file)
    {
        Serial.print("File not found");
        return;
    }
    // Parse BMP header
    if (read16(file) == 0x4D42) // BMP signature
    {
        uint32_t fileSize     = read32(file);
        uint32_t creatorBytes = read32(file);
        (void)creatorBytes;                   // unused
        uint32_t imageOffset  = read32(file); // start of image data
        uint32_t headerSize   = read32(file);
        uint32_t width        = read32(file);
        int32_t height        = (int32_t)read32(file);
        uint16_t planes       = read16(file);
        uint16_t depth        = read16(file); // bits per pixel
        uint32_t format       = read32(file);
        
        if ((planes == 1) && ((format == 0) || (format == 3))) // uncompressed is handled, 565 also
        {
            Serial.print("File size: ");
            Serial.println(fileSize);
            Serial.print("Image Offset: ");
            Serial.println(imageOffset);
            Serial.print("Header size: ");
            Serial.println(headerSize);
            Serial.print("Bit Depth: ");
            Serial.println(depth);
            Serial.print("Image size: ");
            Serial.print(width);
            Serial.print('x');
            Serial.println(height);
            
            // BMP rows are padded (if needed) to 4-byte boundary
            uint32_t rowSize = (width * depth / 8 + 3) & ~3;
            if (depth < 8)
                rowSize = ((width * depth + 8 - depth) / 8 + 3) & ~3;
            if (height < 0)
            {
                height = -height;
                flip = false;
            }
            uint16_t w = width;
            uint16_t h = height;
            if ((x + w - 1) >= display.width())
                w = display.width() - x;
            if ((y + h - 1) >= display.height())
                h = display.height() - y;
            // if (w <= max_row_width) // handle with direct drawing
            {
                valid = true;
                uint8_t bitmask = 0xFF;
                uint8_t bitshift = 8 - depth;
                uint16_t red, green, blue;
                bool whitish = false;
                bool colored = false;
                if (depth == 1) {
                    with_color = false;
                }
                if (depth <= 8)
                {
                    if (depth < 8) {
                        bitmask >>= depth;
                    }
                    // file.seek(54); //palette is always @ 54
                    file.seek(imageOffset - (4 << depth)); // 54 for regular, diff for colorsimportant
                    for (uint16_t pn = 0; pn < (1 << depth); pn++)
                    {
                        blue  = file.read();
                        green = file.read();
                        red   = file.read();
                        file.read();

                        whitish = with_color ? ((red > 0x80) && (green > 0x80) && (blue > 0x80)) : ((red + green + blue) > 3 * 0x80); // whitish
                        colored = (red > 0xF0) || ((green > 0xF0) && (blue > 0xF0));                                                  // reddish or yellowish?
                        if (0 == pn % 8)
                            mono_palette_buffer[pn / 8] = 0;
                        mono_palette_buffer[pn / 8] |= whitish << pn % 8;
                        if (0 == pn % 8)
                            color_palette_buffer[pn / 8] = 0;
                        color_palette_buffer[pn / 8] |= colored << pn % 8;
                        rgb_palette_buffer[pn] = ((red & 0xF8) << 8) | ((green & 0xFC) << 3) | ((blue & 0xF8) >> 3);
                    }
                }
                // Center image in case position is not selected
                if (x == 0 && y == 0) {
                    x = (display.width() - width) / 2;
                    y = (display.height() - height) / 2;
                }

                Serial.printf("Image start coordinates x: %i, y: %i\n", x, y);

                if (partial_update) {
                    display.setPartialWindow(x, y, w, h);
                }
                else
                {
                    display.setFullWindow();
                }
                display.firstPage();
                do
                {
                    uint32_t rowPosition = flip ? imageOffset + (height - h) * rowSize : imageOffset;
                    for (uint16_t row = 0; row < h; row++, rowPosition += rowSize) // for each line
                    {
                        uint32_t in_remain = rowSize;
                        uint32_t in_idx   = 0;
                        uint32_t in_bytes = 0;
                        uint8_t in_byte   = 0; // for depth <= 8
                        uint8_t in_bits   = 0; // for depth <= 8
                        uint16_t color    = GxEPD_WHITE;

                        file.seek(rowPosition);
                        for (uint16_t col = 0; col < w; col++) // for each pixel
                        {
                            // Time to read more pixel data?
                            if (in_idx >= in_bytes) // ok, exact match for 24bit also (size IS multiple of 3)
                            {
                                in_bytes = file.read(input_buffer, in_remain > sizeof(input_buffer) ? sizeof(input_buffer) : in_remain);
                                in_remain -= in_bytes;
                                in_idx = 0;
                            }
                            switch (depth)
                            {
                            case 32:
                                blue = input_buffer[in_idx++];
                                green = input_buffer[in_idx++];
                                red = input_buffer[in_idx++];
                                in_idx++;                                                                                                     // skip alpha
                                whitish = with_color ? ((red > 0x80) && (green > 0x80) && (blue > 0x80)) : ((red + green + blue) > 3 * 0x80); // whitish
                                colored = (red > 0xF0) || ((green > 0xF0) && (blue > 0xF0));                                                  // reddish or yellowish?
                                color = ((red & 0xF8) << 8) | ((green & 0xFC) << 3) | ((blue & 0xF8) >> 3);
                                break;
                            case 24:
                                blue = input_buffer[in_idx++];
                                green = input_buffer[in_idx++];
                                red = input_buffer[in_idx++];
                                whitish = with_color ? ((red > 0x80) && (green > 0x80) && (blue > 0x80)) : ((red + green + blue) > 3 * 0x80); // whitish
                                colored = (red > 0xF0) || ((green > 0xF0) && (blue > 0xF0));                                                  // reddish or yellowish?
                                color = ((red & 0xF8) << 8) | ((green & 0xFC) << 3) | ((blue & 0xF8) >> 3);
                                break;
                            case 16:
                            {
                                uint8_t lsb = input_buffer[in_idx++];
                                uint8_t msb = input_buffer[in_idx++];
                                if (format == 0) // 555
                                {
                                    blue = (lsb & 0x1F) << 3;
                                    green = ((msb & 0x03) << 6) | ((lsb & 0xE0) >> 2);
                                    red = (msb & 0x7C) << 1;
                                    color = ((red & 0xF8) << 8) | ((green & 0xFC) << 3) | ((blue & 0xF8) >> 3);
                                }
                                else // 565
                                {
                                    blue = (lsb & 0x1F) << 3;
                                    green = ((msb & 0x07) << 5) | ((lsb & 0xE0) >> 3);
                                    red = (msb & 0xF8);
                                    color = (msb << 8) | lsb;
                                }
                                whitish = with_color ? ((red > 0x80) && (green > 0x80) && (blue > 0x80)) : ((red + green + blue) > 3 * 0x80); // whitish
                                colored = (red > 0xF0) || ((green > 0xF0) && (blue > 0xF0));                                                  // reddish or yellowish?
                            }
                            break;
                            case 1:
                            case 2:
                            case 4:
                            case 8:
                            {
                                if (0 == in_bits)
                                {
                                    in_byte = input_buffer[in_idx++];
                                    in_bits = 8;
                                }
                                uint16_t pn = (in_byte >> bitshift) & bitmask;
                                whitish = mono_palette_buffer[pn / 8] & (0x1 << pn % 8);
                                colored = color_palette_buffer[pn / 8] & (0x1 << pn % 8);
                                in_byte <<= depth;
                                in_bits -= depth;
                                color = rgb_palette_buffer[pn];
                            }
                            break;
                            }
                            if (with_color && has_multicolors)
                            {
                                // keep color
                            }
                            else if (whitish)
                            {
                                color = GxEPD_WHITE;
                            }
                            else if (colored && with_color)
                            {
                                color = GxEPD_COLORED;
                            }
                            else
                            {
                                color = GxEPD_BLACK;
                            }
                            uint16_t yrow = y + (flip ? h - row - 1 : row);
                            display.drawPixel(x + col, yrow, color);
                        } // end pixel
                    } // end line
                    Serial.print("page loaded in ");
                    Serial.print(millis() - startTime);
                    Serial.println(" ms");
                } while (display.nextPage());

                Serial.print("loaded in ");
                Serial.print(millis() - startTime);
                Serial.println(" ms");
            }
        }
    }
    file.close();
    if (!valid)
    {
        Serial.println("Bitmap format not handled.");
    }
}

uint16_t ImageDrawer::read16(File &f)
{
    // BMP data is stored little-endian, same as Arduino.
    uint16_t result;
    ((uint8_t *)&result)[0] = f.read(); // LSB
    ((uint8_t *)&result)[1] = f.read(); // MSB
    return result;
}

uint32_t ImageDrawer::read32(File &f)
{
    // BMP data is stored little-endian, same as Arduino.
    uint32_t result;
    ((uint8_t *)&result)[0] = f.read(); // LSB
    ((uint8_t *)&result)[1] = f.read();
    ((uint8_t *)&result)[2] = f.read();
    ((uint8_t *)&result)[3] = f.read(); // MSB
    return result;
}