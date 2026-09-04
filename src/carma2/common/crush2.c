#include "crush2.h"

#include "car.h"
#include "displays.h"
#include "explosions.h"
#include "funks.h"
#include "globvars.h"
#include "globvrpb.h"
#include "grafdata.h"
#include "loading.h"
#include "mainloop.h"
#include "oil.h"
#include "opponent.h"
#include "sound.h"
#include "spark.h"
#include "structur.h"
#include "utility.h"

// GLOBAL: CARMA2_HW 0x0074d3b0
int gCredits_wasting_car[3];

// GLOBAL: CARMA2_HW 0x0075b930
float gCredits_per_unit_car_damage[3];

// GLOBAL: CARMA2_HW 0x0075b9b0
int gTime_rolling_car[3];

// GLOBAL: CARMA2_HW 0x00761a50
int gCredits_rolling_car[3];

// GLOBAL: CARMA2_HW 0x00761ed0
float gSeconds_per_unit_car_damage[3];

// GLOBAL: CARMA2_HW 0x00763520
int gTime_wasting_car[3];

// GLOBAL: CARMA2_HW 0x00763530
int gTime_per_ped_kill[3];

// CheckLastCar

// KnackerThisCar

// ExplodeMeCarUpMeOldMatey

// FUNCTION: CARMA2_HW 0x0043f730
int C2_HOOK_FASTCALL SetKnackeredFlag(tCar_spec* pCar) {

    if (gNet_mode == eNet_mode_client || pCar->knackered) {
        return 0;
    }

    if (!(pCar->damage_units[eDamage_engine].damage_level >= 99
          || pCar->damage_units[eDamage_transmission].damage_level >= 99
          || pCar->damage_units[eDamage_driver].damage_level >= 99
          || (pCar->damage_units[eDamage_lf_wheel].damage_level >= 99
              && pCar->damage_units[eDamage_rf_wheel].damage_level >= 99
              && pCar->damage_units[eDamage_lr_wheel].damage_level >= 99
              && pCar->damage_units[eDamage_rr_wheel].damage_level >= 99))) {
        return 0;
    }

    Explode(&gExplosion_pix_animation_groups, pCar->car_master_actor, &pCar->car_actor->model->bounds, NULL);

    if (pCar != NULL && pCar->driver == eDriver_local_human) {
        DRS3StartSound(gCar_outlet, gExplosion_sound_id);
    } else {
        DRS3StartSound3D(gCar_outlet, gExplosion_sound_id, &pCar->pos, &gZero_v__car, 1, 255, 0x10000, 0x10000);
    }

    if (!pCar->invulnerable_no_wastage && gNet_mode != eNet_mode_client) {
        if (gNet_mode == eNet_mode_none && pCar != NULL && pCar->driver == eDriver_local_human) {
            RaceCompleted(eRace_over_6);
        }
        pCar->knackered = 1;
        QueueWastedMessage(pCar->index);

        if (gNet_mode == eNet_mode_none) {
            if (GetCarCount(eVehicle_opponent) != 0 && NumberOfOpponentsLeft() == 0) {
                if (gCurrent_graf_data->field_0x210 == 0) {
                    NewTextHeadupSlot(4, 0, 5000, 0xfffffffc, GetMiscString(0xa));
                    RaceCompleted(eRace_over_2);
                } else if (gCurrent_graf_data->field_0x210 == 1) {
                    RaceCompleted(eRace_over_2);
                }
            }
        }
        QueueOilSpill(pCar);

        if (gNet_mode == eNet_mode_none) {
            KillGroovadelic(pCar->index);
            KillFunkotronic(pCar->index);
        }
    }

    if (gNet_mode == eNet_mode_none) {
        if (IRandomBetween(0, 1)) {
            if (gNet_mode == eNet_mode_none) {
                StopCarSmoking(pCar);
                CreateSmokeColumn(pCar, 0, IRandomBetween(0, 0xb), 20000);
            }
        }
        CreateSmokeColumn(pCar, 0, IRandomBetween(0, 0xb), 180000);
    }

    return 1;
}

// DamageUnit2

// RecordLastDamage

// DoDamage

// CalcModifiedLocation

// DoPratcamHit

// DamageSystems

// GetDirection

// SetSmokeLastDamageLevel

// SortOutSmoke

// DoCrashEarnings

// DoWheelDamage

// CrashEarnings

// PercentageCarHealth

// ChristmasLights