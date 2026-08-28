#ifndef GUARD_GRAFDATA_H
#define GUARD_GRAFDATA_H

#include "c2_hooks.h"
#include "carpocalypse2_types.h"

extern int gGraf_data_index;
extern tGraf_data* gCurrent_graf_data;

extern void C2_HOOK_FASTCALL CalcGrafDataIndex(void);

#define CARPOCALYPSE2_GRAFDATA_H

#include "c2_hooks.h"

#include "carpocalypse2_types.h"


extern tGraf_data gGraf_data[2];

extern int gReal_graf_data_index;

extern float gMap_render_x;

extern float gMap_render_y;

extern float gMap_render_width;

extern float gMap_render_height;

#endif // GUARD_GRAFDATA_H
