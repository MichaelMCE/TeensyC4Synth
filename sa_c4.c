

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "sa_c4.h"
//#include "ctrl_c4.h"
//#include "xpre.h"





void printHex16 (const uint8_t *data, int length)
{
	for (int i = 0; i < length; i += 16){
		for (int j = i; j < i+16 && j < length; j++){
			printf("%2.2X ", data[j]);
		}
		printf("\r\n");
	}
	printf("\r\n");
}

void printHex32 (const uint8_t *data, int length)
{
	for (int i = 0; i < length; i += 32){
		for (int j = i; j < i+32 && j < length; j++){
			printf("%2.2X ", data[j]);
		}
		printf("\r\n");
	}
	printf("\r\n");
}

void dumpHex (const uint8_t *data, int length)
{
	for (int i = 0; i < length; i++){
		printf("%.2X ", data[i]);
	}
	printf("\r\n");
}

void dumpASCII (const uint8_t *data, int length)
{
	for (int i = 0; i < length; i++){
		if (data[i] > 31 && data[i] < 186)
			printf("%c ", data[i]);
		else
			printf("  ");
	}
	printf("\r\n");
}

char *rtrim (char *buffer, int len)
{
	while (len > 0 && buffer[--len] == ' ')
		buffer[len] = 0;
	return buffer;
}

/*
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
*/


int as_write (as_t *as_ctx, const void *buffer, const int bLength)
{
	if (!bLength) return -1;
	if (bLength%AS_PACKET_LENGTH){
		printf("as_write(): write length not a multiple of AS_PACKET_LENGTH\r\n");
		return -2;
	}

	//char tmp[64];
	int writeTotal = 0;
	uint8_t *buffer8 = (uint8_t*)buffer;
	
	for (int i = 0; i < bLength; i += AS_PACKET_LENGTH){
		if (hid_write(NULL, &buffer8[i], AS_PACKET_LENGTH) != AS_PACKET_LENGTH){
			return 0;
		}else{
			writeTotal += AS_PACKET_LENGTH;
			//hid_read_timeout(NULL, (void*)tmp, AS_PACKET_LENGTH, AS_READ_TIMEOUT);
			delay(10);
		}
	}
	
	return writeTotal;
}

int as_read (as_t *as_ctx, void *buffer, const int bLength)
{
	
	if (bLength < AS_PACKET_LENGTH) return -2;
	
#if 1
	return hid_read_timeout(NULL, (uint8_t*)buffer, AS_PACKET_LENGTH, AS_READ_TIMEOUT);
#else
	int readlen = hid_read(NULL, (uint8_t*)buffer, AS_PACKET_LENGTH);
	if (readlen != AS_REPORT_LENGTH)
		return 0;
	else
		return readlen;
#endif
}

/*
************************************************************************************************
************************************************************************************************
************************************************************************************************
************************************************************************************************
************************************************************************************************
*/

int as_getHardwareConfig (as_t *as_ctx, as_hw_config_t *cfg)
{

	uint8_t buffer[AS_PACKET_LENGTH];
	memset(buffer, 0, sizeof(buffer));

	buffer[0] = AS_CMD_CONFIG_GET;

	if (hid_write(NULL, buffer, AS_PACKET_LENGTH) != AS_PACKET_LENGTH)
		return -2;

	memset(buffer, 0, sizeof(buffer));

	int readlen = hid_read_timeout(NULL, buffer, AS_PACKET_LENGTH, AS_READ_TIMEOUT);
	if (readlen != AS_REPORT_LENGTH || buffer[0] != 0x32)
		return 0;

	memcpy(cfg, &buffer[1], sizeof(*cfg));
	return 1;
}

int as_getPresetName (as_t *as_ctx, const uint8_t presetIdx, char *name)
{
	
	const int presetIndex = presetIdx&0x7F;
	const uint32_t addr = AS_PRESET_ADDRESS_BASE + (presetIndex * AS_PRESET_ADDRESS_LENGTH) + AS_PRESET_ADDRESS_NAME;
	
	uint8_t buffer[AS_PACKET_LENGTH];
	memset(buffer, 0, sizeof(buffer));
	
	buffer[0] = AS_CMD_FLASH_READ;
	buffer[1] = (addr>>16)&0xFF;
	buffer[2] = (addr>>8)&0xFF;
	buffer[3] = addr&0xFF;
	buffer[4] = 0x00;		

	if (hid_write(NULL, buffer, AS_PACKET_LENGTH) != AS_PACKET_LENGTH)
		return -2;

	int readlen = hid_read_timeout(NULL, buffer, AS_PACKET_LENGTH, AS_READ_TIMEOUT);
	if (readlen != AS_REPORT_LENGTH || buffer[0] != AS_CMD_FLASH_READ || buffer[1] == 0xFF){
		if (buffer[1] == 0xFF)
			return -1;	//empty
		else
			return 0;
	}

	buffer[AS_PRESET_NAME_LENGTH+1] = 0;
	strcpy(name, (char*)&buffer[1]);
	return 1;
}

