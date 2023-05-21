
#ifndef _XPRE_H
#define _XPRE_H




#define 	XPRE_FORMAT_DATA		0			// preset 2 binary conversion. length == AS_PRESET_LENGTH
#define 	XPRE_FORMAT_WRITE		0x01		// as above but formatted as a packet ready for hid_write. length == 4*AS_PACKET_LENGTH
#define 	XPRE_FORMAT_NAME		0x02		// as above but include preset name, if available. length == 5*AS_PACKET_LENGTH
#define 	XPRE_FORMAT_SAVE		(XPRE_FORMAT_WRITE|XPRE_FORMAT_NAME)
#define 	XPRE_FORMAT_STORE		(XPRE_FORMAT_WRITE)



typedef struct _xpre_t {
	char *buffer;
	int32_t bufferLen;
	int32_t readPos;
	uint32_t bufferPos;
	
	struct{
		uint16_t indentCt;
		uint8_t bytepadding;
		uint8_t bitpadding:7;
		uint8_t userbuffer:1;
	}flags;
}xpre_t;


int xpre_init (xpre_t *xpre);
void xpre_cleanup (xpre_t *xpre);
int xpre_preparePath (xpre_t *xpre, char *path);
int xpre_prepareString (xpre_t *xpre, const char *str, const int strLen);		// for use with string2bin
int xpre_prepareBuffer (xpre_t *xpre, char *buffer, const int bufferLen);		// for use with bin2string
int xpre_preset2bin (xpre_t *xpre, uint8_t *buffer, const uint32_t formatWrite, const int presetLocationIdx);
int xpre_append (xpre_t *xpre, const char *name, const char *value);
void xpre_reset (xpre_t *xpre);

int xpre_bin2string (xpre_t *xpre, void *buffer, const int bufferLen);
void *xpre_loadFile (const char *filename, size_t *flength);




#endif

