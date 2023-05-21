
#ifndef _SA_C4_H_
#define _SA_C4_H_


#include <Arduino.h>

#define		AS_VID						0x29A4
#define		AS_PID_C4SYNTH				0x0302

#define		AS_CMD_CTRL_SET				0x70		// set a single control value (of the working preset)
#define		AS_CMD_CTRL_GET				0x75		// read a control value from RAM
#define		AS_CMD_ACTIVE_STORE			0x76		// write control values (a preset) to working storage (RAM) area, don't save
#define		AS_CMD_ACTIVE_SET			0x77		// set active (working) preset. pedal copies controls values to RAM as working preset
#define		AS_CMD_ACTIVE_WRITE			0x6E		// save to flash currently active preset. Writes to eprom/flash working control values as a complete preset.
#define		AS_CMD_CONFIG_SET			0x6F		// set hardware config
#define		AS_CMD_CONFIG_GET			0x45		// get hardware config initialization variables (firmware, midi chn, etc..)
#define		AS_CMD_PRESET_ERASE			0x38		// select preset (0x77) then 0x38 to erase
#define		AS_ACK_ERASE				0x37		// sent after AS_CMD_PRESET_ERASE upon erase completion
#define		AS_CMD_FLASH_READ			0x36
#define		AS_CMD_FLASH_WRITE			0x35
#define		AS_CMD_EEPROM_READ			0x80
#define		AS_CMD_EEPROM_WRITE			0x81


/*
Updating firmware process:
From 1.20 to 1.21

0xA0  160  replies with 0x33 51 
0xA1  161  replies with 0x37 55
0xAA  170
0xAC  172
...        total of 768 writes with 32byte payload per packet, for total 24,576 data bytes 
0xAC  172
0xA7  167  replies with 0x35 53
0xA4  164
*/

#define		AS_PRESET_ADDRESS_DEFAULTS	0x03C000		// 6 default presets begin here with a 4096byte seperation
#define		AS_PRESET_ADDRESS_BASE		0x080000		// presets begin here
#define		AS_PRESET_ADDRESS_LENGTH	0x1000			// pitch
#define		AS_PRESET_ADDRESS_NAME		0xA0			// preset name offset location

#define		AS_REPORT_LENGTH			(38)		// 
#define		AS_PACKET_LENGTH			(AS_REPORT_LENGTH/*+1*/)	// report size is 38 + single byte HID report ID (+1 not needed Teensy)
#define		AS_PAYLOAD_LENGTH			(32)					// data length within packet
#define		AS_PRESET_LENGTH			(4*AS_PAYLOAD_LENGTH)	// excludes preset name
#define		AS_PRESET_SIZE				(5*AS_PAYLOAD_LENGTH)	// with preset name
#define		AS_PRESET_MAX_LENGTH		(5*AS_PACKET_LENGTH)	// maximum length of binary preset including name
#define		AS_PRESET_NAME_LENGTH		(32)
#define		AS_PRESET_TOTAL				(128)
#define		AS_PRESET_DEFAULTS_TOTAL	(6)
#define		AS_EEPROM_SIZE				(256)		// Length of C4's EEPROM 

#define		AS_READ_TIMEOUT				(1000)

#define		AS_VOICE_TOTAL				(4)
#define		AS_FILTER_TOTAL				(2)
#define		AS_ENVELOPE_TOTAL			(2)
#define		AS_SEQUENCER_TOTAL			(2)
#define		AS_SEQUENCER_POINTS			(16)
#define		AS_EXT_TOTAL				(3)


