#define dropButton 33
#define drinkButton 26
#define flatButton 23
#define careButton 17
#define reminderButton 18
#define cancelButton 14
#define buzzerPin 13

volatile unsigned long lastInterruptTime = 0;
const int cooldown = 1000; //Set maximum cooldown between transmissions
const int debouncetime = 20;

#include <WiFi.h>
#include <ESPmDNS.h>

//Change depending on wifi network name and password
const char* ssid = "Name";
const char* password =  "Password";
  
#include <WiFiUdp.h>
WiFiUDP udp;
char packetBuffer[255];
unsigned int localPort = 9999;
unsigned int serverport = 8888;
IPAddress serverIp;

bool expectReply = false;

void setup() {
  pinMode(buzzerPin, OUTPUT);
  pinMode(dropButton, INPUT_PULLUP);
  pinMode(drinkButton, INPUT_PULLUP);
  pinMode(flatButton, INPUT_PULLUP);
  pinMode(careButton, INPUT_PULLUP);
  pinMode(reminderButton, INPUT_PULLUP);
  pinMode(cancelButton, INPUT_PULLUP);

  Serial.begin(115200);
   
  WiFi.begin(ssid, password);
     
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
     
  while(!MDNS.begin("sender1")) {
    Serial.println("Starting mDNS...");
    delay(1000);
  }
 
  Serial.println("MDNS started");
 //following is necessary to extract the other esp's ip address
 
  while (serverIp.toString() == "0.0.0.0") {
    Serial.println("Resolving host...");
    delay(250);
    serverIp = MDNS.queryHost("commsys");
  }
 
  Serial.println("Host address resolved:");
  Serial.println(serverIp.toString());   
 //initiate udp
  udp.begin(localPort);
  Serial.printf("UDP Client : %s:%i \n", WiFi.localIP().toString().c_str(), localPort);
}
   
void transmit(char sendmessage[]){
  //for sending udp data
  udp.beginPacket(serverIp, serverport);
  //Serial.println("transmitted message");
  udp.printf(sendmessage);
  udp.endPacket();
}

void msgSend() {
  expectReply = true;
  lastInterruptTime = millis();
  tone(buzzerPin, 1000, 1000); 
}

void loop() {
  //for recieving udp data
  int packetSize = udp.parsePacket();
  if (packetSize) {
    Serial.print(" Received packet from : "); 
    Serial.println(udp.remoteIP());
    int len = udp.read(packetBuffer, 255);
    Serial.printf("Data : %s\n", packetBuffer);
    Serial.println();
  }

  unsigned long currentTime = millis();
  if ((currentTime - lastInterruptTime >= cooldown) && digitalRead(dropButton) == LOW){
    transmit("Dropped object");
    msgSend();
  }
  if ((currentTime - lastInterruptTime >= cooldown) && digitalRead(drinkButton) == LOW){
    transmit("Drink needed");
    msgSend();
  }
  if ((currentTime - lastInterruptTime >= cooldown) && digitalRead(flatButton) == LOW){
    transmit("Help returning to flat");
    msgSend();
  }
  if ((currentTime - lastInterruptTime >= cooldown) && digitalRead(careButton) == LOW){
    transmit("General care needed");
    msgSend();
  }
  if ((currentTime - lastInterruptTime >= cooldown) && digitalRead(reminderButton) == LOW){
    transmit("Event reminder");
    msgSend();
  }
  if ((currentTime - lastInterruptTime >= debouncetime) && digitalRead(cancelButton) == LOW){
    transmit("Sender cancelled");
    expectReply = false;
    lastInterruptTime = millis();
  }
  
  if (strcmp(packetBuffer, "All good") == 0 && expectReply == true) {
    expectReply = false;
    tone(buzzerPin, 2000);
    delay(1000);
    noTone(buzzerPin);
    Serial.println("All good (formally acknow) recieved");
    packetBuffer[0] = '\0';
  }
}
