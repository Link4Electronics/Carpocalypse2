#ifndef CARPOCALYPSE2_CRUSH_H
#define CARPOCALYPSE2_CRUSH_H

#include "c2_hooks.h"

#include "c2_stdio.h"

#include "carpocalypse2_types.h"

typedef struct tCar_crush_context {
    tCar_spec* car;
    tCar_crush* crush;
    float force;
} tCar_crush_context;
intptr_t C2_HOOK_CDECL FUN_0043b470(br_actor* pActor, tModel_mash_context* pContext);
int C2_HOOK_CDECL FUN_00434320(br_actor* pActor, int pArg2);
int C2_HOOK_CDECL FindActorByCrushEntryID(br_actor* pActor, void* pArg);
int C2_HOOK_FASTCALL GetNetworkDataSize(tU8* pNet_data);


typedef struct tFind_crush_actor_arg {
    tU8 id;
    br_actor* actor;
} tFind_crush_actor_arg;

typedef struct tSmashable_noncar_action {
    undefined4 field_0x0;
    tU32 random_seed;
    tU16 non_car_index;
    tU8 shrapnel_actor_identifier[2];
    br_vector3 velocity;
    br_vector3 angular_velocity;
    br_matrix34 transform;
    int smoke_chance;
    int smoke_count;
    int smoke_min;
    int smoke_max;
} tSmashable_noncar_action;

extern br_vector3 gBatty_gravity;
extern tU8 gCrush_data_entry_counter;
extern const char* gSoftness_names[6];

float C2_HOOK_FASTCALL NormalizedAngle(tU16 pValue);
float C2_HOOK_FASTCALL ComputeCarImpact(tCar_spec* pCar, int pDelta, float* pOut);
void C2_HOOK_FASTCALL FUN_0042d1e0(tPhysics_object* pObject, int pCount, br_vector3* pArg3, br_vector3* pArg4);
void C2_HOOK_FASTCALL AddDroneCrush(tQueued_drone_crush_item* pItem, const br_vector3* pVector, float pMagnitude, int pCount, const br_vector3* pPoints);
extern const char* gCrush_type_names[3];
extern const char* gEase_of_detachment_names[5];
extern const char* gCar_crush_shape_names[2];
extern const char* gDetach_type_names[4];
extern const char* gPosition_type_names[2];
extern undefined4 gCrush_array_0067a190[10];
extern undefined4 gDAT_00679440;
extern tCrush_info_buffer gDetached_bit_crush_info_buffer;
extern tPhysics_object* gDetached_bit_collision_infos;
extern tDriver gDetached_bit_driver;
extern tPhysics_joint* gTrack_crush_joints[32];
extern tCrush_info_buffer gSplit_car_crush_info_buffer;
extern tPhysics_object* gSplit_car_collision_infos;
extern tDriver gSplit_car_driver;
extern tPhysics_shape* gGonad_sphere_collision_shape;
extern int gCount_toggled_doors;
extern tToggled_door gToggled_doors[16];
extern tCrush_net_detach_list_item gNet_crush_detach_list[8];
extern tCrush_net_semi_detach_bit_list_item gNet_crush_semi_detach_bit_list[8];
extern tCrush_net_full_detach_bit_list_item gNet_crush_full_detach_bit_list[8];
extern tCrush_net_reattach_bit_list_item gNet_crush_reattach_bit_list[8];
extern tCrush_detach_list_item gCrush_detach_list[16];
extern tCar_damge_crush_list_item gCar_damage_crush_list[8];
extern int gCount_crush_detach_list;
extern tU32 gINT_0067be84;
extern int gCount_net_crush_detach_list;
extern int gCount_net_crush_semi_detach_bit_list;
extern int gCount_net_crush_full_detach_bit_list;
extern br_scalar gMax_detach_time_ms;
extern tCrush_list_item gCrush_lists[8];
extern br_scalar gMin_bend_force;
extern int gMin_bend_damage;
extern int gMax_bend_damage;
extern const br_scalar gFloat_00589ae4;
extern const br_scalar gFloat_00589ae8;
extern float gWobble_spam_y[8];
extern float gWobble_spam_z[8];
extern float gWheel_circ_to_width;

