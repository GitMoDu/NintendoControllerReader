## NintendoControllerReader
Library for reading Nintendo game controllers on Arduino boards, targeted at STM32F1/Maple Mini boards.

Low level bit bang protocol forked from GameControllersSTM32 (https://github.com/arpruss/GameControllersSTM32).

## Supported controllers include:
  * Nintendo 64
  * GameCube


## Usage  
* `N64Controller Controller(CONTROLLER_PIN);`

* `bool Controller.Read(N64Data_t* data);`  (Returns false on fail and tries to reconnect controller)
