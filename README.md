# Riverside-Alert-System
## Overview
This project was developed as part of Project Impactive 2024/5. Our goal was to create a wireless alert system which met the needs of the sender (residents) and reeiver (staff). We took an iterative design process involving all end users.

During the preliminary user research phase, we interviewed residents and staff at a specially designed retirement housing complex for seniors. Residents each have their own flats with access to caterers and shared spaces. Residents wish to call the staff at reception from their communal living spaces for various reasons. Their existing medical alert system is a wearable pendant, while proving useful for medical emergencies, tend to be misused by residents for non-emergencies, which can divert attention from residents in serious situations and is very distracting for staff.

From the inteviews, it became clear that the reasons for using the call button can be summarised in 6 broad categories:
+ Residents have dropped something and needs help picking it up.
+ Resident needs beverages or food.
+ Resident wishes to return to their flat.
+ Resident needs reminders of events.
+ Resident needs general care (e.g. hygiene) / other reasons.

Table of user requirements:
| Caller end (Residents)  | Receiver end (Staff) |
| ------------- | ------------- |
| Must have easy to push buttons with clear labels  | Two-way feedback system  |
| Audio feedback after button has been pushed | Indication of sender position  |
| Integrated limit to prevent button spamming | Mains-powered and attached to wall or table  |
| Battery powered |   |

## initial design sketches
<img width="371" height="165" alt="image" src="https://github.com/user-attachments/assets/1afd8bcf-c198-4961-b2fb-2d4ad59fe6b7" />

This system consists of two esp32 communicating via ESP-NOW protocol. The sender module is a large dice with an IMU to measure orientation. The receiver has an LCD display and buzzer. By rotating the device, the user can send one of 6 encoded messages to the receiver, which provides audible/visual feedback. 

<img width="350" height="141" alt="image" src="https://github.com/user-attachments/assets/9ca60f25-3a74-49a1-93e3-03356d72e9d2" />

This system consists of two ESP32 communicating again via ESP-NOW. The difference being the sender uses buttons to send encoded messages instead. 

## Final design
We ended up with the second design owing to its simplicity, with some modifications. 

![IMG_8196](https://github.com/user-attachments/assets/166b256c-5f92-4234-be04-39f587164614)

![IMG_8197](https://github.com/user-attachments/assets/c69aa577-7ecb-45ad-bcdb-91c3738cefe3)

![IMG_8198](https://github.com/user-attachments/assets/46477b55-d063-41dc-ad41-5badd26bf711)


First, due to the THT buttons on the receievers malfunctioning after soldering, I took the creative liberty to design a HTML page with buttons for the staff to interact with the sender device. However it was found that the web server functionality was incompatible with the ESP-NOW protocol. I fixed this issue by utilising the ESP32 user datagram protocol (UDP). 

UDP (User Datagram Protocol) is like sending a postcard in the mail. You write your message and send it straight to the recipient. It’s quick and simple, but there’s no guarantee it arrives, arrives in order, or arrives at all, unlike TCP (transmission control protocol), UDP just sends the message and moves on.

An advantage of UDP was that we also coincidentally overcame the ESP-NOW range limit. Now, the sender and reciever can be as far apart as we like, as long as they're both connected to the same network. 

I also used mDNS (multicast DNS) to make it so that the sender and receiver do not require each other's IP addresses when communicating via UDP. With mDNS, you can reach your ESP32’s web server using a simple name like “somethingsomething.local” instead of the jumble of numbers that makes up its IP address. If the ESP32’s IP changes, the hostname still works without any extra setup. The hostname of the sender and receiever can be changed to whatever you wish, however any hostname changes must be the reflected in the partner device's code. 

## Installation notes
+ Upload the sender and reciever codes onto seperate esp32 with the Arduino IDE. Be sure to modify the preprocessor directive space where it reads
```
const char* ssid = "Name";
const char* password =  "Password";
```
Ensure the bold and italic characters are replaced with your network's credentials.
You can also manually change the cooldown on the sender code to change the allowed interval between messages. 
```
const int cooldown = 1000; //Set maximum cooldown between transmissions
```
I set it to 1 second in the code provided for testing purposes. Feel free to extend it.
I also set the reciever web page interface address as "commsys.local". This can again be changed in the receiever code, but be sure to make the same name change in the sender code. For example the following will create a web page at Whateveryouwant.local

```
  while(!MDNS.begin("Whateveryouwant")) {
    Serial.println("Starting mDNS...");
    delay(1000);
  }
```
+ Solder all the peripherals according to the pins defined within the code. For the LCD display connect D22 to SCL, and D21 to SDA.
+ I'm powering the sender using a [18650 battery shield](amazon.co.uk/AZDelivery-Lithium-Li-ion-Battery-Expansion/dp/B086W7326Q/) (Not the exact link, I'm using the 2 battery version). The receiever is powered using a type C to DIP adaptor and a wall AC/DC adaptor providing 5V.
+ 3D print the casings for both devices. Attach the battery shield to the sender using heat set inserts and M3 screws. The sender uses a snap mechanism to click together after everything is installed. The reciever uses M3 screws for the LCD display and buzzer, and M2 screws for the perfboard. Heat set inserts are used to install the buzzer. 

## Use instructions
1. Staff should connect to the web page listed in the LCD (commsys.local by default) using any web browser. Staff must be connected to the same network as the communication device to interact with it.
2. When the resident presses one of the buttons sender, both sender and reciever devices should play a note, prompting the staff to connect and refresh the web page and providing feedback to the resident that the device is indeed working as intended.
3. Web page will display the button pressed by the resident, and prompt the staff to respond by allowing them to press the respond button.
4. Pressing the respond button will cause the receiever device to play a different note, allowing staff to respond. Refreshing the web page now should show all is well.
5. The sender has a red cancel button to cancel any requests. 

## Final notes
+ Ensure you have installed all the dependent libraries. Wifi.h and ESPmDNS.h should be part of the ESP32 add on. [Here is a helpful tutorial if you get stuck.](https://randomnerdtutorials.com/installing-the-esp32-board-in-arduino-ide-windows-instructions/)
+ The battery shield allows sender device operation with/without the 18650 batteries as long as its plugged in. Please handle lipo batteries with care.

## Acknowledgements
Special thanks to Project Impactive organisers, especially Rozenn Raffaut for assistance with casing 3D printing.
I am also very grateful to the care home staff and residents for their patience with our questions. It has been a pleasure collaborating with you.