typedef struct tCrushNetworkTransformContext {
    br_matrix34 matrix;
    br_scalar corner_0_x;
    br_scalar corner_0_y;
    br_scalar corner_0_z;
    br_scalar corner_1_x;
    br_scalar corner_1_y;
    br_scalar corner_1_z;
    br_scalar corner_2_x;
    br_scalar corner_2_y;
    br_scalar corner_2_z;
    br_scalar corner_3_x;
    br_scalar corner_3_y;
    br_scalar corner_3_z;
    float* crush_data;
    tCar_spec* car;
} tCrushNetworkTransformContext;

void C2_HOOK_FASTCALL FUN_0043a150(br_actor* pActor, float* pCrush_data, short pAngle_x, short pAngle_y, short pAngle_z, tCar_spec* pCar);
void C2_HOOK_FASTCALL FUN_0043aa20(void* pThis, void* pArg2);

void C2_HOOK_FASTCALL FUN_0043d810(tU8* pMessage);

int C2_HOOK_FASTCALL FUN_0043a560(void* pThis, void* pArg2, void* pArg3);

int C2_HOOK_FASTCALL LineBoxColl(br_vector3* pRay_position, br_vector3* pRay_endpoint, br_bounds3* pBounds, br_vector3* pHit_position, float* pFactor);

tFace_ref* C2_HOOK_FASTCALL CastRayDownThruFaces(br_vector3* pPosition, tFace_ref* pFaces, int pFace_count, float pClearance, br_vector3* pHit_pos);

void C2_HOOK_FASTCALL FUN_0043c7a0(br_vector3* pVertex, int pCount, float* pVectors, br_vector3* pDirection, br_scalar pForce, tU32 pFlags, const tCar_crush_limits* pLimits, int pFound);

extern float gUNK_00589fb0;
extern float gUNK_00589fb8;
extern const float gUNK_00589fac;


void C2_HOOK_FASTCALL InitCrushSystems(void);

void C2_HOOK_FASTCALL DamageCarInDirection(tCar_spec* pCar, float* pPosition, float* pImpulse, int pCollision_with_car);
void C2_HOOK_FASTCALL DoPratcamHit(br_vector3* pVelocity);

void C2_HOOK_FASTCALL ClearCrushLists(void);

tCrush_info* C2_HOOK_FASTCALL FUN_0042f170(tCrush_info_buffer* pInfo_buffer, br_actor* pActor, tCar_spec* pCar);

void C2_HOOK_FASTCALL FUN_0042e2e0(tCar_spec* pCar, float* pPlane, int pFlag);
void C2_HOOK_FASTCALL FUN_004f5a60(tCar_spec* pCar);

void C2_HOOK_FASTCALL ResetCrushSystems(void);

void C2_HOOK_FASTCALL ReadCrushSettings(FILE* pF);

void C2_HOOK_FASTCALL LoadMinMax(FILE* pF, br_bounds3* pBounds);

void C2_HOOK_FASTCALL LoadNoncarActivation(FILE* pF, tNoncar_activation** pNoncar_activations, int* pCount_noncar_activations);

void C2_HOOK_FASTCALL ReadAward(FILE* pF, tAward_info* pAward_info);

void C2_HOOK_FASTCALL LoadSmashableLevels(FILE* pF, tSmashable_level** pSmashable_levels, int* pCount_smashable_levels, int pIs_texture_change, tBrender_storage* pBrender_storage);

void C2_HOOK_FASTCALL LoadCarCrushSmashDataEntries(FILE* pF, tCar_crush_buffer_entry* pCar_crush_buffer_entry, tBrender_storage* pBrender_storage);

