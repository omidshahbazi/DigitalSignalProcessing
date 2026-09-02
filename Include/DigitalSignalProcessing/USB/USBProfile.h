#pragma once
#ifndef USB_DEVICE_PROFILE_H
#define USB_DEVICE_PROFILE_H

#include "../Common.h"

// Maximum number of classes that can be registered in a profile.
static constexpr uint8 MaxClassCount = 4;

// Defines standard packet sizes used for USB endpoints.
enum class PacketSizes : uint8
{
	PacketSizes8 = 8,   // 8 bytes packet size.
	PacketSizes16 = 16, // 16 bytes packet size.
	PacketSizes32 = 32, // 32 bytes packet size.
	PacketSizes64 = 64, // 64 bytes packet size.

	Max = PacketSizes64 // Maximum allowed packet size.
};

// Configuration parameters for the CDC (Communication Device Class).
struct CDCClassConfig
{
public:
	PacketSizes ReceiveBufferSize; // Buffer size allocated for receiving data.
	PacketSizes SendBufferSize;    // Buffer size allocated for sending data.
};

// Configuration parameters for the AMC (Audio Multi-Channel Class).
struct AMCClassConfig
{
public:
	uint32 SupportedSampleRates[SAMPLE_RATE_COUNT]; // Array of supported sample rates (e.g., 44100, 48000).
	uint8 SupportedSampleRateCount;                 // Total number of supported sample rates.
	uint8 DefaultSampleRateIndex;                   // Array index for the default sample rate.

	uint8 SupportedBitDepths[BIT_DEPTH_COUNT];      // Array of supported bit depths (e.g., 16, 24).
	uint8 SupportedBitDepthCount;                   // Total number of supported bit depths.
	uint8 DefaultBitDepthIndex;                     // Array index for the default bit depth.

	uint8 InputChannelCount;                        // Number of input (capture) audio channels.
	uint8 OutputChannelCount;                       // Number of output (playback) audio channels.

	bool EnableHardwareVolumeControl;               // Indicates if hardware volume control is supported/enabled.
	bool EnableHardwareMute;                        // Indicates if hardware mute control is supported/enabled.
};

// Enumeration of supported USB device classes.
enum class USBDeviceClasses
{
	CDC = 0, // Communication Device Class (Serial Port).
	AMC,     // Audio Multi-Channel Class.
};

// Represents a node containing a specific USB class configuration.
struct USBClassNode
{
public:
	USBDeviceClasses Class; // The selected USB class type for this node.

	// Union to hold the specific configuration based on the class type.
	union
	{
		CDCClassConfig CDC; // CDC specific configuration.
		AMCClassConfig AMC; // AMC specific configuration.
	};
};

// Profile configuration for a USB Device.
struct USBDeviceProfile
{
public:
	uint16 VendorID;      // The Vendor ID (VID) of the USB device.
	uint16 ProductID;     // The Product ID (PID) of the USB device.
	uint16 Version;       // The device release number (bcdDevice).

	cstr Manufacturer;    // String representing the manufacturer's name.
	cstr Product;         // String representing the product's name.
	cstr SerialNumber;    // String representing the device's serial number.

	uint8 MaxPowerCurrent; // Maximum power consumption in mA (must be mapped properly in descriptors).

	bool IsSelfPowered;    // Indicates if the device is self-powered (true) or bus-powered (false).

	USBClassNode ClassNodes[MaxClassCount]; // Array of configured class nodes for this device.
	uint8 ClassNodeCount;                   // Total count of active class nodes.
};

// Enumeration of supported USB host classes.
enum class USBHostClasses
{
	MIDI // MIDI Host Class.
};

// Information regarding a device attached to the USB Host.
struct USBAttachedDeviceInfo
{
public:
	uint16 VendorID;       // Vendor ID of the attached device.
	uint16 ProductID;      // Product ID of the attached device.
	uint8 DeviceClass;     // Standard class code of the attached device.
	uint8 DeviceSubClass;  // Standard subclass code of the attached device.
	uint8 Protocol;        // Standard protocol code of the attached device.
	uint8 Speed;           // Operating speed of the attached device (e.g., Low, Full, High).
	uint8 MaxPacketSize0;  // Maximum packet size supported by endpoint 0.

	char Manufacturer[64]; // Parsed manufacturer string from the attached device.
	char Product[64];      // Parsed product string from the attached device.
	char SerialNumber[64]; // Parsed serial number string from the attached device.
};

// Profile configuration for a USB Host.
struct USBHostProfile
{
public:
	uint16 MaxSuppliedCurrent; // Maximum current the host is capable of supplying to devices (in mA).

	bool EnableVBUSPowerControl; // Indicates whether the host actively controls VBUS power.

	USBHostClasses SupportedClasses[MaxClassCount]; // Array of USB classes supported by this host.
	uint8 SupportedClassCount;                      // Total number of supported host classes.
};

// Enumeration of overall USB operational modes.
enum class USBModes
{
	Device = 0, // USB operates as a Peripheral Device.
	Host        // USB operates as a Host controller.
};

// The primary profile structure configuring the entire USB peripheral.
struct USBProfile
{
public:
	USBModes Mode; // The operational mode determining how the USB hardware behaves.

	// Union holding the corresponding profile based on the selected mode.
	union
	{
		USBDeviceProfile Device; // Configuration utilized when operating in Device mode.
		USBHostProfile Host;     // Configuration utilized when operating in Host mode.
	};
};

#endif