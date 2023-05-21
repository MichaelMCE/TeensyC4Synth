


#ifndef _USBH_COMMON_H_
#define _USBH_COMMON_H_


#define USBH_ENUMSTATE_END						15

#define USBD_MSG_NOTREADY						1
#define USBD_MSG_BUFFEREXHUSTED					2
#define USBD_MSG_WRITEREADY						3
#define USBD_MSG_WRITEEND						4
#define USBD_MSG_DEVICEREADY					5


#define USBH_DESCRIPTORTYPE_DEVICE				0x01
#define USBH_DESCRIPTORTYPE_CONFIG				0x02
#define USBH_DESCRIPTORTYPE_STRING				0x03
#define USBH_DESCRIPTORTYPE_INTERFACE			0x04
#define USBH_DESCRIPTORTYPE_ENDPOINT			0x05
#define USBH_DESCRIPTORTYPE_HID					0x21
#define USBH_DESCRIPTORTYPE_REPORT				0x22
#define USBH_DESCRIPTORTYPE_PHYSICAL			0x23
#define USBH_DESCRIPTORTYPE_HUB					0x29



#define USBH_DEVICECLASS_UNSPECIFIED			0x00	// Device class is unspecified, use class information in the Interface Descriptors
#define USBH_DEVICECLASS_AUDIO					0x01	// speaker, microphone, sound card, MIDI
#define USBH_DEVICECLASS_CDC					0x02	// modem, Ethernet adapter, Wi-Fi adapter, RS232 serial adapter. Used together with class 0Ah (below)
#define USBH_DEVICECLASS_HID					0x03	// keyboard, mouse, joystick, touch device, etc..
//#define USBH_DEVICECLASS_						0x04
#define USBH_DEVICECLASS_PID					0x05	// Physical Interface Device, eg; Force feedback joystick
#define USBH_DEVICECLASS_IMAGE					0x06	// PTP/MTP, webcam, scanner
#define USBH_DEVICECLASS_PRINTER				0x07
#define USBH_DEVICECLASS_MASSSTORAGE			0x08	// MSC or UMS. USB flash drive, memory card reader, digital audio player, digital camera, external drive
#define USBH_DEVICECLASS_USBHUB					0x09
#define USBH_DEVICECLASS_CDCDATA				0x0A
#define USBH_DEVICECLASS_VIDEO					0x0E	// webcam
#define USBH_DEVICECLASS_PHDC					0x0F	// Personal Healthcare Device Class. eg; Pulse monitor (watch)
#define USBH_DEVICECLASS_AUDIOVIDIO				0x10
#define USBH_DEVICECLASS_WIRELESSCONTROLLER		0xE0
#define USBH_DEVICECLASS_MISC					0xEF
#define USBH_DEVICECLASS_APPLICATION			0xFE
#define USBH_DEVICECLASS_VENDOR					0xFF


#define USBH_DEVICESUBCLASS_AUDIO				0x01
#define USBH_DEVICESUBCLASS_MIDI				0x03

#define USBH_ENDPOINTTYPE_CONTROL				0x00
#define USBH_ENDPOINTTYPE_ISOCHRONOUS			0x01
#define USBH_ENDPOINTTYPE_BULK					0x02
#define USBH_ENDPOINTTYPE_INTERRUPT				0x03




typedef struct {
	int32_t x1;
	int32_t y1;
	int32_t x2;
	int32_t y2;
}area_t;




typedef struct {
	uint8_t bLength;				// Size of Descriptor in Bytes (9 Bytes)
	uint8_t bDescriptorType;		// this Descriptotion type (eg; 0x04 == interface)
	
	uint8_t bInterfaceNumber;		// Number of Interface
	uint8_t bAlternateSetting;		// Value used to select alternative setting
	uint8_t bNumEndpoints;			// Number of Endpoints used for this interface
	uint8_t bInterfaceClass;		// Class Code (Assigned by USB Org)
	uint8_t bInterfaceSubClass;		// Subclass Code (Assigned by USB Org)
	uint8_t bInterfaceProtocol;		// Protocol Code (Assigned by USB Org)
	uint8_t iInterface;				// Index of String Descriptor Describing this interface
}__attribute__((packed)) descriptor_interface_t;


typedef struct {
	uint8_t bLength;				// Size of Descriptor in Bytes (9 Bytes)
	uint8_t bDescriptorType;		// this Descriptotion type (eg; 0x04 == interface)
	
	uint8_t bEndpointAddress;		// Endpoint Address
									// Bits 0..3b Endpoint Number.
									// Bits 4..6b Reserved. Set to Zero
									// Bits 7 Direction 0 = Out, 1 = In (Ignored for Control Endpoints)
									
	uint8_t bmAttributes;			// Bits 0..1 Transfer Type
									//   00 = Control, 01 = Isochronous, 10 = Bulk, 11 = Interrupt
									// Bits 2..7 are reserved. If Isochronous endpoint, 
									// Bits 3..2 = Synchronisation Type (Iso Mode)
									//   00 = No Synchonisation, 01 = Asynchronous, 10 = Adaptive, 11 = Synchronous
									// Bits 5..4 = Usage Type (Iso Mode) 
									//   00 = Data Endpoint, 01 = Feedback Endpoint, 10 = Explicit Feedback Data Endpoint, 11 = Reserved
									
	uint16_t wMaxPacketSize;		// Maximum Packet Size this endpoint is capable of sending or receiving
	uint8_t bInterval;				// Interval for polling endpoint data transfers. Value in frame counts.
									// Ignored for Bulk & Control Endpoints. Isochronous must equal 1 and
									// field may range from 1 to 255 for interrupt endpoints.
}__attribute__((packed)) descriptor_endpoint_t;



void SaC4_setCallbackFunc (int (*func)(uint32_t msg, intptr_t *value1, uint32_t value2));


#endif
