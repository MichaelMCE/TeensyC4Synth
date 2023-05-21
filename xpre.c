
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <inttypes.h>

#include "sa_c4.h"
#include "xpre.h"




static char *trimTrailingWhite (const char *string)
{
	// trim trailing space
	char *str = strdup(string);
	if (!str) return NULL;
	char *end = str + strlen(str) - 1;
	while(end > str && isspace((unsigned char)*end))
		end--;
	end[1] = '\0';
	return str;
}

/*
void dumpHex16 (const unsigned char *data, int length)
{
	for (int i = 0; i < length; i += 16){
		for (int j = i; j < i+16 && j < length; j++){
			printf("%2.2X ", data[j]);
		}
		printf("\n");
	}
	printf("\n");
}
*/


// reenable if reading from sdcard and add your interface
#if 0	

static size_t fileLength (FILE *fp)
{
	fpos_t pos;
	
	fgetpos(fp, &pos);
	fseek(fp, 0, SEEK_END);
	size_t fl = ftell(fp);
	fsetpos(fp, &pos);
	
	return fl;
}

// create a new buffer, load file in to that buffer
// return buffer on success, NULL on failure.
void *xpre_loadFile (const char *filename, size_t *flength)
{

	*flength = 0;
	void *data = NULL;
	FILE *fd = fopen(filename, "rb");
	
	if (fd){
		*flength = fileLength(fd);
		if (*flength > 63){
			data = malloc(*flength);
			if (data){
				size_t ret = (size_t)fread(data, 1, *flength, fd);
				if (ret != *flength){
					printf("getfile(): file read error\r\n");
					free(data);
					data = NULL;
					*flength = 0;
				}
			}
		}
		fclose(fd);
	}
	
	return data;
}
#endif

/*
void *loadFile (const char *filename, size_t *flength);

void *xpre_loadFile (const char *filename, size_t *flength)
{
	return loadFile(filename, flength);
}*/

int xpre_init (xpre_t *xpre)
{
	memset(xpre, 0, sizeof(*xpre));
	return 1;
}

void xpre_cleanup (xpre_t *xpre)
{
	if (!xpre->flags.userbuffer && xpre->buffer)
		free(xpre->buffer);
}

int xpre_prepare (xpre_t *xpre, const char *buffer, const int bufferLen)
{
	if (!bufferLen) return 0;

	if (bufferLen < 0)
		xpre->bufferLen = strlen(buffer) + 1;
	else
		xpre->bufferLen = bufferLen + 1;

	if (xpre->bufferLen < 8) return 0;
	
	xpre->buffer = (char*)calloc(1, xpre->bufferLen);
	if (xpre->buffer == NULL) return 0;
	
	xpre->buffer = strcpy(xpre->buffer, buffer);
	xpre->readPos = 0;
	xpre->flags.userbuffer = 0;
	return 1;
}

int xpre_prepareBuffer (xpre_t *xpre, char *buffer, const int bufferLen)
{
	memset(buffer, 0, bufferLen);
	xpre->bufferLen = bufferLen;
	xpre->buffer = buffer;
	xpre->readPos = 0;
	xpre->flags.userbuffer = 1;
	return 1;
}

int xpre_prepareString (xpre_t *xpre, const char *buffer, const int bufferLen)
{
	return xpre_prepare(xpre, buffer, bufferLen);
}

int xpre_preparePath (xpre_t *xpre, char *path)
{
	size_t flength = 0;
	char *fileXML = (char*)xpre_loadFile(path, &flength);
	if (!fileXML){
		//printf("xpre_preparePath(): load failed\n");
		return 0;
	}
	
	int ret = xpre_prepare(xpre, fileXML, flength);
	if (fileXML) free(fileXML);
	return ret;
}

void xpre_reset (xpre_t *xpre)
{
	xpre->readPos = 0;
	xpre->bufferPos = 0;
	//xpre->flags.indentCt = 0;
}

char *findFieldValue (xpre_t *xpre, const char *field)
{
	const int flen = strlen(field) + 3;
	
	char formattedField[flen];
	sprintf(formattedField, "<%s>", field);
	formattedField[flen-1] = 0;

	char *str = strstr(&xpre->buffer[xpre->readPos], formattedField);
	if (!str){
		str = strstr(xpre->buffer, formattedField);
	 	if (!str){
			return NULL;
		}
	}
	
	xpre->readPos = (str - xpre->buffer) + (flen-1);
	//printf("\n%s\n\n", &xpre->buffer[xpre->readPos]);
	return &xpre->buffer[xpre->readPos];
}

char *xpre_getStr (xpre_t *xpre, const char *field)
{
	char *location = findFieldValue(xpre, field);
	if (!location){
		//printf("<%s> not found\n", field);
		return NULL;
	}
	
	char value[AS_PRESET_NAME_LENGTH + 1];
	memset(value, 0, sizeof(value));

	char *end = strchr(location, '<');
	if (!end){
		printf("</%s> not found\r\n", field);
		return NULL;
	}

	snprintf(value, (end-location), "%s", location);
	value[AS_PRESET_NAME_LENGTH] = 0;
	return strdup(value);
}

int xpre_hasField (xpre_t *xpre, const char *field)
{
	return (findFieldValue(xpre, field) != NULL);
}

int32_t xpre_getInt32 (xpre_t *xpre, const char *field)
{
	
	char *location = findFieldValue(xpre, field);
	if (!location){
		printf("<%s> not found\r\n", field);
		return 0;
	}
	
	int32_t value = 0;
	int ret = sscanf(location, "%i<", (int*)&value);
	if (ret == EOF)
		printf("<%s> not found\r\n", field);
	
	return value;
}

uint8_t xpre_getInt8 (xpre_t *xpre, const char *field)
{
	char *location = findFieldValue(xpre, field);
	if (!location){
		//printf("<%s> not found\n", field);
		return 0;
	}
	
	int32_t value = 0;
	int ret = sscanf(location, "%i<", (int*)&value);
	if (ret == EOF)
		printf("<%s> not found\r\n", field);
	
	if (value > 255) value = 255;
	else if (value < 0) value = 0;
	return value;
}

