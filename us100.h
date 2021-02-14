#include "esphome.h"

/* To start measuring the distance, output a 0x55 over the serial port and
 * read back the two byte distance in high byte, low byte format. The
 * distance returned is measured in millimeters. Use the following formula
 * to obtain the distance as millimeters:
 *
 *     Millimeters = FirstByteRead * 256 + SecondByteRead
 *
 * This module can also output the temperature when using serial output
 * mode. To read the temperature, output a 0x50 byte over the serial port
 * and read back a single temperature byte. The actual temperature is
 * obtained by using the following formula:
 *
 *     Celsius = ByteRead - 45
 */

class US100 : public PollingComponent, public UARTDevice {

  public:
    US100(UARTComponent *parent) : PollingComponent(10000), UARTDevice(parent) {}

    Sensor *tempsensor = new Sensor();
    Sensor *distsensor = new Sensor();

    void setup() override {
    }

    void loop() override {
      if(bytesexpected==2 && available() >= 2) {
        // we're expecting a distance measurement to come in, and there are
        // enough bytes for it, process it
        unsigned int mm = read() * 256 + read();
        if((mm>1) && (mm<10000)) {
          //ESP_LOGD("us100","distance is %u",mm);
          distsensor->publish_state(mm);
        }
        // finished with distance measurement, move on to temperature
        flush();
        write(0x50); // tell the US100 to start a temperature measurement
        bytesexpected=1; // we should start looking for a temperature reading
      } else if(bytesexpected==1 && available() >= 1) {
        // we are looking for a temperature and there are bytes to read
        int temp = read();
        if((temp>1) && (temp<130)) {
          temp -= 45;
          //ESP_LOGD("us100","temperature is %d",temp);
          tempsensor->publish_state(temp);
        }
        bytesexpected=0; // stop looking for bytes
      }
    }
    void update() override {
      flush();
      write(0x55); // tell the US100 to start a distance measurement
      bytesexpected=2; // tell loop() that it should start looking for a distance
    }

  private:
    unsigned int bytesexpected=0;
};
