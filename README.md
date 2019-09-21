## GameControllersSTM32  
GamecubeControllerReaderSTM32 is a fork of the GameControllersSTM32 library that allows reading some miscellaneous game controllers on STM32F1 boards.
Supported controllers include:
 
## Purpose  
Read data from:

 * GameCube


## Usage  
* `GameCubeController controller = GameCubeController(unsigned pin);`

* `bool controller.begin();` (Returns false on fail)

* `bool controller.read(GameControllerData_t* data);`  (Returns false on fail)