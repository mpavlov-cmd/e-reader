#ifndef GXEPD2DISPLAYTOOLS_H
#define GXEPD2DISPLAYTOOLS_H

#include <Arduino.h>

#include <GxEPD2_GFX.h>
#include <GxEPD2_BW.h>
#include <GxEPD2_display_selection_new_style.h>
#include <Fonts/FreeSans9pt7b.h>

void initDisplay()
{
	// Use this for Waveshare boards with "clever" reset circuit, 2ms reset pulse;
	// Use serial_diag_bitrate = 115200 for logging
	// display.init(0, true, 2, false); 
	display.init(0); 

	display.setRotation(3);
	display.setFont(&FreeSans9pt7b);
	display.setTextSize(1);
	display.setTextColor(GxEPD_BLACK);
}

void clearDisplay()
{
	display.setFullWindow();
	display.firstPage();
	do
	{
		display.fillScreen(GxEPD_WHITE);
	} while (display.nextPage());
}

#endif