void C2_HOOK_FASTCALL LoadCarCrushDataEntry(FILE* pF, tCar_crush_buffer_entry* pCar_crush_buffer_entry,tBrender_storage *pBrender_storage);

int C2_HOOK_FASTCALL LoadCarCrush(tCar_crush_buffer* pCar_crush_buffer, const char* pPath, tBrender_storage* pBrender_storage, tCar_crush_spec** pCar_crush_spec);

void C2_HOOK_FASTCALL LinkCarCrushDatas(br_actor *pActor, tCar_crush_buffer *pBuffer);

int C2_HOOK_CDECL LinkCrushData(br_actor* pActor, void* pData);

int C2_HOOK_CDECL LinkCarCrushData(br_actor* pActor, void* data);

int C2_HOOK_CDECL AllocateUserDetailLevel(br_actor* pActor, void* pData);

int C2_HOOK_CDECL LinkCrushModel(br_actor* pActor, void* pData);

intptr_t C2_HOOK_CDECL AccumulateSquashVertices(br_actor* actor, void* pData);

int C2_HOOK_CDECL IncreasingCompare(const void* pValue1, const void* pValue2);

int C2_HOOK_CDECL DecreasingCompare(const void* pValue1, const void* pValue2);

tU16 C2_HOOK_FASTCALL CrushLimitNumber(const br_vector3* pPoint, const tCar_crush_limits* pLimits, const tCar_crush_count_limits* pCount_limits, int* pInvalid);

void C2_HOOK_FASTCALL InitModelMasterCrushData(tCar_spec* pCar_spec);

tU16 C2_HOOK_FASTCALL FindNearestVertex(br_vector3* pPoint, const br_model* pModel);

tU16 C2_HOOK_FASTCALL FindNearestParentVertex(br_actor* pActor, br_vector3* pPos);

void C2_HOOK_FASTCALL SetUpSemiDetachJointStuff(tCar_crush_detach_data* pDetach_data, const br_model* pModel, const br_bounds3* pBounds);

intptr_t C2_HOOK_CDECL InitPhysCrushDataCB(br_actor* actor, void* data);
intptr_t C2_HOOK_CDECL FUN_0043b2e0(br_actor* pActor, tCrush_displacement_context* pContext);
int C2_HOOK_FASTCALL FUN_0043b400(tPhysics_object* pObject, tCrush_displacement_context* pContext);
float C2_HOOK_FASTCALL DoCrushNetwork(tCar_spec* pCar, int pAmount, float* pOut);

void C2_HOOK_FASTCALL InitPhysCrushData(tCar_spec* pCar_spec);

void C2_HOOK_FASTCALL CalculateReferencePoints(br_model* pModel, br_model* pParent_model, tCar_crush_detach_data* pDetach_data, br_actor* pActor);

intptr_t C2_HOOK_FASTCALL SoftnessOfNearestPointCB(br_actor* pActor, br_vector3* pPoint, void* pData);

intptr_t C2_HOOK_FASTCALL DRActorEnumRecurseWithTranslation(br_actor* pActor, br_vector3* pDelta, tDRActorEnumRecurseWithTranslation_cbfn* cbfn, void* pContext);

intptr_t C2_HOOK_FASTCALL DRActorEnumRecurseWithSnart(br_actor* pActor, const br_matrix34 *pMat, tDRActorEnumRecurseWithSnart_cbfn* pCallback, void* pContext);

float C2_HOOK_FASTCALL SoftnessOfNearestPoint(tCar_spec* pCar_spec, br_vector3* pPoint);

void C2_HOOK_FASTCALL SetUpShapeLimitingStuff(tCar_crush_spec* pCar_crush, tCar_spec* pCar_spec);

intptr_t C2_HOOK_CDECL InitModelCrushDataCB(br_actor* actor, void* data);