int as_setPresetActive (as_t *as_ctx, const int presetIdx)
{
	const uint8_t presetIndex = presetIdx&0x7F;
	uint8_t buffer[AS_PACKET_LENGTH];
	memset(buffer, 0, sizeof(buffer));

	buffer[0] = AS_CMD_ACTIVE_SET;
	buffer[1] = presetIndex;
	buffer[2] = 0x00;
	
	int ret = hid_write(NULL, buffer, AS_PACKET_LENGTH);
	
	// pedal needs at least 500ms to reinitialize after preset change
	// unless you know better, do not adjust
	delay(500);

	return (ret == AS_PACKET_LENGTH);
}

int as_getActivePreset (as_t *as_ctx)
{
	as_hw_config_t cfg = {0};
	if (!as_getHardwareConfig(as_ctx, &cfg)){		// if we failed here then the problem occured before we got here.
		//printf("as_getHardwareConfig failed\n");
		return 0;
	}
	return cfg.activePreset;
}

int as_getVersion (as_t *as_ctx)
{
	as_hw_config_t cfg = {0};
	memset(&cfg, 0, sizeof(cfg));
	
	if (!as_getHardwareConfig(as_ctx, &cfg)){
		//printf("as_getHardwareConfig failed\n");
		return 0;
	}
	return cfg.firmwareVersion;
}

int as_getControlValue (as_t *as_ctx, uint8_t ctrl, uint8_t *value)
{
	uint8_t buffer[AS_PACKET_LENGTH];
	memset(buffer, 0, sizeof(buffer));
	
	buffer[0] = AS_CMD_CTRL_GET;
	buffer[1] = 0;
	buffer[2] = ctrl;
	buffer[3] = AS_PAYLOAD_LENGTH;
	
	if (hid_write(NULL, buffer, AS_PACKET_LENGTH) != AS_PACKET_LENGTH)
		return -2;

	memset(buffer, 0, sizeof(buffer));
	int readlen = hid_read_timeout(NULL, buffer, AS_PACKET_LENGTH, AS_READ_TIMEOUT);
	if (readlen != AS_REPORT_LENGTH || buffer[0] != AS_CMD_CTRL_GET)
		return 0;

	*value = buffer[1];
	return 1;
}

int as_setControlValue (as_t *as_ctx, uint8_t ctrl, uint16_t value)
{
	uint8_t buffer[AS_PACKET_LENGTH];
	memset(buffer, 0, sizeof(buffer));
	
	buffer[0] = AS_CMD_CTRL_SET;
	buffer[1] = ctrl;
	buffer[2] = (value>>8)&0xFF;
	buffer[3] = value&0xFF;
	
	if (hid_write(NULL, buffer, AS_PACKET_LENGTH) != AS_PACKET_LENGTH)
		return -2;

	return 1;
}

int as_erase (as_t *as_ctx, const uint8_t presetIdx)
{
	uint8_t buffer[AS_PACKET_LENGTH];
	memset(buffer, 0, sizeof(buffer));
	
	const int presetIndex = (presetIdx&0x7F) | 0x80;
	
	buffer[0] = AS_CMD_PRESET_ERASE;
	buffer[1] = presetIndex;
	buffer[2] = 0;
	buffer[3] = 0;
	
	if (hid_write(NULL, buffer, AS_PACKET_LENGTH) != AS_PACKET_LENGTH)
		return -2;

	return hid_read_timeout(NULL, buffer, AS_PACKET_LENGTH, AS_READ_TIMEOUT);

}

int as_getPreset (as_t *as_ctx, uint8_t *buffer, const int bufferLen, int preset, int includeName)
{
	includeName &= 0x01;
	preset &= 0x7F;

	memset(buffer, 0, bufferLen);
	uint8_t writerBuffer[AS_PACKET_LENGTH];
	memset(writerBuffer, 0, sizeof(writerBuffer));
	
	for (int idx = 1; idx < 5+includeName; idx++){
		uint32_t addr = AS_PRESET_ADDRESS_BASE + (preset * AS_PRESET_ADDRESS_LENGTH) + (idx * AS_PAYLOAD_LENGTH);

		writerBuffer[0] = AS_CMD_FLASH_READ;
		writerBuffer[1] = (addr>>16)&0xFF;
		writerBuffer[2] = (addr>>8)&0xFF;
		writerBuffer[3] = addr&0xFF;
		writerBuffer[4] = 0x00;

		if (hid_write(NULL, writerBuffer, AS_PACKET_LENGTH) != AS_PACKET_LENGTH)
			return -1;

		writerBuffer[0] = 0;
		int readlen = hid_read_timeout(NULL, writerBuffer, AS_PACKET_LENGTH, AS_READ_TIMEOUT);
		if (readlen != AS_REPORT_LENGTH || writerBuffer[0] != AS_CMD_FLASH_READ)
			return 0;
		memcpy(&buffer[(idx-1)*AS_PAYLOAD_LENGTH], &writerBuffer[1], AS_PAYLOAD_LENGTH);
	}
	return 1;
}

