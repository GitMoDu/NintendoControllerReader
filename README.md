## NintendoControllerReader
Library for reading Nintendo game controllers on Arduino boards, targeted at STM32F1/Maple Mini boards.

## Bit Banged JoyBus
Low level bit bang protocol forked from GameControllersSTM32 (https://github.com/arpruss/GameControllersSTM32).
Stops interrupts for the duration of the protocol data exchange (~200us for N64, ~400us for GC).

## JoyBus Over Serial
Based on the work published by qwertymodo (http://www.qwertymodo.com/hardware-projects/n64/n64-controller), this alternate driver uses a serial port, a diode and a resistor to emulate the controllers' JoyBus protocol using a hacked serial port.

(Ab)uses Serial Port to emulate the OneWire-like protocol of JoyBus.

In order to wire this up, we need to tie the UART's Rx and Tx pins together using a Schottky diode, as shown here (https://github.com/dword1511/onewire-over-uart).

Any Arduino compatible MCU should work with this library, Hardware required/used is a serial port capable of 1250000 bps baud-rate. 

Interrupts aren't blocked or used.


## Supported controllers include:
  * Nintendo 64
  * GameCube


## Usage  
`BitBangN64Controller Controller(CONTROLLER_PIN);` (BitBang Version)

`SerialJoyN64Controller  Controller(&Serial3);` (JoyBusOverSerial Version)

`Controller.Poll();` (Request data poll)
`bool Controller.Read();` (Returns true on poll success, false on fail and tries to reconnect controller)

`Controller.Data //(N64Data_t);` (Raw controller data is public)
