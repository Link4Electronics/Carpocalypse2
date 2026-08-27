#ifndef GUARD_72_GRAPHICS_H
#define GUARD_72_GRAPHICS_H

#include "carpocalypse2_types.h"

extern tFrontend_spec gFrontend_GRAPHICS;

extern int C2_HOOK_FASTCALL Graphics_Infunc(tFrontend_spec* pFrontend);
extern int C2_HOOK_FASTCALL Graphics_Outfunc(tFrontend_spec* pFrontend);
extern void C2_HOOK_FASTCALL GetGraphicsSettingsData(tFrontend_spec* pFrontend);
extern void C2_HOOK_FASTCALL SetGraphicsSettingsData(tFrontend_spec* pFrontend);

extern int C2_HOOK_FASTCALL GetCarSimplificationLevel(void);
extern br_scalar C2_HOOK_STDCALL GetYon(void);
extern br_scalar C2_HOOK_STDCALL GetYonFactor(void);
extern tShadow_level C2_HOOK_FASTCALL GetShadowLevel(void);
extern int C2_HOOK_FASTCALL GetSoundDetailLevel(void);
extern int C2_HOOK_FASTCALL GetHowMuchBloodAndSnotToSmearAbout(void);
extern int C2_HOOK_FASTCALL GetSkyTextureOn(void);
extern int C2_HOOK_FASTCALL GetDepthCueingOn(void);
extern int C2_HOOK_FASTCALL GetDronesOn(void);
extern int C2_HOOK_FASTCALL GetAnimalsOn(void);
extern int C2_HOOK_FASTCALL IsItOkayToFireHorribleBallsOfNastyNapalmDeathAtPerfectlyInnocentPassersByAndByInnocentIDoMeanInTheBiblicalSense(void);
extern int C2_HOOK_FASTCALL IsItReallyOKThatWeDontMakeAnyEffortToProtectAnySadFuckersOutThereThatDontWishToSeeInnocentPeopleBlownToBitsByHighExplosiveMinesAndShells(void);

#endif /* GUARD_72_GRAPHICS_H */