int as_getPresetDefault (as_t *as_ctx, uint8_t *buffer, const size_t bufferLen, int preset)
{
	preset &= 0x7F;

	memset(buffer, 0, bufferLen);
	uint8_t writerBuffer[AS_PACKET_LENGTH];
	memset(writerBuffer, 0, sizeof(writerBuffer));
	
	for (int idx = 1; idx < 6; idx++){
		uint32_t addr = AS_PRESET_ADDRESS_DEFAULTS + (preset * AS_PRESET_ADDRESS_LENGTH) + (idx * AS_PAYLOAD_LENGTH);

		writerBuffer[0] = AS_CMD_FLASH_READ;
		writerBuffer[1] = (addr>>16)&0xFF;
		writerBuffer[2] = (addr>>8)&0xFF;
		writerBuffer[3] = addr&0xFF;
		writerBuffer[4] = 0x00;

		if (hid_write(NULL, writerBuffer, AS_PACKET_LENGTH) != AS_PACKET_LENGTH)
			return -1;

		writerBuffer[0] = 0;
		int readlen = hid_read_timeout(NULL, writerBuffer, AS_PACKET_LENGTH, AS_READ_TIMEOUT);
		if (readlen != AS_REPORT_LENGTH || writerBuffer[0] != AS_CMD_FLASH_READ)
			return 0;
		memcpy(&buffer[(idx-1)*AS_PAYLOAD_LENGTH], &writerBuffer[1], AS_PAYLOAD_LENGTH);
	}
	return 1;
}

int as_setPresetName (as_t *as_ctx, char *name, const int presetIdx)
{

	char presetName[AS_PRESET_NAME_LENGTH];
	memset(presetName, ' ', AS_PRESET_NAME_LENGTH);
	
	int slen = strlen((char*)name);
	if (!slen) return 0;
	if (slen > AS_PRESET_NAME_LENGTH)
		slen = AS_PRESET_NAME_LENGTH;
	if (slen < AS_PRESET_NAME_LENGTH){
		memcpy(presetName, name, slen);
		name = presetName;
	}

	int pos = 0;
	uint8_t buffer[AS_PACKET_LENGTH];
	
	buffer[pos++] = AS_CMD_ACTIVE_WRITE;
	buffer[pos++] = presetIdx&0x7F;
	buffer[pos++] = 1;
	memset(&buffer[pos], 32, AS_PRESET_NAME_LENGTH);
	memcpy((char*)&buffer[pos], name, AS_PRESET_NAME_LENGTH);
			
	pos += AS_PRESET_NAME_LENGTH;
	buffer[pos++] = 0;
	buffer[pos++] = 0;
	buffer[pos++] = 0;

	int ret = (hid_write(NULL, buffer, AS_PACKET_LENGTH) == AS_PACKET_LENGTH);
	delay(500);		// needed or bad things happen
	return ret;
}

int as_dumpFlash (as_t *as_ctx)
{
	printf("\r\n");
	uint8_t buffer[AS_PACKET_LENGTH];
				
	const uint32_t startAdd = 0x00;
	const uint32_t endAdd = AS_C4_FLASHSIZE;

	for (uint32_t i = startAdd; i < endAdd; i += (uint32_t)AS_PAYLOAD_LENGTH){
		memset(buffer, 0, sizeof(buffer));

		uint32_t addr = i;
		buffer[0] = AS_CMD_FLASH_READ;
		buffer[1] = (addr>>16)&0xFF;
		buffer[2] = (addr>>8)&0xFF;
		buffer[3] = addr&0xFF;
		buffer[4] = 0x00;

		if (hid_write(NULL, buffer, AS_PACKET_LENGTH) != AS_PACKET_LENGTH)
			return -1;
		delay(1); // may not be needed
		
		memset(buffer, 0, sizeof(buffer));
		int readlen = hid_read_timeout(NULL, buffer, AS_PACKET_LENGTH, AS_READ_TIMEOUT);
		if (readlen != AS_REPORT_LENGTH)
			return -2;

		printf("%.6X:  ", (int)i);
		if (1)
			dumpHex(&buffer[1], AS_PAYLOAD_LENGTH);
		else
			dumpASCII(&buffer[1], AS_PAYLOAD_LENGTH);
	}
	printf("\r\n");
	return 1;
}

