#include "win32net.h"

#include "win32.h"

#include "errors.h"
#include "globvrpb.h"
#include "network.h"
#include "platform.h"

#include "c2_string.h"

#include <winsock.h>
#include <wsipx.h>

#define BROADCAST_HEADER "CAR2MSG"
#define JOINABLE_GAMES_CAPACITY 16


// GLOBAL: CARMA2_HW 0x006b3380
char gPathNetworkIni[240];

// GLOBAL: CARMA2_HW 0x006ad4c4
int gPathNetworkIniValid;

// GLOBAL: CARMA2_HW 0x006ac12c
tU16 gIpx_sock_addr;

// GLOBAL: CARMA2_HW 0x006ac310
struct sockaddr gLocal_address;

// GLOBAL: CARMA2_HW 0x006abf08
struct sockaddr gLocal_address_2;

// GLOBAL: CARMA2_HW 0x006ac358
struct sockaddr* gPtr_local_address; /* FIXME: rename to gLocal_addr_ipx? */

// GLOBAL: CARMA2_HW 0x006ac118
struct sockaddr gListen_address;

// GLOBAL: CARMA2_HW 0x006ac340
struct sockaddr* gPtr_listen_address;  /* FIXME: rename to gRemote_addr_ipx? */

// GLOBAL: CARMA2_HW 0x006ac300
struct sockaddr gBroadcast_address;

// GLOBAL: CARMA2_HW 0x006ac348
struct sockaddr gLast_received_addr;

// GLOBAL: CARMA2_HW 0x006abf00
struct sockaddr* gPtr_broadcast_address;

// GLOBAL: CARMA2_HW 0x006ac170
WSADATA gWSA_data;

// GLOBAL: CARMA2_HW 0x006619b0
SOCKET gSocket = SOCKET_ERROR;

// GLOBAL: CARMA2_HW 0x006ac320
char gLocal_address_text[32];

// GLOBAL: CARMA2_HW 0x006ac128
int gNumber_of_networks;

// GLOBAL: CARMA2_HW 0x006abf04
int gMsg_header_strlen;

// GLOBAL: CARMA2_HW 0x006ac578
int gSpecial_server;

// GLOBAL: CARMA2_HW 0x006ac130
char gIpx_network_numbers [16][4];

// GLOBAL: CARMA2_HW 0x006ac360
char gSend_buffer[512];

// GLOBAL: CARMA2_HW 0x006abf18
char gReceive_buffer[512];

// GLOBAL: CARMA2_HW 0x006ac35c
int gNumber_of_hosts;

// GLOBAL: CARMA2_HW 0x006ac574
tPD_net_game_info* gJoinable_games;

#ifdef CARPOCALYPSE2_MATCHING
extern tNet_message* C2_HOOK_FASTCALL NetAllocateMessage(int pSize);
extern int C2_HOOK_FASTCALL NetDisposeMessage(tNet_game_details* pDetails, tNet_message* pMessage);
extern tU32 C2_HOOK_FASTCALL PDGetTotalTime(void);
extern void C2_HOOK_CDECL dr_dprintf();
extern tNet_mode gNet_mode;
#endif
// FUNCTION: CARMA2_HW 0x0051a190
void C2_HOOK_FASTCALL PDNetObtainSystemUserName(char* pName, int pMax_length) {
    char buffer[16];
    DWORD size = sizeof(buffer);
    char *p;

    dr_dprintf("PDNetObtainSystemUserName()");

    if (GetComputerNameA(buffer, &size) && size != 0) {
        strncpy(pName, buffer, pMax_length - 1);
        pName[pMax_length - 1] = '\0';
        while ((p = strpbrk(pName, "_=(){}[]<>!$%^&*/:@~;'#,?\\|`\"")) != NULL) {
            *p = '-';
        }
    }
}