double xpre_getReal (xpre_t *xpre, const char *field)
{
	char *location = findFieldValue(xpre, field);
	if (!location){
		printf("<%s> not found\r\n", field);
		return 0;
	}
	
	double value = 0.0;
	int ret = sscanf(location, "%lf<", &value);
	if (ret == EOF)
		printf("<%s> not read, EOF\r\n", field);

	return value;
}

static int xpre_string2bin (xpre_t *xpre, uint8_t *buffer, const uint32_t formatWrite, const int presetLocationIdx)
{
	int pos = 0;

	if (formatWrite&XPRE_FORMAT_WRITE){
		//buffer[pos++] = 0;	// hid ident
		buffer[pos++] = AS_CMD_ACTIVE_STORE; // CMD
		buffer[pos++] = 0;
		buffer[pos++] = 0;					 // address write offset
		buffer[pos++] = AS_PAYLOAD_LENGTH;		 // LENGTH
	}

/*	
	42 input1_gain   
	7f input2_gain   
	fe master_depth  
	00 mod_source    
	aa bass          
	7f treble        
	fe mix           
	00 lo_retain     
	2c output        
	7f output_balance
*/
	buffer[pos++] = xpre_getInt32(xpre, "input1_gain");
	buffer[pos++] = xpre_getInt32(xpre, "input2_gain");
	buffer[pos++] = xpre_getInt32(xpre, "master_depth");
	buffer[pos++] = xpre_getInt32(xpre, "mod_source");
	buffer[pos++] = xpre_getInt32(xpre, "bass");
	buffer[pos++] = xpre_getInt32(xpre, "treble");
	buffer[pos++] = xpre_getInt32(xpre, "mix");
	buffer[pos++] = xpre_getInt32(xpre, "lo_retain");
	buffer[pos++] = xpre_getInt32(xpre, "output");
	buffer[pos++] = xpre_getInt32(xpre, "output_balance");
	
/*	
	7f voice1_level                                                                                           
	00 voice1_pan                                                                                             
	86 voice1_detune                                                                                          
	00 voice1_tremolo   
*/			
	
	buffer[pos++] = xpre_getInt32(xpre, "voice1_level");
	buffer[pos++] = xpre_getInt32(xpre, "voice1_pan");
	buffer[pos++] = xpre_getInt32(xpre, "voice1_detune");
	buffer[pos++] = xpre_getInt32(xpre, "voice1_tremolo");
	
/*
	43 voice1_semitone << 3 | voice1_octave                                                                   
	55 voice1_envelope << 6 | voice1_source << 2 | voice1_mode                                                
	1a voice1_enable << 4 | voice1_modulate << 3 | voice1_tremolo_source << 2 | voice1_destination
*/
	
	buffer[pos++] = ((xpre_getInt8(xpre, "voice1_semitone")&0x1F) << 3) | (xpre_getInt8(xpre, "voice1_octave")&0x07);
	buffer[pos++] = ((xpre_getInt8(xpre, "voice1_envelope")&0x03) << 6) | ((xpre_getInt8(xpre, "voice1_source")&0x3F) << 2) | (xpre_getInt8(xpre, "voice1_mode")&0x03);
	buffer[pos++] = ((xpre_getInt8(xpre, "voice1_enable")&0x01) << 4) | ((xpre_getInt8(xpre, "voice1_modulate")&0x01) << 3) | ((xpre_getInt8(xpre, "voice1_tremolo_source")&0x01)<<2) | (xpre_getInt8(xpre, "voice1_destination")&0x03);

/*
	5a voice2_level                                                                                           
	00 voice2_pan                                                                                             
	78 voice2_detune                                                                                          
	00 voice2_tremolo                                                                                         
*/
	buffer[pos++] = xpre_getInt32(xpre, "voice2_level");
	buffer[pos++] = xpre_getInt32(xpre, "voice2_pan");
	buffer[pos++] = xpre_getInt32(xpre, "voice2_detune");
	buffer[pos++] = xpre_getInt32(xpre, "voice2_tremolo");
/*
	24 voice2_semitone << 3 | voice2_octave                                                                   
	9a voice2_envelope << 6 | voice2_source << 2 | voice2_mode                                                
	14 voice2_enable << 4 | voice2_modulate << 3 | voice2_tremolo_source << 2 | voice2_destination
*/

	buffer[pos++] = ((xpre_getInt8(xpre, "voice2_semitone")&0x1F) << 3) | (xpre_getInt8(xpre, "voice2_octave")&0x07);
	buffer[pos++] = ((xpre_getInt8(xpre, "voice2_envelope")&0x03) << 6) | ((xpre_getInt8(xpre, "voice2_source")&0x3F) << 2) | (xpre_getInt8(xpre, "voice2_mode")&0x03);
	buffer[pos++] = ((xpre_getInt8(xpre, "voice2_enable")&0x01) << 4) | ((xpre_getInt8(xpre, "voice2_modulate")&0x01) << 3) | ((xpre_getInt8(xpre, "voice2_tremolo_source")&0x01)<<2) | (xpre_getInt8(xpre, "voice2_destination")&0x03);

/*
	7f voice3_level                                                                               
	00 voice3_pan                                                                                 
	7f voice3_detune                                                                              
	00 voice3_tremolo                                                                             
*/
	buffer[pos++] = xpre_getInt32(xpre, "voice3_level");
	buffer[pos++] = xpre_getInt32(xpre, "voice3_pan");
	buffer[pos++] = xpre_getInt32(xpre, "voice3_detune");
	buffer[pos++] = xpre_getInt32(xpre, "voice3_tremolo");

/*
	5c voice3_semitone << 3 | voice3_octave                                                       
	9e voice3_envelope << 6 | voice3_source << 2 | voice3_mode                                    
	1d voice3_enable << 4 | voice3_modulate << 3 | voice3_tremolo_source << 2 | voice3_destination
*/

	buffer[pos++] = ((xpre_getInt8(xpre, "voice3_semitone")&0x1F) << 3) | (xpre_getInt8(xpre, "voice3_octave")&0x07);
	buffer[pos++] = ((xpre_getInt8(xpre, "voice3_envelope")&0x03) << 6) | ((xpre_getInt8(xpre, "voice3_source")&0x3F) << 2) | (xpre_getInt8(xpre, "voice3_mode")&0x03);
	buffer[pos++] = ((xpre_getInt8(xpre, "voice3_enable")&0x01) << 4) | ((xpre_getInt8(xpre, "voice3_modulate")&0x01) << 3) | ((xpre_getInt8(xpre, "voice3_tremolo_source")&0x01)<<2) | (xpre_getInt8(xpre, "voice3_destination")&0x03);

/*
	7f voice4_level                                                                               
	00 voice4_pan                                                                                 
	7f voice4_detune                                                                              
	00 voice4_tremolo   
*/
	buffer[pos++] = xpre_getInt32(xpre, "voice4_level");
	
	if (formatWrite&XPRE_FORMAT_WRITE){
		buffer[pos++] = 0;
		buffer[pos++] = 0;			// end row
		
		//buffer[pos++] = 0;	// hid ident
		buffer[pos++] = AS_CMD_ACTIVE_STORE;		// CMD
		buffer[pos++] = 0;
		buffer[pos++] = 0x20;		// store offset
		buffer[pos++] = AS_PAYLOAD_LENGTH;		// LENGTH
	}
	
	
	buffer[pos++] = xpre_getInt32(xpre, "voice4_pan");
	buffer[pos++] = xpre_getInt32(xpre, "voice4_detune");
	buffer[pos++] = xpre_getInt32(xpre, "voice4_tremolo");
	
/*                                                                          
	5c voice4_semitone << 3 | voice4_octave                                                       
	6c voice4_envelope << 6 | voice4_source << 2 | voice4_mode                                    
	02 voice4_enable << 4 | voice4_modulate << 3 | voice4_tremolo_source << 2 | voice4_destination
*/

	buffer[pos++] = ((xpre_getInt8(xpre, "voice4_semitone")&0x1F) << 3) | (xpre_getInt8(xpre, "voice4_octave")&0x07);
	buffer[pos++] = ((xpre_getInt8(xpre, "voice4_envelope")&0x03) << 6) | ((xpre_getInt8(xpre, "voice4_source")&0x3F) << 2) | (xpre_getInt8(xpre, "voice4_mode")&0x03);
	buffer[pos++] = ((xpre_getInt8(xpre, "voice4_enable")&0x01) << 4) | ((xpre_getInt8(xpre, "voice4_modulate")&0x01) << 3) | ((xpre_getInt8(xpre, "voice4_tremolo_source")&0x01)<<2) | (xpre_getInt8(xpre, "voice4_destination")&0x03);

/*
	b3 filter1_depth                                                                   
	8c filter1_frequency                                                               
	00 filter1_q                                                                       
	80 filter1_enable << 7 | filter1_invert << 6 | filter1_envelope << 5 | filter1_type
	15 mix1_enable << 4 | mix1_destination << 2 | filter1_pitch_track
*/
	buffer[pos++] = xpre_getInt32(xpre, "filter1_depth");
	buffer[pos++] = xpre_getInt32(xpre, "filter1_frequency");
	buffer[pos++] = xpre_getInt32(xpre, "filter1_q");
	buffer[pos++] = ((xpre_getInt8(xpre, "filter1_enable")&0x01) << 7) | ((xpre_getInt8(xpre, "filter1_invert")&0x01) << 6) | ((xpre_getInt8(xpre, "filter1_envelope")&0x01) << 5) | (xpre_getInt8(xpre, "filter1_type")&0x0F);
	buffer[pos++] = ((xpre_getInt8(xpre, "mix1_enable")&0x01) << 4) | ((xpre_getInt8(xpre, "mix1_destination")&0x03)<<2) | (xpre_getInt8(xpre, "filter1_pitch_track")&0x03);

/*
	7f filter2_depth                                                                   
	7f filter2_frequency                                                               
	7f filter2_q                                                                       
	00 filter2_enable << 7 | filter2_invert << 6 | filter2_envelope << 5 | filter2_type
	14 mix2_enable << 4 | mix2_destination << 2 | filter2_pitch_track         
*/
	buffer[pos++] = xpre_getInt32(xpre, "filter2_depth");
	buffer[pos++] = xpre_getInt32(xpre, "filter2_frequency");
	buffer[pos++] = xpre_getInt32(xpre, "filter2_q");
	buffer[pos++] = ((xpre_getInt8(xpre, "filter2_enable")&0x01) << 7) | ((xpre_getInt8(xpre, "filter2_invert")&0x01) << 6) | ((xpre_getInt8(xpre, "filter2_envelope")&0x01) << 5) | (xpre_getInt8(xpre, "filter2_type")&0x0F);
	buffer[pos++] = ((xpre_getInt8(xpre, "mix2_enable")&0x01) << 4) | ((xpre_getInt8(xpre, "mix2_destination")&0x03)<<2) | (xpre_getInt8(xpre, "filter2_pitch_track")&0x03);

/*
	b1 envelope1_sensitivity                
	be envelope1_speed                      
	14 envelope1_gate                       
	05 envelope1_input << 4 | envelope1_type
*/

	buffer[pos++] = xpre_getInt8(xpre, "envelope1_sensitivity");
	buffer[pos++] = xpre_getInt8(xpre, "envelope1_speed");
	buffer[pos++] = xpre_getInt8(xpre, "envelope1_gate");
	buffer[pos++] = ((xpre_getInt8(xpre, "envelope1_input")&0x0F) << 4) | (xpre_getInt8(xpre, "envelope1_type")&0x0F);

/*
	00 envelope2_sensitivity                
	00 envelope2_speed                      
	00 envelope2_gate                       
	01 envelope2_input << 4 | envelope2_type
*/
	buffer[pos++] = xpre_getInt8(xpre, "envelope2_sensitivity");
	buffer[pos++] = xpre_getInt8(xpre, "envelope2_speed");
	buffer[pos++] = xpre_getInt8(xpre, "envelope2_gate");
	buffer[pos++] = ((xpre_getInt8(xpre, "envelope2_input")&0x0F) << 4) | (xpre_getInt8(xpre, "envelope2_type")&0x0F);

/*
	fe distortion_drive                        
	fe distortion_mix                          
	00 distortion_output                       
	11 distortion_enable << 4 | distortion_type
*/
	buffer[pos++] = xpre_getInt32(xpre, "distortion_drive");
	buffer[pos++] = xpre_getInt32(xpre, "distortion_mix");
	buffer[pos++] = xpre_getInt32(xpre, "distortion_output");
	buffer[pos++] = ((xpre_getInt8(xpre, "distortion_enable")&0x0F) << 4) | (xpre_getInt32(xpre, "distortion_type")&0x0F);

/*
	2b fm_sine1                                                  
	00 fm_sine2                                                  
	01 fm_sine2_input << 1 | fm_sine1_input                      
	7f mono_pitch_filter1                                        
	98 mono_pitch_filter2 
*/

	buffer[pos++] = xpre_getInt32(xpre, "fm_sine1");
	buffer[pos++] = xpre_getInt32(xpre, "fm_sine2");
	buffer[pos++] = (xpre_getInt8(xpre, "fm_sine2_input") << 1) | (xpre_getInt8(xpre, "fm_sine1_input")&0x01);
	buffer[pos++] = xpre_getInt32(xpre, "mono_pitch_filter1");
	
	if (formatWrite&XPRE_FORMAT_WRITE){
		buffer[pos++] = 0;
		buffer[pos++] = 0;			// end row
		
		//buffer[pos++] = 0; // hid ident
		buffer[pos++] = AS_CMD_ACTIVE_STORE;		// CMD
		buffer[pos++] = 0;
		buffer[pos++] = 0x40;		// address write offset
		buffer[pos++] = AS_PAYLOAD_LENGTH;		// LENGTH
	}

	buffer[pos++] = xpre_getInt32(xpre, "mono_pitch_filter2");

/*
	00 lfo_speed                                                 
	00 lfo_env_to_speed                                          
	00 lfo_env_to_depth                                          
	00 lfo_2_phase                                               
	01 lfo_2_multiply 
	96 lfo_beat_division << 5 | lfo_restart << 4 | lfo_shape     
*/

	buffer[pos++] = xpre_getInt32(xpre, "lfo_speed");
	buffer[pos++] = xpre_getInt32(xpre, "lfo_env_to_speed");
	buffer[pos++] = xpre_getInt32(xpre, "lfo_env_to_depth");
	buffer[pos++] = xpre_getInt32(xpre, "lfo_2_phase");
	buffer[pos++] = xpre_getInt32(xpre, "lfo_2_multiply");
	buffer[pos++] = ((xpre_getInt8(xpre, "lfo_beat_division")&0x07) << 5) | ((xpre_getInt8(xpre, "lfo_restart")&0x01) << 4) | (xpre_getInt8(xpre, "lfo_shape")&0x0F);

/*
	ce  lfo_tempo        & 0xFF
	19 (lfo_tempo >>  8) & 0xFF
	51 (lfo_tempo >> 16) & 0xFF
	01 (lfo_tempo >> 24) & 0xFF
*/
	uint32_t lfo_tempo = xpre_getInt32(xpre, "lfo_tempo");
	buffer[pos++] =  lfo_tempo        & 0xFF;
	buffer[pos++] = (lfo_tempo >>  8) & 0xFF;
	buffer[pos++] = (lfo_tempo >> 16) & 0xFF;
	buffer[pos++] = (lfo_tempo >> 24) & 0xFF;

/*
	0e sequencer1_steps  
	18 sequencer1_value0 
	18 sequencer1_value1 
	18 sequencer1_value2 
	18 sequencer1_value3 
	18 sequencer1_value4 
	18 sequencer1_value5 
	18 sequencer1_value6 
	18 sequencer1_value7 
	18 sequencer1_value8 
	18 sequencer1_value9 
	18 sequencer1_value10
	18 sequencer1_value11
	18 sequencer1_value12
	18 sequencer1_value13
	18 sequencer1_value14
	18 sequencer1_value15
*/

	buffer[pos++] = xpre_getInt32(xpre, "sequencer1_steps");
	buffer[pos++] = xpre_getInt32(xpre, "sequencer1_value0");
	buffer[pos++] = xpre_getInt32(xpre, "sequencer1_value1");
	buffer[pos++] = xpre_getInt32(xpre, "sequencer1_value2");
	buffer[pos++] = xpre_getInt32(xpre, "sequencer1_value3");
	buffer[pos++] = xpre_getInt32(xpre, "sequencer1_value4");
	buffer[pos++] = xpre_getInt32(xpre, "sequencer1_value5");
	buffer[pos++] = xpre_getInt32(xpre, "sequencer1_value6");
	buffer[pos++] = xpre_getInt32(xpre, "sequencer1_value7");
	buffer[pos++] = xpre_getInt32(xpre, "sequencer1_value8");
	buffer[pos++] = xpre_getInt32(xpre, "sequencer1_value9");
	buffer[pos++] = xpre_getInt32(xpre, "sequencer1_value10");
	buffer[pos++] = xpre_getInt32(xpre, "sequencer1_value11");
	buffer[pos++] = xpre_getInt32(xpre, "sequencer1_value12");
	buffer[pos++] = xpre_getInt32(xpre, "sequencer1_value13");
	buffer[pos++] = xpre_getInt32(xpre, "sequencer1_value14");
	buffer[pos++] = xpre_getInt32(xpre, "sequencer1_value15");


/*
	0e sequencer2_steps  
	18 sequencer2_value0 
	18 sequencer2_value1 
	18 sequencer2_value2 
	18 sequencer2_value3 
	18 sequencer2_value4 
	18 sequencer2_value5 
	18 sequencer2_value6 
	18 sequencer2_value7 
	18 sequencer2_value8 
	18 sequencer2_value9 
	18 sequencer2_value10
	18 sequencer2_value11
	18 sequencer2_value12
	18 sequencer2_value13
	18 sequencer2_value14
	18 sequencer2_value15
*/
	
	buffer[pos++] = xpre_getInt32(xpre, "sequencer2_steps");
	buffer[pos++] = xpre_getInt32(xpre, "sequencer2_value0");
	buffer[pos++] = xpre_getInt32(xpre, "sequencer2_value1");
	buffer[pos++] = xpre_getInt32(xpre, "sequencer2_value2");
	
	if (formatWrite&XPRE_FORMAT_WRITE){
		buffer[pos++] = 0;
		buffer[pos++] = 0;			// end row
		
		//buffer[pos++] = 0; // hid ident
		buffer[pos++] = AS_CMD_ACTIVE_STORE;		// CMD
		buffer[pos++] = 1;
		buffer[pos++] = 0x60;		// address write offset
		buffer[pos++] = AS_PAYLOAD_LENGTH;		// LENGTH
	}
	

	buffer[pos++] = xpre_getInt32(xpre, "sequencer2_value3");
	buffer[pos++] = xpre_getInt32(xpre, "sequencer2_value4");
	buffer[pos++] = xpre_getInt32(xpre, "sequencer2_value5");
	buffer[pos++] = xpre_getInt32(xpre, "sequencer2_value6");
	buffer[pos++] = xpre_getInt32(xpre, "sequencer2_value7");
	buffer[pos++] = xpre_getInt32(xpre, "sequencer2_value8");
	buffer[pos++] = xpre_getInt32(xpre, "sequencer2_value9");
	buffer[pos++] = xpre_getInt32(xpre, "sequencer2_value10");
	buffer[pos++] = xpre_getInt32(xpre, "sequencer2_value11");
	buffer[pos++] = xpre_getInt32(xpre, "sequencer2_value12");
	buffer[pos++] = xpre_getInt32(xpre, "sequencer2_value13");
	buffer[pos++] = xpre_getInt32(xpre, "sequencer2_value14");
	buffer[pos++] = xpre_getInt32(xpre, "sequencer2_value15");
	
	
/*	
	64 harmony_tuning                                                          
	13 harmony_interval1 << 4 | harmony_key                                    
	60 harmony_interval2 << 5 | harmony_mode                                   
	50 pitch_detect_low_note << 2 | pitch_detect_mode << 1 | pitch_detect_input
	15 pitch_detect_high_note                                                  
	1c knob1_assign                                                            
	1d knob2_assign                                                            
	40 lfo_midi_clock_sync << 6 | ext_control_enable << 5 | on_off_status << 4 | filter2_correction << 3 | routing_option      
*/
	buffer[pos++] =   xpre_getInt8(xpre, "harmony_tuning");
	buffer[pos++] = ((xpre_getInt8(xpre, "harmony_interval1")&0x0F) << 4) | (xpre_getInt8(xpre, "harmony_key")&0x0F);
	buffer[pos++] = ((xpre_getInt8(xpre, "harmony_interval2")&0x07) << 5) | (xpre_getInt8(xpre, "harmony_mode")&0x1F);	
	buffer[pos++] = ((xpre_getInt8(xpre, "pitch_detect_low_note")&0x3F) << 2) |
	                ((xpre_getInt8(xpre, "pitch_detect_mode")&0x01) << 1)     |
	                 (xpre_getInt8(xpre, "pitch_detect_input")&0x01);
	buffer[pos++] =   xpre_getInt8(xpre, "pitch_detect_high_note");
	buffer[pos++] =   xpre_getInt8(xpre, "knob1_assign");
	buffer[pos++] =   xpre_getInt8(xpre, "knob2_assign");		
	buffer[pos++] = ((xpre_getInt8(xpre, "lfo_midi_clock_sync")&0x03) << 6) | 
	                ((xpre_getInt8(xpre, "ext_control_enable")&0x01) << 5)  |
	                ((xpre_getInt8(xpre, "on_off_status")&0x01) << 4)       | 
	                ((xpre_getInt8(xpre, "filter2_correction")&0x01)<<3)    |
	                 (xpre_getInt8(xpre, "routing_option")&0x07);
	

/*
	62 ext1_source << 6 | ext1_destination                                     
	00 ext1_min                                                                
	fe ext1_max                                                                
	63 ext2_source << 6 | ext2_destination                                     
	00 ext2_min                                                                
	7f ext2_max                                                                
	64 ext3_source << 6 | ext3_destination                                     
	7f ext3_min                                                                
	fe ext3_max  
*/
	buffer[pos++] = ((xpre_getInt8(xpre, "ext1_source")&0x03) << 6) | (xpre_getInt8(xpre, "ext1_destination")&0x03F);
	buffer[pos++] =   xpre_getInt8(xpre, "ext1_min");
	buffer[pos++] =   xpre_getInt8(xpre, "ext1_max");
	buffer[pos++] = ((xpre_getInt8(xpre, "ext2_source")&0x03) << 6) | (xpre_getInt8(xpre, "ext2_destination")&0x03F);
	buffer[pos++] =   xpre_getInt8(xpre, "ext2_min");
	buffer[pos++] =   xpre_getInt8(xpre, "ext2_max");
	buffer[pos++] = ((xpre_getInt8(xpre, "ext3_source")&0x03) << 6) | (xpre_getInt8(xpre, "ext3_destination")&0x03F);
	buffer[pos++] =   xpre_getInt8(xpre, "ext3_min");
	buffer[pos++] =   xpre_getInt8(xpre, "ext3_max");	
	
	buffer[pos++] = 0;
	buffer[pos++] = 0;		// end of row
	
	if (formatWrite&XPRE_FORMAT_WRITE){
		buffer[pos++] = 0;
		buffer[pos++] = 0;		
	}


	if ((formatWrite&XPRE_FORMAT_NAME) && (formatWrite&XPRE_FORMAT_WRITE)){
		char *name = xpre_getStr(xpre, "preset_name");
		if (name){
			if (formatWrite){
				//buffer[pos++] = 0; // hid ident
				buffer[pos++] = AS_CMD_ACTIVE_WRITE;
				buffer[pos++] = presetLocationIdx&0x7F;
				buffer[pos++] = 1;
			}
		
			memset(&buffer[pos], 32, AS_PRESET_NAME_LENGTH);
			memcpy((char*)&buffer[pos], name, strlen(name));
			
			//printf("strlen == %i\n", strlen(name));
			//printf("&buffer[pos] '%s'\n", &buffer[pos]);
			
			pos += AS_PRESET_NAME_LENGTH;
			
			if (formatWrite){
				buffer[pos++] = 0;
				buffer[pos++] = 0;
				buffer[pos++] = 0;
			}
		}
	}else if (formatWrite&XPRE_FORMAT_NAME){
		char *name = xpre_getStr(xpre, "preset_name");
		if (name){
			memset(&buffer[pos], AS_PAYLOAD_LENGTH, AS_PRESET_NAME_LENGTH);
			memcpy((char*)&buffer[pos], name, strlen(name));
			pos += AS_PRESET_NAME_LENGTH;
			
			buffer[pos++] = 0;
			buffer[pos++] = 0;
			buffer[pos++] = 0;
			buffer[pos++] = 0;
		}
	}
	
	
	//printf("%i %i\n", pos, sizeof(as_preset_t));
	return pos;
}

