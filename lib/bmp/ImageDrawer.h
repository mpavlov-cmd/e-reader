#ifndef IMAGEDRAWER_H
#define IMAGEDRAWER_H

#include <Arduino.h>
#include <GxEPD2_GFX.h>
#include <GxEPD2_BW.h>
#include <FileManager.h>


struct ImageDrawer
{
    private:
        GxEPD2_GFX& display;

        static const uint16_t input_buffer_pixels = 800; // may affect performance

        static const uint16_t max_row_width = 1448; // for up to 6" display 1448x1072
        static const uint16_t max_palette_pixels = 256; // for depth <= 8

        uint8_t input_buffer[3 * input_buffer_pixels];        // up to depth 24
        uint8_t output_row_mono_buffer[max_row_width / 8];    // buffer for at least one row of b/w bits
        uint8_t output_row_color_buffer[max_row_width / 8];   // buffer for at least one row of color bits
        uint8_t mono_palette_buffer[max_palette_pixels / 8];  // palette buffer for depth <= 8 b/w
        uint8_t color_palette_buffer[max_palette_pixels / 8]; // palette buffer for depth <= 8 c/w
        uint16_t rgb_palette_buffer[max_palette_pixels];      // palette buffer for depth <= 8 for buffered graphics, needed for 7-color display

        uint16_t read16(File &f);
        uint32_t read32(File &f);

    public:
        ImageDrawer(GxEPD2_GFX& display);

        void drawBitmapFromSD_Buffered(
            File& file,
            int16_t x,
            int16_t y,
            bool with_color,
            bool partial_update
        );

};


#endif