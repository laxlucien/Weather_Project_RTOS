# Weather_Project_CS452
 Final Project for CS 452 By: Dawson Burgess, Lucien Lee, Alphonse Crittenden
 ---
# Project Proposal:
#### The proposed Home Control System project offers an exciting opportunity to explore the practical application of RTOS in automating home functions. By integrating various sensors and actuators, the system aims to enhance user comfort, convenience, and energy efficiency while showcasing the versatility of real-time operating systems in IoT applications.

#### This will be completed with the use of FreeRTOS, to allow several tasks to be run at the same time to maximize the efficency of the ESP32 that we will be using for this project.
---
# Project Components:
1. Light Sensor: Utilizes a light sensor to detect ambient light levels. When it's dark outside, the system triggers LED lights to turn on, providing illumination.
2. GPS Module: Integrates a GPS module to obtain coordinates and relay them to a central hub(website). The hub connects to Wi-Fi to fetch weather information based on the location, enhancing user experience and providing real-time weather updates.
3. Pixels: Incorporates pixel LEDs to change color based on proximity to a set bedtime, providing visual cues for optimal sleep routines.
4. Temperature Sensor: Monitors apartment temperature levels, transmitting data to the central website for display and analysis.
5. Display Screen: Utilizes a display screen to present information gathered from various sensors, including light levels, weather forecasts, temperature, and system status.
6. Wi-Fi Connection Indicator: Indicates a valid Wi-Fi connection status on the display screen, ensuring seamless connectivity.
7. Button Controls: Implements buttons for user interaction:
- Button 1: Acts as a light switch, allowing users to manually control the LED lights.
- Button 2: Serves as a kill switch for the entire system, providing emergency shut-off functionality.
- Button 3: Allows users to cycle through different displays on the LED screen, providing flexibility in information presentation.
---
# Project Goals
- Design and implement a robust RTOS-based home control system.
- Integrate multiple sensors and actuators to automate home functions intelligently.
- Provide user-friendly interaction through display screens and button controls.
- Ensure reliability and efficiency in system operation, with a focus on real-time responsiveness.
---
# Deliverables    
- Enhance user comfort and convenience by adapting to environmental conditions and user preferences. Expected Deliverables
- Fully functional home control system prototype.
- Documentation including system architecture, hardware/software components, and user manual.
- Presentation outlining project development, challenges faced, and future improvements.
