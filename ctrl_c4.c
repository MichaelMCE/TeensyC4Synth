
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "ctrl_c4.h"



static ctrl_t controls_c4[] = {
	{"input1_gain",          0, 0, 8, 0},
	{"input2_gain",          1, 1, 8, 0},
	{"master_depth",         2, 2, 8, 0},
	{"mod_source",           3, 3, 8, 0},
	{"bass",                 4, 4, 8, 0},
	{"treble",               5, 5, 8, 0},
	{"mix",                  6, 6, 8, 0},
	{"lo_retain",            7, 7, 8, 0},
	{"output",               8, 8, 8, 0},
	{"output_balance",       9, 9, 8, 0},

	{"voice1_level",         10, 10, 8, 0},
	{"voice1_pan",           11, 11, 8, 0},
	{"voice1_detune",        12, 12, 8, 0},
	{"voice1_tremolo",       13, 13, 8, 0},
	{"voice1_octave",        14, 14, 3, 0},
	{"voice1_semitone",      15, 14, 5, 3},
	{"voice1_mode",          16, 15, 2, 0},
	{"voice1_source",        17, 15, 4, 2},
	{"voice1_envelope",      18, 15, 2, 6},
	{"voice1_destination",   19, 16, 2, 0},
	{"voice1_tremolo_source",20, 16, 1, 1},
	{"voice1_modulate",      21, 16, 1, 2},
	{"voice1_enable",        22, 16, 4, 4},

	{"voice2_level",         23, 17, 8, 0},
	{"voice2_pan",           24, 18, 8, 0},
	{"voice2_detune",        25, 19, 8, 0},
	{"voice2_tremolo",       26, 20, 8, 0},
	{"voice2_octave",        27, 21, 3, 0},
	{"voice2_semitone",      28, 21, 5, 3},
	{"voice2_mode",          29, 22, 2, 0},
	{"voice2_source",        30, 22, 4, 2},
	{"voice2_envelope",      31, 22, 2, 6},
	{"voice2_destination",   32, 23, 2, 0},
	{"voice2_tremolo_source",33, 23, 1, 1},
	{"voice2_modulate",      34, 23, 1, 2},
	{"voice2_enable",        35, 23, 4, 4},

	{"voice3_level",         36, 24, 8, 0},
	{"voice3_pan",           37, 25, 8, 0},
	{"voice3_detune",        38, 26, 8, 0},
	{"voice3_tremolo",       39, 27, 8, 0},
	{"voice3_octave",        40, 28, 3, 0},
	{"voice3_semitone",      41, 28, 5, 3},
	{"voice3_mode",          42, 29, 2, 0},
	{"voice3_source",        43, 29, 4, 2},
	{"voice3_envelope",      44, 29, 2, 6},
	{"voice3_destination",   45, 30, 2, 0},
	{"voice3_tremolo_source",46, 30, 1, 1},
	{"voice3_modulate",      47, 30, 1, 2},
	{"voice3_enable",        48, 30, 4, 4},

	{"voice4_level",         49, 31, 8, 0},
	{"voice4_pan",           50, 32, 8, 0},
	{"voice4_detune",        51, 33, 8, 0},
	{"voice4_tremolo",       52, 34, 8, 0},
	{"voice4_octave",        53, 35, 3, 0},
	{"voice4_semitone",      54, 35, 5, 3},
	{"voice4_mode",          55, 36, 2, 0},
	{"voice4_source",        56, 36, 4, 2},
	{"voice4_envelope",      57, 36, 2, 6},
	{"voice4_destination",   58, 37, 2, 0},
	{"voice4_tremolo_source",59, 37, 1, 1},
	{"voice4_modulate",      60, 37, 1, 2},
	{"voice4_enable",        61, 37, 4, 4},

	{"distortion_drive",     62, 56, 8, 0},
	{"distortion_mix",       63, 57, 8, 0},
	{"distortion_output",    64, 58, 8, 0},
	{"distortion_type",      65, 59, 4, 0},
	{"distortion_enable",    66, 59, 4, 4},

	{"filter1_depth",        67, 38, 8, 0},
	{"filter1_frequency",    68, 39, 8, 0},
	{"filter1_q",            69, 40, 8, 0},
	{"filter1_type",         70, 41, 5, 0},
	{"filter1_envelope",     71, 41, 1, 5},
	{"filter1_invert",       72, 41, 1, 6},
	{"filter1_enable",       73, 41, 1, 7},
	{"filter1_pitch_track",  74, 42, 2, 0},
	{"mix1_destination",     75, 42, 2, 2},

	{"filter2_depth",        76, 43, 8, 0},
	{"filter2_frequency",    77, 44, 8, 0},
	{"filter2_q",            78, 45, 8, 0},
	{"filter2_type",         79, 46, 5, 0},
	{"filter2_envelope",     80, 46, 1, 5},
	{"filter2_invert",       81, 46, 1, 6},
	{"filter2_enable",       82, 46, 1, 7},
	{"filter2_pitch_track",  83, 47, 2, 0},
	{"mix2_destination",     84, 47, 2, 2},

	{"envelope1_sensitivity",85, 48, 8, 0},
	{"envelope1_speed",      86, 49, 8, 0},
	{"envelope1_gate",       87, 50, 8, 0},
	{"envelope1_type",       88, 51, 4, 0},
	{"envelope1_input",      89, 51, 4, 4},

	{"envelope2_sensitivity",90, 52, 8, 0},
	{"envelope2_speed",      91, 53, 8, 0},
	{"envelope2_gate",       92, 54, 8, 0},
	{"envelope2_type",       93, 55, 4, 0},
	{"envelope2_input",      94, 55, 4, 4},

	{"fm_sine1",             95, 60, 8, 0},
	{"fm_sine2",             96, 61, 8, 0},
	{"fm_sine1_input",       97, 62, 1, 0},
	{"fm_sine2_input",       98, 62, 7, 1},
	{"mono_pitch_filter1",   99, 63, 8, 0},
	{"mono_pitch_filter2",   100, 64, 8, 0},

	{"lfo_speed",            101, 65, 8, 0},
	{"lfo_env_to_speed",     102, 66, 8, 0},
	{"lfo_env_to_depth",     103, 67, 8, 0},
	{"lfo_2_phase",          104, 68, 8, 0},
	{"lfo_2_multiply",       105, 69, 8, 0},
	{"lfo_shape",            106, 70, 4, 0},
	{"lfo_restart",          107, 70, 1, 4},
	{"lfo_beat_division",    108, 70, 3, 5},
	{"lfo_tempo",            -1,  71, 8, 0},  // set only. 32bit - 4 bytes: 71 - 74

	{"sequencer1_steps",     109, 75, 8, 0},
	{"sequencer1_value0",    110, 76, 8, 0},
	{"sequencer1_value1",    111, 77, 8, 0},
	{"sequencer1_value2",    112, 78, 8, 0},
	{"sequencer1_value3",    113, 79, 8, 0},
	{"sequencer1_value4",    114, 80, 8, 0},
	{"sequencer1_value5",    115, 81, 8, 0},
	{"sequencer1_value6",    116, 82, 8, 0},
	{"sequencer1_value7",    117, 83, 8, 0},
	{"sequencer1_value8",    118, 84, 8, 0},
	{"sequencer1_value9",    119, 85, 8, 0},
	{"sequencer1_value10",   120, 86, 8, 0},
	{"sequencer1_value11",   121, 87, 8, 0},
	{"sequencer1_value12",   122, 88, 8, 0},
	{"sequencer1_value13",   123, 89, 8, 0},
	{"sequencer1_value14",   124, 90, 8, 0},
	{"sequencer1_value15",   125, 91, 8, 0},
	{"sequencer2_steps",     126, 92, 8, 0},
	{"sequencer2_value0",    127, 93, 8, 0},
	{"sequencer2_value1",    128, 94, 8, 0},
	{"sequencer2_value2",    129, 95, 8, 0},
	{"sequencer2_value3",    130, 96, 8, 0},
	{"sequencer2_value4",    131, 97, 8, 0},
	{"sequencer2_value5",    132, 98, 8, 0},
	{"sequencer2_value6",    133, 99, 8, 0},
	{"sequencer2_value7",    134, 100, 8, 0},
	{"sequencer2_value8",    135, 101, 8, 0},
	{"sequencer2_value9",    136, 102, 8, 0},
	{"sequencer2_value10",   137, 103, 8, 0},
	{"sequencer2_value11",   138, 104, 8, 0},
	{"sequencer2_value12",   139, 105, 8, 0},
	{"sequencer2_value13",   140, 106, 8, 0},
	{"sequencer2_value14",   141, 107, 8, 0},
	{"sequencer2_value15",   142, 108, 8, 0},

	{"harmony_tuning",       143, 109, 8, 0},
	{"harmony_key",          144, 110, 4, 0},
	{"harmony_mode",         145, 111, 5, 0},
	{"harmony_interval1",    146, 110, 4, 4},
	{"harmony_interval2",    147, 111, 3, 5},

	{"pitch_detect_input",   148, 112, 1, 0},
	{"pitch_detect_low_note",149, 112, 6, 2},
	{"pitch_detect_high_note",150,113, 8, 0},
                                               
	{"knob1_assign",         151, 114, 8, 0},
	{"knob2_assign",         152, 115, 8, 0},

   	{"filter2_correction",   -1,  116, 1, 3},   		//  = 248?
   
	{"ext1_destination",     153, 117, 6, 0},
	{"ext1_source",          154, 117, 2, 6},
	{"ext1_min",             155, 118, 8, 0},
	{"ext1_max",             156, 119, 8, 0},

	{"ext2_destination",     157, 120, 6, 0},
	{"ext2_source",          158, 120, 2, 6},
	{"ext2_min",             159, 121, 8, 0},
	{"ext2_max",             160, 122, 8, 0},

	{"ext3_destination",     161, 123, 6, 0},
	{"ext3_source",          162, 123, 2, 6},
	{"ext3_min",             163, 124, 8, 0},
	{"ext3_max",             164, 125, 8, 0},

	{"ext_control_enable",   165, 116, 1, 5},
   	{"lfo_midi_clock_sync",  166, 116, 2, 6},
	{"pitch_detect_mode",    167, 112, 1, 1},
	{"mix1_enable",          168, 42,  4, 4},
	{"mix2_enable",          169, 47,  4, 4},
	{"routing_option",       170, 116, 3, 0},
	{"on_off_status",        171, 116, 1, 4},
};

int ctrl_c4_total ()
{
	return sizeof(controls_c4) / sizeof(*controls_c4);
}

ctrl_t *ctrl_c4_get (const int ctrlIdx)
{
	if (ctrlIdx >= 0 && ctrlIdx < ctrl_c4_total())
		return &controls_c4[ctrlIdx];
	else
		return NULL;
}

ctrl_t *ctrl_c4_find (const char *name)
{
	const int tCtrls = ctrl_c4_total();
	
	for (int i = 0; i < tCtrls; i++){
		ctrl_t *ctrl = &controls_c4[i];
		if (!strcmp(name, ctrl->label))
			return ctrl;
	}
	return NULL;
}

