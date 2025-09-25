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
| integrated limit to prevent button spamming | Mains-powered and attached to wall or table  |
| Battery powered |   |

## initial design sketches
<img width="371" height="165" alt="image" src="https://github.com/user-attachments/assets/1afd8bcf-c198-4961-b2fb-2d4ad59fe6b7" />
This system consists of two esp32 communicating via ESP-NOW protocol. The sender module is a large dice with an IMU to measure orientation. The receiver has an LCD display and buzzer. By rotating the device, the user can send one of 6 encoded messages to the receiver, which provides audible/visual feedback. 
<img width="350" height="141" alt="image" src="https://github.com/user-attachments/assets/9ca60f25-3a74-49a1-93e3-03356d72e9d2" />
This system consists of two ESP32 communicating again via ESP-NOW. The difference being the sender uses buttons to send encoded messages instead. 

## Final design
We ended up with the second design owing to its simplicity, with some modifications. 


First, due to the THT buttons on the receievers malfunctioning after being soldered onto the protoboard, I took the creative liberty to design a HTML page with buttons for the staff to interact with the sender device. However it was found that the web server functionality was incompatible with the ESP-NOW protocol. I fixed this issue by utilising the ESP32 user datagram protocol (UDP). 

UDP (User Datagram Protocol) is like sending a postcard in the mail:
+You write your message and send it straight to the recipient.
+It’s quick and simple, but there’s no guarantee it arrives, arrives in order, or arrives at all.
+Unlike a phone call (which checks the line constantly), UDP just sends the message and moves on.

An advantage of UDP was that we also coincidentally overcame the ESP-NOW range limit. Now, the sender and reciever can be as far apart as we like, as long as they're both connected to the same network. 
