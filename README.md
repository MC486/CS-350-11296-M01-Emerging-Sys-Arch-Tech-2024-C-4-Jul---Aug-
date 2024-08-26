# CS-350-11296-M01-Emerging-Sys-Arch-Tech-2024-C-4-Jul---Aug-

## Morse Code Project - `gpiointerrupt.c`

### Project Overview
This project implements a Morse code simulator on a Texas Instruments CC3220x LaunchPad. The system uses LEDs to represent Morse code signals, with one LED indicating a dot and another indicating a dash. The program allows users to toggle between two different Morse code messages (e.g., "SOS" and "OK") using a button on the board. The project showcases skills in embedded systems, specifically in utilizing GPIO, timers, and interrupts.

### What Did I Do Well?
I successfully implemented a state machine to control the timing and sequencing of the Morse code signals. The use of interrupts to handle button presses was effective and ensured that the system could switch between messages without missing a beat.

### Where Could I Improve?
The readability and maintainability of the code could be improved by breaking down some of the larger functions into smaller, more focused functions. Additionally, more robust error handling could be added to handle edge cases, such as button debouncing.

### Tools and Resources Added to Support Network
For this project, I made extensive use of the TI Code Composer Studio and the TI documentation for the CC3220x LaunchPad. These tools will be valuable resources for future embedded systems projects. Additionally, the experience gained in using GPIO, timers, and interrupts is a key addition to my skill set.

### Transferable Skills
The skills developed in this project, particularly in state machine design, use of hardware interrupts, and timer configuration, are highly transferable to other embedded systems projects. These skills are essential for creating responsive and efficient systems.

### How Did I Make the Project Maintainable, Readable, and Adaptable?
I made the project maintainable by organizing the code into clear sections, with each part of the system (GPIO setup, timer setup, state machine, etc.) clearly delineated. Comments were added throughout the code to explain the purpose of each section and critical lines. The code was kept adaptable by using variables and macros for key parameters, making it easy to adjust the timing or LED behavior without needing to rewrite significant portions of the code.

---

## Thermostat Project - `gpiointerrupt.c`

### Project Overview
This project involved creating a smart thermostat prototype using the Texas Instruments CC3220x LaunchPad. The system reads the room temperature from a TMP006 temperature sensor via I2C, allows the user to adjust the set temperature using buttons (SW2 to decrease, SW3 to increase), and controls an LED (D10) to indicate whether the heating system is on. The UART was used to simulate sending temperature data to a server. This project demonstrates the integration of multiple peripherals (GPIO, I2C, UART) in an embedded system.

### What Did I Do Well?
I effectively integrated multiple hardware peripherals and ensured that they all worked together seamlessly. The use of UART for debugging and simulating server communication was particularly well-implemented, providing clear and consistent output.

### Where Could I Improve?
One area for improvement would be the handling of button presses. In future projects, I would consider implementing software debouncing or using hardware features to ensure more reliable button press detection. Additionally, the overall code structure could be improved by modularizing the code further.

### Tools and Resources Added to Support Network
In addition to TI Code Composer Studio and TI documentation, I utilized online resources and datasheets for the TMP006 sensor to ensure correct implementation of the I2C communication. The knowledge gained in integrating I2C and UART peripherals is a valuable addition to my skill set.

### Transferable Skills
The skills in peripheral integration, especially I2C communication and UART setup, are highly transferable to other projects. Understanding how to work with sensors and communicate with external devices is essential for many embedded systems applications.

### How Did I Make the Project Maintainable, Readable, and Adaptable?
I made the project maintainable by clearly commenting on each section of the code, explaining the purpose of critical functions and variables. The project was kept adaptable by using well-defined macros for key parameters, allowing for easy adjustment of the set temperature or other settings without needing to rewrite large portions of the code. The structure was kept modular, with separate functions handling different peripherals, making it easier to update or expand the system in the future.
