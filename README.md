# espbs

ESP32 arduino base template.<br/>

Checkout the [espjs project](https://github.com/Lillifee/espjs) to adapt the website.<br/>
The bundled website (data) is stored on spiffs partition. <br/>

### `pio run -t upload` - Compile and upload the program

### `pio run -t uploadfs` - Build and upload the website partition

<br/>

## BSEC - Bosch Sensortec

To run the BSECHelper with BME680 for additional IAQ (indoor air quality) - check the BSEC-Arduino-library.
The library is not included in this repo.
https://github.com/BoschSensortec/BSEC-Arduino-library

Copy the library into ./pio/libdeps/BSECSoftwareLibrary and enable the special build flags in the platformio.ini
