#ifndef _NINTENDO_GAMECUBE_CONTROLLER_READER_STM32_H
#define _NINTENDO_GAMECUBE_CONTROLLER_READER_STM32_H

#include <NintendoControllerData.h>

#ifndef ARDUINO_ARCH_STM32F1
#error This library driver only supports STM32F1.
#else
#include "BitBangJoyBus\BitBangJoyBusSTM32.h"

class BitBangGCController : public BitBangJoyBusSTM32
{
public:
	GameCube::Data_t Data;

public:
	enum GamecubeButtons
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

public:
	BitBangGCController(const uint8_t pin) : BitBangJoyBusSTM32(pin)
	{
	}

	// Each update lasts around 400us.
	const bool Poll()
	{
		return ReadWithRumble(&Data, false);
	}

private:
	const bool ReadWithRumble(GameCube::Data_t* data, bool rumble)
	{
		bool success;

		nvic_globalirq_disable();
		sendBits(rumble ? 0b0100000000000011000000011l : 0b0100000000000011000000001l, 25);
		success = receiveBits(data, 64);
		nvic_globalirq_enable();

		if (success && (data->Buttons & 0x8000))
		{
			return true;
		}
		else
		{
			SendWakeUp();

			return false;
		}
	}

	void SendWakeUp()
	{
		nvic_globalirq_disable();
		sendBits(0b000000001l, 9);
		nvic_globalirq_enable();
	}
};
#endif
#endif