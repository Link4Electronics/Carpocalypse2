#ifndef CARPOCALYPSE2_FLAP_H
#define CARPOCALYPSE2_FLAP_H

#include "c2_hooks.h"

#include "carpocalypse2_types.h"

struct tCollision_contact;
#include "physics.h"


void C2_HOOK_FASTCALL ComputeSplitEdgeIntersections(br_vector3* pPositive_intersection, br_vector3* pNegative_intersection, const br_vector3* pPositive_point, const br_vector3* pNegative_point, float pPositive_distance, float pNegative_distance);
void C2_HOOK_FASTCALL FUN_00430150(tU16 pVertex_a, tU16 pVertex_b, tU16* pOutput_a, tU16* pOutput_b, float* pDistances);

int C2_HOOK_FASTCALL FUN_00430e30(tPhysics_joint_data* pJoint_data, float* pForces, int pCount);
int C2_HOOK_FASTCALL TrySplitCarOnVerticalForce(tPhysics_joint_data* pJoint_data, float* pForces, int pCount);
int C2_HOOK_FASTCALL FullyDetachSnappedBits(struct tCollision_contact* pContacts, float* pImpulses, int pCount);
int C2_HOOK_FASTCALL FUN_00430d80(float* pAngle, undefined4 pArg2, float pStep, float pStart, float pTarget);
void C2_HOOK_FASTCALL FUN_0042f5d0(br_model* pSource_model, br_model** pModel_out1, br_model** pModel_out2, float* pPlane, int pArg5);

void C2_HOOK_FASTCALL SplitPolyhedronByPlane(int* pSource, int pPositive_output, int pNegative_output, float* pPlane);

tPhysics_object* C2_HOOK_FASTCALL DetachBit(tCar_spec* pCar, br_actor* pActor, br_bounds3* pBounds);

void C2_HOOK_FASTCALL InitDetachedBitPhysicsObject(tPhysics_object* pObject, br_actor* pActor, br_model* pModel, tCar_crush_detach_data* pDetach_data, tCar_crush_spec* pCar_crush, const br_bounds3* pBounds, tPhysics_object* pParent_collision_info);

tPhysics_object* C2_HOOK_FASTCALL SemiDetachBit(tCar_spec* pCar, br_actor* pActor, float pArg3, int* pArg4, br_vector3* pArg5, br_bounds3* pArg6, br_vector3* pArg7, br_vector3* pArg8);

void C2_HOOK_FASTCALL FUN_00433b90(br_model* pModel, const br_vector3* pOffset);

void C2_HOOK_FASTCALL FUN_00437480(undefined4* pOut, int pDetach_data, int pFace_data, int pModel, undefined4 pPhysics_object, undefined4* pPoint);

void C2_HOOK_FASTCALL SetUpFlapJoint(tPhysics_joint* pJoint, tCar_crush_flap_data* pFlap_data, br_model* pModel, tPhysics_object* pObject, br_vector3* pPoint1, br_vector3* pPoint2, br_vector3* pPoint3);

int C2_HOOK_FASTCALL GetSDBJointPosAndBounds(br_vector3* pP1, br_vector3* pP2, br_vector3* pP33, br_bounds3* pBounds, br_actor* pActor);

void C2_HOOK_FASTCALL SendSemiDetachBit(tCar_spec* pCar, br_actor* pActor, float pArg3, undefined4* pArg4);

void C2_HOOK_FASTCALL FUN_0042da20(undefined4* pRecipient);

int C2_HOOK_FASTCALL BitIsInBentPartOfCar(br_actor* pActor, float pArg2, float pArg3);

void C2_HOOK_FASTCALL DoDetaching(void);

void C2_HOOK_FASTCALL SetBitForDetachment(br_actor* pActor, tCar_spec* pCar, float pArg3, int pArg4);

void C2_HOOK_FASTCALL AddToggledDoor(br_actor* pActor, tCar_spec* pCar, int pFlag, float pDelay);

void C2_HOOK_FASTCALL MakeModelMaterialsDoubleSided(br_model* pModel);
intptr_t C2_HOOK_CDECL MakeCarModelsMaterialsDoubleSided(br_actor* pActor, void* pArg);

intptr_t C2_HOOK_FASTCALL FlapBit(br_actor* pActor, br_matrix34* pMat, void* pContext);

void C2_HOOK_FASTCALL FlapBits(tCar_spec* pCar);

void C2_HOOK_FASTCALL DoFlapping(void);

int C2_HOOK_FASTCALL BitObjectIsSufficientlyOutsideCarObjectToDetach(tPhysics_object* pObject);

void C2_HOOK_FASTCALL SendFullyDetachBit(tCar_spec* pCar, tPhysics_object* pObject);

void C2_HOOK_FASTCALL MungeDetachLists(tCar_crush_spec* pCar_crush);

void C2_HOOK_FASTCALL FullyDetachBit(tCar_spec *pCar, tPhysics_object* pObject);

void C2_HOOK_FASTCALL DoFullyDetaching(void);


void C2_HOOK_FASTCALL BendCar(tCar_spec* pCar, br_angle pAngle_x, br_angle pAngle_y, br_angle pAngle_z, const br_vector3* pArg5, float pArg6, int pArg7);
int C2_HOOK_FASTCALL BendCarOneForce(tCar_spec* pCar, tCar_crush* pCar_crush);


br_angle C2_HOOK_FASTCALL QuantizeAngle(br_angle pAngle, br_angle pMin, br_angle pMax, br_angle pQuant);


void C2_HOOK_FASTCALL DoBending(void);


#endif
