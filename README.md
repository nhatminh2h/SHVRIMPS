#SHVRIMPS: Synchronised High-Voltage aRbItrary Modular Power Supply

SHVRIMPS is a high-voltage generator and optical imaging setup, capable of
• Outputting arbitrary high-voltage waveforms with an amplitude of 15 kV and a rise time less than 10 ms for load resistances below 8 MΩ.
• Eight independent synchronous and asynchronous 16-bit output channels.
• Monitoring the output high voltages and currents at adjustable sampling rate. Maximum rate of 9875 samples per second.
• Recording video of samples at a rate of 120 fps at 640x480 pixels.
• Displaying live video feed, control panel, and monitoring data on a GUI, accessible over Ethernet.

![alt text](https://github.com/nhatminh2h/SHVRIMPS/blob/main/diagrams/Graphical%20Abstract.png)

It consists of three main components: 1) an imaging system based on a Raspberry Pi 4B with a camera, producing a live video stream 2) a DAQ board compatible with either Arduino Nano 33 BLE or ESP32 DevKitC, and c) a multi-channel arbitrary waveform HV supply.

The repository is structured as follows:
• demonstration: videos and matlab figures
• diagrams: block diagrams of the entire setup and individual components
• hardware: KiCAD files showing the PCB design files
• setup: setup files
• software: source files for the Pi and DAQ board