int xpre_preset2bin (xpre_t *xpre, uint8_t *buffer, const uint32_t formatWrite, const int presetLocationIdx)
{
	return xpre_string2bin(xpre, buffer, formatWrite, presetLocationIdx);
}

void xpre_addIndent (xpre_t *xpre)
{
	for (int i = 0; i < xpre->flags.indentCt; i++)
		xpre->buffer[xpre->bufferPos++] = '\t';
	xpre->buffer[xpre->bufferPos] = 0;
}

int xpre_add (xpre_t *xpre, const char *name, const int32_t value)
{
	//printf("%s\n", name);
	
	xpre_addIndent(xpre);
	sprintf(&xpre->buffer[xpre->bufferPos], "<%s>%i</%s>\r\n", name, (int)value, name);

	xpre->bufferPos += strlen(&xpre->buffer[xpre->bufferPos]);
	xpre->buffer[xpre->bufferPos] = 0;	
	return xpre->bufferPos;
}

int xpre_addStr (xpre_t *xpre, const char *name, const char *string)
{
	char *str = trimTrailingWhite(string);
	if (!str) return 0;

	xpre_addIndent(xpre);
	sprintf(&xpre->buffer[xpre->bufferPos], "<%s>%s</%s>\r\n", name, str, name);

	xpre->bufferPos += strlen(&xpre->buffer[xpre->bufferPos]);
	xpre->buffer[xpre->bufferPos] = 0;
	free(str);
	return xpre->bufferPos;
}

