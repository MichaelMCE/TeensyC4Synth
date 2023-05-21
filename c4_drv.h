


#ifndef _C4_DRV_H
#define _C4_DRV_H


#define AS_VERSION		(1.0)		// Initial release
#define AS_DATE			(260423)



class SaC4 : public USBDriver {
public:
	enum { SYSEX_MAX_LEN = 60 };
	
	SaC4 (USBHost &host)
	{
		init();
	}
	SaC4 (USBHost *host)
	{
		init();
	}

	
	void setCallbackFunc (int (*func)(uint32_t msg, intptr_t *value1, uint32_t value2))
	{
		callbackFunc = func;
	}

	int writeData (const void *data, const size_t size);
	int readData (const void *data, const size_t size);
	
	uint32_t pid = 0;


	typedef struct {
		void *buffer;
		uint32_t len;		// length in  bytes
		int32_t rows;		// how many rows does this buffer describe
	
		int32_t row;		// next write points to data for this row
						// next pixel read request askas for data for this row
						
		int32_t frame;
	}tx_descript_t;

	tx_descript_t tx_writeCtx;
	
	int (*callbackFunc) (uint32_t msg, intptr_t *value1, uint32_t value2);
	
	bool usb_control_msg (Device_t *dev, uint32_t orequestType, uint32_t omsg, uint32_t ovalue, uint32_t oindex, void *buff, uint32_t len)
	{
		setup_t setup;
		mk_setup(setup, orequestType, omsg, ovalue, oindex, len);
		return queue_Control_Transfer(dev, &setup, buff, NULL);
	}

	bool usb_bulk_write (USBDriver *drv, Pipe_t *pipe, const void *buffer, const uint32_t len)
	{
		return queue_Data_Transfer(pipe, (void*)buffer, len, drv);
	}

	bool usb_bulk_read (USBDriver *drv, Pipe_t *pipe, const void *buffer, const uint32_t len)
	{
		return queue_Data_Transfer(pipe, (void*)buffer, len, drv);
	}

	bool usb_interrupt_write (USBDriver *drv, Pipe_t *pipe, const void *buffer, const uint32_t len)
	{
		return queue_Data_Transfer(pipe, (void*)buffer, len, drv);
	}
	
	
	static void tx_callback (const Transfer_t *transfer);	
	static void rx_callback (const Transfer_t *transfer);
	
	#define QTD_TOGGLE      (1 << 31)       /* data toggle */
	#define QTD_LENGTH(tok) (((tok)>>16) & 0x7fff)		/* length available */
	#define QTD_IOC         (1 << 15)       /* interrupt on complete */
	#define QTD_CERR(tok)   (((tok)>>10) & 0x3)
	#define QTD_PID(tok)    (((tok)>>8) & 0x3)
	#define QTD_STS_ACTIVE  (1 << 7)        /* HC may execute this */
	#define QTD_STS_HALT    (1 << 6)        /* halted on error */
	#define QTD_STS_DBE     (1 << 5)        /* data buffer error (in HC) */
	#define QTD_STS_BABBLE  (1 << 4)        /* device was babbling (qtd halted) */
	#define QTD_STS_XACT    (1 << 3)        /* device gave illegal response */
	#define QTD_STS_MMF     (1 << 2)        /* incomplete split transaction */
	#define QTD_STS_STS     (1 << 1)        /* split transaction state */
	#define QTD_STS_PING    (1 << 0)        /* issue PING? */

protected:

	virtual void driverReady ();

	virtual void Task ();
	virtual bool claim (Device_t *device, int type, const uint8_t *descriptors, uint32_t len);
	virtual void control (const Transfer_t *transfer);
	virtual void disconnect ();
	
	void init();

private:

	Device_t *device;
	Pipe_t *rxpipe;
	Pipe_t *txpipe;
	
	uint32_t start = 0;

	uint16_t tx_size;
	uint16_t rx_size;
	uint8_t tx_ep;
	uint8_t rx_ep;			

	Pipe_t mypipes[4] __attribute__ ((aligned(32)));
	Transfer_t mytransfers[32] __attribute__ ((aligned(32)));

};



#endif


