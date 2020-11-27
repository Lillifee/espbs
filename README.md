# espbs

Collection of my ESP32 projects.<br/>

The project is set up with [platformIO](https://platformio.org/), and I'm using VS-Code as a code editor. <br/>
To reuse and maintain the base libraries, I decided to create one single project for different applications, switchable in the platform.ini.

The bundled configuration website is already part of this project (data) and stored on a separate spiffs partition on the ESP. If you want to adjust the website, please see [espjs project](https://github.com/Lillifee/espjs) <br/>

## Switch application

To switch the application from e.g. cube to knob

- open the platformio.ini file
- check the [board](https://docs.platformio.org/en/latest/boards/index.html) type (lolin_d32, lolin32, wemos_d1_mini32 etc.)
- comment the cube section
- uncomment the knob section

## Upload the project

You can eigher run the commands in the platformIO terminal or just use the PlatformIO tab in VSCode

### `pio run -t upload` - Build and upload the firmware

### `pio run -t uploadfs` - Build and upload the website partition

<br/>

![picture](screenshots/upload.png)

- `Upload` - build and upload the firmware (firmware.bin)
- `Upload Filesystem Image` - Upload the website partition (spiffs.bin)

## Setup

On the first startup, the ESP creates an access point called "espbs" to configure your Wi-Fi settings.
</br> Connect to the WLAN, open the browser with http://espbs or http://192.168.4.1, and add your Wi-Fi settings. </br> I would also recommend using a static IP address to improve the startup time of the ESP.
After applying the changes the ESP will automatically reboot and connect to your network.

![picture](screenshots/website.png)

Some applications are running in deep-sleep to reduce power consumption. Don't forget to send the esp to deep sleep with the ![picture](screenshots/sleep.png) button in the right bottom corner of the "Update" section on the website.

To change the settings while the esp is in sleep mode, you can press the RESET button to start the webserver again.

The reset button will not affect your settings, so if you misconfigured your ESP (e.g. wrong Wi-Fi settings) you can erase the flash from the PlatformIO menu and start from scratch.

## Update firmware

Once the espbs is running, it's possible to update the firmware over the web interface.

- Navigate to the platformIO section in VS Code and press `Build`
- If you have done changes on the website as well press `Build Filesystem Image`
- Once the build is finished you should find a firmware.bin and a spiffs.bin (website) in your output folder /.pio/build/{board_name}/

Open the website and upload the new firmware.

- ![picture](screenshots/firmware.png) Press the update firmware button and choose the firmware.bin file.
- ![picture](screenshots/updateWebsite.png) Press the update website button and choose the spiffs.bin file.

# Applications

## Cube

The cube is a remote control for 6 different functions. It uses an MPU6050 to figure out which side is on top and send a UDP request into your network. To save the battery life, the ESP32 remains in a deep sleep until the cube gets rotated. I switch four different light scenes in my living room and control the blinds with the remaining sides.

![picture](screenshots/cube.png)

## CO2

The co2 sends the air quality as a UDP request to your home automation. I added two separate sensors to measure air quality in my living room, the MHZ-19 for the CO2 measurements, and a Bosh BME680 for the overall air quality. This application doesn't use a battery, therefore don't send the ESP to deep sleep after configuration.

You can find the 3d printing files on https://www.prusaprinters.org/prints/47612-air-quality-sensor-case-esp32

![picture](screenshots/co2.png)

### BSEC - Bosch Sensortec

To run the BSECHelper with BME680 for additional IAQ (indoor air quality) - check the BSEC-Arduino-library.
The library is not included in this repo.
https://github.com/BoschSensortec/BSEC-Arduino-library

Copy the library into ./pio/libdeps/BSECSoftwareLibrary and enable the special build flags in the platformio.ini

## Display

The display uses a waveshare 7.5 inch display to visualize the values of your smart home. I use it to visualize the power consumption and the air quality in the living room. To save the battery life, the ESP32 remains in deep sleep and updates in the configured interval. At the moment, it's not configurable, but you can adjust it to your needs in the lib/WaveshareHelper library.

You can find the 3d printing files on https://www.prusaprinters.org/prints/47605-e-paper-display-stand-esp32

![picture](screenshots/display.png)

## Knob

The knob uses a rotary encoder to e.g. dim the light over Wi-Fi.
To save the battery life the ESP32 remains in a deep sleep until the knob gets pressed. On rotation, the current value is sent as a UDP packet into the network to my home automation server. After a few seconds without value change, the ESP goes back to deep sleep.

You can find the 3d printing files on https://www.prusaprinters.org/prints/47614-wifi-knob-esp32

![picture](screenshots/knob.png)
