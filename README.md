# Arduino-Group-Project

The IoT integrated smart vertical hydroponic system is designed to make farming easier and
accessible to anyone, including disabled individuals The system is comprised of multiple
components, including a lighting system, water pump, ultrasonic water level meter, humidity
sensor, fan system, OLED display, website, and Siri integration. Together, these elements foster
the best conditions for plant development while also enabling remote control and monitoring
through the website and Siri. This project aims to provide a hardware prototype for this implementation. I have added the code file for arduino in C++ and below, I have mentioned functional specifications for this project.


**Functional Specifications**

1. Mainboard with all the sensors, OLED display, WiFi module, and actuators.
2. Two auxiliary boards for relays controlling fans and pumps.
3. Environmental Parameter Monitoring: The system is able to monitor and control the
environmental parameters necessary for plant growth, such as temperature, humidity, and
water level. This functionality is incorporated using sensors and actuators that provide
precise control over the environmental parameters.
4. Water Management System: The system is able to manage water usage efficiently by
incorporating a water level meter using an ultrasonic sensor to ensure that the plants
receive the correct amount of water.
5. Lighting System: The system includes a lighting system that provides optimal lighting
conditions for plant growth.
6. Renewable Energy Integration: The system incorporates renewable energy sources such
as solar power to promote sustainable agriculture. This is achieved by the natural light it
uses.
7. Voice Control: The system incorporates voice control through Siri integration, making it
accessible to everyone, including those with disabilities.
8. User Interface: The system provides an easy-to-use interface for users to control the
functions of the system through a website interface.
9. Real-time Monitoring: The system provides real-time monitoring of the environmental
parameters and the plant growth progress using an OLED display and the website.
10. Remote Control: The system allows for remote control of the system through a website
interface, making it convenient and accessible to users.


**IoT and Accessibility**
1. C++ codes for reading sensor data and writing to the relays and other actuators.
2. Core database on ThingSpeak to store environmental data, log Arduino actions, and store
control values for a unified control path.
3. Web interface, hosted on w3school, to:
a. Display environment data and log of Arduino actions performed.
b. Manipulate the database directly and control the Arduino board indirectly.
4. C++ code (on Arduino) to send environment data and logging.
5. Siri integration (with Apple system application: Shortcut) to:
a. Provide an auditory output of environmental data to the users
b. Manipulate the database directly and control the Arduino board indirectly using
voice commands.
