// SerialJoyBusN64Controller.h

#ifndef _SERIAL_JOYBUS_NINTENDO_64_CONTROLLER_READER_h
#define _SERIAL_JOYBUS_NINTENDO_64_CONTROLLER_READER_h

#ifndef ARDUINO_ARCH_STM32F1
#error This library driver only supports STM32F1.
#else

#include "SerialJoyBus\SerialJoyBus.h"
#include <NintendoControllerData.h>


// Basic controller poller-reader, no support for controller paks.
class SerialJoyBusN64Controller : public SerialJoyBus<4>
{
private:
	enum class CommandCode : uint8_t
	{
		StatusCode = 0x00,
		PollCode = 0x01,
		ReadCode = 0x02,
		WriteCode = 0x03,
		ResetCode = 0xFF,
		NoCommandCode = 0xF0
	};

	enum class ResponseCode : uint8_t
	{
		ReadCode = (uint8_t)~(uint8_t)CommandCode::ReadCode,
		ResetCode = (uint8_t)~(uint8_t)CommandCode::ResetCode
	};

	enum class ResponseStatusCode : uint8_t
	{
		ErrorCRC = 0x04,
		NoPakDetected = 0x02,
		ControllerOrRumblePakDetected = 0x01,
	};

	enum class ResponseSize : uint8_t
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

	/// <summary>
	/// To be called once per target period.
	/// </summary>
	void Poll()
	{
		SerialDiscard();
		BufferDiscard();
		LastCommandSent = CommandCode::PollCode;
		Transmit1Byte((uint8_t)CommandCode::PollCode);
	}

	/// <summary>
	/// Can be called after ~1 ms of poll, if low latency is desired.
	/// Will update controller values and fire events.
	/// </summary>
	/// <returns>True when a response was found.</returns>
	const bool Read()
	{
		if (GetResponseBuffer())
		{
			// Validate for size based on expected response.
			switch (LastCommandSent)
			{
			case CommandCode::StatusCode:
				if (ResponseBufferSize >= (uint8_t)ResponseSize::StatusSize)
				{
					//TODO: Handle on status received.
				}
				break;
			case CommandCode::PollCode:
				if (ResponseBufferSize >= (uint8_t)ResponseSize::PollSize)
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
				if (ResponseBufferSize >= (uint8_t)ResponseSize::ReadSize)
				{
					// TODO: Handle on data read.
				}
				break;
			case CommandCode::WriteCode:
				if (ResponseBufferSize >= (uint8_t)ResponseSize::WriteSize)
				{
					// Validate Ok response.
					if (ResponseBuffer[0] == (uint8_t)ResponseCode::ReadCode)
					{
						//TODO: Handle on write ok.
					}
				}
				break;
			case CommandCode::ResetCode:
				if (ResponseBufferSize >= (uint8_t)ResponseSize::ResetSize)
				{
					// Check for Command response.
					if (ResponseBuffer[0] == (uint8_t)ResponseCode::ResetCode)
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

	/// <summary>
	/// Can be called after ~1 ms of poll, if low latency is desired.
	/// Might need to be called multiple times,
	/// before updating controller values and fire events.
	/// </summary>
	/// <returns>True when a response was found.</returns>
	const bool ReadAsync()
	{
		if (GetResponseBufferAsync())
		{
			// Validate for size based on expected response.
			switch (LastCommandSent)
			{
			case CommandCode::StatusCode:
				if (ResponseBufferSize >= (uint8_t)ResponseSize::StatusSize)
				{
					//TODO: Handle on status received.
				}
				break;
			case CommandCode::PollCode:
				if (ResponseBufferSize >= (uint8_t)ResponseSize::PollSize)
				{
					// Update controller values.
					// 7th bit of second byte is undefined.
					Data.Buttons = ResponseBuffer[0] + ((ResponseBuffer[1] & 0xBF) << 8);
					Data.JoystickX = ResponseBuffer[2];
					Data.JoystickY = ResponseBuffer[3];

					BufferDiscard();
					SerialDiscard();
					return true;
				}
				break;
			case CommandCode::ReadCode:
				if (ResponseBufferSize >= (uint8_t)ResponseSize::ReadSize)
				{
					// TODO: Handle on data read.
				}
				break;
			case CommandCode::WriteCode:
				if (ResponseBufferSize >= (uint8_t)ResponseSize::WriteSize)
				{
					// Validate Ok response.
					if (ResponseBuffer[0] == (uint8_t)ResponseCode::ReadCode)
					{
						//TODO: Handle on write ok.
					}
				}
				break;
			case CommandCode::ResetCode:
				if (ResponseBufferSize >= (uint8_t)ResponseSize::ResetSize)
				{
					// Check for Command response.
					if (ResponseBuffer[0] == (uint8_t)ResponseCode::ResetCode)
					{
						//TODO: Handle on reset command ok.
					}
				}
				break;
			default:
				// Should never happen.
				break;
			}

			BufferDiscard();
			SerialDiscard();
		}

		return false;
	}
};
#endif
#endif