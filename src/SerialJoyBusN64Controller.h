// SerialJoyBusN64Controller.h

#ifndef _SERIAL_JOYBUS_NINTENDO_64_CONTROLLER_READER_h
#define _SERIAL_JOYBUS_NINTENDO_64_CONTROLLER_READER_h

#include "SerialJoyBus\SerialJoyBus.h"
#include <NintendoControllerData.h>


// Basic controller poller-reader, no support for controller paks.
class SerialJoyBusN64Controller : public SerialJoyBus<4>
{
private:
	enum CommandCode
	{
		StatusCode = 0x00,
		PollCode = 0x01,
		ReadCode = 0x02,
		WriteCode = 0x03,
		ResetCode = 0xFF,
		NoCommandCode = 0xF0
	};

	enum ResponseStatusCode
	{
		ErrorCRC = 0x04,
		NoPakDetected = 0x02,
		ControllerOrRumblePakDetected = 0x01,
	};

	enum ResponseSize
	{
		StatusSize = 3,
		PollSize = 4,
		ReadSize = 32,
		WriteSize = 1,
		ResetSize = 1
	};

	CommandCode LastCommandSent = CommandCode::NoCommandCode;

public:
	Nintendo64::Data_t Data;

public:
	SerialJoyBusN64Controller(HardwareSerial* serial)
		: SerialJoyBus<4>(serial), Data()
	{}

	// To be called once per target period.
	void Poll()
	{
		SerialDiscard();
		BufferDiscard();
		LastCommandSent = CommandCode::PollCode;
		Transmit1Byte(CommandCode::PollCode);
	}

	// Can be called after ~200 us of poll, if low latency is desired.
	// Will update controller values and fire events.
	// Returns true when a response was found.
	const bool Read()
	{
		if (GetResponseBuffer())
		{
			// Validate for size based on expected response.
			switch (LastCommandSent)
			{
			case CommandCode::StatusCode:
				if (ResponseBufferSize >= ResponseSize::StatusSize)
				{
					//TODO: Handle on status received.
				}
				break;
			case CommandCode::PollCode:
				if (ResponseBufferSize >= ResponseSize::PollSize)
				{
					// Update controller values.
					// 7th bit of second byte is undefined.
					Data.Buttons = ResponseBuffer[0] + ((ResponseBuffer[1] & 0xBF) << 8);
					Data.JoystickX = ResponseBuffer[2];
					Data.JoystickY = ResponseBuffer[3];

					BufferDiscard();
					return true;
				}
				break;
			case CommandCode::ReadCode:
				if (ResponseBufferSize >= ResponseSize::ReadSize)
				{
					// TODO: Handle on data read.
				}
				break;
			case CommandCode::WriteCode:
				if (ResponseBufferSize >= ResponseSize::WriteSize)
				{
					// Validate Ok response.
					if (ResponseBuffer[0] == ~ReadCode)
					{
						//TODO: Handle on write ok.
					}
				}
				break;
			case CommandCode::ResetCode:
				if (ResponseBufferSize >= ResponseSize::ResetSize)
				{
					// Check for Command response.
					if (ResponseBuffer[0] == ~ResetCode)
					{
						//TODO: Handle on reset command ok.
					}
				}
				break;
			default:
				// Should never happen.
				break;
			}
		}

		BufferDiscard();

		return false;
	}
};
#endif