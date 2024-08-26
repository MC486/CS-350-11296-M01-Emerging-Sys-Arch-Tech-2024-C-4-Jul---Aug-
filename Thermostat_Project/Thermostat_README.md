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
