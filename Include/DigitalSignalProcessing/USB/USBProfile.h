#pragma once
#ifndef USB_DEVICE_PROFILE_H
#define USB_DEVICE_PROFILE_H

#include "../Common.h"

static constexpr uint8 MaxClassCount = 4;

enum class PacketSizes : uint8
{
	PacketSizes8 = 8,
	PacketSizes16 = 16,
	PacketSizes32 = 32,
	PacketSizes64 = 64,

	Max = PacketSizes64
};

struct AMCClassConfig
{
public:
	uint32 SupportedSampleRates[SAMPLE_RATE_COUNT];
	uint8 SupportedSampleRateCount;
	uint8 DefaultSampleRateIndex;

	uint8 SupportedBitDepths[BIT_DEPTH_COUNT];
	uint8 SupportedBitDepthCount;
	uint8 DefaultBitDepthIndex;

	uint8 InputChannelCount;
	uint8 OutputChannelCount;

	bool EnableHardwareVolumeControl;
	bool EnableHardwareMute;

	//uint16 BufferSizeFrames;
};

struct CDCClassConfig
{
public:
	PacketSizes ReceiveBufferSize;
	PacketSizes SendBufferSize;
};

enum class USBDeviceClasses
{
	//Multi Channel Audio
	AMC = 0,

	//Serial Port
	CDC
};

struct USBClassNode
{
public:
	USBDeviceClasses Class;

	union
	{
		AMCClassConfig AMC;
		CDCClassConfig CDC;
	};
};

struct USBDeviceProfile
{
public:
	uint16 VendorID;
	uint16 ProductID;
	uint16 Version;

	cstr Manufacturer;
	cstr Product;
	cstr SerialNumber;

	uint8 MaxPowerCurrent;

	bool IsSelfPowered;

	USBClassNode ClassNodes[MaxClassCount];
	uint8 ClassNodeCount;
};

enum class USBHostClasses
{
	MIDI
};

struct USBAttachedDeviceInfo
{
public:
	uint16 VendorID;
	uint16 ProductID;
	uint8 DeviceClass;
	uint8 DeviceSubClass;
	uint8 Protocol;
	uint8 Speed;
	uint8 MaxPacketSize0;

	char Manufacturer[64];
	char Product[64];
	char SerialNumber[64];
};

struct USBHostProfile
{
public:
	uint16 MaxSuppliedCurrent;

	bool EnableVBUSPowerControl;

	USBHostClasses SupportedClasses[MaxClassCount];
	uint8 SupportedClassCount;

	void (*OnDeviceConnected)(const USBAttachedDeviceInfo& Info);
	void (*OnDeviceDisconnected)();
};

enum class USBModes
{
	Device = 0,
	Host
};

struct USBProfile
{
public:
	USBModes Mode;

	union
	{
		USBDeviceProfile Device;
		USBHostProfile Host;
	};
};

#endif