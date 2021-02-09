## NintendoControllerReader
Library for reading Nintendo game controllers on Arduino boards, targeted at STM32F1/Maple Mini boards.

##

![](https://raw.githubusercontent.com/GitMoDu/NintendoControllerReader/master/media/GameCubeControllerTesting.gif)


## JoyBus Over Serial
Based on the work published by qwertymodo (http://www.qwertymodo.com/hardware-projects/n64/n64-controller), this alternate driver uses a serial port, a diode and a resistor to emulate the controllers' JoyBus protocol using a hacked serial port.

(Ab)uses Serial Port to emulate the OneWire-like protocol of JoyBus.

In order to wire this up, we need to tie the UART's Rx and Tx pins together using a Schottky diode, as shown here (https://github.com/dword1511/onewire-over-uart).
![](https://raw.githubusercontent.com/GitMoDu/NintendoControllerReader/master/media/pp2od_rd.png)

Any Arduino compatible MCU with fast enough serial should work with this library, Hardware required/used is a serial port capable of 1250000 bps baud-rate. That does not include the AVR line of Arduinos, sadly.

Interrupts aren't blocked or used.

## Bit Banged JoyBus
Low level bit bang protocol forked from GameControllersSTM32 (https://github.com/arpruss/GameControllersSTM32).

Stops interrupts for the duration of the protocol data exchange (~200us for N64, ~400us for GC).

Only used as a fallback solution where serial isn't available.


## Supported controllers:
  * Nintendo 64
  * GameCube


## Usage  
`SerialJoyN64Controller  Controller(&Serial3);` (JoyBusOverSerial Version)

`BitBangN64Controller Controller(CONTROLLER_PIN);` (BitBang Version)


`Controller.Poll();` (Request data poll)

`delay(1)` (MCU can do other things while the serial port fills up with the response)

`bool Controller.Read();` (Returns true on poll success, tries to reconnect controller on fail)

`Controller.Data //(N64Data_t);` (Raw controller data is public)
