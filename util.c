

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "sa_c4.h"
#include "ctrl_c4.h"
#include "xpre.h"



static const uint8_t ctrlBitmasks[8] = {0x00, 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F};




int util_getActivePreset ()
{
	as_hw_config_t cfg = {0};
	if (!as_getHardwareConfig(NULL, &cfg)){
		printf("as_getHardwareConfig failed\r\n");
		return -1;
	}

	return cfg.activePreset;
}

void util_printActivePresetAndName ()
{
	as_hw_config_t cfg = {0};
	if (!as_getHardwareConfig(NULL, &cfg)){
		printf("as_getHardwareConfig failed\r\n");
		return;
	}

	char presetName[AS_PRESET_NAME_LENGTH+1];
	int ret = as_getPresetName(NULL, cfg.activePreset, presetName);

	if (ret == 1)
		printf("%i: %s\r\n", cfg.activePreset+1, presetName);
	else if (ret == -1)
		printf("%i: --empty--\r\n", cfg.activePreset+1);
}

void util_listCtrls ()
{
	const int tCtrls = ctrl_c4_total();
	
	for (int i = 0; i < tCtrls; i++){
		ctrl_t *ctrl = ctrl_c4_get(i);
		if (ctrl->getIdx == -1) continue;

		printf("%-22s\r\n", ctrl->label);
	}
}

void util_listCtrlValues ()
{
	const int tCtrls = ctrl_c4_total();
	
	for (int i = 0; i < tCtrls; i++){
		ctrl_t *ctrl = ctrl_c4_get(i);
		if (ctrl->getIdx == -1) continue;
		
		as_getControlValue(NULL, ctrl->getIdx, &ctrl->u.val8);
		
		if (ctrl->width < 8){
			ctrl->u.val8 >>= ctrl->bitPosition;
			ctrl->u.val8 &= ctrlBitmasks[ctrl->width];
		}

		printf("%-22s %-3i\r\n", ctrl->label, ctrl->u.val8);
	}
}

void util_getCtrlValue (const char *name)
{
	ctrl_t *ctrl = ctrl_c4_find(name);
	if (ctrl){
		if (ctrl->getIdx == -1){
			printf("not implemented\r\n");
			return;
		}
		uint8_t ctrlIdx = ctrl->getIdx&0xFF;
		uint8_t value8 = 0;
		
		// use ctrlIdx+n for each byte of 16/32bit values
		as_getControlValue(NULL, ctrlIdx, &value8);

		if (ctrl->width < 8){
			value8 >>= ctrl->bitPosition;
			value8 &= ctrlBitmasks[ctrl->width];
		}

		//printf("%s: %i\n", ctrl->label, value8);
		printf("%i\r\n", value8);
	}else{
		printf("Control '%s' not recognised\r\n", name);
	}
}

void util_setCtrlValue (const char *name, const int value)
{
	ctrl_t *ctrl = ctrl_c4_find(name);
	if (ctrl){
		if (ctrl->setIdx == -1){
			printf("not implemented\r\n");
			return;
		}
		
		uint8_t ctrlIdx = ctrl->setIdx&0xFF;
		uint16_t value16 = value&0xFFFF;
		if (ctrl->width < 8)
			value16 &= ctrlBitmasks[ctrl->width];

		as_setControlValue(NULL, ctrlIdx, value16);

		printf("%s set to %i\r\n", ctrl->label, value16);
	}else{
		printf("'%s' not recognised\r\n", name);
	}
}

void util_setPresetActive (const int presetIdx)
{
	if (presetIdx < 0 || presetIdx >= AS_PRESET_TOTAL)
		return;
		
	if (as_setPresetActive(NULL, presetIdx))
		printf("Preset %i selected\r\n", presetIdx+1);
	else
		printf("Preset not set\r\n");
}

