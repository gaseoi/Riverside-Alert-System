#include <esp_now.h>
#include <WiFi.h>
#include <LiquidCrystal.h>

const int rs = 12, en = 14, d4 = 27, d5 = 26, d6 = 25, d7 = 33;
LiquidCrystal lcd (rs, en, d4, d5, d6, d7);

#define CHANNEL 1

uint8_t newData;

int LED = 13;

void setup() {
  Serial.begin(115200);
  
  WiFi.mode(WIFI_AP);
  WiFi.softAP("RX_1", "RX_1_Password", CHANNEL, 0);
  
  esp_now_init();
  esp_now_register_recv_cb(OnDataRecv);

  pinMode(LED, OUTPUT); 
  lcd.begin(16, 2);

}

void loop() {
  if (newData == 1){
    digitalWrite(LED, HIGH);
    Serial.println("Data recieved");
    delay(500);
    lcd.print("alarm pressed");
  }
  else{
    digitalWrite(LED, LOW);
    Serial.println("no data");
    delay(500);
    lcd.print("standby");
  }
}

/** callback when data is recv from Master **/
void OnDataRecv(const esp_now_recv_info_t *info, const uint8_t *data, int data_len) {
  Serial.print("I just received -> ");
  Serial.println(*data);
  memcpy(&newData, data, sizeof(newData));
}