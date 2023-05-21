
#ifndef _CTRL_C4_H
#define _CTRL_C4_H




typedef struct _ctrl {
	char label[32];
	
	int setIdx;
	int getIdx;
		
	uint8_t width;			// variable bit width 
	uint8_t bitPosition;	// .width bits wide beginning at .bitPosition

	union {
		uint8_t val8;
		uint16_t val16;
		uint32_t val32;
		uint64_t val64;
		float valflt;
		double valdbl;
	}u;
}ctrl_t;



int ctrl_c4_total ();
ctrl_t *ctrl_c4_find (const char *name);
ctrl_t *ctrl_c4_get (const int ctrlIdx);


#endif
