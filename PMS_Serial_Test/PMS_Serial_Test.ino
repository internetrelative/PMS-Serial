#include <Arduino.h>
#include <SoftwareSerial.h>

// (FOR Arduino Uno R4) Note any pin can be used for TX, but only the following pins can be used for RX:
// D0, D1, D2, D3, D8, D14, D15, A1, A2, A3, A4, A5

SoftwareSerial pmSerial(13, 12);  // RX , TX

byte dataArray[32];
int dataIndex = 0;

void setup() {
  Serial.begin(115200);
  pmSerial.begin(9600);
  delay(2000);  
  Serial.println("Started!");
}

void loop() {
  if (pmSerial.available()) {
    byte incomingByte = pmSerial.read();

    if (dataIndex == 0) {
      if (incomingByte == 0x42) {  // First byte is usually 0x42 but it's 0x43 on PM2008M
        dataArray[dataIndex++] = incomingByte;
      }
    } else if (dataIndex == 1) {
      if (incomingByte == 0x4d) {
        dataArray[dataIndex++] = incomingByte;
      } else {
        dataIndex = 0;
      }
    } else {
      dataArray[dataIndex++] = incomingByte;


      if (dataIndex == 32) {
        // Output the entire byte array in hex format to the serial terminal
        // Serial.print("{");
        // for (int i = 0; i < 32; i++) {
        //   Serial.print("0x");
        //   if (dataArray[i] < 16) {
        //     Serial.print("0");
        //   }
        //   Serial.print(dataArray[i], HEX);
        //   if (i < 31) {
        //     Serial.print(",");
        //   }
        // }
        // Serial.println("}");


        uint8_t checksum_highByte = dataArray[30];
        uint8_t checksum_lowByte = dataArray[31];
        uint16_t checksum = (checksum_highByte << 8) | checksum_lowByte;
        uint16_t sum = 0;

        for (int i = 0; i < 30; i++) {
          sum += dataArray[i];
        }
        if (sum == checksum) {
          uint8_t highByte = dataArray[10];
          uint8_t lowByte = dataArray[11];
          uint16_t fullByte = (highByte << 8) | lowByte;

          Serial.print("PM1 ");
          Serial.print((int)fullByte);
          Serial.println("ug/m3");

          highByte = dataArray[12];
          lowByte = dataArray[13];
          fullByte = (highByte << 8) | lowByte;

          Serial.print("PM2.5 ");
          Serial.print((int)fullByte);
          Serial.println("ug/m3");

          highByte = dataArray[14];
          lowByte = dataArray[15];
          fullByte = (highByte << 8) | lowByte;

          Serial.print("PM10 ");
          Serial.print((int)fullByte);
          Serial.println("ug/m3\n");
          delay(1000);
        } else {
          Serial.println("Checksum Error\n");
          delay(1000);
        }
        dataIndex = 0;
      }
    }
  }
  else
  {
    Serial.println("pmSerial not available");
    delay(1000);
  }
}