void util_setPresetNext ()
{
	
	as_hw_config_t cfg = {0};
	if (!as_getHardwareConfig(NULL, &cfg))
		return;
	
	int nextPreset = cfg.activePreset + 1;
	if (nextPreset < cfg.numberOfPresets){
		
		/*int asret =*/ as_setPresetActive(NULL, nextPreset);
		//printf("as_setPresetActive ret: %i\r\n", asret);
		
		/*asret =*/ as_getHardwareConfig(NULL, &cfg);
		//printf("as_getHardwareConfig ret: %i\r\n", asret);
		
		char presetName[AS_PRESET_NAME_LENGTH+1] = {0};
		int ret = as_getPresetName(NULL, cfg.activePreset, presetName);
		if (ret == 1)
			printf("%i: %s\r\n", cfg.activePreset+1, presetName);
		else if (ret == -1)
			printf("%i: --empty--\r\n", cfg.activePreset+1);
		else
			printf("as_getPresetName() failed %i\r\n", ret);
	}
}

void util_setPresetPrevious ()
{
	
	as_hw_config_t cfg = {0};
	if (!as_getHardwareConfig(NULL, &cfg))
		return;

	int previousPreset = cfg.activePreset - 1;
	if (previousPreset >= 0){
		//printf("Setting Preset: %i\n", previousPreset+1);
		
		as_setPresetActive(NULL, previousPreset);
		as_getHardwareConfig(NULL, &cfg);
		
		char presetName[AS_PRESET_NAME_LENGTH+1];
		int ret = as_getPresetName(NULL, cfg.activePreset, presetName);
		if (ret == 1)
			printf("%i: %s\r\n", cfg.activePreset+1, presetName);
		else if (ret == -1)
			printf("%i: --empty--\r\n", cfg.activePreset+1);
	}
}

void util_getPresetBin (const int presetIdx, as_preset_t *preset)
{
	if (presetIdx < 0 || presetIdx >= AS_PRESET_TOTAL)
		return;
			
	//as_preset_t preset = {0};
	as_getPreset(NULL, (uint8_t*)preset, sizeof(as_preset_t), presetIdx, 1);
	
	//printHex32((uint8_t*)&preset, AS_PRESET_LENGTH);
}

static int isEqual (const as_preset_t *preset1, const as_preset_t *preset2)
{
	uint8_t *buffer1 = (uint8_t*)preset1;
	uint8_t *buffer2 = (uint8_t*)preset2;
	
	for (int i = 0; i < AS_PRESET_LENGTH; i++){
		if (buffer1[i] != buffer2[i])
			return 0;
	}
	return 1;
}

static int isDefault (const int presetIdx)
{
	return ((presetIdx >= 0) && (presetIdx < AS_PRESET_DEFAULTS_TOTAL));
}

static int isValid (const as_preset_t *preset)
{
	uint8_t *buffer = (uint8_t*)preset;
	if ((buffer[AS_PRESET_LENGTH-3] == 0xFF) || (buffer[AS_PRESET_LENGTH-2] == 0xFF))
		return 0;
	else
		return 1;
}

static void applyPresetSeqStepsFixup (as_preset_t *preset)
{
	for (int i = 0; i < AS_SEQUENCER_TOTAL; i++)
		preset->sequencer[i].steps += 2;
}

void util_getPresetPreXml (const int presetIdx)
{
	if (presetIdx < 0 || presetIdx >= AS_PRESET_TOTAL)
		return;

	as_preset_t preset = {0};
	if (as_getPreset(NULL, (uint8_t*)&preset, sizeof(preset), presetIdx, 1) != 1 || !isValid(&preset)){
		printf("Preset %i not available\r\n", presetIdx+1);
		return;
	}	
	
	int bufferLen = 10*1024;			// a C4 preset is ~6.7K. 10k should never not be enough.
	char *buffer = (char*)calloc(1, bufferLen);
	if (!buffer){
		printf("emotional damage\r\n");
		return;
	}

	applyPresetSeqStepsFixup(&preset);
	
	xpre_t xpre;
	xpre_init(&xpre);
	xpre_prepareBuffer(&xpre, buffer, bufferLen);
	xpre_bin2string(&xpre, &preset, sizeof(preset));
	
	printf("\r\n%s\r\n", xpre.buffer);
	free(buffer);
}