int xpre_add_open (xpre_t *xpre, const char *string)
{
	xpre_addIndent(xpre);
	sprintf(&xpre->buffer[xpre->bufferPos], "<%s>\r\n", string);

	xpre->bufferPos += strlen(&xpre->buffer[xpre->bufferPos]);
	xpre->buffer[xpre->bufferPos] = 0;
	xpre->flags.indentCt++;
	return xpre->bufferPos;
}

int xpre_add_close (xpre_t *xpre, const char *string)
{
	xpre->flags.indentCt--;
	xpre_addIndent(xpre);
	sprintf(&xpre->buffer[xpre->bufferPos], "</%s>\r\n", string);

	xpre->bufferPos += strlen(&xpre->buffer[xpre->bufferPos]);
	xpre->buffer[xpre->bufferPos] = 0;
	return xpre->bufferPos;
}

int xpre_bin2string (xpre_t *xpre, void *buffer, const int bufferLen)
{
	
	if (bufferLen != sizeof(as_preset_t)){
		printf("emotional damager\r\n");
		return 0;
	}
	
	as_preset_t *pre = (as_preset_t*)buffer;
	

	xpre_add_open(xpre, "neuro_preset");
	
	xpre_add_open(xpre, "info");
	xpre_add(xpre, "product_id", AS_PRODUCT_C4_SYNTH);
	
	if (pre->name){
#if 0
		if (pre->name[0] == 0xFF)
			xpre_addStr(xpre, "preset_name", "--empty--");
		else
#endif
			xpre_addStr(xpre, "preset_name", (char*)pre->name);
	}
	
	xpre_add_close(xpre, "info");

	xpre_add(xpre, "input1_gain", pre->level.input1_gain);
	xpre_add(xpre, "input2_gain", pre->level.input2_gain);
	xpre_add(xpre, "master_depth", pre->level.master_depth);
	xpre_add(xpre, "mod_source", pre->level.mod_source);
	xpre_add(xpre, "bass", pre->level.bass);
	xpre_add(xpre, "treble", pre->level.treble);
	xpre_add(xpre, "mix", pre->level.mix);
	xpre_add(xpre, "lo_retain", pre->level.lo_retain);
	xpre_add(xpre, "output", pre->level.output);
	xpre_add(xpre, "output_balance", pre->level.output_balance);

	xpre_add(xpre, "voice1_source",   pre->voice[0].source);
	xpre_add(xpre, "voice1_envelope", pre->voice[0].envelope);
	xpre_add(xpre, "voice1_octave",   pre->voice[0].octave);
	xpre_add(xpre, "voice1_semitone", pre->voice[0].semitone);
	xpre_add(xpre, "voice1_mode",     pre->voice[0].mode);
	xpre_add(xpre, "voice1_level",    pre->voice[0].level);
	xpre_add(xpre, "voice1_pan",      pre->voice[0].pan);
	xpre_add(xpre, "voice1_tremolo",  pre->voice[0].tremolo);
	xpre_add(xpre, "voice1_tremolo_source", pre->voice[0].tremolo_source);
	xpre_add(xpre, "voice1_detune",         pre->voice[0].detune);
	xpre_add(xpre, "voice1_modulate",       pre->voice[0].modulate);
	xpre_add(xpre, "voice1_destination",    pre->voice[0].destination);
	xpre_add(xpre, "voice1_enable",         pre->voice[0].enable);

	xpre_add(xpre, "voice2_source",   pre->voice[1].source);
	xpre_add(xpre, "voice2_envelope", pre->voice[1].envelope);
	xpre_add(xpre, "voice2_octave",   pre->voice[1].octave);
	xpre_add(xpre, "voice2_semitone", pre->voice[1].semitone);
	xpre_add(xpre, "voice2_mode",     pre->voice[1].mode);
	xpre_add(xpre, "voice2_level",    pre->voice[1].level);
	xpre_add(xpre, "voice2_pan",      pre->voice[1].pan);
	xpre_add(xpre, "voice2_tremolo",  pre->voice[1].tremolo);
	xpre_add(xpre, "voice2_tremolo_source", pre->voice[1].tremolo_source);
	xpre_add(xpre, "voice2_detune",         pre->voice[1].detune);
	xpre_add(xpre, "voice2_modulate",       pre->voice[1].modulate);
	xpre_add(xpre, "voice2_destination",    pre->voice[1].destination);
	xpre_add(xpre, "voice2_enable",         pre->voice[1].enable);

	xpre_add(xpre, "voice3_source",   pre->voice[2].source);
	xpre_add(xpre, "voice3_envelope", pre->voice[2].envelope);
	xpre_add(xpre, "voice3_octave",   pre->voice[2].octave);
	xpre_add(xpre, "voice3_semitone", pre->voice[2].semitone);
	xpre_add(xpre, "voice3_mode",     pre->voice[2].mode);
	xpre_add(xpre, "voice3_level",    pre->voice[2].level);
	xpre_add(xpre, "voice3_pan",      pre->voice[2].pan);
	xpre_add(xpre, "voice3_tremolo",  pre->voice[2].tremolo);
	xpre_add(xpre, "voice3_tremolo_source", pre->voice[2].tremolo_source);
	xpre_add(xpre, "voice3_detune",         pre->voice[2].detune);
	xpre_add(xpre, "voice3_modulate",       pre->voice[2].modulate);
	xpre_add(xpre, "voice3_destination",    pre->voice[2].destination);
	xpre_add(xpre, "voice3_enable",         pre->voice[2].enable);
	
	xpre_add(xpre, "voice4_source",   pre->voice[3].source);
	xpre_add(xpre, "voice4_envelope", pre->voice[3].envelope);
	xpre_add(xpre, "voice4_octave",   pre->voice[3].octave);
	xpre_add(xpre, "voice4_semitone", pre->voice[3].semitone);
	xpre_add(xpre, "voice4_mode",     pre->voice[3].mode);
	xpre_add(xpre, "voice4_level",    pre->voice[3].level);
	xpre_add(xpre, "voice4_pan",      pre->voice[3].pan);
	xpre_add(xpre, "voice4_tremolo",  pre->voice[3].tremolo);
	xpre_add(xpre, "voice4_tremolo_source", pre->voice[3].tremolo_source);
	xpre_add(xpre, "voice4_detune",         pre->voice[3].detune);
	xpre_add(xpre, "voice4_modulate",       pre->voice[3].modulate);
	xpre_add(xpre, "voice4_destination",    pre->voice[3].destination);
	xpre_add(xpre, "voice4_enable",         pre->voice[3].enable);

	xpre_add(xpre, "filter1_depth", pre->filter[0].depth);
	xpre_add(xpre, "filter1_frequency", pre->filter[0].frequency);
	xpre_add(xpre, "filter1_q", pre->filter[0].q);
	xpre_add(xpre, "filter1_type", pre->filter[0].type);
	xpre_add(xpre, "filter1_invert", pre->filter[0].invert);
	xpre_add(xpre, "filter1_envelope", pre->filter[0].envelope);
	xpre_add(xpre, "filter1_pitch_track", pre->filter[0].pitch_track);
	xpre_add(xpre, "filter1_enable", pre->filter[0].enable);
	xpre_add(xpre, "mix1_destination", pre->filter[0].mix_destination);
	xpre_add(xpre, "mix1_enable", pre->filter[0].mix_enable);	
	
	xpre_add(xpre, "filter2_depth", pre->filter[1].depth);
	xpre_add(xpre, "filter2_frequency", pre->filter[1].frequency);
	xpre_add(xpre, "filter2_q", pre->filter[1].q);
	xpre_add(xpre, "filter2_type", pre->filter[1].type);
	xpre_add(xpre, "filter2_invert", pre->filter[1].invert);
	xpre_add(xpre, "filter2_envelope", pre->filter[1].envelope);
	xpre_add(xpre, "filter2_pitch_track", pre->filter[1].pitch_track);
	xpre_add(xpre, "filter2_enable", pre->filter[1].enable);
	xpre_add(xpre, "mix2_destination", pre->filter[1].mix_destination);
	xpre_add(xpre, "mix2_enable", pre->filter[1].mix_enable);
	
	xpre_add(xpre, "envelope1_speed", pre->envelope[0].speed);
	xpre_add(xpre, "envelope1_sensitivity", pre->envelope[0].sensitivity);
	xpre_add(xpre, "envelope1_gate", pre->envelope[0].gate);
	xpre_add(xpre, "envelope1_type", pre->envelope[0].type);
	xpre_add(xpre, "envelope1_input", pre->envelope[0].input);
	
	xpre_add(xpre, "envelope2_speed", pre->envelope[1].speed);
	xpre_add(xpre, "envelope2_sensitivity", pre->envelope[1].sensitivity);
	xpre_add(xpre, "envelope2_gate", pre->envelope[1].gate);
	xpre_add(xpre, "envelope2_type", pre->envelope[1].type);
	xpre_add(xpre, "envelope2_input", pre->envelope[1].input);
	
	xpre_add(xpre, "distortion_drive", pre->distortion.drive);
	xpre_add(xpre, "distortion_mix", pre->distortion.mix);
	xpre_add(xpre, "distortion_output", pre->distortion.output);
	xpre_add(xpre, "distortion_type", pre->distortion.type);
	xpre_add(xpre, "distortion_enable", pre->distortion.enable);

	xpre_add(xpre, "lfo_speed", pre->lfo.speed);
	xpre_add(xpre, "lfo_env_to_speed", pre->lfo.env_to_speed);
	xpre_add(xpre, "lfo_env_to_depth", pre->lfo.env_to_depth);
	xpre_add(xpre, "lfo_2_phase", pre->lfo.to_phase);
	xpre_add(xpre, "lfo_shape", pre->lfo.shape);
	xpre_add(xpre, "lfo_restart", pre->lfo.restart);
	xpre_add(xpre, "lfo_2_multiply", pre->lfo.to_multiply);
	xpre_add(xpre, "lfo_beat_division", pre->lfo.beat_division);
	xpre_add(xpre, "fm_sine1", pre->fm_sine1);
	xpre_add(xpre, "fm_sine2", pre->fm_sine2);
	xpre_add(xpre, "fm_sine1_input", pre->fm_sine1_input);
	xpre_add(xpre, "fm_sine2_input", pre->fm_sine2_input);
	xpre_add(xpre, "mono_pitch_filter1", pre->mono_pitch_filter1);
	xpre_add(xpre, "mono_pitch_filter2", pre->mono_pitch_filter2);
	xpre_add(xpre, "lfo_tempo", pre->lfo.tempo);
	xpre_add(xpre, "lfo_midi_clock_sync", pre->lfo_midi_clock_sync);


	char temp[64];
	for (int i = 0; i < AS_SEQUENCER_TOTAL; i++){
		snprintf(temp, sizeof(temp), "sequencer%i_steps", i+1);
		xpre_add(xpre, temp, pre->sequencer[i].steps);
		
		for (int j = 0; j < AS_SEQUENCER_POINTS; j++){
			snprintf(temp, sizeof(temp), "sequencer%i_value%i", i+1, j);
			xpre_add(xpre, temp, pre->sequencer[i].value[j]);
		}
	}

	xpre_add(xpre, "pitch_detect_input", pre->pitchdetect.input);
	xpre_add(xpre, "pitch_detect_low_note", pre->pitchdetect.low_note);
	xpre_add(xpre, "pitch_detect_high_note", pre->pitchdetect.high_note);
	xpre_add(xpre, "pitch_detect_mode", pre->pitchdetect.mode);
	xpre_add(xpre, "harmony_key", pre->harmony.key);
	xpre_add(xpre, "harmony_tuning", pre->harmony.tuning);
	xpre_add(xpre, "harmony_mode", pre->harmony.mode);
	xpre_add(xpre, "harmony_interval1", pre->harmony.interval1);
	xpre_add(xpre, "harmony_interval2", pre->harmony.interval2);
	xpre_add(xpre, "knob1_assign", pre->knob1_assign);
	xpre_add(xpre, "knob2_assign", pre->knob2_assign);

	xpre_add(xpre, "ext1_source", pre->ext[0].source);
	xpre_add(xpre, "ext1_destination", pre->ext[0].destination);
	xpre_add(xpre, "ext1_min", pre->ext[0].min);
	xpre_add(xpre, "ext1_max", pre->ext[0].max);
	xpre_add(xpre, "ext2_source", pre->ext[1].source);
	xpre_add(xpre, "ext2_destination", pre->ext[1].destination);
	xpre_add(xpre, "ext2_min", pre->ext[1].min);
	xpre_add(xpre, "ext2_max", pre->ext[1].max);
	xpre_add(xpre, "ext3_source", pre->ext[2].source);
	xpre_add(xpre, "ext3_destination", pre->ext[2].destination);
	xpre_add(xpre, "ext3_min", pre->ext[2].min);
	xpre_add(xpre, "ext3_max", pre->ext[2].max);

	xpre_add(xpre, "ext_control_enable", pre->ext_control_enable);
	xpre_add(xpre, "routing_option", pre->routing_option);
	xpre_add(xpre, "on_off_status", pre->on_off_status);

	xpre_add(xpre, "filter2_correction", pre->filter2_correction);
	int total = xpre_add_close(xpre, "neuro_preset");

	return total;
}

int xpre_append (xpre_t *xpre, const char *name, const char *value)
{
	const int newLen = xpre->bufferLen + strlen(name) + strlen(value) + 64;
	char *temp = (char*)calloc(1, newLen);
	
	snprintf(temp, newLen-1, "%s<%s>%s</%s>\r\n", xpre->buffer, name, value, name);
	if (xpre->buffer) free(xpre->buffer);
	xpre->buffer = temp;
	xpre->bufferLen  = strlen(temp) + 1;
	
	//printf("\n\n####%s####\n\n", temp);
	
	return xpre->bufferLen;
}