// FUNCTION: CARMA2_HW 0x0051a010
void C2_NAKED C2_HOOK_FASTCALL NetNowIPXLocalTarget2String(char* pString, struct sockaddr* pSock_addr) {
#ifndef CARPOCALYPSE2_MATCHING

    sprintf(pString,
        "%2.2x%2.2x%2.2x%2.2x:%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x,%2.2x%2.2x",
        pSock_addr->sa_data[0],
        pSock_addr->sa_data[1],
        pSock_addr->sa_data[2],
        pSock_addr->sa_data[3],
        pSock_addr->sa_data[4],
        pSock_addr->sa_data[5],
        pSock_addr->sa_data[6],
        pSock_addr->sa_data[7],
        pSock_addr->sa_data[8],
        pSock_addr->sa_data[9],
        pSock_addr->sa_data[10],
        pSock_addr->sa_data[11]);
#else
    __asm {
            xor eax, eax
            mov al, byte ptr [edx + 0xd]
            and eax, 0xff
            push eax
            xor eax, eax
            mov al, byte ptr [edx + 0xc]
            push eax
            xor eax, eax
            mov al, byte ptr [edx + 0xb]
            push eax
            xor eax, eax
            mov al, byte ptr [edx + 0xa]
            push eax
            xor eax, eax
            mov al, byte ptr [edx + 9]
            push eax
            xor eax, eax
            mov al, byte ptr [edx + 8]
            push eax
            xor eax, eax
            mov al, byte ptr [edx + 7]
            push eax
            xor eax, eax
            mov al, byte ptr [edx + 6]
            push eax
            xor eax, eax
            mov al, byte ptr [edx + 5]
            push eax
            xor eax, eax
            mov al, byte ptr [edx + 4]
            push eax
            xor eax, eax
            mov al, byte ptr [edx + 3]
            push eax
            xor eax, eax
            mov al, byte ptr [edx + 2]
            push eax
            push offset gSocket+0x84
            push ecx
            call sprintf
            add esp, 0x38
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00519560
int C2_NAKED C2_HOOK_FASTCALL PDNetInitialise(void) {
#ifndef CARPOCALYPSE2_MATCHING
    char default_null;
    char s[32];
    static u_long broadcast = 1;
    LINGER linger;
    int namelen;

    default_null = '\0';
    dr_dprintf("PDNetInitialise()");
    gIpx_sock_addr = 0xfe2f;
    if (gPathNetworkIniValid && GetPrivateProfileStringA("Network", "IPXSocket", &default_null, s, sizeof(s), gPathNetworkIni) != 0) {
        int ipx_address;
        int count;

        count = sscanf(s, "%x", &ipx_address);
        if (count != 0 && count != -1) {
            dr_dprintf("New IPX socket %4.4x", socket);
            /* byte swap */
            gIpx_sock_addr = (ipx_address << 8) | ((ipx_address >> 8) & 0xff);
        }
    }

    memset(gLocal_address.sa_data, 0, sizeof(gLocal_address.sa_data));
    gLocal_address.sa_family = AF_IPX;
    gLocal_address.sa_data[10] = gIpx_sock_addr & 0xff;
    gLocal_address.sa_data[11] = gIpx_sock_addr >> 8;
    gPtr_local_address = &gLocal_address;

    memset(gListen_address.sa_data, 0, sizeof(gListen_address.sa_data));
    gListen_address.sa_family = AF_IPX;
    gListen_address.sa_data[10] = gIpx_sock_addr & 0xff;
    gListen_address.sa_data[11] = gIpx_sock_addr >> 8;
    gPtr_listen_address = &gListen_address;

    memset(gBroadcast_address.sa_data, 0, sizeof(gListen_address.sa_data));
    gBroadcast_address.sa_family = AF_IPX;
    gBroadcast_address.sa_data[4] = 0xffu;
    gBroadcast_address.sa_data[5] = 0xffu;
    gBroadcast_address.sa_data[6] = 0xffu;
    gBroadcast_address.sa_data[7] = 0xffu;
    gBroadcast_address.sa_data[8] = 0xffu;
    gBroadcast_address.sa_data[9] = 0xffu;
    gBroadcast_address.sa_data[10] = gIpx_sock_addr & 0xff;
    gBroadcast_address.sa_data[11] = gIpx_sock_addr >> 8;
    gPtr_broadcast_address = &gBroadcast_address;

    if (WSAStartup(MAKEWORD(1, 1), &gWSA_data) == -1) {
        /* FIXME WSAStartup returns error code */
        dr_dprintf("PDNetInitialise(): WSAStartup() failed");
        return -1;
    }

    gSocket = socket(AF_IPX, SOCK_DGRAM, NSPROTO_IPX);
    if (gSocket == INVALID_SOCKET) {
        dr_dprintf("PDNetInitialise(): Failed to create socket - WSAGetLastError=%d", WSAGetLastError());
        WSACleanup();
        return -1;
    }
    setsockopt(gSocket, SOL_SOCKET, SO_BROADCAST, (const char*)&broadcast, sizeof(broadcast));

    linger.l_onoff = 1;
    linger.l_linger = 0;
    setsockopt(gSocket, SOL_SOCKET, SO_LINGER, (const char*)&linger, sizeof(linger));

    if (ioctlsocket(gSocket, FIONBIO, &broadcast) == SOCKET_ERROR) {
        dr_dprintf("Error on ioctlsocket() - WSAGetLastError=%d", WSAGetLastError());
        closesocket(gSocket);
        WSACleanup();
        return -1;
    }

    C2_HOOK_BUG_ON(sizeof(SOCKADDR_IPX) != 14);

    if (bind(gSocket, &gLocal_address, sizeof(SOCKADDR_IPX)) == SOCKET_ERROR) {
        dr_dprintf("Error on bind() - WSAGetLastError=%d", WSAGetLastError());
        closesocket(gSocket);
        WSACleanup();
        return -1;
    }

    namelen = sizeof(SOCKADDR_IPX);
    getsockname(gSocket, &gLocal_address, &namelen);
    NetNowIPXLocalTarget2String(gLocal_address_text, gPtr_local_address);

    memcpy(gIpx_network_numbers[0], gPtr_local_address->sa_data, 4);
    gNumber_of_networks = 1;
    if (gPathNetworkIniValid) {
        char key[32];
        int i;

        for (i = 0; i < 16; i++) {
            sprintf(key, "%s%d", "Network", i);
            if (GetPrivateProfileStringA("Network", key, &default_null, s, sizeof(s), gPathNetworkIni) != 0) {
                int count;
                tU32 netnum;

                count = sscanf(s, "%x", &netnum);
                if (count != 0 && count != -1) {
                    dr_dprintf("Found network number '%x' in INI file", netnum);
                    gIpx_network_numbers[gNumber_of_networks][0] = netnum >> 24;
                    gIpx_network_numbers[gNumber_of_networks][1] = netnum >> 16;
                    gIpx_network_numbers[gNumber_of_networks][2] = netnum >> 8;
                    gIpx_network_numbers[gNumber_of_networks][3] = netnum >> 0;
                    if (gIpx_network_numbers[gNumber_of_networks][0] != gPtr_local_address->sa_data[0]
                        || gIpx_network_numbers[gNumber_of_networks][1] != gPtr_local_address->sa_data[1]
                        || gIpx_network_numbers[gNumber_of_networks][2] != gPtr_local_address->sa_data[2]
                        || gIpx_network_numbers[gNumber_of_networks][3] != gPtr_local_address->sa_data[3]) {
                        gNumber_of_networks++;
                    }
                }
            }
        }
        dr_dprintf("Total networks = %d", gNumber_of_networks);
    }
    memcpy(&gLocal_address_2, gPtr_local_address, sizeof(SOCKADDR_IPX));
    dr_dprintf("Socket bound OK; local address is '%s'", gLocal_address_text);

    gMsg_header_strlen = strlen(BROADCAST_HEADER);
    if (strstr(gLocal_address_text, "00a0240f9fac") != NULL) {
        gSpecial_server = 1;
    }
    return 0;
#else
    __asm {
            sub esp, 0x58
            push ebx
            push ebp
            push esi
            push edi
            push 0x661b60
            mov dword ptr [esp + 0x20], 0xe
            mov byte ptr [esp + 0x17], 0
            call dr_dprintf
            mov ecx, dword ptr ds:[gPathNetworkIniValid]
            mov ebp, dword ptr ds:[0x589054]
            add esp, 4
            mov ax, 0xfe2f
            or ebx, 0xffffffff
            mov word ptr ds:[gIpx_sock_addr], ax
            test ecx, ecx
            je L00519616
            push offset gPathNetworkIni
            lea eax, [esp + 0x2c]
            push 0x20
            lea ecx, [esp + 0x1b]
            push eax
            push ecx
            push 0x661b54
            push 0x661b4c
            call ebp
            test eax, eax
            je L00519610
            lea edx, [esp + 0x14]
            lea eax, [esp + 0x28]
            push edx
            push 0x661b48
            push eax
            call sscanf
            add esp, 0xc
            test eax, eax
            je L00519610
            cmp eax, ebx
            je L00519610
            mov ecx, dword ptr [esp + 0x14]
            push ecx
            push 0x661b30
            call dr_dprintf
            mov eax, dword ptr [esp + 0x1c]
            xor ecx, ecx
            mov dl, al
            mov cl, ah
            and edx, 0xff
            add esp, 8
            shl edx, 8
            add edx, ecx
            mov ax, dx
            mov word ptr ds:[gIpx_sock_addr], ax
            jmp L00519616
    L00519610:
            mov ax, word ptr ds:[gIpx_sock_addr]
    L00519616:
            xor edx, edx
            xor ecx, ecx
            mov dword ptr ds:[gLocal_address], edx
            mov dword ptr ds:[gListen_address], ecx
            mov dword ptr ds:[0x6ac314], edx
            mov dword ptr ds:[0x6ac11c], ecx
            mov dword ptr ds:[0x6ac318], edx
            mov dword ptr ds:[0x6ac120], ecx
            mov dword ptr ds:[0x6ac31c], edx
            mov dword ptr ds:[gBroadcast_address], edx
            mov dword ptr ds:[0x6ac304], edx
            mov esi, 6
            mov dword ptr ds:[0x6ac308], edx
            push offset gWSA_data
            mov dword ptr ds:[0x6ac124], ecx
            mov dword ptr ds:[0x6ac30c], edx
            push 0x101
            mov dword ptr ds:[gPtr_local_address], 0x6ac310
            mov dword ptr ds:[gPtr_listen_address], 0x6ac118
            mov dword ptr ds:[gPtr_broadcast_address], 0x6ac300
            mov word ptr ds:[gLocal_address], si
            mov word ptr ds:[gListen_address], si
            mov word ptr ds:[gBroadcast_address], si
            mov word ptr ds:[0x6ac31c], ax
            mov word ptr ds:[0x6ac124], ax
            mov word ptr ds:[0x6ac30c], ax
            mov byte ptr ds:[0x6ac306], bl
            mov byte ptr ds:[0x6ac307], bl
            mov byte ptr ds:[0x6ac308], bl
            mov byte ptr ds:[0x6ac309], bl
            mov byte ptr ds:[0x6ac30a], bl
            mov byte ptr ds:[0x6ac30b], bl
            _emit 0xe8
        _emit 0xdf
        _emit 0x47
        _emit 0x00
        _emit 0x00
            cmp eax, ebx
            jne L005196fa
            push 0x661b08
            call dr_dprintf
            add esp, 4
            mov eax, ebx
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x58
            ret
    L005196fa:
            push 0x3e8
            push 2
            push esi
            call socket
            cmp eax, ebx
            mov dword ptr ds:[gSocket], eax
            jne L00519732
            call WSAGetLastError
            push eax
            push 0x661ac8
            call dr_dprintf
            add esp, 8
            call WSACleanup
            mov eax, ebx
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x58
            ret
    L00519732:
            push 4
            push 0x6619b4
            push 0x20
            push 0xffff
            push eax
            call setsockopt
            mov ecx, dword ptr ds:[gSocket]
            lea eax, [esp + 0x20]
            push 8
            push eax
            push 0x80
            mov edi, 1
            push 0xffff
            push ecx
            mov dword ptr [esp + 0x34], edi
            mov dword ptr [esp + 0x38], 0
            call setsockopt
            mov edx, dword ptr ds:[gSocket]
            push 0x6619b4
            push 0x8004667e
            push edx
            call ioctlsocket
            cmp eax, ebx
            jne L005197bb
            call WSAGetLastError
            push eax
            push 0x661a9c
            call dr_dprintf
            mov eax, dword ptr ds:[gSocket]
            add esp, 8
            push eax
            call closesocket
            call WSACleanup
            mov eax, ebx
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x58
            ret
    L005197bb:
            mov ecx, dword ptr ds:[gSocket]
            push 0xe
            push offset gLocal_address
            push ecx
            call bind
            cmp eax, ebx
            jne L00519800
            call WSAGetLastError
            push eax
            push 0x661a74
            call dr_dprintf
            mov edx, dword ptr ds:[gSocket]
            add esp, 8
            push edx
            call closesocket
            call WSACleanup
            mov eax, ebx
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x58
            ret
    L00519800:
            mov ecx, dword ptr ds:[gSocket]
            lea eax, [esp + 0x1c]
            push eax
            push offset gLocal_address
            push ecx
            _emit 0xe8
        _emit 0x78
        _emit 0x46
        _emit 0x00
        _emit 0x00
            mov eax, dword ptr ds:[gPtr_local_address]
            xor edx, edx
            xor ecx, ecx
            mov dl, byte ptr [eax + 0xd]
            mov cl, byte ptr [eax + 0xc]
            and edx, 0xff
            push edx
            push ecx
            xor edx, edx
            xor ecx, ecx
            mov dl, byte ptr [eax + 0xb]
            mov cl, byte ptr [eax + 0xa]
            push edx
            push ecx
            xor edx, edx
            xor ecx, ecx
            mov dl, byte ptr [eax + 9]
            mov cl, byte ptr [eax + 8]
            push edx
            push ecx
            xor edx, edx
            xor ecx, ecx
            mov dl, byte ptr [eax + 7]
            mov cl, byte ptr [eax + 6]
            push edx
            push ecx
            xor edx, edx
            xor ecx, ecx
            mov dl, byte ptr [eax + 5]
            mov cl, byte ptr [eax + 4]
            push edx
            push ecx
            xor edx, edx
            xor ecx, ecx
            mov dl, byte ptr [eax + 3]
            mov cl, byte ptr [eax + 2]
            push edx
            push ecx
            push 0x661a34
            push offset gLocal_address_text
            call sprintf
            mov eax, dword ptr ds:[gPtr_local_address]
            mov ecx, dword ptr ds:[gPathNetworkIniValid]
            add esp, 0x38
            mov edx, dword ptr [eax + 2]
            mov dword ptr ds:[gNumber_of_networks], edi
            test ecx, ecx
            mov dword ptr ds:[gIpx_network_numbers], edx
            je L005199a1
    L0051989d:
            push edi
            push 0x661b4c
            lea eax, [esp + 0x50]
            push 0x661a2c
            push eax
            call sprintf
            add esp, 0x10
            lea ecx, [esp + 0x28]
            lea edx, [esp + 0x13]
            lea eax, [esp + 0x48]
            push offset gPathNetworkIni
            push 0x20
            push ecx
            push edx
            push eax
            push 0x661b4c
            call ebp
            test eax, eax
            je L0051997a
            lea ecx, [esp + 0x18]
            lea edx, [esp + 0x28]
            push ecx
            push 0x661b48
            push edx
            call sscanf
            add esp, 0xc
            test eax, eax
            je L0051997a
            cmp eax, ebx
            je L0051997a
            mov eax, dword ptr [esp + 0x18]
            push eax
            push 0x661a04
            call dr_dprintf
            mov ecx, dword ptr [esp + 0x20]
            mov eax, dword ptr ds:[gNumber_of_networks]
            mov edx, ecx
            mov esi, dword ptr ds:[gPtr_local_address]
            shr edx, 0x18
            mov byte ptr [eax*4 + gIpx_network_numbers], dl
            mov edx, ecx
            shr edx, 0x10
            mov byte ptr [eax*4 + 0x6ac131], dl
            mov edx, ecx
            shr edx, 8
            mov byte ptr [eax*4 + 0x6ac132], dl
            mov byte ptr [eax*4 + 0x6ac133], cl
            mov dl, byte ptr [eax*4 + gIpx_network_numbers]
            add esp, 8
            cmp dl, byte ptr [esi + 2]
            jne L00519972
            mov dl, byte ptr [eax*4 + 0x6ac131]
            cmp dl, byte ptr [esi + 3]
            jne L00519972
            mov dl, byte ptr [eax*4 + 0x6ac132]
            cmp dl, byte ptr [esi + 4]
            jne L00519972
            cmp cl, byte ptr [esi + 5]
            je L0051997f
    L00519972:
            inc eax
            mov dword ptr ds:[gNumber_of_networks], eax
            jmp L0051997f
    L0051997a:
            mov eax, dword ptr ds:[gNumber_of_networks]
    L0051997f:
            inc edi
            cmp edi, 0xf
            jle L0051989d
            push eax
            push 0x6619f0
            call dr_dprintf
            mov eax, dword ptr ds:[gPtr_local_address]
            add esp, 8
            mov edi, 1
    L005199a1:
            mov ecx, dword ptr [eax]
            push offset gLocal_address_text
            mov dword ptr ds:[gLocal_address_2], ecx
            mov edx, dword ptr [eax + 4]
            mov dword ptr ds:[0x6abf0c], edx
            mov ecx, dword ptr [eax + 8]
            mov dword ptr ds:[0x6abf10], ecx
            mov dx, word ptr [eax + 0xc]
            push 0x6619c8
            mov word ptr ds:[0x6abf14], dx
            call dr_dprintf
            add esp, 8
            mov dword ptr ds:[gMsg_header_strlen], 7
            push 0x6619b8
            push offset gLocal_address_text
            call strstr
            add esp, 8
            test eax, eax
            je L005199fe
            mov dword ptr ds:[gSpecial_server], edi
    L005199fe:
            pop edi
            pop esi
            pop ebp
            xor eax, eax
            pop ebx
            add esp, 0x58
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x00519a10
int C2_HOOK_FASTCALL PDNetShutdown(void) {

    dr_dprintf("PDNetShutdown()");
    if (gSocket != SOCKET_ERROR) {
        closesocket(gSocket);
    }
    gSocket = SOCKET_ERROR;
    return 0;
}

// FUNCTION: CARMA2_HW 0x00519a40
void C2_HOOK_FASTCALL PDNetStartProducingJoinList(void) {

    C2_HOOK_BUG_ON(sizeof(tPD_net_game_info) * JOINABLE_GAMES_CAPACITY != 0x140);

    dr_dprintf("PDNetStartProducingJoinList()");
    gNumber_of_hosts = 0;
    gJoinable_games = BrMemAllocate(sizeof(tPD_net_game_info) * JOINABLE_GAMES_CAPACITY, BR_MEMORY_APPLICATION);
    if (gJoinable_games == NULL) {
        PDFatalError("Can't allocate memory for joinable games");
    }
}

// FUNCTION: CARMA2_HW 0x00519a80
void C2_HOOK_FASTCALL PDNetEndJoinList(void) {

    dr_dprintf("PDNetEndJoinList()");
    if (gJoinable_games != NULL) {
        BrMemFree(gJoinable_games);
    }
    gJoinable_games = NULL;
}

int C2_HOOK_FASTCALL BroadcastMessage(void) {
    int i;
    int errors;
    char broadcast_addr_string[32];

    errors = 0;
    for (i = 0; i < gNumber_of_networks; i++) {
        memcpy(gPtr_broadcast_address->sa_data, gIpx_network_numbers[0], 4);
        NetNowIPXLocalTarget2String(broadcast_addr_string, gPtr_broadcast_address);
        dr_dprintf("Broadcasting on address '%s'", broadcast_addr_string);
        if (sendto(gSocket, gSend_buffer, strlen(gSend_buffer) + 1, 0, &gBroadcast_address, sizeof(gBroadcast_address)) == SOCKET_ERROR) {
            dr_dprintf("BroadcastMessage(): Error on sendto() - WSAGetLastError=%d", WSAGetLastError());
            errors = 1;
        }
    }
    return errors == 0;
}

void C2_HOOK_FASTCALL MakeMessageToSend(int pMessage_type) {

#if defined(CARPOCALYPSE2_FIX_BUGS)
    sprintf(gSend_buffer, "XXXX%s%1d", BROADCAST_HEADER, pMessage_type);
#else
    sprintf(gSend_buffer, "XXXX%s%0.1d", BROADCAST_HEADER, pMessage_type);
#endif
}

int C2_HOOK_FASTCALL SameEthernetAddress(struct sockaddr* pAddr_ipx1, struct sockaddr* pAddr_ipx2) {

    return memcmp(&pAddr_ipx1->sa_data[4], &pAddr_ipx2->sa_data[4], 6) == 0;
}

int C2_HOOK_FASTCALL GetMessageTypeFromMessage(char* pMessage_str) {
    char* real_msg;
    int msg_type_int;

    real_msg = &pMessage_str[4];
    msg_type_int = 0;

    // FIXME: "CW95MSG" value is used in and depends on platform
    if (strncmp(real_msg, BROADCAST_HEADER, gMsg_header_strlen) == 0) {
        if (isdigit(real_msg[gMsg_header_strlen])) {
            msg_type_int = real_msg[gMsg_header_strlen] - '0';
        }
        if (msg_type_int != 0 && msg_type_int < 2) {
            return msg_type_int;
        }
    }
    return 999;
}

static __inline int C2_HOOK_FASTCALL ReceiveHostResponses(void) {
    char str[256];
    int i;
    int already_registered;

    char addr_string[32];
    int sa_len;
    int wsa_error;

    C2_HOOK_BUG_ON(sizeof(gReceive_buffer) != 0x200);

    sa_len = sizeof(SOCKADDR_IPX);
    while (1) {
        if (recvfrom(gSocket, gReceive_buffer, sizeof(gReceive_buffer), 0, &gListen_address, &sa_len) == SOCKET_ERROR) {
            break;
        }
        NetNowIPXLocalTarget2String(addr_string, gPtr_listen_address);
        dr_dprintf("ReceiveHostResponses(): Received string '%s' from %s", gReceive_buffer, addr_string);

        if (SameEthernetAddress(gPtr_local_address, gPtr_listen_address)) {
            dr_dprintf("*** Discounting the above 'cos we sent it ***");
            continue;
        }
        if (GetMessageTypeFromMessage(gReceive_buffer) != 2) {
            dr_dprintf("*** Discounting the above 'cos it's not a host reply ***");
            continue;
        }

        dr_dprintf("*** It's a host reply! ***");
        already_registered = 0;
        for (i = 0; i < gNumber_of_hosts; i++) {
            if (SameEthernetAddress((struct sockaddr*)&gJoinable_games[i].addr, gPtr_listen_address)) {
                already_registered = 1;
                break;
            }
        }
        if (already_registered) {
            dr_dprintf("That game already registered");
            gJoinable_games[i].last_response = PDGetTotalTime();
        } else {
            dr_dprintf("Adding joinable game to slot #%d", gNumber_of_hosts);
            memcpy(gJoinable_games[gNumber_of_hosts].addr, gPtr_listen_address, sizeof(struct sockaddr));
            gJoinable_games[gNumber_of_hosts].last_response = PDGetTotalTime();
            gNumber_of_hosts++;
            dr_dprintf("Number of games found so far: %d", gNumber_of_hosts);
        }
        if (gNumber_of_hosts != 0) {
            dr_dprintf("Currently registered net games:");
            for (i = 0; i < gNumber_of_hosts; i++) {
                NetNowIPXLocalTarget2String(str, (struct sockaddr*)&gJoinable_games[i].addr);
                dr_dprintf("%d: Host addr %s", i, str);
            }
        }
    }
    wsa_error = WSAGetLastError() != WSAEWOULDBLOCK;
    if (wsa_error == 0) {
        return 1;
    }
    dr_dprintf("ReceiveHostResponses(): Error on recvfrom() - WSAGetLastError=%d", wsa_error);
    return 0;
}

// FUNCTION: CARMA2_HW 0x00519ab0
int C2_NAKED C2_HOOK_FASTCALL PDNetGetNextJoinGame(tNet_game_details* pGame, int pIndex) {
#ifndef CARPOCALYPSE2_MATCHING
    static tU32 next_broadcast_time = 0;
    int i;
    int j;
    int number_of_hosts_has_changed;

    dr_dprintf("PDNetGetNextJoinGame(): pIndex is %d", pIndex);
    if (pIndex == 0) {
        do {
            number_of_hosts_has_changed = 0;
            for (i = 0; i < gNumber_of_hosts; i++) {
                if (gJoinable_games[i].last_response + 10000 < (tU32)PDGetTotalTime()) {
                    number_of_hosts_has_changed = 1;
                    for (j = i; j < gNumber_of_hosts - 1; j++) {
                        memmove(&gJoinable_games[j], &gJoinable_games[j + 1], sizeof(tPD_net_game_info));
                    }
                }
            }
            if (number_of_hosts_has_changed) {
                gNumber_of_hosts--;
            }
        } while (number_of_hosts_has_changed);
        if ((tU32)PDGetTotalTime() > next_broadcast_time) {
            next_broadcast_time = PDGetTotalTime() + 3000;
            MakeMessageToSend(1);
            if (BroadcastMessage() == 0) {
                dr_dprintf("PDNetGetNextJoinGame(): Error on BroadcastMessage()");
            }
        }
    }
    ReceiveHostResponses();
    if (gNumber_of_hosts <= pIndex) {
        return 0;
    }
    dr_dprintf("PDNetGetNextJoinGame(): Adding game.");
    memcpy(&pGame->pd_net_info.addr, &gJoinable_games[pIndex].addr, sizeof(pGame->pd_net_info.addr));
    return 1;
#else
    __asm {
            sub esp, 0x12c
            push ebx
            push ebp
            push esi
            mov esi, edx
            push edi
            push esi
            mov dword ptr [esp + 0x18], ecx
            push 0x661e50
            mov dword ptr [esp + 0x20], esi
            call dr_dprintf
            add esp, 8
            test esi, esi
            jne L00519ca6
            mov esi, dword ptr ds:[gNumber_of_hosts]
    L00519ae0:
            xor eax, eax
            xor ebx, ebx
            test esi, esi
            mov dword ptr [esp + 0x10], eax
            jle L00519b5b
            xor ebp, ebp
    L00519aee:
            call PDGetTotalTime
            mov ecx, dword ptr ds:[gJoinable_games]
            mov edx, dword ptr [ecx + ebp + 0x10]
            add edx, 0x2710
            cmp eax, edx
            jbe L00519b49
            mov esi, dword ptr ds:[gNumber_of_hosts]
            mov dword ptr [esp + 0x10], 1
            mov edx, ebx
            lea eax, [esi - 1]
            cmp ebx, eax
            jge L00519b4f
            mov eax, ebp
            jmp L00519b28
    L00519b22:
            mov ecx, dword ptr ds:[gJoinable_games]
    L00519b28:
            lea esi, [ecx + eax + 0x14]
            lea edi, [ecx + eax]
            mov ecx, 5
            inc edx
            rep movsd
            mov esi, dword ptr ds:[gNumber_of_hosts]
            add eax, 0x14
            lea ecx, [esi - 1]
            cmp edx, ecx
            jl L00519b22
            jmp L00519b4f
    L00519b49:
            mov esi, dword ptr ds:[gNumber_of_hosts]
    L00519b4f:
            inc ebx
            add ebp, 0x14
            cmp ebx, esi
            jl L00519aee
            mov eax, dword ptr [esp + 0x10]
    L00519b5b:
            test eax, eax
            je L00519b6e
            dec esi
            test eax, eax
            mov dword ptr ds:[gNumber_of_hosts], esi
            jne L00519ae0
    L00519b6e:
            call PDGetTotalTime
            cmp eax, dword ptr ds:[0x6ac580]
            jbe L00519ca6
            call PDGetTotalTime
            add eax, 0xbb8
            push 1
            push 0x661e48
            push 0x661e3c
            push offset gSend_buffer
            mov dword ptr ds:[0x6ac580], eax
            call sprintf
            mov eax, dword ptr ds:[gNumber_of_networks]
            add esp, 0x10
            xor ebp, ebp
            xor ebx, ebx
            test eax, eax
            jle L00519c95
            mov esi, offset gIpx_network_numbers
    L00519bbd:
            mov eax, dword ptr ds:[gPtr_broadcast_address]
            mov edx, dword ptr [esi]
            xor ecx, ecx
            mov dword ptr [eax + 2], edx
            mov eax, dword ptr ds:[gPtr_broadcast_address]
            xor edx, edx
            mov cl, byte ptr [eax + 0xd]
            mov dl, byte ptr [eax + 0xc]
            and ecx, 0xff
            push ecx
            push edx
            xor ecx, ecx
            xor edx, edx
            mov cl, byte ptr [eax + 0xb]
            mov dl, byte ptr [eax + 0xa]
            push ecx
            push edx
            xor ecx, ecx
            xor edx, edx
            mov cl, byte ptr [eax + 9]
            mov dl, byte ptr [eax + 8]
            push ecx
            push edx
            xor ecx, ecx
            xor edx, edx
            mov cl, byte ptr [eax + 7]
            mov dl, byte ptr [eax + 6]
            push ecx
            push edx
            xor ecx, ecx
            xor edx, edx
            mov cl, byte ptr [eax + 5]
            mov dl, byte ptr [eax + 4]
            push ecx
            push edx
            xor ecx, ecx
            xor edx, edx
            mov cl, byte ptr [eax + 3]
            mov dl, byte ptr [eax + 2]
            push ecx
            push edx
            lea eax, [esp + 0x4c]
            push 0x661a34
            push eax
            call sprintf
            add esp, 0x38
            lea ecx, [esp + 0x1c]
            push ecx
            push 0x661e1c
            call dr_dprintf
            mov edi, offset gSend_buffer
            or ecx, 0xffffffff
            xor eax, eax
            add esp, 8
            repne scasb
            mov edx, dword ptr ds:[gSocket]
            push 0xe
            push offset gBroadcast_address
            push 0
            not ecx
            push ecx
            push offset gSend_buffer
            push edx
            call sendto
            cmp eax, -1
            jne L00519c84
            call WSAGetLastError
            push eax
            push 0x661de0
            call dr_dprintf
            add esp, 8
            mov ebp, 1
    L00519c84:
            mov eax, dword ptr ds:[gNumber_of_networks]
            inc ebx
            add esi, 4
            cmp ebx, eax
            jl L00519bbd
    L00519c95:
            test ebp, ebp
            je L00519ca6
            push 0x661dac
            call dr_dprintf
            add esp, 4
    L00519ca6:
            mov ecx, dword ptr ds:[gSocket]
            lea eax, [esp + 0x10]
            push eax
            push offset gListen_address
            push 0
            push 0x200
            push offset gReceive_buffer
            push ecx
            mov dword ptr [esp + 0x28], 0xe
            _emit 0xe8
        _emit 0xf4
        _emit 0x41
        _emit 0x00
        _emit 0x00
            xor edx, edx
            cmp eax, -1
            setne dl
            mov eax, edx
            test eax, eax
            je L00519f83
    L00519ce2:
            jle L00519f50
            mov eax, dword ptr ds:[gPtr_listen_address]
            xor ecx, ecx
            xor edx, edx
            mov cl, byte ptr [eax + 0xd]
            mov dl, byte ptr [eax + 0xc]
            and ecx, 0xff
            push ecx
            push edx
            xor ecx, ecx
            xor edx, edx
            mov cl, byte ptr [eax + 0xb]
            mov dl, byte ptr [eax + 0xa]
            push ecx
            push edx
            xor ecx, ecx
            xor edx, edx
            mov cl, byte ptr [eax + 9]
            mov dl, byte ptr [eax + 8]
            push ecx
            push edx
            xor ecx, ecx
            xor edx, edx
            mov cl, byte ptr [eax + 7]
            mov dl, byte ptr [eax + 6]
            push ecx
            push edx
            xor ecx, ecx
            xor edx, edx
            mov cl, byte ptr [eax + 5]
            mov dl, byte ptr [eax + 4]
            push ecx
            push edx
            xor ecx, ecx
            xor edx, edx
            mov cl, byte ptr [eax + 3]
            mov dl, byte ptr [eax + 2]
            push ecx
            push edx
            lea eax, [esp + 0x4c]
            push 0x661a34
            push eax
            call sprintf
            add esp, 0x38
            lea ecx, [esp + 0x1c]
            push ecx
            push offset gReceive_buffer
            push 0x661d74
            call dr_dprintf
            mov eax, dword ptr ds:[gPtr_local_address]
            mov ecx, dword ptr ds:[gPtr_listen_address]
            add esp, 0xc
            mov edx, dword ptr [eax + 6]
            mov esi, dword ptr [ecx + 6]
            cmp edx, esi
            jne L00519d8d
            mov ax, word ptr [eax + 0xa]
            cmp ax, word ptr [ecx + 0xa]
            jne L00519d8d
            push 0x661d44
            jmp L00519f48
    L00519d8d:
            mov ecx, dword ptr ds:[gMsg_header_strlen]
            xor esi, esi
            push ecx
            push 0x661e48
            push 0x6abf1c
            call strncmp
            add esp, 0xc
            test eax, eax
            jne L00519e04
            cmp dword ptr ds:[0x673c5c], 1
            jle L00519dcf
            mov edx, dword ptr ds:[gMsg_header_strlen]
            push 4
            movsx eax, byte ptr [edx + 0x6abf1c]
            push eax
            call _isctype
            add esp, 8
            jmp L00519de7
    L00519dcf:
            mov ecx, dword ptr ds:[gMsg_header_strlen]
            mov eax, dword ptr ds:[0x673a50]
            movsx edx, byte ptr [ecx + 0x6abf1c]
            mov al, byte ptr [eax + edx*2]
            and eax, 4
    L00519de7:
            test eax, eax
            je L00519dfb
            mov ecx, dword ptr ds:[gMsg_header_strlen]
            movsx esi, byte ptr [ecx + 0x6abf1c]
            sub esi, 0x30
    L00519dfb:
            test esi, esi
            je L00519e04
            cmp esi, 3
            jl L00519e09
    L00519e04:
            mov esi, 0x3e7
    L00519e09:
            cmp esi, 2
            jne L00519f43
            push 0x661d28
            call dr_dprintf
            mov ecx, dword ptr ds:[gNumber_of_hosts]
            add esp, 4
            xor ebx, ebx
            xor esi, esi
            test ecx, ecx
            jle L00519e5b
            mov edx, dword ptr ds:[gPtr_listen_address]
            mov eax, dword ptr ds:[gJoinable_games]
            add eax, 0xa
            mov edi, dword ptr [edx + 6]
    L00519e3e:
            cmp dword ptr [eax - 4], edi
            jne L00519e4c
            mov bp, word ptr [eax]
            cmp bp, word ptr [edx + 0xa]
            je L00519e56
    L00519e4c:
            inc esi
            add eax, 0x14
            cmp esi, ecx
            jge L00519e5b
            jmp L00519e3e
    L00519e56:
            mov ebx, 1
    L00519e5b:
            test ebx, ebx
            je L00519e80
            push 0x661d08
            call dr_dprintf
            add esp, 4
            call PDGetTotalTime
            mov edx, dword ptr ds:[gJoinable_games]
            lea ecx, [esi + esi*4]
            mov dword ptr [edx + ecx*4 + 0x10], eax
            jmp L00519eed
    L00519e80:
            push ecx
            push 0x661ce4
            call dr_dprintf
            mov eax, dword ptr ds:[gNumber_of_hosts]
            mov ecx, dword ptr ds:[gJoinable_games]
            add esp, 8
            lea eax, [eax + eax*4]
            lea edx, [ecx + eax*4]
            mov eax, dword ptr ds:[gPtr_listen_address]
            mov ecx, dword ptr [eax]
            mov dword ptr [edx], ecx
            mov ecx, dword ptr [eax + 4]
            mov dword ptr [edx + 4], ecx
            mov ecx, dword ptr [eax + 8]
            mov dword ptr [edx + 8], ecx
            mov ax, word ptr [eax + 0xc]
            mov word ptr [edx + 0xc], ax
            call PDGetTotalTime
            mov ecx, dword ptr ds:[gNumber_of_hosts]
            mov edx, dword ptr ds:[gJoinable_games]
            lea ecx, [ecx + ecx*4]
            mov dword ptr [edx + ecx*4 + 0x10], eax
            mov eax, dword ptr ds:[gNumber_of_hosts]
            inc eax
            push eax
            push 0x661cc0
            mov dword ptr ds:[gNumber_of_hosts], eax
            call dr_dprintf
            add esp, 8
    L00519eed:
            mov eax, dword ptr ds:[gNumber_of_hosts]
            test eax, eax
            je L00519f50
            push 0x661ca0
            call dr_dprintf
            mov eax, dword ptr ds:[gNumber_of_hosts]
            add esp, 4
            xor esi, esi
            test eax, eax
            jle L00519f50
            xor edi, edi
    L00519f10:
            mov eax, dword ptr ds:[gJoinable_games]
            lea ecx, [esp + 0x3c]
            lea edx, [eax + edi]
            call NetNowIPXLocalTarget2String
            lea ecx, [esp + 0x3c]
            push ecx
            push esi
            push 0x661c8c
            call dr_dprintf
            mov eax, dword ptr ds:[gNumber_of_hosts]
            add esp, 0xc
            inc esi
            add edi, 0x14
            cmp esi, eax
            jl L00519f10
            jmp L00519f50
    L00519f43:
            push 0x661c50
    L00519f48:
            call dr_dprintf
            add esp, 4
    L00519f50:
            mov eax, dword ptr ds:[gSocket]
            lea edx, [esp + 0x10]
            push edx
            push offset gListen_address
            push 0
            push 0x200
            push offset gReceive_buffer
            push eax
            _emit 0xe8
        _emit 0x53
        _emit 0x3f
        _emit 0x00
        _emit 0x00
            xor ecx, ecx
            cmp eax, -1
            setne cl
            mov eax, ecx
            test eax, eax
            jne L00519ce2
    L00519f83:
            call WSAGetLastError
            xor edx, edx
            cmp eax, 0x2733
            setne dl
            mov eax, edx
            test eax, eax
            je L00519fa6
            push eax
            push 0x661c0c
            call dr_dprintf
            add esp, 8
    L00519fa6:
            mov esi, dword ptr [esp + 0x18]
            mov eax, dword ptr ds:[gNumber_of_hosts]
            cmp esi, eax
            jge L00519ff8
            push 0x661be4
            call dr_dprintf
            mov ecx, dword ptr ds:[gJoinable_games]
            lea eax, [esi + esi*4]
            add esp, 4
            lea edx, [ecx + eax*4]
            mov eax, dword ptr [esp + 0x14]
            mov ecx, dword ptr [edx]
            mov dword ptr [eax], ecx
            mov ecx, dword ptr [edx + 4]
            mov dword ptr [eax + 4], ecx
            mov ecx, dword ptr [edx + 8]
            mov dword ptr [eax + 8], ecx
            mov dx, word ptr [edx + 0xc]
            mov word ptr [eax + 0xc], dx
            mov eax, 1
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x12c
            ret
    L00519ff8:
            pop edi
            pop esi
            pop ebp
            xor eax, eax
            pop ebx
            add esp, 0x12c
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0051a6a0
int C2_HOOK_FASTCALL PDNetGetHeaderSize(void) {

    return 0;
}

// FUNCTION: CARMA2_HW 0x0051a0a0
int C2_HOOK_FASTCALL PDNetJoinGame(tNet_game_details* pGame_details, const char* pName) {

    dr_dprintf("PDNetJoinGame()");
    return 0;
}

// FUNCTION: CARMA2_HW 0x0051a0c0
void C2_HOOK_FASTCALL PDNetHostFinishGame(void) {

    dr_dprintf("PDNetHostFinishGame()");
}

// FUNCTION: CARMA2_HW 0x0051a0b0
void C2_HOOK_FASTCALL PDNetLeaveGame(void) {

    dr_dprintf("PDNetLeaveGame()");
}

// FUNCTION: CARMA2_HW 0x0051a0d0
tU32 C2_HOOK_FASTCALL PDNetExtractGameID(tNet_game_details* pDetails) {
    dr_dprintf("PDNetExtractGameID()");
    dr_dprintf("...returning %8.8x",
        ((((tU8*)pDetails->pd_net_info.addr)[8] << 8
          | ((tU8*)pDetails->pd_net_info.addr)[9]) << 8
         | ((tU8*)pDetails->pd_net_info.addr)[10]) << 8
        | ((tU8*)pDetails->pd_net_info.addr)[11]);
    return ((((tU8*)pDetails->pd_net_info.addr)[8] << 8
             | ((tU8*)pDetails->pd_net_info.addr)[9]) << 8
            | ((tU8*)pDetails->pd_net_info.addr)[10]) << 8
           | ((tU8*)pDetails->pd_net_info.addr)[11];
}

tPlayer_ID C2_HOOK_FASTCALL PDNetExtractPlayerID(tNet_game_details* pDetails) {

    (void)pDetails;
    return gLocal_net_ID;
}

// FUNCTION: CARMA2_HW 0x0051a380
C2_NAKED tNet_message* C2_HOOK_FASTCALL PDNetGetNextMessage(tNet_game_details* pDetails, void** pSender_address) {
#ifndef CARPOCALYPSE2_MATCHING
    char addr_str[32];
    int sa_len;
    int res;
    tNet_message* msg;
    char* receive_buffer;

    sa_len = 14;  /* FIXME: sizeof(gListen_address) == 16 */
    msg = NetAllocateMessage(512);
    receive_buffer = (char*)msg;
    res = recvfrom(gSocket, receive_buffer, 512, 0, &gListen_address, &sa_len) != SOCKET_ERROR;
    if (res == SOCKET_ERROR) {
        WSAGetLastError();
    } else {
        NetNowIPXLocalTarget2String(addr_str, gPtr_listen_address);
        if (!SameEthernetAddress(gPtr_local_address, gPtr_listen_address)) {
            int msg_type;

            msg_type = GetMessageTypeFromMessage(receive_buffer);
            switch (msg_type) {
            case 1:
                if (gNet_mode == eNet_mode_host) {
                    dr_dprintf("PDNetGetNextMessage(): Received '%s' from '%s', replying to joiner", receive_buffer, addr_str);
                    MakeMessageToSend(2);
                    if (sendto(gSocket, gSend_buffer, strlen(gSend_buffer) + 1, 0, &gListen_address, sizeof(gListen_address)) == SOCKET_ERROR) {
                        dr_dprintf("PDNetGetNextMessage(): Error on sendto() - WSAGetLastError=%d", WSAGetLastError());
                    }
                }
                break;
            case 2:
                // no-op
                break;
            default:
                dr_dprintf("PDNetGetNextMessage(): res is %d, received message type %d from '%s', passing up", res, msg->contents.raw.header.type, addr_str);
                memcpy(&gLast_received_addr, gPtr_listen_address, sizeof(gLast_received_addr));
                *pSender_address = &gLast_received_addr;
                return msg;
            }
        }
    }
    msg->header.field_0x14 = 0;
    NetDisposeMessage(pDetails, msg);
    return NULL;
#else
    __asm {
            sub esp, 0x2c
            push ebx
            push ebp
            mov dword ptr [esp + 8], ecx
            push esi
            push edi
            mov dword ptr [esp + 0x18], edx
            mov ecx, 0x200
            mov dword ptr [esp + 0x14], 0xe
            call NetAllocateMessage
            mov ecx, dword ptr ds:[gSocket]
            mov esi, eax
            lea eax, [esp + 0x14]
            push eax
            push offset gListen_address
            push 0
            push 0x200
            push esi
            push ecx
            _emit 0xe8
        _emit 0x03
        _emit 0x3b
        _emit 0x00
        _emit 0x00
            xor edx, edx
            cmp eax, -1
            setne dl
            mov ebp, edx
            test ebp, ebp
            je L0051a5a7
            jle L0051a5ac
            mov eax, dword ptr ds:[gPtr_listen_address]
            xor ecx, ecx
            xor edx, edx
            mov cl, byte ptr [eax + 0xd]
            mov dl, byte ptr [eax + 0xc]
            and ecx, 0xff
            push ecx
            push edx
            xor ecx, ecx
            xor edx, edx
            mov cl, byte ptr [eax + 0xb]
            mov dl, byte ptr [eax + 0xa]
            push ecx
            push edx
            xor ecx, ecx
            xor edx, edx
            mov cl, byte ptr [eax + 9]
            mov dl, byte ptr [eax + 8]
            push ecx
            push edx
            xor ecx, ecx
            xor edx, edx
            mov cl, byte ptr [eax + 7]
            mov dl, byte ptr [eax + 6]
            push ecx
            push edx
            xor ecx, ecx
            xor edx, edx
            mov cl, byte ptr [eax + 5]
            mov dl, byte ptr [eax + 4]
            push ecx
            push edx
            xor ecx, ecx
            xor edx, edx
            mov cl, byte ptr [eax + 3]
            mov dl, byte ptr [eax + 2]
            push ecx
            push edx
            lea eax, [esp + 0x4c]
            push 0x661a34
            push eax
            call sprintf
            mov eax, dword ptr ds:[gPtr_local_address]
            mov ecx, dword ptr ds:[gPtr_listen_address]
            add esp, 0x38
            mov edx, dword ptr [eax + 6]
            mov edi, dword ptr [ecx + 6]
            cmp edx, edi
            jne L0051a461
            mov ax, word ptr [eax + 0xa]
            cmp ax, word ptr [ecx + 0xa]
            je L0051a5ac
    L0051a461:
            mov ecx, dword ptr ds:[gMsg_header_strlen]
            lea edi, [esi + 4]
            push ecx
            push 0x661e48
            push edi
            xor ebx, ebx
            call strncmp
            add esp, 0xc
            test eax, eax
            jne L0051a4d0
            cmp dword ptr ds:[0x673c5c], 1
            jle L0051a49f
            mov edx, dword ptr ds:[gMsg_header_strlen]
            push 4
            movsx eax, byte ptr [edi + edx]
            push eax
            call _isctype
            add esp, 8
            jmp L0051a4b4
    L0051a49f:
            mov ecx, dword ptr ds:[gMsg_header_strlen]
            mov eax, dword ptr ds:[0x673a50]
            movsx edx, byte ptr [edi + ecx]
            mov al, byte ptr [eax + edx*2]
            and eax, 4
    L0051a4b4:
            test eax, eax
            je L0051a4c5
            mov ecx, dword ptr ds:[gMsg_header_strlen]
            movsx ebx, byte ptr [edi + ecx]
            sub ebx, 0x30
    L0051a4c5:
            test ebx, ebx
            je L0051a4d0
            cmp ebx, 3
            mov eax, ebx
            jl L0051a4d5
    L0051a4d0:
            mov eax, 0x3e7
    L0051a4d5:
            dec eax
            je L0051a535
            dec eax
            je L0051a5ac
            xor eax, eax
            lea edx, [esp + 0x1c]
            mov al, byte ptr [esi + 0x19]
            push edx
            push eax
            push ebp
            push 0x662070
            call dr_dprintf
            mov ecx, dword ptr ds:[gPtr_listen_address]
            add esp, 0x10
            mov edx, dword ptr [ecx]
            mov dword ptr ds:[gLast_received_addr], edx
            mov eax, dword ptr [ecx + 4]
            mov dword ptr ds:[0x6ac34c], eax
            mov edx, dword ptr [ecx + 8]
            mov dword ptr ds:[0x6ac350], edx
            mov ax, word ptr [ecx + 0xc]
            mov ecx, dword ptr [esp + 0x18]
            mov word ptr ds:[0x6ac354], ax
            mov eax, esi
            mov dword ptr [ecx], 0x6ac348
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x2c
            ret
    L0051a535:
            cmp dword ptr ds:[gNet_mode], 2
            jne L0051a5ac
            lea edx, [esp + 0x1c]
            push edx
            push esi
            push 0x66202c
            call dr_dprintf
            add esp, 0xc
            push 2
            push 0x661e48
            push 0x661e3c
            push offset gSend_buffer
            call sprintf
            mov edi, offset gSend_buffer
            or ecx, 0xffffffff
            xor eax, eax
            add esp, 0x10
            repne scasb
            mov eax, dword ptr ds:[gSocket]
            push 0xe
            push offset gListen_address
            push 0
            not ecx
            push ecx
            push offset gSend_buffer
            push eax
            call sendto
            cmp eax, -1
            jne L0051a5ac
            push ebp
            push 0x661fec
            call dr_dprintf
            add esp, 8
            jmp L0051a5ac
    L0051a5a7:
            call WSAGetLastError
    L0051a5ac:
            mov ecx, dword ptr [esp + 0x10]
            mov edx, esi
            mov word ptr [esi + 0x14], 0
            call NetDisposeMessage
            xor eax, eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            add esp, 0x2c
            ret
    }
#endif
}

// FUNCTION: CARMA2_HW 0x0051a630
int C2_HOOK_FASTCALL PDNetSendMessageToAddress(tNet_game_details* pDetails, tNet_message* pMessage, void* pAddress) {

    /* FIXME: sizeof(struct sockaddr) == 16 */

    if (sendto(gSocket, (const char*)pMessage, pMessage->header.field_0x16, 0, pAddress, 14) == SOCKET_ERROR) {
        dr_dprintf("PDNetSendMessageToAddress(): Error on sendto() - WSAGetLastError=%d", WSAGetLastError());
        NetDisposeMessage(pDetails, pMessage);
        return 1;
    }
    NetDisposeMessage(pDetails,(tNet_message *)pMessage);
    return 0;
}
// FUNCTION: CARMA2_HW 0x0051a070
void C2_HOOK_FASTCALL PDNetDisposeGameDetails(void) {

    dr_dprintf("PDNetDisposeGameDetails()");
}

// FUNCTION: CARMA2_HW 0x0051a210
int C2_HOOK_FASTCALL PDNetSendMessageToPlayer(tNet_game_details* pDetails, tNet_message* pMessage, tPlayer_ID pPlayer_id) {
    int i;
    char* p;
    tPD_net_player_info* to;

    i = 0;
    if (0 < gNumber_of_net_players) {
        p = gNet_players[0].pd_net_info.addr + 9;
        while (i < gNumber_of_net_players) {
            if (p[-1] == (char)(pPlayer_id >> 24) && p[0] == (char)(pPlayer_id >> 16) && p[1] == (char)(pPlayer_id >> 8) && p[2] == (char)pPlayer_id) {
                to = &gNet_players[i].pd_net_info;
                goto found;
            }
            i++;
            p += sizeof(tNet_game_player_info);
        }
    }
    to = NULL;
found:
    if (to != NULL && sendto(gSocket, (const char*)pMessage, pMessage->header.field_0x16, 0, (const struct sockaddr*)to, 14) == SOCKET_ERROR) {
        dr_dprintf("PDNetSendMessageToPlayer(): Error on sendto() - WSAGetLastError=%d", WSAGetLastError());
        NetDisposeMessage(pDetails, pMessage);
        return 1;
    }
    NetDisposeMessage(pDetails, pMessage);
    return 0;
}

// FUNCTION: CARMA2_HW 0x0051a2e0
int C2_HOOK_FASTCALL PDNetSendMessageToAllPlayers(tNet_game_details* pDetails, tNet_message* pMessage) {
    int i;

    for (i = 0; i < gNumber_of_net_players; i++) {
        if (i != gThis_net_player_index) {
            if (sendto(gSocket, (const char*)pMessage, pMessage->header.field_0x16, 0, (const struct sockaddr*)&gNet_players[i].pd_net_info, 14) == SOCKET_ERROR) {
                dr_dprintf("PDNetSendMessageToAllPlayers(): Error on sendto() - WSAGetLastError=%d", WSAGetLastError());
                NetDisposeMessage(pDetails, pMessage);
                return 1;
            }
        }
    }
    NetDisposeMessage(pDetails, pMessage);
    return 0;
}

// FUNCTION: CARMA2_HW 0x0051a5d0
void* C2_HOOK_FASTCALL FUN_0051a5d0(size_t pSize) {

    return BrMemAllocate(pSize, 0x80);
}

// FUNCTION: CARMA2_HW 0x0051a620
void C2_HOOK_FASTCALL PDNetDisposePlayer(void) {

    dr_dprintf("PDNetDisposePlayer()");
}

// FUNCTION: CARMA2_HW 0x0051a690
int C2_HOOK_FASTCALL PDNetInitClient(void) {

    dr_dprintf("PDNetInitClient()");
    return 0;
}