void C2_HOOK_FASTCALL InitModelCrushData(tCar_spec* pCar_spec);

void C2_HOOK_FASTCALL CopyShapePolyhedron(tCollision_shape_polyhedron* pDest, const tCollision_shape_polyhedron* pSrc);

void C2_HOOK_FASTCALL InitNetworkShapesStuff(tCar_crush_spec* pCar_crush);

void C2_HOOK_FASTCALL InitShapeStuff(tCar_crush_spec* pCar_crush, tPhysics_object* pCollision_info, tCar_spec* pCar_spec);

void C2_HOOK_FASTCALL InitPhysMasterCrushData(tCar_spec* pCar_spec);

float C2_HOOK_FASTCALL PointEdgeDistSq(const br_vector3* pP, const br_vector3* pA, const br_vector3* pB);

float C2_HOOK_FASTCALL CrushShapeDistSq(br_vector3* pPoint, int pCount, br_vector3* pVectors);

float C2_HOOK_FASTCALL FUN_0042ca30(float* pPoint, float* pA, float* pB, float* pC);

float C2_HOOK_FASTCALL PointRectDistSq(const br_vector3* pP, const br_vector3* pA, const br_vector3* pB, const br_vector3* pC);

int C2_HOOK_FASTCALL TooFarToCrush(const br_model* pModel, int pCount, const br_vector3* pVertices, float pMax_dist_sq);

void C2_HOOK_FASTCALL SetFlapCheckVertices(tCar_crush_flap_data *pFlap_data, const br_model* pModel, tModel_detail_vertex_data* pVertex_data);

intptr_t C2_HOOK_CDECL InitVertexDataCB(br_actor* pActor, void* data);

void C2_HOOK_FASTCALL InitVertexData(tCar_spec* pCar_spec);

void C2_HOOK_FASTCALL CheckWheelPositions(const tCar_spec* pCar_spec);

void C2_HOOK_FASTCALL InitPhysModCrushData(tCar_spec * pCar_spec);

void C2_HOOK_FASTCALL MakeModelMaterialsSingleSided(br_model *pModel);

intptr_t C2_HOOK_CDECL MakeCarModelsMaterialsSingleSided(br_actor* pActor, void* pArg);

void C2_HOOK_CDECL JitModelUpdate(br_actor* pActor, br_model* pModel, br_material* pMaterial, void* pOrder_table, br_uint_8 pStyle, int pOn_screen);

void C2_HOOK_FASTCALL FUN_00432b80(br_model* pModel, int pCrush_count, float* pCrush_points, const br_vector3* pDirection, float pForce, const float* pLimits, int pAxis, int pLimit, float* pMax_bend, float* pMax_distance, tU16* pMax_vertex);

void C2_HOOK_FASTCALL SetModelForUpdate(br_model* pModel);

int C2_HOOK_FASTCALL CrushModelVertices(br_model* pModel, tModel_mash_context* pContext);

void C2_HOOK_FASTCALL TotallyRepairModel(br_model* pModel);

intptr_t C2_HOOK_CDECL TotallyRepairModels(br_actor* pActor, void* pUser);

int C2_HOOK_FASTCALL TotallyRepairObject(tPhysics_object* pCollision_info, void* pUser_data);

intptr_t C2_HOOK_CDECL BattenDownTheHatches(br_actor* pActor, void* pUser);

int C2_HOOK_FASTCALL BattenDownTheObjects(tPhysics_object* pCollision_info, void* pUser_data);

void C2_HOOK_FASTCALL TotallyRepairCarCollisionShapes(tCar_spec *pCar_spec);

void C2_HOOK_FASTCALL CompletelyUnBendCollisionShape(tCar_crush_shape_info *pShape);

void C2_HOOK_FASTCALL CompletelyUnBendCollisionShapes(tCar_spec* pCar_spec);

void C2_HOOK_FASTCALL CompletelyUnBendWheels(tCar_spec* pCar_spec);

