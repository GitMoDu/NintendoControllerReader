#ifndef _BIT_BANG_NINTENDO_64_CONTROLLER_READER_H
#define _BIT_BANG_NINTENDO_64_CONTROLLER_READER_H

#include <NintendoControllerData.h>

#ifdef ARDUINO_ARCH_STM32F1
#include "BitBangJoyBus\BitBangJoyBusSTM32.h"

class BitBangN64Controller : public BitBangJoyBusSTM32
{
public:
	Nintendo64::Data_t Data;

public:
	BitBangN64Controller(const uint8_t pin) : BitBangJoyBusSTM32(pin)
	{
	}

	// Each update lasts around 200us.	
	const bool Poll()
	{
		bool success;

		nvic_globalirq_disable();
		sendBits(0b000000010l, 9);
		success = receiveBits(&Data, 32);
		nvic_globalirq_enable();

		if (success)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
};
#endif
#endif