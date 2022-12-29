#include "JumoDO.h"

JumoDOClass::JumoDOClass() {
}

JumoDOClass::JumoDOClass(ModbusRTUClientClass& modbusRTUClient) : modbusRTUClient(&modbusRTUClient), errorNum(0) {
}

JumoDOClass::~JumoDOClass() {
}

uint8_t JumoDOClass::begin(unsigned long baudrate, uint8_t slaveAddress) {
  this->slaveAddr = slaveAddress;
  this->waitingTime = JMD_DEFAULT_WAITING_TIME;
  this->measuringStatus = 0;
  
  if (this->modbusRTUClient->begin(baudrate, SERIAL_8N1)) {
    // To do: Macro Logger Info
  } else {
    this->errorNum = JUMODOERR_MBRTUBEGIN;
    return 0;
  }
  
  return 1;
}

uint8_t JumoDOClass::requestFromModbus(uint8_t modbus_function, uint16_t register_address, uint16_t qnt_registers, uint8_t retries) {  
  uint8_t _retries = 0;
  int8_t result = 0;
  
  while (!result && (_retries < retries)) {
    result = this->modbusRTUClient->requestFrom(this->slaveAddr, modbus_function, register_address, qnt_registers);
    if(!result){
      delay(100);
      _retries++;
    } else {
      // To do: Macro Logger
    }
  }

  return result;
}

uint8_t JumoDOClass::requestFromModbusHoldingRegisters(uint16_t register_address, uint16_t qnt_registers) {
  return requestFromModbus(HOLDING_REGISTERS, register_address, qnt_registers);
}

uint8_t JumoDOClass::readMeasures(float &temperature, float &oxygenSAT, float &oxygenMGL, float &oxygenPPM) {  
  uint8_t retries = 0;
  int8_t result = 0; // uint8_t ?
  uint16_t sensorStatus = 65535; // > 0, 4095 is 12 bits set to 1, status 7 for TEMP, SAT, MGL, PPM
 
  while (!result && (retries < 5)) {
    result = this->modbusRTUClient->holdingRegisterWrite(this->slaveAddr, JMD_START_MEASURING_REG, JMD_CMD_START_SAT_MGL);    
    if(!result) {
      delay(100);
      retries++;
    }    
  }
  if (!result) {
    // To do: Macro Logger this->modbusRTUClient->lastError()
    this->errorNum = JUMODOERR_MBHRW;
    return 0;
  }

  delay(waitingTime + 20);
  retries = 0;
  
  while ( (sensorStatus > 0) && (retries < 3) ) {
    result = requestFromModbusHoldingRegisters(JMD_MEASURING_STATUS_REG, 0x0001);
    if (result) {
      sensorStatus = this->modbusRTUClient->read();

      if (sensorStatus == 511) { // 7 for TEMP, SAT and MGL - Measurement not yet complete - tested
        if (retries == 2) {
          this->errorNum = JUMODOERR_TIMEOUT;          
          return 0;            
        }
        delay(waitingTime);
        retries++;
      } else if (sensorStatus == 365) { // 5 for TEMP, SAT and MGL - Measurement NOK, membrane cap missing from sensor or is damaged - untested
        this->errorNum = JUMODOERR_MEMBRANE;
        return 0;
      } else if (sensorStatus != 0) {
        this->errorNum = JUMODOERR_MEAFAIL;
        return 0;        
      }
    } else {
      // To do: Macro Logger this->modbusRTUClient->lastError()
      this->errorNum = JUMODOERR_MBHRR;
      return 0;  
    }
  }

  result = requestFromModbusHoldingRegisters(JMD_TEMPERATURE_REG, 0x0002);

  if (result) {
    this->u16ItoFloat.ints[1] = this->modbusRTUClient->read();
    this->u16ItoFloat.ints[0] = this->modbusRTUClient->read();
    temperature = this->u16ItoFloat.toFloat;
  } else {
    // To do: Macro Logger this->modbusRTUClient->lastError()
    this->errorNum = JUMODOERR_MBHRR;    
    return 0;
  }

  result = requestFromModbusHoldingRegisters(JMD_OXYGEN_MGL_REG, 0x0002);
  
  if (result) {
    this->u16ItoFloat.ints[1] = this->modbusRTUClient->read();
    this->u16ItoFloat.ints[0] = this->modbusRTUClient->read();
    oxygenMGL = this->u16ItoFloat.toFloat;
  } else {
    // To do: Macro Logger this->modbusRTUClient->lastError()
    this->errorNum = JUMODOERR_MBHRR;    
    return 0;
  }

  return result;
}

uint8_t JumoDOClass::doMeasurement(void)
{
  // Initialize variables
  sensorMeasures.temperature = 0;
  sensorMeasures.oxygenSAT = 0;
  sensorMeasures.oxygenMGL = 0;
  sensorMeasures.oxygenPPM = 0;

  uint8_t retries = 0;
  int8_t result = 0;
  uint8_t validMeasure = 0;
  
  while (!validMeasure && (retries < 3)) {
    result = readMeasures(sensorMeasures.temperature, sensorMeasures.oxygenSAT, sensorMeasures.oxygenMGL, sensorMeasures.oxygenPPM);

    if ((sensorMeasures.temperature != 0) || (sensorMeasures.oxygenSAT != 0) || (sensorMeasures.oxygenMGL != 0) || (sensorMeasures.oxygenPPM != 0)) {
      validMeasure = 1;
    }
    else {
      delay(1000);
    }
    retries++;
  }

  return result;
}

const char* JumoDOClass::lastError() {
  if (this->errorNum == 0) {
    return NULL;
  }
  switch (this->errorNum) {
    case JUMODOERR_MBRTUBEGIN:
      return "Modbus RTU Client initialization error, check wiring and address";
    case JUMODOERR_MBRTUCLIENT:
      return "Modbus RTU Client error";
    case JUMODOERR_MBHRR:
      return "Modbus RTU Client Holding Register read error";
    case JUMODOERR_MBHRW:
      return "Modbus RTU Client Holding Register write error";
    case JUMODOERR_TIMEOUT:
      return "Measurement timeout";
    case JUMODOERR_MEMBRANE:
      return "Membrane cap missing from sensor or is damaged";
    case JUMODOERR_MEAFAIL:
      return "Measurement performed with error";
    default:
      return "Unknown";
  }
}

//JumoDOClass JumoDO;
