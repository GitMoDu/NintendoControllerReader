#ifndef _NINTENDO_GAMECUBE_CONTROLLER_READER_STM32_H
#define _NINTENDO_GAMECUBE_CONTROLLER_READER_STM32_H

#include <Support\dwt.h>

typedef struct
{
	uint16_t Buttons;
	int8_t JoystickX;
	int8_t JoystickY;
} N64Data_t;


typedef struct
{
	uint16_t Buttons;
	uint8_t JoystickX;
	uint8_t JoystickY;
	uint8_t JoystickCX;
	uint8_t JoystickCY;
	uint8_t SliderLeft;
	uint8_t SliderRight;
} GameCubeData_t;

class NintendoProtocolController
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
	NintendoProtocolController(const uint8_t pin)
	{
		device = digitalPinToPort(pin);
		pinNumber = PIN_MAP[pin].gpio_bit;

		Setup();
	}

	void Setup()
	{
		gpio_set_mode(device, pinNumber, GPIO_OUTPUT_OD); // set open drain output
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

class N64Controller : public NintendoProtocolController
{
public:
	enum N64Buttons : uint8_t
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
		L = 13
	};

public:
	N64Controller(const uint8_t pin) : NintendoProtocolController(pin)
	{
	}

	bool Read(N64Data_t* data)
	{
		bool success;

		nvic_globalirq_disable();
		sendBits(0b000000010l, 9);
		success = receiveBits(data, 32);
		nvic_globalirq_enable();

		if (success)
		{
			return true;
		}
		else
		{
			SendWakeUp();

			return false;
		}
	}

private:
	void SendWakeUp()
	{
		nvic_globalirq_disable();
		sendBits(0b00000000l, 8);
		nvic_globalirq_enable();
	}
};

class GameCubeController : public NintendoProtocolController
{
private:
	GameCubeData_t gcData;

public:
	enum GamecubeButtons : uint8_t
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
	GameCubeController(const uint8_t pin) : NintendoProtocolController(pin)
	{
	}

	bool Read(GameCubeData_t* data) {
		return ReadWithRumble(data, false);
	}

	bool ReadWithRumble(GameCubeData_t* data, bool rumble) {
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

private:
	void SendWakeUp()
	{
		nvic_globalirq_disable();
		sendBits(0b000000001l, 9);
		nvic_globalirq_enable();
	}
};
#endif