int as_inspectFlash (as_t *as_ctx, const size_t address, const size_t length)
{
	printf("\r\n");
	uint8_t buffer[AS_PACKET_LENGTH];
				
	const uint32_t startAdd = address;
	uint32_t endAdd = address+length-1;
	if (endAdd > AS_C4_FLASHSIZE) endAdd = AS_C4_FLASHSIZE;
	if (startAdd > endAdd-(AS_PAYLOAD_LENGTH-1)) return -3;

	for (uint32_t i = startAdd; i < endAdd; i += (uint32_t)AS_PAYLOAD_LENGTH){
		memset(buffer, 0, sizeof(buffer));

		uint32_t addr = i;
		buffer[0] = AS_CMD_FLASH_READ;
		buffer[1] = (addr>>16)&0xFF;
		buffer[2] = (addr>>8)&0xFF;
		buffer[3] = addr&0xFF;
		buffer[4] = 0x00;

		if (hid_write(NULL, buffer, AS_PACKET_LENGTH) != AS_PACKET_LENGTH)
			return -1;
		delay(1); // may not be needed
		
		memset(buffer, 0, sizeof(buffer));
		int readlen = hid_read_timeout(NULL, buffer, AS_PACKET_LENGTH, AS_READ_TIMEOUT);
		if (readlen != AS_REPORT_LENGTH)
			return -2;

		printf("%.6X:  ", (int)i);
		if (1)
			dumpHex(&buffer[1], AS_PAYLOAD_LENGTH);
		else
			dumpASCII(&buffer[1], AS_PAYLOAD_LENGTH);
	}
	printf("\r\n");
	return 1;
}

int as_getEEPROM (as_t *as_ctx, uint8_t *buffer, const int bsize)
{
	if (bsize < AS_EEPROM_SIZE) return -3;
	
	uint8_t writebuffer[AS_PACKET_LENGTH];
	memset(writebuffer, 0, sizeof(writebuffer));

	for (int i = 0; i < AS_EEPROM_SIZE; i += AS_PAYLOAD_LENGTH){
		writebuffer[0] = AS_CMD_EEPROM_READ;
		writebuffer[1] = i;
		writebuffer[2] = 0x20;
		writebuffer[3] = 0;
		writebuffer[4] = 0;

		if (hid_write(NULL, writebuffer, AS_PACKET_LENGTH) != AS_PACKET_LENGTH)
			return -2;

		int readlen = hid_read_timeout(NULL, writebuffer, AS_PACKET_LENGTH, AS_READ_TIMEOUT);
		if (readlen != AS_REPORT_LENGTH || writebuffer[0] != AS_CMD_EEPROM_READ){
			if (buffer[1] == 0xFF)
				return -4;	//empty
			else
				return 0;
		}

		memcpy(buffer, &writebuffer[1], AS_PAYLOAD_LENGTH);
		buffer += AS_PAYLOAD_LENGTH;
	}

	return 1;
}

int as_writePreset (as_t *as_ctx, as_preset_t *preset, const int bsize, const int presetIdx)
{
	if (bsize % AS_PAYLOAD_LENGTH){
		printf("bsize:%i is not a multiple of as_payload_length\r\n", bsize);
		return -1;
	}

	uint8_t buffer[AS_PACKET_LENGTH];
	uintptr_t ppreset = (uintptr_t)preset;
	const int twrites = AS_PRESET_LENGTH / AS_PAYLOAD_LENGTH;
	
	for (int i = 0; i < twrites; i++){
		memset(buffer, 0, AS_PACKET_LENGTH);
		buffer[0] = AS_CMD_ACTIVE_STORE;
		buffer[1] = (i == twrites-1);
		buffer[2] = i*AS_PAYLOAD_LENGTH;
		buffer[3] = AS_PAYLOAD_LENGTH;
		
		memcpy(&buffer[4], (void*)ppreset, AS_PAYLOAD_LENGTH);
		ppreset += (uintptr_t)AS_PAYLOAD_LENGTH;

		if (hid_write(NULL, buffer, AS_PACKET_LENGTH) != AS_PACKET_LENGTH){
			printf("write failed %i\r\n", (int)i);
			return 0;
		}
		delay(1);
	}
	
	// allow time for pedal to reinit. do not change
	// instead of waiting, one could change of an ACK signal, if any
	delay(500);	
	
	return as_setPresetName(as_ctx, (char*)preset->name, presetIdx);
}



/*

################################################################################################
################################################################################################
################################################################################################
################################################################################################
################################################################################################
*/
