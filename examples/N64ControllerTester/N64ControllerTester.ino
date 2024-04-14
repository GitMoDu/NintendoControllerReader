#include <BitBangN64Controller.h>
#include <SerialJoyBusN64Controller.h>

const uint32_t SERIAL_BAUD_RATE = 115200;

Nintendo64::Data_t RawData;

#if defined(ARDUINO_ARCH_STM32F1)
HardwareSerial* SerialInstance = &Serial3;

const uint32_t CONTROLLER_PIN = D11;
BitBangN64Controller BitBangN64(CONTROLLER_PIN);
#else
HardwareSerial* SerialInstance = &Serial;
#endif

SerialJoyBusN64Controller JoyBusN64(SerialInstance);

void setup()
{
	Serial.begin(SERIAL_BAUD_RATE);
	while (!Serial)
		;
	delay(1000);

	// Serial Joy requires a start to initialize the serial hardware.
	JoyBusN64.Start();

	Serial.println(F("Nintendo 64 Controller Tester"));
}

const bool SerialJoyLoop()
{
	// Poll can be sent and response read later, asynchronously.
	JoyBusN64.Poll();
	delay(1); // Here we just use delay for example.
	if (JoyBusN64.Read())
	{
		RawData.Buttons = JoyBusN64.Data.Buttons;
		RawData.JoystickX = JoyBusN64.Data.JoystickX;
		RawData.JoystickY = JoyBusN64.Data.JoystickY;

		return true;
	}
	else
	{
		RawData.Buttons = 0;
		RawData.JoystickX = 0;
		RawData.JoystickY = 0;

		return false;
	}
}

#if defined(ARDUINO_ARCH_STM32F1)
const bool BitBangJoyLoop()
{
	if (BitBangN64.Poll())
	{
		RawData.Buttons = BitBangN64.Data.Buttons;
		RawData.JoystickX = BitBangN64.Data.JoystickX;
		RawData.JoystickY = BitBangN64.Data.JoystickY;

		return true;
	}
	else
	{
		RawData.Buttons = 0;
		RawData.JoystickX = 0;
		RawData.JoystickY = 0;


		return false;
	}
}
#endif

void loop()
{
	if (SerialJoyLoop())
	//if (BitBangJoyLoop())
	{
		Serial.print(F("Buttons: 0b"));
		for (uint8_t i = 0; i < sizeof(RawData.Buttons) * 8; i++)
		{
			Serial.print((RawData.Buttons >> i) & 0b1);
		}
		Serial.print(F("\tJoystick: "));
		Serial.print(RawData.JoystickX);
		Serial.print('\t');
		Serial.print(RawData.JoystickY);
		Serial.println();

		Serial.println();

		delay(10);
	}
	else
	{
		Serial.println(F("No controller found."));
		delay(1000);
	}
}