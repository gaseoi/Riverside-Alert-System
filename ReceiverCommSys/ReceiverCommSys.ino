#include <WiFi.h>
#include <ESPmDNS.h>
#include <LiquidCrystal_I2C.h>
 
#define BUZZER 18
int lcdColumns = 16;
int lcdRows = 2;
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows); 

// Replace depending on the name and password of network
const char* ssid = "name";
const char* password =  "password";

const String messages[] = {"Dropped object","Drink needed","Help returning to flat","General care needed","Event reminder"};

//needed for UDP 
#include <WiFiUdp.h>
WiFiUDP udp;
char packetBuffer[255];
unsigned int localPort = 8888;
unsigned int serverport = 9999;
IPAddress serverIp;

//needed for web page generation
unsigned long currentTime = millis();
unsigned long previousTime = 0; 
const long timeoutTime = 2000;
WiFiServer server(80);
String header;

bool pinged = false;
bool respondbuttongen = false;

void setup() {
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();
  WiFi.begin(ssid, password);
     
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
 
  while(!MDNS.begin("commsys")) {
    Serial.println("Starting mDNS...");
    delay(1000);
  }

  lcd.setCursor(0,0);
  lcd.print("commsys.local");

  while (serverIp.toString() == "0.0.0.0") {
    Serial.println("Resolving host...");
    delay(250);
    serverIp = MDNS.queryHost("sender1");
  }
  
  lcd.setCursor(0, 1);
  lcd.print("Waiting4sender");

  Serial.println("Host address resolved:");
  Serial.println(serverIp.toString());   
  udp.begin(localPort);
  Serial.printf("UDP Client : %s:%i \n", WiFi.localIP().toString().c_str(), localPort);

 //initialise web server
  server.begin();

  lcd.setCursor(0, 1);
  lcd.print("                ");

  lcd.setCursor(0, 1);
  lcd.print("Active");
}

void transmit(char sendmessage[]){
  //for sending udp data
  udp.beginPacket(serverIp, serverport);
  Serial.println("transmitted message");
  udp.printf(sendmessage);
  udp.endPacket();
}

void loop() {
  int packetSize = udp.parsePacket();
  if (packetSize) {
    memset(packetBuffer, 0, sizeof(packetBuffer));
    Serial.print(" Received packet from : "); 
    Serial.println(udp.remoteIP());
    int len = udp.read(packetBuffer, 255);
    Serial.printf("Data : %s\n", packetBuffer);
    Serial.println();
  }

  for (int i = 0; i < 5; i++){
    if (strcmp(packetBuffer, messages[i].c_str()) == 0) {
      strcat(packetBuffer, "!");
      pinged = false;
      if (pinged == false){
        pinged = true;
        tone(BUZZER, 1000, 500);
        lcd.setCursor(0, 1);
        lcd.print("                ");
        lcd.setCursor(0,1);
        lcd.print("Help needed");
        Serial.println("LCD PRINT help needed"); 
        respondbuttongen = true;
      }
    }
  }

  if (strcmp(packetBuffer, "Sender cancelled") == 0){
    strcat(packetBuffer, "!");
    pinged = false;
    if (pinged = true){
      tone(BUZZER, 1000, 500);
      lcd.setCursor(0, 1);
      lcd.print("                ");
      lcd.setCursor(0,1);
      lcd.print("Sender cancelled");
      respondbuttongen = false;
    }
  }

  WiFiClient client = server.available();   
  if (client){
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New client");
    String currentLine = ""; //currentLine holds data from client
    while (client.connected() && currentTime - previousTime <= timeoutTime){
      currentTime = millis();
      if (client.available()){
        char c = client.read();
        Serial.write(c);
        header += c;
        if (c == '\n'){
          if (currentLine.length() == 0){
          client.println("HTTP/1.1 200 OK");
          client.println("Content-type:text/html");
          client.println("Connection: close");
          client.println();

          if (header.indexOf("GET /button1press/") >= 0){
            respondbuttongen = false;
            transmit("All good");
            Serial.println("All good (formally acknow) sent");
            lcd.setCursor(0, 1);
            lcd.print("                ");
            lcd.setCursor(0,1);
            lcd.print("Responded");
            Serial.println("LCD PRINT RESPONDED");
            strcpy(packetBuffer, "All good");
          }

          client.println("<!DOCTYPE html><html>");
          client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
          client.println("<link rel=\"icon\" href=\"data:,\">");
          client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
          client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
          client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
          client.println(".button2 {background-color: #cad9d6;}</style></head>");

          client.println("<body><h1>Devices:</h1>");

          client.print("<p>Device 1 - State: ");
          client.print(packetBuffer);
          client.println("</p>");

          if (packetBuffer[0] == '\0') {
            client.println("<p>No data yet.</p>");
          }
          
          if (strcmp(packetBuffer, "All good") == 0){
            client.println("<p>No action required.</p>");
          }

          if (strcmp(packetBuffer, "Sender cancelled") == 0){
            client.println("<p>Message cancelled.</p>");
          }

          if (respondbuttongen == true) {
            client.println("<p><a href=\"/button1press/\"><button class=\"button\">Respond</button></a></p>");
          }

          client.println("<p><a href=\"/refresh/\"><button class=\"button2\" onclick='location.reload()'>Refresh page</button></a></p>");

          client.println("</body></html>");
          client.println();
          break;
          }
          else { //if there is newline, clear currentline
            currentLine = "";
          }
        }
        else if (c != '\r'){ //if anything other than carriage return, add to the end of currentline
          currentLine += c;
        }
      }
    }
    header = "";
    client.stop();
    Serial.println("Client disconnected");
    Serial.println("");
  }
}
