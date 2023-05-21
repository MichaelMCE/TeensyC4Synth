
#ifndef _SA_UTIL_PRINTPRESETSTRUCT_H
#define _SA_UTIL_PRINTPRESETSTRUCT_H

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "sa_c4.h"




void util_printPresetStruct (as_preset_t *preset)
{	
	printf("Name: %s\r\n", (char*)preset->name);
	printf(" Levels:\r\n");
	
	printf("input1_gain    %i\r\n", (int)preset->level.input1_gain);
	printf("input2_gain    %i\r\n", (int)preset->level.input2_gain);
	printf("master_depth   %i\r\n", (int)preset->level.master_depth);
	printf("mod_source     %i\r\n", (int)preset->level.mod_source);
	printf("bass           %i\r\n", (int)preset->level.bass);
	printf("treble         %i\r\n", (int)preset->level.treble);
	printf("mix            %i\r\n", (int)preset->level.mix);
	printf("lo_retain      %i\r\n", (int)preset->level.lo_retain);
	printf("output         %i\r\n", (int)preset->level.output);
	printf("output_balance %i\r\n", (int)preset->level.output_balance);
	printf("\r\n");
	
	for (int i = 0; i < AS_VOICE_TOTAL; i++){
		printf(" Filter: %i\r\n", i+1);
		printf("depth           %i\r\n", (int)preset->filter[i].depth); // GCC BUG, ignore: "warning: iteration 2 invokes undefined behavior" GCC BUG
		printf("frequency       %i\r\n", (int)preset->filter[i].frequency);
		printf("q               %i\r\n", (int)preset->filter[i].q);
		printf("type            %i\r\n", (int)preset->filter[i].type);
		printf("envelope        %i\r\n", (int)preset->filter[i].envelope);
		printf("invert	        %i\r\n", (int)preset->filter[i].invert);
		printf("enable	        %i\r\n", (int)preset->filter[i].enable);
		printf("pitch_track     %i\r\n", (int)preset->filter[i].pitch_track);
		printf("mix_destination %i\r\n", (int)preset->filter[i].mix_destination);
		printf("mix_enable      %i\r\n", (int)preset->filter[i].mix_enable);
		printf("\r\n");
	}
	
	for (int i = 0; i < AS_ENVELOPE_TOTAL; i++){
		printf(" Envelope: %i\r\n", i+1);
		printf("sensitivity    %i\r\n", (int)preset->envelope[i].sensitivity);
		printf("speed          %i\r\n", (int)preset->envelope[i].speed);
		printf("gate           %i\r\n", (int)preset->envelope[i].gate);
		printf("type;          %i\r\n", (int)preset->envelope[i].type);
		printf("input          %i\r\n", (int)preset->envelope[i].input);      
		printf("\r\n");
	}

	printf(" Distortion:\r\n");
	printf("drive             %i\r\n", (int)preset->distortion.drive); 
	printf("mix               %i\r\n", (int)preset->distortion.mix);
	printf("output            %i\r\n", (int)preset->distortion.output);
	printf("type              %i\r\n", (int)preset->distortion.type);
	printf("enable            %i\r\n", (int)preset->distortion.enable);
	printf("\r\n");
	
	printf("fm_sine1           %i\r\n", (int)preset->fm_sine1);
	printf("fm_sine2           %i\r\n", (int)preset->fm_sine2);
	printf("fm_sine1_input     %i\r\n", (int)preset->fm_sine1_input);
	printf("fm_sine2_input     %i\r\n", (int)preset->fm_sine2_input);
	printf("mono_pitch_filter1 %i\r\n", (int)preset->mono_pitch_filter1);
	printf("mono_pitch_filter2 %i\r\n", (int)preset->mono_pitch_filter2);
	printf("\r\n");


	printf(" LFO:\r\n");
	printf("speed             %i\r\n", (int)preset->lfo.speed);
	printf("env_to_speed      %i\r\n", (int)preset->lfo.env_to_speed);
	printf("env_to_depth      %i\r\n", (int)preset->lfo.env_to_depth);
	printf("to_phase          %i\r\n", (int)preset->lfo.to_phase);
	printf("to_multiply       %i\r\n", (int)preset->lfo.to_multiply);
	printf("shape             %i\r\n", (int)preset->lfo.shape);
	printf("restart           %i\r\n", (int)preset->lfo.restart);
	printf("beat_division     %i\r\n", (int)preset->lfo.beat_division);
	printf("tempo             %i\r\n", (int)preset->lfo.tempo);
	printf("\r\n");
	
	for (int i = 0; i < AS_SEQUENCER_TOTAL; i++){
		printf(" Sequencer: %i\r\n", i+1);
		printf("steps          %i\r\n", (int)preset->sequencer[i].steps);
		
		for (int j = 0; j < preset->sequencer[i].steps; j++)
			printf("value %i   \t%i\r\n", j+1, preset->sequencer[i].value[j]);
		printf("\r\n");
	}

	printf(" Harmony:\r\n");
	printf("tuning         %i\r\n", (int)preset->harmony.tuning);
	printf("key            %i\r\n", (int)preset->harmony.key);
	printf("interval1      %i\r\n", (int)preset->harmony.interval1);
	printf("mode           %i\r\n", (int)preset->harmony.mode);
	printf("interval2      %i\r\n", (int)preset->harmony.interval2);
	printf("\r\n");


	printf(" Pitch detect:\r\n");
	printf("input          %i\r\n", (int)preset->pitchdetect.input);
	printf("mode           %i\r\n", (int)preset->pitchdetect.mode);
	printf("low_note       %i\r\n", (int)preset->pitchdetect.low_note);
	printf("high_note      %i\r\n", (int)preset->pitchdetect.high_note);
	printf("\r\n");

	printf(" General:\r\n");
	printf("knob1_assign        %i\r\n", (int)preset->knob1_assign);
	printf("knob2_assign        %i\r\n", (int)preset->knob2_assign);
	printf("routing_option      %i\r\n", (int)preset->routing_option);
	printf("filter2_correction  %i\r\n", (int)preset->filter2_correction);
	printf("on_off_status       %i\r\n", (int)preset->on_off_status);
	printf("ext_control_enable  %i\r\n", (int)preset->ext_control_enable);
	printf("lfo_midi_clock_sync %i\r\n", (int)preset->lfo_midi_clock_sync);
	printf("\r\n");

	for (int i = 0; i < AS_EXT_TOTAL; i++){
		printf(" Ext: %i\r\n", i+1);
		printf("destination %i\r\n", (int)preset->ext[i].destination);
		printf("source      %i\r\n", (int)preset->ext[i].source);
		printf("min         %i\r\n", (int)preset->ext[i].min);
		printf("max         %i\r\n", (int)preset->ext[i].max);
		printf("\r\n");
	}
}



#endif
