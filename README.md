# arduino-jumo-do
This is an simple Arduino library for JUMO ecoLine O-DO Optical Sensor for Dissolved Oxygen.
Tested on Arduino Mega 2560 with DFRobot RS485 to UART Signal Converter ([DFR0845](https://www.dfrobot.com/product-2392.html)) connected to Serial1 (pins 18 and 19).
By default sensor address is set to 255. If you don't have access to Jumo DSM software to change this address, you need to modify ArduinoModbus library. Open local copy of modbus-rtu.cpp, find _modbus_set_slave function and change "slave <= 247" to "slave <= 255".

## Dependencies
- [ArduinoModbus](https://www.arduino.cc/en/ArduinoModbus/ArduinoModbus)

## License
MIT License. Based in the work of [FTTech Brasil](https://github.com/FTTechBrasil/AqualaboSensor).
