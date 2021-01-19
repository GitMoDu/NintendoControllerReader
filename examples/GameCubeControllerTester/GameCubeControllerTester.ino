#include <BitBangGCController.h>
#include <SerialJoyBusGCController.h>


const uint32_t SERIAL_BAUD_RATE = 115200;
const uint32_t CONTROLLER_PIN = PA0;

GameCube::Data_t RawData;
BitBangGCController BitBangGC(CONTROLLER_PIN);

HardwareSerial* SerialInstance = &Serial3;

SerialJoyBusGCController JoyBusGC(SerialInstance);

void setup()
{
	Serial.begin(SERIAL_BAUD_RATE);
	while (!Serial)
		;
	delay(1000);

	// Serial Joy requires a start to initialize the serial hardware.
	JoyBusGC.Start();

	Serial.println(F("GameCube Controller Tester"));
}

const bool SerialJoyLoop()
{
	// Poll can be sent and response read later, asynchronously.
	JoyBusGC.Poll();
	delay(1); // Here we just use delay for example.
	if (JoyBusGC.Read())
	{
		RawData.Buttons = JoyBusGC.Data.Buttons;
		RawData.JoystickX = JoyBusGC.Data.JoystickX;
		RawData.JoystickY = JoyBusGC.Data.JoystickY;
		RawData.JoystickCX = JoyBusGC.Data.JoystickCX;
		RawData.JoystickCY = JoyBusGC.Data.JoystickCY;
		RawData.SliderLeft = JoyBusGC.Data.SliderLeft;
		RawData.SliderRight = JoyBusGC.Data.SliderRight;

		return true;
	}
	else
	{
		return false;
	}
}

const bool BitBangJoyLoop()
{
	if (BitBangGC.Poll())
	{
		RawData.Buttons = JoyBusGC.Data.Buttons;
		RawData.JoystickX = JoyBusGC.Data.JoystickX;
		RawData.JoystickY = JoyBusGC.Data.JoystickY;
		RawData.JoystickCX = JoyBusGC.Data.JoystickCX;
		RawData.JoystickCY = JoyBusGC.Data.JoystickCY;
		RawData.SliderLeft = JoyBusGC.Data.SliderLeft;
		RawData.SliderRight = JoyBusGC.Data.SliderRight;

		return true;
	}
	else
	{
		return false;
	}
}

void loop()
{
	if (SerialJoyLoop())
		//if (BitBangJoyLoop())
	{
		for (uint8_t i = 0; i < sizeof(RawData.Buttons) * 8; i++)
		{
			Serial.print((RawData.Buttons >> i) & 0b1);
		}

		Serial.print(F("\tJoystick: "));
		Serial.print(RawData.JoystickX);
		Serial.print('\t');
		Serial.print(RawData.JoystickY);

		Serial.print(F("\tC-stick: "));
		Serial.print(RawData.JoystickCX);
		Serial.print('\t');
		Serial.print(RawData.JoystickCY);

		Serial.print(F("\tSliders: "));
		Serial.print(RawData.SliderLeft);
		Serial.print('\t');
		Serial.print(RawData.SliderRight);

		Serial.println();

		delay(20);
	}
	else
	{
		Serial.println(F("No controller found."));
		delay(1000);
	}
}