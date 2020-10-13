# LIDAR Compass Laser Feedback Module

Hackaday Prize 2020 CalEarth

## Overview

See the main LIDAR Compass project repository at https://github.com/alexwhittemore/CalEarth-LIDAR-Compass

This project comprises the laser feedback module of the LIDAR compass. An Arduino Pro Micro module

1) measures the rotation of the LIDAR rotor by means of an infrared photodetector registering a white dot painted on the rotor, and
2) receives instructions from the main (Raspberry Pi) module about how to fire the feedback laser depending on this rotation.

## Operational Theory

The primary goal of the feedback module is to turn a visible laser on where the measured distance is "correct" per the loaded template, and turn it off where the distance is out of bounds.

This requires 1. measuring the rotation of the LIDAR rotor we're bouncing the feedback laser off of in order to constrain the measurement angle to the feedback angle, and 2. turning the feedback laser on and off at the right times. 

### Rotation Detection

A white dot is painted on the rotor, and an infrared range finder module has its sensitivity tuned to create a pulse whenever the white dot comes in view. The code herein measures the time delta between two successive pulses, i.e. the time elapsed during one full rotation. Dividing this time by 360 gives the duration of a single degree of rotation, and the ability to time out when that degree-slice starts.

### Laser firing order communication

The code times out "Degree 1 until 1*N microseconds," then "Degree 2 until 2*N microseconds" and so on, but must know for each degree whether to fire the laser or not. The Raspberry Pi handling the LIDAR transmits a sentence at the end of every calculation consisting of 45 bytes (45*8=360), where each bit is whether that degree of rotation is "correct" (distance matches expectation per the template). The code unpacks these 360 bits into an array it steps through each new degree of rotation of the LIDAR rotor.

## Current Status

The code herein, in combination with the code above on the Raspberry Pi, works as a functional proof of concept.

### Deficiencies

* Rotation timing isn't super consistent
* The time required to handle receiving serial data shows up as a glitch in the laser feedback
* Laser feedback has a substantial amount of jitter (probably due largely to the point above)
* Modulating the laser by cutting power to the driver itself seems to suffer a relatively large delay, causing erroneous feedback


## Next steps/opportunities for improvement

* The laser modulation circuit should be reworked to remove the time-constant impact of driver capacitance. Instead of cutting power to the laser module's driver circuit, the modulation signal should be fed in somewhere INSIDE that driver circuit.
* The current Arduino module runs at only 8MHz due to the 3.3V supply. An easy win would be switching to the 5V/16MHz version, since 3.3V was chosen for Rapberry Pi i/o compatibility, but the project currently uses a USB<>Serial device for communication anyway. But switching to a more modern, faster microcontroller would provide a lot more overhead for computation
* The rotation measurment scheme is currently interrupt-driven, but stability and accuracy could probably further be improved by rewriting the laser firing scheme to be interrupt-driven as well. 
* Stability would likely be improved by averaging the last N rotaiton durations, rather than relying on only the last duration for calculating degree offsets. 