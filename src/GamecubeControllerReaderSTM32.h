#ifndef _NINTENDO_GAMECUBE_CONTROLLER_READER_STM32_H
#define _NINTENDO_GAMECUBE_CONTROLLER_READER_STM32_H

#include <Support\dwt.h>           

#define INPUT_NOT_IMPLEMENTED ((unsigned)-1)


typedef struct {
	gpio_dev* device;
	uint32_t mask;
	uint32_t pinNumber;
} PortData;

typedef struct {
	uint16_t buttons;
	uint8_t joystickX;
	uint8_t joystickY;
	uint8_t cX;
	uint8_t cY;
	uint8_t shoulderLeft;
	uint8_t shoulderRight;
} GameCubeData_t;


class GameController {
protected:
	void setPortData(PortData* p, unsigned pin) {
		if (pin == INPUT_NOT_IMPLEMENTED) {
			p->device = NULL;
		}
		else {
			p->device = digitalPinToPort(pin);
			p->mask = digitalPinToBitMask(pin);
			p->pinNumber = PIN_MAP[pin].gpio_bit;
		}
	}
public:
	GameController()
	{}
};

class GameCubeController : public GameController
{
private:
	static const uint8_t maxFails = 4;
	static const uint32_t cyclesPerUS = (SystemCoreClock / 1000000ul);
	static const uint32_t microsPerTry = 50;
	static const uint32_t quarterBitSendingCycles = cyclesPerUS * 5 / 4;
	static const uint32_t bitReceiveCycles = cyclesPerUS * 4;
	static const uint32_t halfBitReceiveCycles = cyclesPerUS * 1;

private:
	PortData port;
	unsigned fails;
	GameCubeData_t gcData;

private:
	// at most 32 bits can be sent
	void sendBits(uint32_t data, uint8_t bits) {
		data <<= 32 - bits;
		DWT->CYCCNT = 0;
		uint32_t timerEnd = DWT->CYCCNT;
		do {
			gpio_write_bit(port.device, port.pinNumber, 0);
			if (0x80000000ul & data)
				timerEnd += quarterBitSendingCycles;
			else
				timerEnd += 3 * quarterBitSendingCycles;
			while (DWT->CYCCNT < timerEnd);
			gpio_write_bit(port.device, port.pinNumber, 1);
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
			if (!gpio_read_bit(port.device, port.pinNumber)) {

				DWT->CYCCNT = 0;
				while (DWT->CYCCNT < halfBitReceiveCycles - 2);
				if (gpio_read_bit(port.device, port.pinNumber)) {

					*data |= bitmap;
				}
				bitmap >>= 1;
				bits--;
				if (bitmap == 0) {
					data++;
					bitmap = 0x80;
					if (bits)
						* data = 0;
				}
				while (!gpio_read_bit(port.device, port.pinNumber) && --timeout);
				if (timeout == 0) {
					break;
				}
			}
		} while (--timeout && bits);

		return bits == 0;
	}

public:
	bool begin()
	{
		gpio_set_mode(port.device, port.pinNumber, GPIO_OUTPUT_OD); // set open drain output
		CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
		DWT->CTRL |= 1;
		fails = maxFails; // force update

		return true;
	}

	bool read(GameCubeData_t* data) {
		return readWithRumble(data, false);
	}

	bool readWithRumble(GameCubeData_t* data, bool rumble) {
		if (fails >= maxFails) {
			nvic_globalirq_disable();
			sendBits(0b000000001l, 9);
			nvic_globalirq_enable();
			delayMicroseconds(microsPerTry);
			fails = 0;
		}
		nvic_globalirq_disable();
		sendBits(rumble ? 0b0100000000000011000000011l : 0b0100000000000011000000001l, 25);
		bool success = receiveBits(&gcData, 64);
		nvic_globalirq_enable();
		if (success && 0 == (gcData.buttons & 0x80) && (gcData.buttons & 0x8000)) {
			*data = gcData;
			return true;
		}
		else {
			fails++;
			return false;
		}
	}
	GameCubeController(const unsigned pin) : GameController()
	{
		setPortData(&port, pin);
	}
};

#endif // _NINTENDO_CONTROLLER_H