#define		AS_PRODUCT_REFLEX			(163)
#define		AS_PRODUCT_HUB              (164)
#define		AS_PRODUCT_MULTIWAVE        (220)
#define		AS_PRODUCT_MULTIWAVE_BASS   (221)
#define		AS_PRODUCT_MANTA            (223)
#define		AS_PRODUCT_STINGRAY         (224)
#define		AS_PRODUCT_DIMENSION        (225)
#define		AS_PRODUCT_ORBITAL          (226)
#define		AS_PRODUCT_OFD              (227)
#define		AS_PRODUCT_OFD_BASS         (228)
#define		AS_PRODUCT_MERCURY          (240)
#define		AS_PRODUCT_LUNAR            (241)
#define		AS_PRODUCT_GEMINI           (242)
#define		AS_PRODUCT_VERTIGO          (243)
#define		AS_PRODUCT_LALADY           (244)
#define		AS_PRODUCT_KINGMAKER        (245)
#define		AS_PRODUCT_AFTERSHOCK       (246)
#define		AS_PRODUCT_TRUE_SPRING      (247)
#define		AS_PRODUCT_SPECTRUM_FILTER  (248)
#define		AS_PRODUCT_C4_SYNTH         (249)
#define		AS_PRODUCT_ULTRAWAVE        (250)
#define		AS_PRODUCT_ULTRAWAVE_BASS   (251)
#define		AS_PRODUCT_NEMESIS          (260)
#define		AS_PRODUCT_VENTRIS          (262)
#define		AS_PRODUCT_COLLIDER         (263)
#define		AS_PRODUCT_EQ2              (270)


#define		AS_C4_FLASHSIZE				0x0FFFFF	// size of user flash storage. 1MB on the C4
#define		AS_C4_STARTUP_WAIT			1500		// milliseconds. Give Pedal time to initiate then wait until signal USBD_MSG_DEVICEREADY raised before communicating

typedef struct _as_product {
	char name[32];			// including NUL
	uint16_t id;
	uint16_t VID;
	uint16_t PID;
}as_product_t; 				// as_product_list



// Source Audio xxxxxxx 
static const as_product_t as_product_list[] = {
	{"Reflex", 				163, AS_VID, 0x0000}, 
	{"Hub", 				164, AS_VID, 0x0000},
	{"Multiwave", 			220, AS_VID, 0x0000},
	{"Multiwave Bass", 		221, AS_VID, 0x0000},
	{"Manta", 				223, AS_VID, 0x0000},
	{"Stingray", 			224, AS_VID, 0x0000},
	{"Dimension", 			225, AS_VID, 0x0000},
	{"Orbital", 			226, AS_VID, 0x0000},
	{"OFD", 				227, AS_VID, 0x0000},
	{"OFD Bass", 			228, AS_VID, 0x0000},
	{"Mercury", 			240, AS_VID, 0x0000},
	{"Lunar", 				241, AS_VID, 0x0000},
	{"Gemini", 				242, AS_VID, 0x0000},
	{"Vertigo", 			243, AS_VID, 0x0000},
	{"LALady", 				244, AS_VID, 0x0000},
	{"Kingmaker", 			245, AS_VID, 0x0000},
	{"Aftershock", 			246, AS_VID, 0x0000},
	{"True Spring", 		247, AS_VID, 0x0000},
	{"Spectrum Filter", 	248, AS_VID, 0x0000},
	{"C4 Synth", 		    AS_PRODUCT_C4_SYNTH, AS_VID, AS_PID_C4SYNTH},
	{"Ultrawave", 			250, AS_VID, 0x0000},
	{"Ultrawave Bass", 		251, AS_VID, 0x0000},
	{"Nemesis", 			260, AS_VID, 0x0000},
	{"Ventris", 			262, AS_VID, 0x0000},
	{"Collider", 			263, AS_VID, 0x0000},
	{"EQ2", 				270, AS_VID, 0x0000}, 
	
	{" ", 					0, 0, 0} 
};


typedef struct __attribute__((__packed__)) _as_hw_config {
	uint16_t firmwareVersion;
	uint8_t deviceModel;
	uint8_t numberOfPresets;
	uint8_t activePreset;
	uint8_t WYSIWYG;
	uint8_t hardwareBypassMode;
	uint8_t midiChannel;
	uint8_t controlInputOption;
	uint8_t externalLoopSafeMode;
	uint8_t powerUpState;
	uint8_t defaultInputRouting;
	uint8_t quickBankSelect;
	uint16_t hubPedelCalMin;
	uint16_t hubPedelCalRange;
	uint16_t inputCalMin;
	uint16_t inputCalRange;
	uint8_t usbMidiSkipPowerCheck;
}as_hw_config_t;

typedef struct __attribute__((__packed__)) _as_preset_level {
	uint8_t input1_gain;
	uint8_t input2_gain;
	uint8_t master_depth;
	uint8_t mod_source;
	uint8_t bass;
	uint8_t treble;
	uint8_t mix;
	uint8_t lo_retain;
	uint8_t output;
	uint8_t output_balance;
}as_preset_level_t;

