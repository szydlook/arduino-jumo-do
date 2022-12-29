#include "JumoDO.h"

RS485Class RS485(Serial1, 18, 19, 19);
ModbusRTUClientClass ModbusRTUClient(RS485);
JumoDOClass JumoDO(ModbusRTUClient);

void setup() {
  Serial.begin(9600);
  uint8_t result = JumoDO.begin(9600, JMD_DEFAULT_ADDRESS);
  if (result) {
    Serial.println(F("Serial 1 Modbus RTU client ok"));
  } else {
    Serial.println(F("Serial 1 Modbus RTU client error"));    
  }
}

void loop() {
  if ( JumoDO.doMeasurement() ) {
    Serial.print(F("Temp: "));
    Serial.print(JumoDO.sensorMeasures.temperature);
    Serial.print(F(" | DO (mgl): "));
    Serial.println(JumoDO.sensorMeasures.oxygenMGL);    
  } else {
    Serial.println( JumoDO.lastError() );
  }
  delay(10000);
}
