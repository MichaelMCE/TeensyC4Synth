

#include <Arduino.h>
#include "usbh_common.h"
#include <USBHost_t36.h>
#include "c4_drv.h"
#include "xpre.h"
#include "sa_c4.h"
#include "sa_files.h"


static USBHost myusb;
static USBHub hub1(myusb);
static SaC4 sac4(myusb);






void usb_start (const uint32_t pid)
{
	sac4.pid = pid;
	myusb.begin();
}

void usb_task ()
{
	myusb.Task();
}

void SaC4_setCallbackFunc (int (*func)(uint32_t msg, intptr_t *value1, uint32_t value2))
{
	sac4.callbackFunc = func;
}

void SaC4::init ()
{
	start = 0;
	callbackFunc = NULL;

	contribute_Pipes(mypipes, sizeof(mypipes)/sizeof(Pipe_t));
	contribute_Transfers(mytransfers, sizeof(mytransfers)/sizeof(Transfer_t));
	
	driver_ready_for_device(this);
}

void SaC4::Task ()
{
	if (device->enum_state == USBH_ENUMSTATE_END){
		if (!start){
			start = millis();
		}else if (millis() - start > AS_C4_STARTUP_WAIT){
			device->enum_state++;
			driverReady();
		}
	}
}

void SaC4::driverReady ()
{
	// println("SaC4 driverReady  = ", (uint32_t)this, HEX);
	if (callbackFunc)
		(*callbackFunc)(USBD_MSG_DEVICEREADY, NULL, 0);
}


// type 0: claim complete device
// type 1: claim per interface
bool SaC4::claim (Device_t *dev, int type, const uint8_t *descriptors, uint32_t len)
{
	// only claim at interface level
	if (type != 1) return false;

	if (dev->idVendor != AS_VID || dev->idProduct != sac4.pid){
		//println("  device is not a SA C4");
		return false;
	}else{
		//println("  found a SA C4");
	}

	device = dev;

	//printf("\r\n\r\n\r\n###############\r\nSaC4 claims, this %p\r\n", (uint32_t)this);
	//printf("claimType = %d\r\n", type);
	//printf("idVendor = %X\r\n" , dev->idVendor);
	//printf("idProduct = %X\r\n" , dev->idProduct);


	const uint8_t *p = descriptors;
	const uint8_t *end = p + len;

	// http://www.beyondlogic.org/usbnutshell/usb5.shtml
	int descriptorLength = p[0];
	int descriptorType = p[1];
	
	//printf("descriptorType = %d\r\n", descriptorType);
	//printf("descriptorLength = %d\r\n",  descriptorLength);
	
	if (!descriptorLength)
		return false;

	if (descriptorType != USBH_DESCRIPTORTYPE_INTERFACE /*|| descriptorLength != 9*/)
		return false;


	descriptor_interface_t *interface = (descriptor_interface_t*)&p[0];
	//printf("bInterfaceClass = %d\r\n", interface->bInterfaceClass);
	//printf("bInterfaceSubClass = %d\r\n",  interface->bInterfaceSubClass);
	
	if (interface->bInterfaceClass != USBH_DEVICECLASS_AUDIO || interface->bInterfaceSubClass != USBH_DEVICESUBCLASS_AUDIO)
		return false;
	
	
	//println("  Interface is SA C4");
	
	p += descriptorLength;	// == sizeof(descriptor_interface_t)
	rx_ep = 0;
	tx_ep = 0;
	//int interfaceCt = 0;

	while (p < end){
		int interfaceLength = p[0];
		if (p + interfaceLength > end){
			return false; // reject if beyond end of data
		}
		
		int interfaceType = p[1];

		//printf(":: interfaceType = %d\r\n", interfaceType);

		if (interfaceType == USBH_DESCRIPTORTYPE_ENDPOINT){
	
			//println(" ");
			//printf("interface number : %d\r\n", interfaceCt++);
			//printf("interfaceType = %d\r\n", interfaceType);
			//printf("interfaceLength = %d\r\n", interfaceLength);
	
	
			descriptor_endpoint_t *endpoint = (descriptor_endpoint_t*)&p[0];
			
			//printf("bEndpointAddress = %X\r\n", endpoint->bEndpointAddress);
			//printf("bmAttributes = %X\r\n", endpoint->bmAttributes);
			//printf("wMaxPacketSize = %d\r\n", endpoint->wMaxPacketSize);
			//printf("bInterval = %d\r\n", endpoint->bInterval);

			int pipeType = endpoint->bmAttributes; //&0x03;
			//uint8_t pipeDir = ((endpoint->bEndpointAddress&0x80) >> 7);
			
			// type: 0 = Control, 1 = Isochronous, 2 = Bulk, 3 = Interrupt
			//printf("  endpoint type : %d\r\n", pipeType);
			//printf("  endpoint dir  : %i\r\n", pipeDir);
			//printf("  endpoint addr : %X\r\n", endpoint->bEndpointAddress/*&0x0F*/);

			if (endpoint->bEndpointAddress == 0x01){			// for data writes
				tx_ep = endpoint->bEndpointAddress; //&0x0F;
				tx_size = endpoint->wMaxPacketSize;

				txpipe = new_Pipe(dev, pipeType, tx_ep, 0, tx_size, endpoint->bInterval);
				//printf("txpipe  = %d\r\n", (uint32_t)txpipe);
				
				if (txpipe){
					//txpipe->callback_function = tx_callback;
					
					device = dev;
					//printf("txpipe device = %d\r\n", (uint32_t)device);
				}
			}else if (endpoint->bEndpointAddress == 0x81){		// for input (reading)
				rx_ep = endpoint->bEndpointAddress&0x0F;
				rx_size = endpoint->wMaxPacketSize;
				
				rxpipe = new_Pipe(dev, pipeType, rx_ep, 1, rx_size, endpoint->bInterval);
				//printf("rxpipe  = %d\r\n", (uint32_t)rxpipe);
				
				if (rxpipe){
					rxpipe->callback_function = rx_callback;

					device = dev;
					//printf("rxpipe device = %d\r\n", (uint32_t)device);
				}
			}

		}

		p += interfaceLength;
		//printf("###\r\n\r\n\r\n");
	}

	//Serial.printf("  endpoint txpipe : %d\r\n", (uint32_t)txpipe);
	//Serial.printf("  endpoint rxpipe  : %d\r\n", (uint32_t)rxpipe);
	//printf("***********************************************\r\n\r\n\r\n");

	return (rxpipe || txpipe);
}


