#ifndef JUMODO_H_INCLUDED
#define JUMODO_H_INCLUDED

#include <ArduinoModbus.h>
#include <ArduinoRS485.h>

// Sensor registers. JMD means JumoDO
#define JMD_START_MEASURING_REG       0x0001
#define JMD_MEASURING_STATUS_REG      0x0052
#define JMD_TEMPERATURE_REG           0x0053
#define JMD_OXYGEN_MGL_REG            0x0057

// Sensor commands. JMD means JumoDO
#define JMD_CMD_START_SAT             0x0003
#define JMD_CMD_START_SAT_MGL         0x0007
#define JMD_CMD_START_SAT_PPM         0x0011

#define JMD_DEFAULT_ADDRESS           255 // default 255
#define JMD_DEFAULT_WAITING_TIME      300 // in ms

#define JUMODOERR_MBRTUBEGIN 1
#define JUMODOERR_MBRTUCLIENT 2
#define JUMODOERR_MBHRR 3
#define JUMODOERR_MBHRW 4
#define JUMODOERR_TIMEOUT 5
#define JUMODOERR_MEMBRANE 6
#define JUMODOERR_MEAFAIL 7

class JumoDOClass {
  public:
    JumoDOClass();
    JumoDOClass(ModbusRTUClientClass& modbusRTUClient);
    virtual ~JumoDOClass();

    uint8_t begin(unsigned long baudrate, uint8_t slaveAddress);
    uint8_t doMeasurement(void);
    const char* lastError();

    struct structSensorMeasures {
      float temperature;
      float oxygenSAT;
      float oxygenMGL;
      float oxygenPPM;
    };
    structSensorMeasures sensorMeasures;

    union {
      uint16_t ints[2];
      float toFloat;
    } u16ItoFloat;

    uint16_t waitingTime;
    uint16_t measuringStatus;
    
  private:
//    ModbusRTUClientClass* modbusRTUClient = &ModbusRTUClient;
    /*
     * To allow multi instances you need also to modify
     * ModbusRTUClient and RS485 library.
     */
    ModbusRTUClientClass* modbusRTUClient;
    uint8_t requestFromModbusHoldingRegisters(uint16_t register_address, uint16_t qnt_registers);
    uint8_t requestFromModbus(uint8_t modbus_function, uint16_t register_address, uint16_t qnt_registers, uint8_t retries=10);
    uint8_t readMeasures(float &temperature, float &oxygenSAT, float &oxygenMGL, float &oxygenPPM);
    uint8_t slaveAddr;
    int errorNum;
};

//extern JumoDOClass JumoDO;
#endif
