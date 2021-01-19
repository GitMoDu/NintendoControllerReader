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

		void Reset()
		{
			Buttons = 0;
			JoystickX = 0;
			JoystickY = 0;
		}
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

		void Reset()
		{
			Buttons = 0;
			JoystickX = 0;
			JoystickY = 0;
			JoystickCX = 0;
			JoystickCY = 0;
			SliderLeft = 0;
			SliderRight = 0;
		}
	};

	//TODO: Check values.
	enum Buttons
	{
		A = 0,
		B = 1,
		X = 2,
		Y = 3,
		Start = 4,
		Left = 8,
		Right = 9,
		Down = 10,
		Up = 11,
		Z = 12,
		R = 13,
		L = 14
	};
}

#endif