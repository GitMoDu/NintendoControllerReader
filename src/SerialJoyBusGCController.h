// SerialJoyBusGCController.h

#ifndef _SERIAL_JOYBUS_GAMECUBE_CONTROLLER_READER_h
#define _SERIAL_JOYBUS_GAMECUBE_CONTROLLER_READER_h

#include "SerialJoyBus\SerialJoyBus.h"
#include <NintendoControllerData.h>


// Controller poller-reader.
// TODO: not working.
class SerialJoyBusGCController : public SerialJoyBus<8>
{
private:
	enum CommandCode
	{
		NoCommandCode = 0xFF,
		//StatusCode = 0x10, //TODO:
		WakeUpCode = 0x00,
		PollCode = 0x01,

	};

	enum ResponseSize
	{
		WakeUpSize = 3, // Full transaction takes ~140 us.
		PollSize = 8, // Full transaction takes ~370 us.
		ResetSize = 1
	};

	static const uint32_t PollCodeNoRumble = 0b000000000000001101000000;
	static const uint32_t PollCodeRumble = 0b000000000000001101000001;

	CommandCode LastCommandSent = CommandCode::NoCommandCode;

	bool ControllerActive = false;

public:
	GameCube::Data_t Data;

public:
	SerialJoyBusGCController(HardwareSerial* serial) : SerialJoyBus<8>(serial), Data()
	{
	}

	// To be called once per target period.
	void Poll()
	{
		SerialDiscard();
		BufferDiscard();

		if (ControllerActive)
		{
			LastCommandSent = CommandCode::PollCode;
			Transmit3Bytes(PollCodeNoRumble);
		}
		else
		{
			// Send a wake-up code, if controller isn't responding to polling.
			LastCommandSent = CommandCode::WakeUpCode;
			Transmit1Byte(CommandCode::WakeUpCode);
		}
	}

	// Can be called after ~2 ms of poll, if low latency is desired;
	// Or just before a poll, for simplicity.
	// Will update controller values.
	const bool Read()
	{
		if (GetResponseBuffer())
		{
			// Validate for size based on expected response.
			switch (LastCommandSent)
			{
				//case CommandCode::StatusCode:
				//	if (ResponseBufferSize >= ResponseSize::StatusSize)
				//	{
				//		//TODO: Handle on status received.
				//	}
				//	break;
			case CommandCode::WakeUpCode:
				if (ResponseBufferSize >= ResponseSize::WakeUpSize)
				{
					//TODO: Handle on status received.
					ControllerActive = true;
				}
				else
				{
					ControllerActive = false;
				}
				break;
			case CommandCode::PollCode:
				if (ResponseBufferSize >= ResponseSize::PollSize &&
					(ResponseBuffer[1] & 0x80)) // Last bit of second byte should be true.
				{
					// Update controller values.
					// 2nd bit of second byte is undefined.
					Data.Buttons = ResponseBuffer[0] + ((ResponseBuffer[1] & 0x7F) << 8);
					Data.JoystickX = ResponseBuffer[2] - INT8_MAX;
					Data.JoystickY = ResponseBuffer[3] - INT8_MAX;
					Data.JoystickCX = ResponseBuffer[4] - INT8_MAX;
					Data.JoystickCY = ResponseBuffer[5] - INT8_MAX;
					Data.SliderLeft = ResponseBuffer[6];
					Data.SliderRight = ResponseBuffer[7];

					ControllerActive = true;
				}
				else
				{
					ControllerActive = false;
				}
				break;
			default:
				// Should never happen.
				ControllerActive = false;
				break;
			}
		}
		else if (LastCommandSent == CommandCode::PollCode)
		{
			// If we've on a failed poll, set device inactive (requires wake-up).
			ControllerActive = false;
		}

		if (!ControllerActive)
		{
			Data.Reset();
		}
		BufferDiscard();

		return ControllerActive && LastCommandSent && CommandCode::PollCode;

	}
};
#endif