typedef struct __attribute__((__packed__)) _as_preset_voice {
	uint8_t level;
	uint8_t pan;
	uint8_t detune;
	uint8_t tremolo;
	
	uint8_t octave:3;
	uint8_t semitone:5;

	uint8_t mode:2;
	uint8_t source:4;
	uint8_t envelope:2;
	
	uint8_t destination:2;
	uint8_t tremolo_source:1;
	uint8_t modulate:1;
	uint8_t enable:4;
}as_preset_voice_t;

typedef struct __attribute__((__packed__)) _as_preset_filter {
	uint8_t depth;
	uint8_t frequency;
	uint8_t q;

	uint8_t type:5;
	uint8_t envelope:1;	
	uint8_t invert:1;	
	uint8_t enable:1;	

	uint8_t pitch_track:2;
	uint8_t mix_destination:2;
	uint8_t mix_enable:4;
}as_preset_filter_t;

typedef struct __attribute__((__packed__)) _as_preset_envelope {
	uint8_t sensitivity;
	uint8_t speed;
	uint8_t gate;
	
	uint8_t type:4;
	uint8_t input:4;
}as_preset_envelope_t;

typedef struct __attribute__((__packed__)) _as_preset_distortion {
	uint8_t drive;
	uint8_t mix;
	uint8_t output;
	
	uint8_t type:4;
	uint8_t enable:4;
}as_preset_distortion_t;

typedef struct __attribute__((__packed__)) _as_preset_sequencer {
	uint8_t steps;
	uint8_t value[AS_SEQUENCER_POINTS];
}as_preset_sequencer_t;

typedef struct __attribute__((__packed__)) _as_preset_ext {
	uint8_t destination:6;
	uint8_t source:2;
	
	uint8_t min;
	uint8_t max;
}as_preset_ext_t;

typedef struct __attribute__((__packed__)) _as_preset_lfo {
	uint8_t speed;
	uint8_t env_to_speed;
	uint8_t env_to_depth;
	uint8_t to_phase;
	uint8_t to_multiply;

	uint8_t shape:4;
	uint8_t restart:1;	
	uint8_t beat_division:3;
	
	uint32_t tempo;
}as_preset_lfo_t;

typedef struct __attribute__((__packed__)) _as_preset_harmony {
	uint8_t tuning;
	
	uint8_t key:4;
	uint8_t interval1:4;
		
	uint8_t mode:5;
	uint8_t interval2:3;
}as_preset_harmony_t;

typedef struct __attribute__((__packed__)) _as_preset_pitchdetect {
	uint8_t input:1;
	uint8_t mode:1;
	uint8_t low_note:6;
			
	uint8_t high_note;	
}as_preset_pitchdetect_t;


typedef struct __attribute__((__packed__)) _as_preset {
	as_preset_level_t level;
	as_preset_voice_t voice[AS_VOICE_TOTAL];
	as_preset_filter_t filter[AS_FILTER_TOTAL];
	as_preset_envelope_t envelope[AS_ENVELOPE_TOTAL];
	as_preset_distortion_t distortion;

	uint8_t fm_sine1;
	uint8_t fm_sine2;
	
	uint8_t fm_sine1_input:1;
	uint8_t fm_sine2_input:1;
	uint8_t fm_sine_padding:6;		// unknown
		
	uint8_t mono_pitch_filter1;
	uint8_t mono_pitch_filter2;

	as_preset_lfo_t lfo;
	as_preset_sequencer_t sequencer[AS_SEQUENCER_TOTAL];
	as_preset_harmony_t harmony;
	as_preset_pitchdetect_t pitchdetect;

	uint8_t knob1_assign;
	uint8_t knob2_assign;

	uint8_t routing_option:3;
	uint8_t filter2_correction:1;
	uint8_t on_off_status:1;
	uint8_t ext_control_enable:1;		
	uint8_t lfo_midi_clock_sync:2;
	
	as_preset_ext_t ext[AS_EXT_TOTAL];
	uint8_t unused[2];							// is always set to 0x00 0x00
	
	uint8_t name[AS_PRESET_NAME_LENGTH + 1];	// +1 for NUL
	uint8_t padding[3];
}as_preset_t;




typedef struct _as {
	void *device;
}as_t;


void usb_start (const uint32_t pid);
void usb_task ();