void C2_HOOK_FASTCALL CompletelyUnBendOtherThings(tCar_spec* pCar_spec);

intptr_t C2_HOOK_CDECL EnableGroovers(br_actor *pActor, void* pData);

int C2_HOOK_FASTCALL MoveCarToSensiblePlace(tCar_spec* pCar_spec);

int C2_HOOK_FASTCALL SwapShapesIfPossible(tCar_spec *pCar_spec);

void C2_HOOK_FASTCALL DoCompletelyUnBentThings(tCar_spec* pCar_spec);

void C2_HOOK_FASTCALL TotallyRepairACar(tCar_spec* pCar_spec);
int C2_HOOK_FASTCALL GetAverageCrushLevel(tCar_spec* pCar);


void C2_HOOK_FASTCALL TotallyRepairCar(void);

void C2_HOOK_FASTCALL WeldCar(tCar_spec* pCar_spec);


void C2_HOOK_FASTCALL ReAttachBit(tCar_spec* pCar, br_actor* pParent_actor, br_actor* pActor, tPhysics_object* pObject);

void C2_HOOK_FASTCALL TotallySpamTheModel(tCar_spec* pCar_spec, float pDamage);
void C2_HOOK_FASTCALL FUN_00435fa0(tCar_spec* pCar);

void C2_HOOK_FASTCALL DoDamage(tCar_spec *pCar, tDamage_type pDamage_type, int pMagnitude);

void C2_HOOK_FASTCALL ApplyRandomDamageToCar(tCar_spec* pCar, int pMinDamage, int pMaxDamage);
void C2_HOOK_FASTCALL FUN_00434f60(br_vector3* pVec, void* pArg2);
void C2_HOOK_FASTCALL FUN_00435110(int* pShape, int pMatrix);
void C2_HOOK_FASTCALL FUN_00435310(tCar_spec* pCar, void* pTransform_context);
void C2_HOOK_FASTCALL FUN_00435550(tCar_spec* pCar, br_actor* pActor, br_angle pAngle_x, br_angle pAngle_y, br_angle pAngle_z, const br_vector3* pArg6, float pArg7, float* pArg8);
void C2_HOOK_FASTCALL FUN_00435960(br_model* pModel, const br_vector3* pOffset, tCar_spec** ppCar, br_actor* pActor);

int C2_HOOK_FASTCALL DoCrashEarnings(tCar_spec* pCar1, tCar_spec* pCar2);

void C2_HOOK_FASTCALL SortOutSmoke(tCar_spec* pCar);

void C2_HOOK_FASTCALL CrashEarnings(tCar_spec* pCar1, tCar_spec* pCar2);

void C2_HOOK_FASTCALL MakeCarAvailable(tCar_spec* pCar);

void C2_HOOK_FASTCALL DoWheelDamage(tU32 pFrame_period);

void C2_HOOK_FASTCALL CrushCar(tCar_spec* pCar, br_actor* pActor, tCar_crush* pCrush);

void C2_HOOK_FASTCALL SmoothCarCrushShapePoints(tCar_spec* pCar, float pStep);

void C2_HOOK_FASTCALL DoSpams(void);

int C2_HOOK_FASTCALL GetCompressedCrushVertexDataSize(tU8* pData);

int C2_HOOK_FASTCALL FUN_0043d900(tCar_spec* pCar);

void C2_HOOK_FASTCALL EncodeNetShapeData(tU8* pField, tCar_spec* pCar);

void C2_HOOK_FASTCALL CompressCrush(tCompressed_car_crush* pDest, const tCar_crush* pSrc);

void C2_HOOK_FASTCALL ExpandCrush(tCar_crush* pDest, const tCompressed_car_crush* pSrc);

void C2_HOOK_FASTCALL DoCrushing(void);

