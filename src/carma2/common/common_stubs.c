/* Stub implementations for symbols referenced by common/ code that
 * haven't been matched yet. These allow linking to succeed. */

/* Globals */
int gReceived_game_scores;

/* Network stubs */
void DisableNetService(void) { }
void ReenableNetService(void) { }
void NetFinishRace(void) { }
void* NetAllocateMessageChunk(void) { return 0; }
void NetBroadcastContents(void* a, int b) { }
void* NetBuildGuaranteedMessage(void) { return 0; }
void NetStartBroadcastContents(void) { }
int NetGetPlayerStatus(void) { return 0; }
void NetGuaranteedSendMessageToAllPlayers(void* a, int b) { }
void NetGuaranteedSendMessageToEverybody(void* a, int b) { }
void NetGuaranteedSendMessageToHost(void* a, int b) { }
void* NetPlayerFromCar(void* a) { return 0; }
void NetSendHeadupToAllPlayers(void* a) { }
void NetSendHeadupToEverybody(void* a) { }
void NetFullScreenMessage(void* a) { }

/* Sound stubs */
void StartMusicTrack(int a) { }
void MungeEnvironmentalSound(void) { }
void DoEnvSound(int a, int b, int c, int d, float e) { }
