#include <Adafruit_NeoPixel.h>

#define RGB_PIN   48  
#define NUM_LEDS  1

Adafruit_NeoPixel rgb(NUM_LEDS, RGB_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(115200);
  rgb.begin();
  rgb.setBrightness(64);  
  rgb.clear();
  rgb.show();
  Serial.println("RGB LED init done, pin=" + String(RGB_PIN));
}

void loop() {
  Serial.println("RED");
  rgb.setPixelColor(0, rgb.Color(255, 0, 0));
  rgb.show();
  delay(1000);

  Serial.println("GREEN");
  rgb.setPixelColor(0, rgb.Color(0, 255, 0));
  rgb.show();
  delay(1000);

  Serial.println("BLUE");
  rgb.setPixelColor(0, rgb.Color(0, 0, 255));
  rgb.show();
  delay(1000);

  Serial.println("WHITE");
  rgb.setPixelColor(0, rgb.Color(255, 255, 255));
  rgb.show();
  delay(1000);

  Serial.println("OFF");
  rgb.setPixelColor(0, 0);
  rgb.show();
  delay(1000);
}
