#ifndef TEXTINDEX_H
#define TEXTINDEX_H

#include <Arduino.h>
#include <FileManager.h>
#include <GxEPD2_GFX.h>
#include <GxEPD2_BW.h>

struct TextIndexConf {
    const uint16_t textW;
    const uint16_t textH;
    const uint16_t pageLim;
    const bool forceIndex;
};

struct TextIndex {

    private:
        GxEPD2_GFX& display;
        FileManager& fm;

        uint16_t textAreaWidth  = 480;
        uint16_t textAreaHeight = 760;

        // Define variables for text size detection
        int16_t x = 0, y = 0;
        uint16_t width = 0, height = 0, wordWidth = 0, linewidth = 0;
        uint8_t spaceWidth = 0;

        // Define containers for word, line and page
        String currentPage = "";
	    String currentLine = ""; // Holds the text for the current line
	    String currentWord = ""; // Holds the current word being built

        // int currentLineWidth = 0;	// Tracks the pixel width of the current line
        char c;
        bool skipLeadingSpaces = true;  // Skip leading spaces on new lines
        bool wrappedLine       = false; // Track if the last line was wrapped
        bool spaceAfterWrap    = false; 
        bool forceIndex        = false;

        uint16_t lineIndex = 0;
        uint16_t pageIndex = 0;

        // max abount of pages to produce
        uint16_t pageLimit = 0;
        uint16_t linesPerPage = 0;

    public:
        TextIndex(GxEPD2_GFX& gxDisplay, FileManager& fileManager);

        /**
         * Generates index of the text file by splitting it by pages and saving every page to the separate file in the folder
         * 
         * @param path full path to the file to index;
         * @returns file pointing to the folder where index is created. File can be checked using if (!file) in the calling code
         */
        File generateIdx(const char *path);


        /**
         * Setting configuration for indexing.
         * Calculates space width, max lines per page; To be executed once font, or text area size changes 
         * 
         * @param conf self explenatory
        */
        void configure(TextIndexConf conf);
};


#endif