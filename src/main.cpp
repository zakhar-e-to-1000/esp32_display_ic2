#include <Adafruit_BMP280.h>
#include <U8g2lib.h>

#include "SHT2x.h"
#include "Wire.h"
#include "meteo_math.h"

#define BUTTON_PIN_1 15
#define BUTTON_PIN_2 4
#define DELAY 300
#define DEBOUNCE_DELAY 50
#define CELC true
#define FAREN false
#define REAL true
#define DELUSIONAL false
#define HPascals true
#define MMHG false
int ms_start;

bool prev_state_1 = false;
bool prev_state_2 = false;

unsigned long last_debounce_1 = 0;
unsigned long last_debounce_2 = 0;

bool temp_mode = CELC;
bool hum_mode = DELUSIONAL;
bool pressure_mod = HPascals;
bool can_change = false;
int change_id = 0;
bool tick = 0;
U8G2_SH1106_128X64_NONAME_F_HW_I2C screen(U8G2_R0, U8X8_PIN_NONE);
SHT2x sht;
Adafruit_BMP280 bmp;

void process_button_1() {
  unsigned long reading = millis();
  bool state_1 = digitalRead(BUTTON_PIN_1) == LOW;
  if (state_1 == prev_state_1 || (reading - last_debounce_1) < DEBOUNCE_DELAY) {
    return;
  }
  prev_state_1 = state_1;
  last_debounce_1 = reading;
  if (state_1 == false) {
    return;
  }
  if (can_change == true) {
    if (change_id == 0) {
      temp_mode = !temp_mode;
    } else if (change_id == 1) {
      hum_mode = !hum_mode;
    } else if (change_id == 2) {
      pressure_mod = !pressure_mod;
    }
  }
  can_change = false;
}

void process_button_2() {
  unsigned long reading = millis();
  bool state_2 = digitalRead(BUTTON_PIN_2) == LOW;
  if (state_2 == prev_state_2 || (reading - last_debounce_2) < DEBOUNCE_DELAY) {
    return;
  }
  prev_state_2 = state_2;
  last_debounce_2 = reading;
  if (state_2 == false) {
    return;
  }
  if (can_change) {
    change_id = (change_id + 1) % 3;
  }
  can_change = true;
}

void setup() {
  Serial.begin(115200);
  Wire.begin();
  sht.begin();
  screen.begin();
  screen.clearBuffer();
  screen.setFont(u8g2_font_ncenB08_tr);
  if (!bmp.begin()) {
    Serial.println("Failed to connect to the baromether");
  }
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */
  pinMode(BUTTON_PIN_1, INPUT_PULLUP);
  pinMode(BUTTON_PIN_2, INPUT_PULLUP);
  ms_start = millis();
}

void draw_on_screen() {
  sht.read();
  float temp = sht.getTemperature();
  float hum = sht.getHumidity();
  float pressure = bmp.readPressure();
  char buffer_temp[30], buffer_hum[30], buffer_pressure[30];
  if (temp_mode == CELC) {
    sprintf(buffer_temp, "TEMP: %.2f *C", temp);
  } else {
    sprintf(buffer_temp, "TEMP: %.2f *F", fahrenheit(temp));
  }
  if (hum_mode == REAL) {
    sprintf(buffer_hum, "HUM: %.2fg/m^3", absolute_humidity(hum / 100, temp));
  } else {
    sprintf(buffer_hum, "HUM: %.2f%%", hum);
  }
  if (pressure_mod == HPascals) {
    sprintf(buffer_pressure, "PRESS: %.2fhPa", pressure / 100);
  } else {
    sprintf(buffer_pressure, "PRESS: %.2f mm. Hg", mm_hg(pressure));
  }
  screen.clearBuffer();
  if (tick == 1 || !can_change || change_id != 0) {
    screen.drawStr(0, 10, buffer_temp);
  }
  if (tick == 1 || !can_change || change_id != 1) {
    screen.drawStr(0, 30, buffer_hum);
  }
  if (tick == 1 || !can_change || change_id != 2) {
    screen.drawStr(0, 50, buffer_pressure);
  }
  screen.sendBuffer();
}
void loop() {
  process_button_1();
  process_button_2();
  int time = millis();
  if (ms_start + DELAY < time || ms_start > time) {
    ms_start = time;
    tick = (tick + 1) % 2;
    draw_on_screen();
  }
}