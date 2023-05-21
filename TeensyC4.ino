
#include <Arduino.h>
#include "usbh_common.h"
#include "sa_c4.h"




// Important! Do not access device until this has been set
static int deviceReady = 0;







int driver_callback (uint32_t msg, intptr_t *value1, uint32_t value2)
{
	if (msg == USBD_MSG_DEVICEREADY){
		printf("\r\n - C4 Synth ready - \r\n\r\n");
		deviceReady = 1;
	}
	return 1;
}

void setup ()
{
	Serial.begin(2000000);
	while (!Serial); // wait for Arduino Serial Monitor
	printf("Source Audio C4 demo\r\n");
	
	usb_start(AS_PID_C4SYNTH);
	SaC4_setCallbackFunc(driver_callback);
}

void printHWcfg ()
{
	as_hw_config_t cfg;
	memset(&cfg, 0, sizeof(cfg));
		
	if (!as_getHardwareConfig(NULL, &cfg))
		return;
		
	printf("Firmware: %i.%i\r\n", cfg.firmwareVersion&0xFF, cfg.firmwareVersion>>8);
	printf("Device model: %i\r\n", cfg.deviceModel);
	printf("Number of presets: %i\r\n", cfg.numberOfPresets);
	printf("Active preset: %i\r\n", cfg.activePreset+1);
	printf("Preset/WYSIWYG: %i\r\n", cfg.WYSIWYG);
	printf("Hardware bypass mode: %i\r\n", cfg.hardwareBypassMode);
	printf("Midi channel (1 to 16): %i \r\n", cfg.midiChannel+1);
	printf("Ccontrol input option: %i\r\n", cfg.controlInputOption);
	printf("External loop safe mode: %i\r\n", cfg.externalLoopSafeMode);
	printf("Power up state: %i\r\n", cfg.powerUpState);
	printf("Default input routing: %i\r\n", cfg.defaultInputRouting);
	printf("Quick bank select: %i\r\n", cfg.quickBankSelect);
	printf("Neuro hub pedel input calibration Min: %i\r\n", cfg.hubPedelCalMin);
	printf("Neuro hub pedel input calibration Range: %i\r\n", cfg.hubPedelCalRange);
	printf("Control input calibration Min: %i\r\n", cfg.inputCalMin);
	printf("control input calibration Range: %i\r\n", cfg.inputCalRange);
	printf("USB-Midi skip power check: %i\r\n", cfg.usbMidiSkipPowerCheck);	
}

void printPresetNameRange (const int fromIdx, const int toIdx)
{
	char presetName[AS_PRESET_NAME_LENGTH+1];

	for (int i = fromIdx; i <= toIdx; i++){
		int ret = as_getPresetName(NULL, i, presetName);
		if (ret == 1)
			printf(" %.3i: %s\r\n", i+1, rtrim(presetName, AS_PRESET_NAME_LENGTH));
		else if (ret == -1)
			printf(" %.3i: --empty--\r\n", i+1);
		else
			printf("emotional damage\r\n");
	}
}

void printPresetBin (const int presetIdx)
{
	as_preset_t preset = {0};
	util_getPresetBin(presetIdx, &preset);
	
	printHex32((uint8_t*)&preset, sizeof(as_preset_t));	
}

void loop ()
{
	usb_task();
	
	if (!deviceReady){
		delay(1);
		return;
	}
	
	
	if (deviceReady == 1){
		deviceReady = 2;

		printf("Hardware Config: \r\n");
		printHWcfg();
		printf("\r\n\r\n\r\n\r\n\r\n");

#if 1
		printf("Presets: \r\n");
		printPresetNameRange(0, 127);
		printf("\r\n\r\n");
#endif

#if 1
		printf("Current control values of active preset: \r\n");
		util_listCtrlValues();
		printf("\r\n\r\n");
		
		printf("Reading: Output volume level: \r\n");
		util_getCtrlValue("output");
		printf("\r\n\r\n");
		
		printf("Writing: Setting output level to 75: \r\n");
		util_setCtrlValue("output", 75);
		printf("\r\n\r\n");

		printf("Reading: Output volume level: \r\n");
		util_getCtrlValue("output");
		printf("\r\n\r\n");
#endif

#if 1
		// Presets are displayed 1 through 128, but always referenced internally from 0 to 127
		printf("Selecting preset 1: \r\n");
		util_setPresetActive(0);		
		printf("\r\n\r\n");

		printf("Active Preset: \r\n");
		util_printActivePresetAndName();
		printf("\r\n\r\n");
		
		printf("Selecting next preset: \r\n");
		util_setPresetNext();
		printf("\r\n\r\n");

		printf("Active preset is now: \r\n");
		util_printActivePresetAndName();
		printf("\r\n\r\n");
#endif


		int presetIdx;
#if 1
		presetIdx = 2;
		// set active preset from where operations are to be performed
		util_setPresetActive(presetIdx);
		
		printf("Printing binary data for preset %i: \r\n", presetIdx+1);
		printPresetBin(presetIdx);
		printf("\r\n\r\n");
		
		
		printf("Printing .pre (XML) for preset %i: \r\n", presetIdx+1);
		util_getPresetPreXml(presetIdx);
		printf("\r\n\r\n");
		
		
		printf("Printing preset values for preset %i: \r\n", presetIdx+1);
		util_printPresetValues(presetIdx);
		printf("\r\n\r\n");
		
		
		printf("Dumping internal EEPROM: \r\n");
		util_dumpEEPROM();
		printf("\r\n\r\n");
#endif

#if 0
		// Erease a preset
		// Cavet: Preset slots 0 to 5 must be replaced with the default presets upon erasing
		// this is automatically handled by the library
		presetIdx = 5;
		printf("Preset %i name: \r\n", presetIdx+1);
		util_getPresetName(presetIdx);
		printf("\r\n\r\n");
		
		printf("Erasing preset %i \r\n", presetIdx+1);
		util_erasePreset(presetIdx);
		printf("\r\n\r\n");
		
		printf("Preset %i name: \r\n", presetIdx+1);
		util_getPresetName(presetIdx);
		printf("\r\n\r\n");
#endif

#if 0
		// duplicate preset 69 on to 70
		printf("Copying preset @ 70 to 71\r\n");
		util_copyPreset(69, 70);
		printf("\r\n\r\n");
#endif

		presetIdx = 70;
		// Search for copies of this preset
		printf("Searching for duplicates of preset %i..\r\n", presetIdx+1);
		util_finddup(presetIdx);
		printf("\r\n\r\n");
		
		// Search all presets for any duplicates
		printf("Searching for any duplicates..\r\n");
		util_finddups();
		printf("\r\n\r\n");
	
#if 0	
		// Rename a preset
		char name[] = "TeensyTest";
		printf("Renaming preset %i to '%s'...\r\n", presetIdx+1, name);
		util_renamePreset(name, presetIdx);
		printf("Preset %i name: \r\n", presetIdx+1);
		util_getPresetName(presetIdx);
		printf("\r\n\r\n");
#endif


	}
	
	delay(30);
}





