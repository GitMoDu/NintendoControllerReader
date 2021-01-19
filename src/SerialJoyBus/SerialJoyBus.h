//	SerialJoyBus.h
//	(Ab)uses Serial Port to emulate the OneWire-like protocol of JoyBus.
//
//	JoyBus data is LSB transmitted at a 250KHz baud rate.
//	'1' bit represented by 1uS low, 3uS high.
//	'0' bit represented by 3uS low, 1uS high.
//	Console Stop bit as 1uS low, 2uS high.
//
//	In order to wire this up, we need to tie the UART's Rx and Tx pins together
//		using a Schottky diode, as shown here (https://github.com/dword1511/onewire-over-uart).
//
//	Hardware baudrate is set for a factor of the JoyBus baudrate.
//	The factor is the number of serial bytes required for a full byte + 1 for stop bit.
//
//	"This results in a timing instead of 1uS and 3uS, we get 0.8uS and 3.2uS.
//	However, this seems to still work just fine."
//
//	The controller expects a stop bit as 1uS low, 2uS high,
//	so we send a purposefully long low, to delay the response until the end of our stop code.
//	This way we don't lose the first 2 bits of the response and just eat the stop code as another echo.
//	
//	Echo bytes are the transmitted bit codes and are discarded based on the transmit size.
//
//	Any Arduino compatible MCU should work with this library, 
//	Hardware required/used is a serial port capable of 1250000 bps baud-rate.
//	Interrupts aren't blocked nor used.


#ifndef _SERIALJOYBUS_h
#define _SERIALJOYBUS_h

#include <HardwareSerial.h>
#include <stdint.h>


template<const uint8_t MaxResponseSize>
class SerialJoyBus
{
private:
	//	JoyBus protocol baud-rate.
	static const uint32_t RealBaudrate = 250000;

	// Each converted byte gives 2 bits.
	static const uint8_t ByteBits = 2;

	// We need 4 bytes of codes to fill a byte.
	static const uint8_t BytesPerByte = 8 / ByteBits;

	// The real hardware baud-rate of 1250000 bps.
	static const uint32_t ConverterBaudrate = RealBaudrate * (BytesPerByte + 1);

	// Data codes converted to serial values.
	enum BitPair
	{
		Code00 = 0x08,
		Code01 = 0xE8,
		Code10 = 0x0F,
		Code11 = 0xEF
	};

	// Stop bit codes converted to serial values.
	enum BitCode
	{
		CodeMasterStop = 0x00,
		CodeSlaveStop = 0xFE,
		// Can happen in slight serial mis-timing. 
		CodeSlaveStopAlternate = 0xFC
	};

	// The incoming byte buffer.
	uint8_t ByteBuffer = 0;
	uint8_t ByteIndex = 0;

	// Transmited bytes expected to discard.
	uint8_t EchoBytes = 0;

	// The serial port used.
	//	In order to wire this up, we need to tie the UART's Rx and Tx pins together
	//	using a Schottky diode, as shown here (https://github.com/dword1511/onewire-over-uart).
	HardwareSerial* SerialInstance;

protected:
	// The buffered response, from the converted bytes from serial.
	uint8_t ResponseBuffer[MaxResponseSize];
	uint8_t ResponseBufferSize = 0;

public:
	SerialJoyBus(HardwareSerial* serial)
		: SerialInstance(serial)
	{}

	void Start()
	{
		SerialInstance->begin(ConverterBaudrate);
		SerialInstance->flush();
		SerialDiscard();
		BufferDiscard();
	}

	void Stop()
	{
		BufferDiscard();
		SerialDiscard();
		SerialInstance->clearWriteError();
		SerialInstance->end();
	}

private:
	// Returns true on stop bit code detected.
	const bool ReadByte()
	{
		uint8_t bitPairCode = SerialInstance->read();

		// LSB reception.
		// Bits are inverted in order, 
		// as we build up a whole byte, from the top.
		switch (bitPairCode)
		{
		case BitPair::Code00:
			// ByteBuffer += 0b00 << ((BytesPerByte - ByteIndex) * ByteBits);
			ByteIndex++;
			break;
		case BitPair::Code01:
			ByteBuffer += 0b01 << ((BytesPerByte - ByteIndex - 1) * ByteBits);
			ByteIndex++;
			break;
		case BitPair::Code10:
			ByteBuffer += 0b10 << ((BytesPerByte - ByteIndex - 1) * ByteBits);
			ByteIndex++;
			break;
		case BitPair::Code11:
			ByteBuffer += 0b11 << ((BytesPerByte - ByteIndex - 1) * ByteBits);
			ByteIndex++;
			break;
		case BitCode::CodeSlaveStop:
			SerialDiscard();
			return true;
		case BitCode::CodeSlaveStopAlternate:
			SerialDiscard();
			return true;
		default:// Invalid code received, message is invalidated.
			BufferDiscard();
			SerialDiscard();
			break;
		}

		if (ByteIndex >= BytesPerByte)
		{	// Message byte end.
			ResponseBuffer[ResponseBufferSize++] = ByteBuffer;
			ByteBuffer = 0;
			ByteIndex = 0;
		}

		return false;
	}

protected:
	const bool IsReceiveBufferFull() {
		return ResponseBufferSize >= MaxResponseSize;
	}

	// Fills the response buffer until serial input is clear
	// or the receive buffer is full.
	// Returns true when a message is available.
	const bool GetResponseBuffer()
	{
		while (ResponseBufferSize <= MaxResponseSize &&
			SerialInstance->available())
		{
			// Discard echo bytes.
			if (EchoBytes > 0)
			{
				SerialInstance->read();
				EchoBytes--;
			}
			else if (ReadByte())
			{
				// Stop bit detected, message received.
				return ResponseBufferSize > 0;
			}
		}

		return false;
	}

	//void TransmitBuffer(uint8_t* buffer, const uint8_t length)
	//{
	//	//TODO:
	//}

	// Pushes 1 byte, 2 bits at a time to the serial buffer.
	void Transmit1Byte(const uint8_t value)
	{
		WriteByte(value);

		SerialInstance->write(BitCode::CodeMasterStop);

		// Expected echo bytes: 4 bit pairs and 1 stop code.
		EchoBytes = BytesPerByte + 1;
	}

	void BufferDiscard()
	{
		ResponseBufferSize = 0;
	}

	void SerialDiscard()
	{
		while (SerialInstance->available())
		{
			SerialInstance->read();
		}

		EchoBytes = 0;

		// Reset byte buffer.
		ByteBuffer = 0;
		ByteIndex = 0;
	}

private:
	void WriteByte(const uint8_t value)
	{
		// Send 8 bits, in 4 x 2 bits at a time.
		// Initiate write command by bringing the line down with the first bit pair.
		for (uint8_t i = 0; i < (BytesPerByte * ByteBits); i += ByteBits)
		{
			// LSB transmission.
			uint8_t bitPair;
			bitPair = ((value >> (6 - i)) & 0b1) + (((value >> (7 - i)) & 0b1) << 1);

			switch (bitPair)
			{
			case 0b00:
				SerialInstance->write(BitPair::Code00);
				break;
			case 0b01:
				SerialInstance->write(BitPair::Code01);
				break;
			case 0b10:
				SerialInstance->write(BitPair::Code10);
				break;
			case 0b11:
				SerialInstance->write(BitPair::Code11);
				break;
			default:
				return;
			}
		}
	}
};
#endif