void util_printPresetValues (const int presetIdx)
{
	if (presetIdx < 0 || presetIdx >= AS_PRESET_TOTAL)
		return;
			
	as_preset_t preset = {0};
	if (as_getPreset(NULL, (uint8_t*)&preset, sizeof(preset), presetIdx, 1) != 1)
		goto exit;
	
	if (isValid(&preset)){
		applyPresetSeqStepsFixup(&preset);
		util_printPresetStruct(&preset);
		return;
	}
	
exit:
	printf("Preset %i not available\r\n", presetIdx+1);
}

void util_dumpEEPROM ()
{
	uint8_t buffer[AS_EEPROM_SIZE] = {0};
	as_getEEPROM(NULL, buffer, AS_EEPROM_SIZE);
	printHex16(buffer, AS_EEPROM_SIZE);
}

void util_savePresetAs (char *name, const int preIdx)
{
	if (strlen(name) > AS_PRESET_NAME_LENGTH)
		name[AS_PRESET_NAME_LENGTH] = 0;

	if (as_setPresetName(NULL, name, preIdx)){
		printf("Preset saved to %i as '%s'\r\n", preIdx+1, name);
		return;
	}

	printf("Save failed - check input\r\n");
}

void util_renamePreset (char *name, const int preIdx)
{
	if (strlen(name) > AS_PRESET_NAME_LENGTH)
		name[AS_PRESET_NAME_LENGTH] = 0;

	if (as_setPresetActive(NULL, preIdx)){
		if (as_setPresetName(NULL, name, preIdx)){
			printf("Preset %i renamed to '%s'\r\n", preIdx+1, name);
			return;
		}
	}
	printf("Rename failed - check input\r\n");
}

void util_isPresetDuplicate (const int preIdx1, const int preIdx2)
{
	as_preset_t preset1;
	as_preset_t preset2;


	if (preIdx1 == preIdx2){
		printf("Preset %i == %i\r\n", preIdx1+1, preIdx2+1);
		return;
	}

	if ((as_getPreset(NULL, (uint8_t*)&preset1, sizeof(preset1), preIdx1, 0) != 1) || !isValid(&preset1)){
		printf("Preset %i not available\r\n", preIdx1+1);
		return;
	}

	if ((as_getPreset(NULL, (uint8_t*)&preset2, sizeof(preset2), preIdx2, 0) != 1) || !isValid(&preset2)){
		printf("Preset %i not available\r\n", preIdx2+1);
		return;
	}

	if (isEqual(&preset1, &preset2)){
		printf("%i <> %i: identical\r\n", preIdx1+1, preIdx2+1);
		return;
	}else{
		printf("%i <> %i: different\r\n", preIdx1+1, preIdx2+1);
		return;
	}
}

void util_erasePreset (const int preIdx)
{
	printf("Erasing %i... ", preIdx+1);
	
	int ret = as_erase(NULL, preIdx);
	if (ret > 0){
	 	if (isDefault(preIdx)){
			as_preset_t preset;
			if ((as_getPresetDefault(NULL, (uint8_t*)&preset, sizeof(preset), preIdx) == 1)){
				if (isValid(&preset)){
					/*int ret = */as_writePreset(NULL, &preset, AS_PRESET_SIZE, preIdx);
					// printf("util_erasePreset(): as_writePreset ret:%i\r\n", ret);
				}else{
					printf("util_erasePreset(): isValid failed\r\n");
					return;
				}
			}else{
				printf("util_erasePreset(): as_getPresetDefault failed\r\n");
				return;
			}
		}
		printf("complete\r\n");
	}else{
		printf("failed\r\n");
	}
}

void util_getPresetName (const int presetIdx)
{
	if (presetIdx < 0 || presetIdx >= AS_PRESET_TOTAL)
		return;
	
	char presetName[AS_PRESET_NAME_LENGTH+1] = {32};

	int ret = as_getPresetName(NULL, presetIdx, presetName);
	if (ret == 1)
		printf("%s \r\n", rtrim(presetName, AS_PRESET_NAME_LENGTH));
	else if (ret == -1)
		printf("--empty--\r\n");
	else
		printf("util_getPresetName(): as_getPresetName() failed\r\n");
}

