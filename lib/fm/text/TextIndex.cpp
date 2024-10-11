#include "TextIndex.h"

TextIndex::TextIndex(GxEPD2_GFX &gxDisplay, FileManager &fileManager) : display(gxDisplay), fm(fileManager)
{
}

File TextIndex::generateIdx(const char *path)
{
    // Open file 
    Serial.printf("Opening file %s\n", path);
	File file = fm.openFile(path, FILE_READ);
	if (!file) {
		Serial.printf("Unable to open file %s\n", path);
        return File();
	}

	Serial.printf("File size: %i\n", file.size());

    // Create temp dir name
    const char* parentDir = fm.getParentDir(file.path());
	Serial.println("Dir Name:");
	Serial.println(parentDir);

	String filename = String(file.name());
	String fileSizeStr = String(file.size());
	filename.toLowerCase();

	String idxDirName = "._" + filename + "_" + fileSizeStr + "_idx";	
	String idxDirPath = String(parentDir) + "/" + idxDirName;

    String idxDirPathCopy = idxDirPath;
	const char* idxDirPathCharArr = idxDirPathCopy.c_str();

    // If direectory exists, consider index complete and return
	bool dirExists = fm.exists(idxDirPathCharArr);
	if (dirExists) {
		if (forceIndex) {
			fm.removeDirRecursive(idxDirPathCharArr);
		} else { 
			Serial.printf("Index for %s already exists, returning dir: %s\n", filename, idxDirPathCharArr);
			return fm.openFile(idxDirPath.c_str(), FILE_READ);
		}
		
	}

	fm.createDir(idxDirPathCharArr);
	Serial.printf("Idx directory path: %s\n", idxDirPathCharArr);

    // ---------------- Start indexing ----------------
    while (file.available())
	{
		c = file.read(); // Read the file character by character

		// If we hit a newline or space, treat it as a word boundary
		if (c == ' ' || c == '\n')
		{
			// Drop values in case word or line are empty
			wordWidth = linewidth = height = 0;
			display.getTextBounds(currentWord, 0, 0, &x, &y, &wordWidth, &height); 
			display.getTextBounds(currentLine, 0, 0, &x, &y, &linewidth, &height);

			// Check if the current word fits on the line
			if (linewidth + spaceWidth + wordWidth >= textAreaWidth - 2)
			{
				// If the word doesn't fit, print the current line and start a new one
				currentPage.concat(currentLine);
				currentPage.concat('\n');
				currentLine = "";		  // Reset current line
				// currentLineWidth = 0;  // Reset width counter
				skipLeadingSpaces = true; // Skip leading spaces for the next line
				wrappedLine = true;		  // Mark that the line was wrapped
				spaceAfterWrap = true;

				lineIndex++;
			}

			// Add the word to the current line, but only if it's not an empty line
			if (!skipLeadingSpaces || currentWord.length() > 0)
			{
				currentLine += currentWord;
				// currentLineWidth += width;

				if (wrappedLine && spaceAfterWrap) {
					currentLine += ' ';
					// currentLineWidth += spaceWidth;
					spaceAfterWrap = false;
				}
			}

			// Add space if it's not the start of a new line
			if (c == ' ')
			{
				// Avoid more then one space in row
				if (!skipLeadingSpaces && (currentLine.length() > 0 && currentLine.charAt(currentLine.length() - 1) != ' '))
				{
					currentLine += ' ';
					// currentLineWidth += spaceWidth;
				}
			}

			// If there's a newline, print the current line and move to the next line
			if (c == '\n')
			{
				if (!wrappedLine)  // Only print the line if it wasn't already wrapped
				{							 
					currentPage.concat(currentLine); // Display the current line
					currentPage.concat('\n');
					lineIndex++;

					currentLine = "";		  // Reset the line
					// currentLineWidth = 0;  // Reset width counter
					skipLeadingSpaces = true; // Skip leading spaces for the next line
				} else {
					// Add space instead of '\n' when line was wrapped
					if (currentLine.length() > 0 && currentLine.charAt(currentLine.length() - 1) != ' ') {
						currentLine += ' ';
						// currentLineWidth += spaceWidth;
					}
				}

				wrappedLine = false;
			}

			// Clear the current word
			currentWord = "";
		}
		
		else if (c != ' ' && isPrintable(c))
		{
			// If it's a normal character, build the current word
			currentWord += c;
			skipLeadingSpaces = false; // We've found non-space content, so we stop skipping spaces
		}

		if (lineIndex >= linesPerPage) {
			// Creating page file on SD card
			String pageFileName = idxDirPath + "/._" + String(pageIndex) + ".page.txt";
			bool pageFileWritten = fm.writeFile(pageFileName.c_str(), currentPage.c_str());
            if (!pageFileWritten) {
                Serial.println("Error creating page file");
            }
			
			// Drop current page, so next is processed
			currentPage = "";
			pageIndex++;
			lineIndex = currentLine.isEmpty() ? 0 : 1;
		}

		// Only check for page limit in case it has value different than
		if (pageLimit != 0 && pageIndex > pageLimit) {
            Serial.printf("Page limit of %i exceeded\n", pageLimit);
			break;
		}
	}

    // TODO: Check if there are cases when current line is not emplty, and so generate additional page

    // if (!currentLine.isEmpty())
	// {
	// 	currentPage.concat(currentLine);
    //  // ...
	// }

    // Close original file
    file.close();

    return fm.openFile(idxDirPath.c_str(), FILE_READ);
}

void TextIndex::configure(TextIndexConf conf)
{
    textAreaWidth = conf.textW;
    textAreaHeight = conf.textH;
    pageLimit = conf.pageLim;
	forceIndex = conf.forceIndex;

    // Get Space width
	display.getTextBounds("s w", 0, 0, &x, &y, &width, &height);
	spaceWidth = width;
	display.getTextBounds("ws", 0, 0, &x, &y, &width, &height);
    spaceWidth = spaceWidth - width;

    Serial.printf("Calculated space width: %i\n", spaceWidth);

    // Calculate lines per page. Find fine one line height 
	display.getTextBounds("LINE\n\nLINE", 0, 0, &x, &y, &width, &height);
	uint8_t lineHeight = height / 3 + 2;
	linesPerPage  = textAreaHeight / lineHeight - 1; 
	Serial.printf("Lines per page: %i, line height: %i\n", linesPerPage, lineHeight);
}
