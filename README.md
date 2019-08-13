# RC-Truck
ESP32 Controller Project

Project intent is to control basic RC Truck with advanced functionality. Disassembling vehicle to allow bread board mounting which will connect all paripheral devices. Ideally the truck will be able to operate in any conditions so long as adequite battery levels are provided. Below is a list of hardware. 

- Generic RC Truck
- Breadboard
- ESP32
- Ultrasonic Sensor HC-SR04
- White LED (X2)
- Red LED (X2)
- Photocell

After adding all necessary components in desired location we can focus on programming. Ideally a functional PID loop would control the steering of the vehicle although this may be impossible due to the factory provided motor which is discrete. All features have been tested and successful. Below is pseudo. Functions developed to handle all commands enhancing the readability and functionality.  

- Define IO and Addressing
- Setup 
  - Define serial
  - Define pins
- Main Loop
  - start motion
  - if distance > setpoint
      - turn wheels straight
      - brake lights off
      - forward motion
  - if distance < setpoint
      - brake lights on
      - stop
      - turn wheels right
      - reverse motion
      - clear object
      - turn wheels straight
      
JD
      
     
