#include <Arduino.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

#define BAUDRATE 11520

const char separator[] ={0x0A,0x0D};

uint8_t initialized = 0;
uint32_t last_update = 0;
uint16_t altitude = 0;
uint8_t acceleration = 0;
uint8_t gyro = 0;

//returns true if a and b are equal for the first len elements
uint8_t memeq(char a[], char b[], int len) {
  for (uint8_t i = 0; i < len; i++) {
    if (a[i] != b[i]) {
      return 0;
    }
  }
  return 1;
}

void setup() {
  Serial.begin(11520);
  while(!Serial) {}

}

void loop() {
  if (!initialized) {
    char buf[3];
    char init_message[] = {'+','+','+'};
    char init_reply[] = {'o','k'};
    Serial.readBytes(buf, 3);
    if (memeq(init_message, buf, 3)) {
      Serial.write(init_reply, 3);
      initialized = 1;
    }
    return;
  }

  uint32_t time = millis();
  if (time - last_update > 50) {
    last_update = time;

    char buf[4];
    buf[0] = time & 255;
    buf[1] = (time >> 8)  & 255;
    buf[2] = (time >> 16) & 255;
    buf[3] = (time >> 24) & 255;

    
    //time - flight
    Serial.write(separator, 2);
    Serial.write(0x40);
    Serial.write(buf, 4);

    //altitude
    Serial.write(separator, 2);
    Serial.write(0x00);
    Serial.write(altitude & 0x00FF);
    Serial.write((altitude & 0xFF00) >> 8);
    altitude += acceleration;
    

    //acceleration
    Serial.write(separator, 2);
    Serial.write(0x01);
    Serial.write(acceleration);
    if (random(1,20) == 1) {
      acceleration = random(1,5);
    }

    //pressure
    int16_t pressure = 0x4FFF - altitude;
    Serial.write(separator, 2);
    Serial.write(0x02);
    Serial.write(pressure & 0x00FF);
    Serial.write((pressure & 0xFF00) >> 8);

    //gyroscope
    if (random(5) == 1){
      gyro++;
    }
    Serial.write(separator, 2);
    Serial.write(0x04);
    Serial.write(gyro);
    Serial.write(gyro + 80);
    Serial.write(gyro + 160);

    
    //catastrophe
    if (random(0, 5) == 1) {
      Serial.write(separator, 2);
      Serial.write(0x03);
      Serial.write(1);
    } else 
    if (random(0, 5) == 2) {
      Serial.write(separator, 2);
      Serial.write(0x03);
      Serial.write(0);
    }
  }
}