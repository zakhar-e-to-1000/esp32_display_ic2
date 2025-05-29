#include <U8g2lib.h>

#include "SHT2x.h"
#include "Wire.h"
#include "meteo_math.h"
U8G2_SH1106_128X64_NONAME_F_HW_I2C screen(U8G2_R0, U8X8_PIN_NONE);
SHT2x sht;
#define BUTTON_PIN_1 12
#define BUTTON_PIN_2 14
#define DELAY 250
#define CELC true
#define FAREN false
#define REAL true
#define DELUSIONAL false
int ms_start;
bool temp_mode = CELC;
bool hum_mode = DELUSIONAL;
int prev_button_state_1 = LOW;
int prev_button_state_2 = LOW;
bool can_change = false;
bool change_id = 0;
bool tick = 0;
void setup() {
  Serial.begin(115200);
  Serial.println(__FILE__);
  Serial.print("SHT2x_LIB_VERSION: \t");
  Serial.println(SHT2x_LIB_VERSION);

  Wire.begin();
  sht.begin();
  screen.begin();
  screen.clearBuffer();
  screen.setFont(u8g2_font_ncenB08_tr);
  uint8_t stat = sht.getStatus();
  Serial.print(stat, HEX);
  Serial.println();

  pinMode(BUTTON_PIN_1, INPUT_PULLUP);
  pinMode(BUTTON_PIN_2, INPUT_PULLUP);
  ms_start = millis();
}

void draw_on_screen() {
  float temp = sht.getTemperature();
  char buffer_temp[30], buffer_hum[30];
  if (temp_mode == CELC) {
    sprintf(buffer_temp, "TEMP: %.2f °C", temp);
  } else {
    sprintf(buffer_temp, "TEMP: %.2f °F", fahrenheit(temp));
  }
  float hum = sht.getHumidity();
  if (hum_mode == REAL) {
    sprintf(buffer_hum, "HUM: %.2f%%", hum);
  } else {
    sprintf(buffer_hum, "HUM: %.2fg/m^3", absolute_humidity(hum / 100, temp));
  }
  Serial.println(buffer_temp);
  Serial.println(buffer_hum);
  Serial.println(digitalRead(BUTTON_PIN_1));
  screen.clearBuffer();
  if (tick == 1 || !can_change || change_id != 0) {
    screen.drawStr(0, 10, buffer_temp);
  }
  if (tick == 1 || !can_change || change_id != 1) {
    screen.drawStr(0, 50, buffer_hum);
  }
  screen.sendBuffer();
}
void loop() {
  sht.read();
  int time = millis();
  int button_state_1 = HIGH - digitalRead(BUTTON_PIN_1);
  int button_state_2 = HIGH - digitalRead(BUTTON_PIN_2);
  if (button_state_1 == HIGH && prev_button_state_1 == LOW) {  // press 1
    if (can_change == true) {
      if (change_id == 0) {
        temp_mode = !temp_mode;
      } else if (change_id == 1) {
        hum_mode = !hum_mode;
      }
    }
    can_change = false;
  }
  if (button_state_2 == HIGH && prev_button_state_2 == LOW) {  // press 2
    if (can_change) {
      change_id = (change_id + 1) % 2;
    }
    can_change = true;
  }
  prev_button_state_1 = button_state_1;
  prev_button_state_2 = button_state_2;
  if (ms_start + DELAY < time || ms_start > time) {
    ms_start = time;
    tick = (tick + 1) % 2;
    draw_on_screen();
  }
}