#include <BitBangGCController.h>


const uint32_t SERIAL_BAUD_RATE = 115200;
const uint32_t CONTROLLER_PIN = PA0;

GameCube::Data_t RawData;
BitBangGCController BitBangGC(CONTROLLER_PIN);

void setup()
{
	Serial.begin(SERIAL_BAUD_RATE);
	while (!Serial)
		;
	delay(1000);
	Serial.println(F("GameCube Controller Tester"));
}

void loop()
{
	if (BitBangGC.Poll())
	{
		RawData.Buttons = BitBangGC.Data.Buttons;
		RawData.JoystickX = BitBangGC.Data.JoystickX;
		RawData.JoystickY = BitBangGC.Data.JoystickY;
		RawData.JoystickCX = BitBangGC.Data.JoystickCX;
		RawData.JoystickCY = BitBangGC.Data.JoystickCY;
		RawData.SliderLeft = BitBangGC.Data.SliderLeft;
		RawData.SliderRight = BitBangGC.Data.SliderRight;

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

		Serial.print(F("\Sliders: "));
		Serial.print(RawData.SliderLeft);
		Serial.print('\t');
		Serial.print(RawData.SliderRight);

		Serial.println();

		delay(200);
	}
	else
	{
		Serial.println(F("No controller found."));
		delay(1000);
	}
}