void util_finddup (const int preIdx)
{
	as_preset_t preset;
	
	if ((as_getPreset(NULL, (uint8_t*)&preset, sizeof(preset), preIdx, 0) != 1) || !isValid(&preset)){
		printf("Preset %i not available\r\n", preIdx+1);
		return;
	}
	
	int ct = 0;
	as_preset_t hay;
	
	for (int i = 0; i < AS_PRESET_TOTAL; i++){
		if (i == preIdx) continue;
		if ((as_getPreset(NULL, (uint8_t*)&hay, sizeof(hay), i, 0) != 1) || !isValid(&hay))
			continue;
			
		if (isEqual(&preset, &hay)){
			printf("%i <> %i: identical\r\n", preIdx+1, i+1);
			ct++;
		}
	}
	
	if (!ct)
		printf("Preset %i not duplicated\r\n", preIdx+1);
	else
		printf("Search complete\r\n");
}

void util_finddups ()
{
	
	int ct = 0;
	as_preset_t hay[AS_PRESET_TOTAL];	// 20992 bytes
	
	for (int i = 0; i < AS_PRESET_TOTAL; i++){
		if ((as_getPreset(NULL, (uint8_t*)&hay[i], sizeof(as_preset_t), i, 0) != 1) || !isValid(&hay[i]))
			continue;
	}

	for (int i = 0; i < AS_PRESET_TOTAL; i++){
		as_preset_t *preset = &hay[i];
		if (!isValid(preset)) continue;
		
		for (int j = 0; j < AS_PRESET_TOTAL; j++){
			if (j == i) continue;

			if (isEqual(preset, &hay[j])){
				printf("%i <> %i: identical\r\n", i+1, j+1);
				ct++;
			}
		}
	}

	if (!ct)
		printf("No duplicates found\r\n");
	else
		printf("Duplicates found\r\n");
}

void util_copyPreset (const int fromIdx, const int toIdx)
{
	char name[AS_PRESET_NAME_LENGTH+1];
	memset(name, 32, AS_PRESET_NAME_LENGTH);
	name[AS_PRESET_NAME_LENGTH] = 0;

	int ret = as_getPresetName(NULL, fromIdx, name);
	if (ret == 1){
		as_setPresetActive(NULL, fromIdx);
		as_setPresetName(NULL, rtrim(name, AS_PRESET_NAME_LENGTH), toIdx);
		
		printf("Preset %i copied to %i\r\n", fromIdx+1, toIdx+1);
	}else{
		printf("Copy failed - check input\r\n");
	}
}

void util_importPresetAndStore (char *path, const int presetIdx)
{
	xpre_t xpre;
	xpre_init(&xpre);
	int ret = xpre_preparePath(&xpre, path);
	if (!ret){
		printf("file not found or invalid\r\n");
		return;
	}

	uint8_t buffer[AS_PRESET_MAX_LENGTH] = {0};
	int len = xpre_preset2bin(&xpre, buffer, XPRE_FORMAT_STORE, presetIdx);

	if (!(len%AS_PACKET_LENGTH)){
		as_setPresetActive(NULL, presetIdx);
		ret = as_write(NULL, buffer, len);
		if (ret == len) ret = 1;
	}else{
		// should never get here
		printf("util_importPresetAndStore():xpre_preset2bin() - conversion failed\r\n");
	}

	xpre_cleanup(&xpre);
	
	if (ret == 1)
		printf("Import success\r\n");
	else
		printf("Import failed\r\n");
}

void util_importPresetAndSave (char *path, const int presetIdx)
{
	xpre_t xpre;
	xpre_init(&xpre);
	int ret = xpre_preparePath(&xpre, path);
	if (!ret){
		printf("file not found or invalid\r\n");
		return;
	}
	
	uint8_t buffer[AS_PRESET_MAX_LENGTH] = {0};
	int len = xpre_preset2bin(&xpre, buffer, XPRE_FORMAT_SAVE, presetIdx);

	if (!(len%AS_PACKET_LENGTH)){
		as_setPresetActive(NULL, presetIdx);
		ret = as_write(NULL, buffer, len);
		if (ret == len) ret = 1;
	}

	xpre_cleanup(&xpre);
	
	if (ret == 1)
		printf("Import success\r\n");
	else
		printf("Import failed\r\n");
}

