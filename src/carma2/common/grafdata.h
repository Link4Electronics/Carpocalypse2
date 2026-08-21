#ifndef CARPOCALYPSE2_GRAFDATA_H
#define CARPOCALYPSE2_GRAFDATA_H

#include "c2_hooks.h"

#include "carpocalypse2_types.h"

extern tGraf_data gGraf_data[2];
extern tGraf_data* gCurrent_graf_data;
extern int gReal_graf_data_index;
extern int gGraf_data_index;
extern float gMap_render_x;
extern float gMap_render_y;
extern float gMap_render_width;
extern float gMap_render_height;

void C2_HOOK_FASTCALL CalcGrafDataIndex(void);

#endif // CARPOCALYPSE2_GRAFDATA_H