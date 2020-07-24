#include <NintendoControllerReaderSTM32.h>


const uint32_t SERIAL_BAUD_RATE = 115200;
const uint32_t CONTROLLER_PIN = PA0;


GameCubeData_t RawData;
GameCubeController Controller(CONTROLLER_PIN);

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
	if (Controller.read(&RawData))
	{
		for (uint8_t i = 0; i < (sizeof(RawData.Buttons) * 8); i++)
		{
			Serial.print((RawData.Buttons & (1 << i)) >> i);
		}

		Serial.print('\t');
		Serial.print(RawData.JoystickX);
		Serial.print('\t');
		Serial.print(RawData.JoystickY);

		Serial.print('\t');
		Serial.print(RawData.JoystickCX);
		Serial.print('\t');
		Serial.print(RawData.JoystickCY);

		Serial.print('\t');
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