void C2_HOOK_FASTCALL FUN_004321d0(tCar_spec* pCar, br_actor* pActor, const br_vector3* pImpact_direction, br_scalar pImpact_distance, tU32 pVertex_index, tU32 pRandom_seed);
void C2_HOOK_FASTCALL FUN_004ee250(tCar_spec* pCar, tPhysics_object* pCollision_info, br_vector3* pPosition, br_vector3* pOptional_position, br_actor* pActor, undefined4 pArg6, tCar_crush_smashable_part* pSmashable, float pArg8, undefined4 pArg9);

void C2_HOOK_FASTCALL CrushDroneObject(undefined4* pArg1, int pArg2, undefined* pArg3, br_vector3* pArg4);
void C2_HOOK_FASTCALL FUN_0042d400(tCar_spec* pCar, int pPoint_count, const br_vector3* pPoints, const br_vector3* pForce, undefined4 pArg5);

void C2_HOOK_FASTCALL DoDroneCrushing(void);

void C2_HOOK_FASTCALL CrushBendFlapRend(void);

void C2_HOOK_FASTCALL StopFlapping(br_actor* pActor, tCar_spec* pCar_spec);

intptr_t C2_HOOK_CDECL MungeMaterialCB(br_actor* pActor, void* data);

void C2_HOOK_FASTCALL MungeMaterial(br_actor* pActor, int pRecursive, br_material* pMaterial_1, br_material* pMaterial_2, int pCapacity_indices, tU16* pIndices, int* pCount_indices, int pCapacity_vertices, tCar_crush_vertex_data* pVertices, int* pCount_vertices, br_vector3* pNormal);
void C2_HOOK_FASTCALL SendDetachedNonCarDataToPlayer(tNet_game_player_info* pRecipient);


void C2_HOOK_FASTCALL LinkSmashies(br_actor* pActor, tCar_crush_buffer_entry* pCrush_data, tModel_detail_vertex_data* pVertex_data);
void C2_HOOK_FASTCALL FUN_004f5a00(tNon_car_spec* pNon_car);
intptr_t C2_HOOK_CDECL FUN_004f5a10(br_actor* pActor, void* pContext);

int C2_HOOK_FASTCALL DRVector3TestForNan(const br_vector3* pV);
void C2_HOOK_FASTCALL FUN_0043d7d0(tNet_game_player_info* pRecipient);


void C2_HOOK_FASTCALL RemoveCarFromCrushLists(tCar_spec* pCar_spec);
void C2_HOOK_FASTCALL FUN_0043c940(void* pParam);


int C2_HOOK_FASTCALL ShapeRayCast(const br_vector3* p1, const br_vector3* p2, const tPhysics_shape* pShape, br_vector3* pPos, float* pFactor, br_vector3* pNormal);

int C2_HOOK_FASTCALL FUN_004f0d50(const float* pPoint, const float* pNormal, float pPlane_distance, const float* pVertex0, const float* pVertex1, const float* pVertex2);

tU32 C2_HOOK_FASTCALL CastRayAgainstShape(const br_vector3* pRay_start, const br_vector3* pRay_end, const tPhysics_shape* pShape, br_vector3* pPosition, br_vector3* pNormal, float* pFactor);

int C2_HOOK_FASTCALL FUN_00460f60(const br_vector3* pRay_start, const br_vector3* pRay_end, const tCollision_shape_sphere_data* pSphere, br_vector3* pPosition, br_vector3* pNormal, float* pFactor);
int C2_HOOK_FASTCALL FUN_00461310(const br_vector3* pRay_start, const br_vector3* pRay_end, const tCollision_shape_polyhedron_data* pShape, br_vector3* pHit_position, br_scalar pClearance, br_scalar* pFactor);

void C2_HOOK_FASTCALL KnackerThisCar(tCar_spec* pCar);

void C2_HOOK_FASTCALL RecordLastDamage(tCar_spec* pCar);

