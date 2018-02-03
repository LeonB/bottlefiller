#include "BottleType.h"
#include "HX711.h"

const int HX711_DOUT = A1;
const int HX711_SCK = A0;
HX711 scale;

const unsigned long DEFAULT_BOTTLE_DEVIATION = 100;

BottleType bottleTypes[10];

void loadBottles(BottleType bottleTypes[10]) {
    /* bottleTypes[0] = BottleType(); */
    strcpy(bottleTypes[0].Name, "Test");
    bottleTypes[0].MinWeight = 0;
    bottleTypes[0].MaxWeight = 123;
    Serial.println("finished loading");
}

void initLoadCell() {
  // parameter "gain" is ommited; the default value 128 is used by the library
  // HX711.DOUT - pin #A1
  // HX711.PD_SCK - pin #A0
  scale.begin(HX711_DOUT, HX711_SCK);
  scale.read_average(20);
  scale.tare(); // reset the scale to 0
}

void setup() {
    Serial.begin(38400);
    initLoadCell();
    loadBottles(bottleTypes);
}

void loop() {
    /* scale.Update(); */
    Serial.println("--------------------");
    Serial.println(bottleTypes[0].Name);
    Serial.println(bottleTypes[0].MinWeight);
    Serial.println(bottleTypes[0].MaxWeight);
    Serial.println(scale.read());
    delay(250);
}
