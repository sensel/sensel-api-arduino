# Sensel API Arduino

The Sensel API Arduino allows users to communicate with Sensel devices from Arduino boards. 

## Getting Started

To get started with the Sensel API for Arduino, you will need to have a Sensel device, like a Sensel Morph, and a Hacker Cable which connects the Sensel device to your Arduino board serial communication.  

## Serial

The Sensel Hacker Cable should be connected to a Hardware Serial on the Arduino Board. While it is possible to use a Software Serial, the signal is unreliable at higher speeds so occaional frames will need to be dropped if improper data is sent.