void C2_HOOK_FASTCALL SetSmokeLastDamageLevel(tCar_spec* pCar);
float C2_HOOK_FASTCALL SmashEnvironment(tPhysics_object* pObject, undefined4* pArg2, float pArg3, br_vector3* pArg4, br_vector3* pArg5, br_vector3* pArg6, int pArg7, int pArg8);


float C2_HOOK_FASTCALL ApplySmashTriggerEffect(tSmashable_item_spec* pSpec, tPhysics_object* pObject, br_actor* pActor, float pArg3, br_vector3* pArg4, br_vector3* pArg5, br_vector3* pArg6, int* pFlag, int pArg8);
void C2_HOOK_FASTCALL ApplySmashTriggerNumber(tPhysics_object* pObject, tFace_ref* pFace, br_scalar pArg3, br_vector3* pArg4, br_vector3* pArg5, br_vector3* pArg6, int pArg7);

float C2_HOOK_FASTCALL BashObject(tPhysics_object* pObject, br_actor* pActor, float pArg3, br_vector3 *pArg4, br_vector3* pArg5, br_vector3* pArg6, int pArg7, int pArg8);

void C2_HOOK_FASTCALL MungeSmashMaterialName(void* pResource, char** ppName, char pIndex);
float C2_HOOK_FASTCALL FUN_004f18d0(tPhysics_object* pObject, void* pArg2_root, br_actor* pActor, tSmashable_item_spec* pSpec1, tSmashable_item_spec* pSpec2, undefined4* pArg2, float pArg3, br_vector3* pArg4, br_vector3* pArg5, br_vector3* pArg6, int pArg11, int pArg12, int* pFlag, int pArg14);
br_actor* C2_HOOK_FASTCALL FUN_004f3820(const void* pArg1, const br_vertex* pVertices, br_material* pMaterial, br_uint_16* pFace_indices, br_uint_16* pVertex_indices);
void C2_HOOK_FASTCALL FUN_004f36c0(const int* pIndices, const float* pPoint, float* pResult);

int C2_HOOK_FASTCALL FUN_004f39a0(tU8* pContext, tU8* pShape_context, float* pVertex_data, tU8* pClip_flags, tU32* pCorner_vertex_addresses, tU32 pSurface_token, tU16* pQuad_indices, tS16* pOutput_indices);

void C2_HOOK_FASTCALL UpdateCrushPainList(void);
void C2_HOOK_FASTCALL ReceivedCrushReattachBit(tNet_message_chunk_crush_reattach_bit* pContents);

void C2_HOOK_FASTCALL SendDetachedNonCarData(tNon_car_spec* pNon_car, undefined4 pArg2, tNet_game_player_info* pRecipient);
void C2_HOOK_FASTCALL ReceivedDetachedNonCarData(tU8* pMessage);
void C2_HOOK_FASTCALL SendSplitHalfNonCarData(tNon_car_spec* pNon_car, undefined4 pArg2, tNet_game_player_info* pRecipient);

void C2_HOOK_FASTCALL ToggleFlapsFromNetwork(tNet_message_chunk* pChunk);

void C2_HOOK_FASTCALL FUN_0043d5f0(const tU8* pMessage);

void C2_HOOK_FASTCALL UpdateCrushTimers(void);
void C2_HOOK_FASTCALL FUN_004f6320(tSmashable_noncar_action* pAction);

void C2_HOOK_FASTCALL SphericizeModel(br_model* pModel, const br_vector3* pCenter, br_scalar pRadius);

void C2_HOOK_FASTCALL SphericizePhysics(tCar_spec* pCar, const br_vector3* pCenter, br_scalar pRadius);

void C2_HOOK_FASTCALL SphericizeCar(tCar_spec* pCar);

void C2_HOOK_FASTCALL ExpandCompressedVector4(float* pDest, const tU16* pSrc);

#endif // CARPOCALYPSE2_CRUSH_H
