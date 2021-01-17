// BitBangJoyBusSTM32.h

#ifndef _NINTENDO_PROTOCOL_BIT_BANGER_STM32_h
#define _NINTENDO_PROTOCOL_BIT_BANGER_STM32_h

#ifdef ARDUINO_ARCH_STM32F1
#include "dwt.h"

class BitBangJoyBusSTM32
{
protected:
	static const uint32_t cyclesPerUS = (SystemCoreClock / 1000000ul);
	static const uint32_t quarterBitSendingCycles = cyclesPerUS * 5 / 4;
	static const uint32_t bitReceiveCycles = cyclesPerUS * 4;
	static const uint32_t halfBitReceiveCycles = cyclesPerUS * 1;

protected:
	gpio_dev* device = nullptr;
	uint8_t pinNumber = 0;

public:
	BitBangJoyBusSTM32(const uint8_t pin)
	{
		device = digitalPinToPort(pin);
		pinNumber = PIN_MAP[pin].gpio_bit;

		Setup();
	}

	void Setup()
	{
		gpio_set_mode(device, pinNumber, GPIO_OUTPUT_OD); // set open drain output
		CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
		DWT->CTRL |= 1;
	}

protected:
	// at most 32 bits can be sent
	void sendBits(uint32_t data, uint8_t bits) {
		data <<= 32 - bits;
		DWT->CYCCNT = 0;
		uint32_t timerEnd = DWT->CYCCNT;
		do {
			gpio_write_bit(device, pinNumber, 0);
			if (0x80000000ul & data)
				timerEnd += quarterBitSendingCycles;
			else
				timerEnd += 3 * quarterBitSendingCycles;
			while (DWT->CYCCNT < timerEnd);
			gpio_write_bit(device, pinNumber, 1);
			if (0x80000000ul & data)
				timerEnd += 3 * quarterBitSendingCycles;
			else
				timerEnd += quarterBitSendingCycles;
			data <<= 1;
			bits--;
			while (DWT->CYCCNT < timerEnd);
		} while (bits);
	}

	// bits must be greater than 0
	bool receiveBits(void* data0, uint32_t bits) {
		uint8_t* data = (uint8_t*)data0;

		uint32_t timeout = bitReceiveCycles * bits / 2 + 4;

		uint8_t bitmap = 0x80;

		*data = 0;
		do {
			if (!gpio_read_bit(device, pinNumber)) {

				DWT->CYCCNT = 0;
				while (DWT->CYCCNT < halfBitReceiveCycles - 2);
				if (gpio_read_bit(device, pinNumber)) {

					*data |= bitmap;
				}
				bitmap >>= 1;
				bits--;
				if (bitmap == 0) {
					data++;
					bitmap = 0x80;
					if (bits)
						*data = 0;
				}
				while (!gpio_read_bit(device, pinNumber) && --timeout);
				if (timeout == 0) {
					break;
				}
			}
		} while (--timeout && bits);

		return bits == 0;
	}
};
#endif
#endif