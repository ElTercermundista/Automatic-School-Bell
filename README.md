This automatic bell was designed to manage the timetables of a school, but it can be used for any other purpose. It was designed for updating the time every day using a NTP (Network Time Protocol) at a certain hour that you can change in the code depending on your needs (lines 220-230). Depending on the region that you are you may need to change the NTP server for better comunications speeds.

It was made with the following components:

-Arduino NANO

-ENC28J60 (for the ethernet conection)

-DS1302

-16X2 Display

-I2C (TL PCF8574AT)

-5V relee module (each having 2 relees)

Take into account the listed components, cause switching to other ones may cause trouble with the libraries and may not function.