#if 1
static int tx_state = 0;

void SaC4::rx_callback (const Transfer_t *transfer)
{
	const int length = transfer->length - QTD_LENGTH(transfer->qtd.token);
	if (length < 1) return;
	
	if (length != AS_REPORT_LENGTH){
		printf("rx_callback(): len %i\r\n", length);
		
		dumpHex((uint8_t*)transfer->buffer, length);
		queue_Data_Transfer(transfer->pipe, transfer->buffer, length, transfer->driver);
	}
}

void SaC4::tx_callback (const Transfer_t *transfer)
{
	// println("SaC4 tx_callback");
	
	if (transfer->driver){
		if (tx_state == -1){
			// println("SaC4 tx_callback tx_state ", tx_state);
			
			//((SaC4*)(transfer->driver))->  ();

		}else if (tx_state == 1){
			tx_descript_t *context = &((SaC4*)(transfer->driver))->tx_writeCtx;
				
			int ret = (*((SaC4*)(transfer->driver))->callbackFunc)(USBD_MSG_WRITEREADY, (intptr_t*)context, 0);
			if (ret){
				//printf("tx_callback %i %i\r\n", (int)context->frame, (int)context->row);
				
				((SaC4*)(transfer->driver))->writeData(context->buffer, context->len);
				context->row += context->rows;
			}else{
				tx_state = 0;
				(*((SaC4*)(transfer->driver))->callbackFunc)(USBD_MSG_WRITEEND, (intptr_t*)context, 0);
			}
		}
	}
}

#endif

void SaC4::control (const Transfer_t *transfer)
{
	// println("SaC4 control()");
}

void SaC4::disconnect ()
{
	// println("SaC4 disconnect()");
	
	// TODO: free resources
	device = NULL;
	start = 0;
}


int SaC4::writeData (const void *data, const size_t size)
{
	//__disable_irq();
	int ret = usb_bulk_write(this, txpipe, data, size);
	//__enable_irq();
	if (ret)
		return size;
	else
		return 0;
}	

int SaC4::readData (const void *data, const size_t size)
{
	//__disable_irq();
	int ret = usb_bulk_read(this, rxpipe, data, size);
	//__enable_irq();
	if (ret)
		return size;
	else
		return 0;
}

int hid_write (void *unused, void *buffer, size_t len)
{
	int ret = sac4.writeData(buffer, len);
	delay(1);
	return ret;
}

int hid_read (void *unused, void *buffer, size_t len)
{
	int ret = sac4.readData(buffer, len);
	delay(1);	// do not remove!
	return ret;
}

int hid_read_timeout (void *unused, uint8_t *buffer, const int len, const int timeMS)
{
	buffer[0] = 0;
	buffer[1] = 0;
	int ret = hid_read(NULL, (void*)buffer, (size_t)len);
	
	// give time for transfer & buffer to fill. without this bad things happen
	delay(1);

	// wait for completion
	if (!buffer[0] && !buffer[1]){
		const int timeStepDelay = 2;		// millisecond

		for (int i = 0; i < timeMS; i+=timeStepDelay){
			if (buffer[0] || buffer[1]){
				
				// first bytes contain data, give time for rest to transfer & fill
				// TODO: probably best using a microsecond delay func - would require extensive testing
				delay(1);
				
				return len;
			}
			delay(timeStepDelay);
		}
	}

	return ret;
}

