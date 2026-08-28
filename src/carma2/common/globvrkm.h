#ifndef CARPOCALYPSE2_GLOBVRKM_H
#define CARPOCALYPSE2_GLOBVRKM_H

#include "c2_hooks.h"

#include "carpocalypse2_types.h"
#include <brender/br_types.h>

extern br_actor* gCamera_list[2];
extern int gCar_flying;
extern tCar_spec* gCar_to_view;
extern tU32 gCar_to_view_id;
extern int gCamera_reset;
extern int gCamera_sign;
extern br_angle gCamera_yaw;
extern br_scalar gCamera_height;
extern br_vector3 gView_direction;
extern br_scalar gMin_camera_car_distance;

#endif //CARPOCALYPSE2_GLOBVRKM_H