// Primary pedal accessors
int hid_read_timeout (void *unused, uint8_t *buffer, const int len, const int timeMS);
int hid_read (void *unused, void *buffer, size_t len);
int hid_write (void *unused, void *buffer, size_t len);


// Ignore "as_ctx", pass NULL in place.
// Maintained for future environment where multiple SourceAudio pedals are supported
int as_setPresetActive (as_t *as_ctx, const int presetIdx);
int as_getPresetName (as_t *as_ctx, const uint8_t presetIdx, char *name);
int as_getHardwareConfig (as_t *as_ctx, as_hw_config_t *cfg);
int as_getPreset (as_t *as_ctx, uint8_t *buffer, const int bufferLen, int preset, int includeName);
int as_erase (as_t *as_ctx, const uint8_t presetIdx);
int as_setControlValue (as_t *as_ctx, uint8_t ctrl, uint16_t value);
int as_getControlValue (as_t *as_ctx, uint8_t ctrl, uint8_t *value);
int as_getVersion (as_t *as_ctx);
int as_getActivePreset (as_t *as_ctx);
int as_writePreset (as_t *as_ctx, as_preset_t *preset, const int bsize, const int presetIdx);
int as_getEEPROM (as_t *as_ctx, uint8_t *buffer, const int bsize);
int as_inspectFlash (as_t *as_ctx, const size_t address, const size_t length);
int as_dumpFlash (as_t *as_ctx);
int as_setPresetName (as_t *as_ctx, char *name, const int presetIdx);
int as_getPresetDefault (as_t *as_ctx, uint8_t *buffer, const size_t bufferLen, int preset);

int as_write (as_t *as_ctx, const void *buffer, const int bLength);
int as_read (as_t *as_ctx, void *buffer, const int bLength);

void printHex16 (const uint8_t *data, int length);
void printHex32 (const uint8_t *data, int length);
void dumpASCII (const uint8_t *data, int length);
void dumpHex (const uint8_t *data, int length);
char *rtrim (char *buffer, int len);


// print a list of recognised controls
void util_listCtrls ();

// retrieve value of control
void util_getCtrlValue (const char *name);

// set a control value
void util_setCtrlValue (const char *name, const int value);

// print active controls' values
void util_listCtrlValues ();

// print active Preset wthn its name
void util_printActivePresetAndName ();

void util_getPresetName (const int presetIdx);

// set this preset the current and active preset
// do this before performing Ops
void util_setPresetActive (const int presetIdx);

// go to and set next/previous preset active
// any unsaved control adjustments will be lost upon preset change
void util_setPresetPrevious ();
void util_setPresetNext ();

// retrieve binary preset data [from pedal]
void util_getPresetBin (const int presetIdx, as_preset_t *preset);

// print preset in .pre (xml) formatted output
void util_getPresetPreXml (const int presetIdx);

// similar to util_listCtrlValues()
// prints control values of select preset
void util_printPresetValues (const int presetIdx);

void util_dumpEEPROM ();

// save current preset (control) values as a new preset
// this will save whatever adjustments have been made the active preset as a new preset, or overwrite current preset
void util_savePresetAs (char *name, const int preIdx);

// rename a preset
// any adjusted control value will be lost
void util_renamePreset (char *name, const int preIdx);

// Copies a preset from one location to another
void util_copyPreset (const int fromIdx, const int toIdx);

// returns active preset (0 - 127)
// -1 on failure
int util_getActivePreset ();

// check if preIdx1 is a copy of preIdx2
// does not compare names
void util_isPresetDuplicate (const int preIdx1, const int preIdx2);

// Dumps to Serial contents of internal preset structure
void util_printPresetStruct (as_preset_t *preset);

// Erase preset location preIdx
// presets 0 through 5 will be replaced with their respective default preset upon deletion
void util_erasePreset (const int preIdx);

// Search for duplicated presets
void util_finddup (const int preIdx);
void util_finddups ();


#if 0
// enable this once SdFat has been implemented
// xpre_loadFile.c::xpre_loadFile() should be modified to suit your environment

// Import a SourceAudio .pre file directly from file to pedal and save
void util_importPresetAndSave (char *path, const int presetIdx);

// Import a SourceAudio .pre file directly from file to pedal and store - don't save
// Preset will be lost on power loss or selection of another perset
void util_importPresetAndStore (char *path, const int presetIdx);

#endif


#endif



