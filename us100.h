#include "esphome.h"

class US100 : public PollingComponent, public UARTDevice {

  public:
    US100(UARTComponent *parent) : PollingComponent(10000), UARTDevice(parent) {}

    Sensor *tempsensor = new Sensor();
    Sensor *distsensor = new Sensor();

    void setup() override {
    }

    void update() override {

      /* To start measuring the distance, output a 0x55 over the serial port and
       * read back the two byte distance in high byte, low byte format. The
       * distance returned is measured in millimeters. Use the following formula
       * to obtain the distance as millimeters:
       *
       *     Millimeters = FirstByteRead * 256 + SecondByteRead
       */
      flush();
      write(0x55);
      delay(500);
      if(available() >= 2)
      {
        unsigned int mm = read() * 256 + read();
        if((mm>1) && (mm<10000)) {
          //ESP_LOGD("us100","distance is %u",mm);
          distsensor->publish_state(mm);
        }
      }

      /* This module can also output the temperature when using serial output
       * mode. To read the temperature, output a 0x50 byte over the serial port
       * and read back a single temperature byte. The actual temperature is
       * obtained by using the following formula:
       *
       *     Celsius = ByteRead - 45
       */
      flush();
      write(0x50);
      delay(500);
      if(available() >= 1) {
        int temp = read();
        if((temp>1) && (temp<130)) {
          temp -= 45;
          //ESP_LOGD("us100","temperature is %d",temp);
          tempsensor->publish_state(temp);
        }
      }

    }
};
