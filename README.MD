 ## TODO List

### Software
- [x] (Temp) Add sleep menu item in the menu. Sleep should be supported on timeout and on demand. Before sleep should pick ramdom image from SD card and print it.
    - [x] Add Sleep Intent, SleepIntent::onStartup() would put device into deep sleep
    - [x] Implement return type for OnAction to switch intenets in main
    - [x] Implement image widget
- [ ] Imlement file manager UI which should work with DirIndex and would allow to select file
- [ ] Implement Book Intent with an abilities to: Show current page, show naviagtion bar (book percent, line, total pages)
- [ ] Implement the flow where the device will wake-up and check wakup reason, if it is timer, it should not refresh full display, instead it should update clock and battery charge (probably separaete intend) and go back to sleep. In case wakeup reason is ext1 (joystick) full wakup should happen.
- [ ] Implement weather widget (Temperature, Rain / Sun / Clouds / Snow, surise, down e.t.c. with icons)
- [ ] Implement settings
    - [ ] Enable WIFI (Randomized SSID)
    - [ ] Configure time and timezone
    - [ ] Configure sleep timeout
- [ ] Implement outlook integration to show calendar (widget + detailed view)

### Hardware
- [ ] Add beter connectors SH 1.0 but horizontal, not vertical
- [ ] FIX footprint for crystal
- [ ] FIX footprint for joystick (rotate 45 degrees)
- [ ] Add backlight for the screen (Using one of the JTAG pins)


## WIKI
 - Display size: 800x480  
 - [Battery Sense](https://github.com/rlogiacco/BatterySense?tab=readme-ov-file)
 - [Material Icons](https://fonts.google.com/icons?icon.set=Material+Icons&icon.size=24&icon.color=%235f6368)
 - [PNG to GIF](https://cloudconvert.com/png-to-gif)
 - [SD Card](https://github.com/espressif/arduino-esp32/tree/master/libraries/SD)
 - [Bitmap From SD](https://github.com/ZinggJM/GxEPD2/blob/master/examples/GxEPD2_SD_Example/GxEPD2_SD_Example.ino)
 - [Interrupts](https://www.youtube.com/watch?v=CJhWlfkf-5M)
 - [Sleep](https://randomnerdtutorials.com/esp32-deep-sleep-arduino-ide-wake-up-sources/)