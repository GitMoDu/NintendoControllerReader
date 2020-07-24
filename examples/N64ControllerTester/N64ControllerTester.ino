#include <NintendoControllerReaderSTM32.h>

const uint32_t SERIAL_BAUD_RATE = 115200;
const uint32_t CONTROLLER_PIN = PA0;


N64Data_t RawData;
N64Controller Controller(CONTROLLER_PIN);

void setup()
{
	Serial.begin(SERIAL_BAUD_RATE);
	while (!Serial)
		;
	delay(1000);
	Serial.println(F("Nintendo 64 Controller Tester"));

	pinMode(PA1, OUTPUT);
}

bool connected = false;
void loop()
{
	if (Controller.Read(&RawData))
	{
		for (uint8_t i = 0; i < (sizeof(RawData.Buttons) * 8); i++)
		{
			Serial.print((RawData.Buttons & (1 << i)) >> i);
		}

		Serial.print('\t');
		Serial.print(RawData.JoystickX);
		Serial.print('\t');
		Serial.print(RawData.JoystickY);
		Serial.println();

		delay(200);
	}
	else
	{
		Serial.println(F("No controller found."));
		delay(1000);
	}
}
