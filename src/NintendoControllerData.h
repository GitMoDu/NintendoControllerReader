// NintendoControllerData.h

#ifndef _NINTENDO_CONTROLLER_DATA_h
#define _NINTENDO_CONTROLLER_DATA_h

namespace Nintendo64
{
	struct Data_t
	{
		uint16_t Buttons;
		int8_t JoystickX;
		int8_t JoystickY;
	};

	enum Buttons
	{
		Right = 0,
		Left = 1,
		Down = 2,
		Up = 3,
		Start = 4,
		Z = 5,
		B = 6,
		A = 7,
		CRight = 8,
		CLeft = 9,
		CDown = 10,
		CUp = 11,
		R = 12,
		L = 13,
		ControllerReset = 15
	};
}

namespace GameCube
{
	struct Data_t
	{
		uint16_t Buttons;
		int8_t JoystickX;
		int8_t JoystickY;
		int8_t JoystickCX;
		int8_t JoystickCY;
		uint8_t SliderLeft;
		uint8_t SliderRight;
	};

	//TODO: Check values.
	enum Buttons
	{

		Right = 0,
		Left = 1,
		Down = 2,
		Up = 3,
		Start = 4,
		Z = 5,
		B = 6,
		A = 7,
		X = 8,
		Y = 9,
		R = 12,
		L = 13,
	};
}

#endif