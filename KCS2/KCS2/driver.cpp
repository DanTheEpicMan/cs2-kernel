
#include <ntdef.h>
#include <ntifs.h>
#include <ntddk.h>
#include <ntddk.h>
#include <cstdint>


#define THREAD_POOL_TAG 'MyTh'

//DRIVER_INITIALIZE DriverEntry;
//#pragma alloc_text(INIT, DriverEntry)



// API function from ntoskrnl.exe which we use
// to copy memory to and from an user process.
extern "C" __declspec (dllimport)
NTSTATUS NTAPI MmCopyVirtualMemory
(
	PEPROCESS SourceProcess,
	PVOID SourceAddress,
	PEPROCESS TargetProcess,
	PVOID TargetAddress,
	SIZE_T BufferSize,
	KPROCESSOR_MODE PreviousMode,
	PSIZE_T ReturnSize
);

NTKERNELAPI
NTSTATUS
PsLookupProcessByProcessId(
	_In_ HANDLE ProcessId,
	_Outptr_ PEPROCESS* Process
);


NTSTATUS KeReadProcessMemory(PEPROCESS Process, PVOID SourceAddress, PVOID TargetAddress, SIZE_T Size)
{
	// Since the process we are reading from is the input process, we set
	// the source process variable for that.
	PEPROCESS SourceProcess = Process;
	// Since the "process" we read the output to is this driver
	// we set the target process as the current module.
	PEPROCESS TargetProcess = PsGetCurrentProcess();
	SIZE_T Result;
	if (NT_SUCCESS(MmCopyVirtualMemory(SourceProcess, SourceAddress, TargetProcess, TargetAddress, Size, KernelMode, &Result)))
		return STATUS_SUCCESS; // operation was successful
	else
		return STATUS_ACCESS_DENIED;
}
NTSTATUS KeWriteProcessMemory(PEPROCESS Process, PVOID SourceAddress, PVOID TargetAddress, SIZE_T Size)
{       // This write func is just like the read func, except vice versa.

	// Since the process writing from is our module
	// change the source process variable for that.
	PEPROCESS SourceProcess = PsGetCurrentProcess();
	// Since the process we write to is the input process
	// we set the target process as the argument
	PEPROCESS TargetProcess = Process;
	SIZE_T Result;

	if (NT_SUCCESS(MmCopyVirtualMemory(SourceProcess, SourceAddress, TargetProcess, TargetAddress, Size, KernelMode, &Result)))
		return STATUS_SUCCESS; // operation was successful
	else
		return STATUS_ACCESS_DENIED;

}


void SleepTime()
{
	LARGE_INTEGER interval;
	interval.QuadPart = (10 /*miliseconds*/ * -10000 /*keep as is*/); // 1 = 10 Nanoseconds 
	KeDelayExecutionThread(KernelMode, FALSE, &interval);
}


//edit if you need to pass through more
typedef struct KBufferStruct
{
	unsigned long long KBuffer;
	int PID;
};

struct view_matrix_t {
	float* operator[ ](int index) {
		return matrix[index];
	}

	float matrix[4][4];
};

struct Vector3
{
	float x, y, z;
};

struct IVector3
{
	int x, y, z;
};

struct Vector2
{
	float x, y;
};

struct playerPOS {
	//make 33 positions
	int x[64],			y[64],		z[64];
	int headX[64],		headY[64],	headZ[64];
	int neckX[64],		neckY[64],	neckZ[64];

	int lShoulderX[64],		lShoulderY[64], lShoulderZ[64];
	int lElbowX[64],		lElbowY[64],	lElbowZ[64];
	int lHandX[64],			lHandY[64],		lHandZ[64];

	int rShoulderX[64],		rShoulderY[64], rShoulderZ[64];
	int rElbowX[64],		rElbowY[64],	rElbowZ[64];
	int rHandX[64],			rHandY[64],		rHandZ[64];

	int pelvisX[64],	pelvisY[64], pelvisZ[64];

	int lKneeX[64],		lKneeY[64],  lKneeZ[64];
	int lFootX[64],		lFootY[64],  lFootZ[64];

	int rKneeX[64],		rKneeY[64],  rKneeZ[64];
	int rFootX[64],		rFootY[64],  rFootZ[64];
	int health[64];
};


VOID MyThreadFunction(PVOID StartContext) {
	UNREFERENCED_PARAMETER(StartContext);



	//hardcoded, fix later ("fix later" its been weeks lamo)
	PVOID KBufferAddr = (PVOID)1054709828328;
	PVOID UBufferAddr = (PVOID)1054709828376;
	PVOID vecAddr = (PVOID)1054709828992;
	PVOID viewMatrixAddr = (PVOID)1054709828128;
	int PID = 10320;

	//Used to get process of our usermode
	PEPROCESS Process; // our target process
	PsLookupProcessByProcessId((PVOID)PID, &Process); //lookup the process by it's id;

	
	//Buffer that usermode writes into
	KBufferStruct KBuffer;		//read, only used for PID
	int gamePID = 0;
	

	playerPOS UBuffer;
	for (int playerIndex = 0; playerIndex < 64; playerIndex++)
	{
		UBuffer.x[playerIndex] = 0; UBuffer.y[playerIndex] = 0; UBuffer.z[playerIndex] = 0;
		UBuffer.headX[playerIndex] = 0; UBuffer.headY[playerIndex] = 0; UBuffer.headZ[playerIndex] = 0;
		UBuffer.neckX[playerIndex] = 0; UBuffer.neckY[playerIndex] = 0; UBuffer.neckZ[playerIndex] = 0;

		UBuffer.lShoulderX[playerIndex] = 0; UBuffer.lShoulderY[playerIndex] = 0; UBuffer.lShoulderZ[playerIndex] = 0;
		UBuffer.lElbowX[playerIndex] = 0; UBuffer.lElbowY[playerIndex] = 0; UBuffer.lElbowZ[playerIndex] = 0;
		UBuffer.lHandX[playerIndex] = 0; UBuffer.lHandY[playerIndex] = 0; UBuffer.lHandZ[playerIndex] = 0;

		UBuffer.rShoulderX[playerIndex] = 0; UBuffer.rShoulderY[playerIndex] = 0; UBuffer.rShoulderZ[playerIndex] = 0;
		UBuffer.rElbowX[playerIndex] = 0; UBuffer.rElbowY[playerIndex] = 0; UBuffer.rElbowZ[playerIndex] = 0;
		UBuffer.rHandX[playerIndex] = 0; UBuffer.rHandY[playerIndex] = 0; UBuffer.rHandZ[playerIndex] = 0;

		UBuffer.pelvisX[playerIndex] = 0; UBuffer.pelvisY[playerIndex] = 0; UBuffer.pelvisZ[playerIndex] = 0;

		UBuffer.lKneeX[playerIndex] = 0; UBuffer.lKneeY[playerIndex] = 0; UBuffer.lKneeZ[playerIndex] = 0;
		UBuffer.lFootX[playerIndex] = 0; UBuffer.lFootY[playerIndex] = 0; UBuffer.lFootZ[playerIndex] = 0;

		UBuffer.rKneeX[playerIndex] = 0; UBuffer.rKneeY[playerIndex] = 0; UBuffer.rKneeZ[playerIndex] = 0;
		UBuffer.rFootX[playerIndex] = 0; UBuffer.rFootY[playerIndex] = 0; UBuffer.rFootZ[playerIndex] = 0;
	}


	while (TRUE) { 
		SleepTime();



		//NOTE. DbgPrintEx can only print out a int so it might overflow
		//read from usermode
		KeReadProcessMemory(Process, KBufferAddr, &KBuffer, sizeof(KBufferStruct));
		//DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Value read: %d\n", KBuffer);

		//if nothing passed
		if (KBuffer.KBuffer == 0x0)
		{
			continue;
		}

		gamePID = KBuffer.PID;
		PEPROCESS gameProcess; // our target process
		PsLookupProcessByProcessId((PVOID)gamePID, &gameProcess); //lookup the process by it's id;

		////KeReadProcessMemory(gameProcess, (PVOID)()/**/, &, sizeof());

		uintptr_t localPlayer;
		KeReadProcessMemory(gameProcess, (PVOID)(KBuffer.KBuffer + 0x180ACA0)/*dwLocalPlayer*/, &localPlayer, sizeof(localPlayer));
		if (!localPlayer) { continue; }

		//DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "LocalPlayer: %d\n", (int)localPlayer);

		view_matrix_t view_matrix;
		KeReadProcessMemory(gameProcess, (PVOID)(KBuffer.KBuffer + 0x1819FC0/*dwViewMatrix*/), &view_matrix, sizeof(view_matrix));

		uintptr_t entity_list;
		KeReadProcessMemory(gameProcess, (PVOID)(KBuffer.KBuffer + 0x17BB820/*dwEntityList*/), &entity_list, sizeof(entity_list));

		int localTeam;
		KeReadProcessMemory(gameProcess, (PVOID)(localPlayer + 0x3BF)/*m_iTeamNum*/, &localTeam, sizeof(localTeam));

		//not needed
		//uintptr_t localPLayerPawn;
		//KeReadProcessMemory(gameProcess, (PVOID)(KBuffer.KBuffer)/*dwlocalPlayerPawn*/, &localPLayerPawn, sizeof(localPLayerPawn));
		//if (!localPLayerPawn) { continue; }

		int playerIndex = 0;

		//DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Team: %d\n", localTeam);
		
		while (true)
		{

			if (playerIndex > 62) //change to 32 later
			{
				break;
			}

			playerIndex++;

			UBuffer.x[playerIndex] = 0; UBuffer.y[playerIndex] = 0; UBuffer.z[playerIndex] = 0;
			UBuffer.headX[playerIndex] = 0; UBuffer.headY[playerIndex] = 0; UBuffer.headZ[playerIndex] = 0;
			UBuffer.neckX[playerIndex] = 0; UBuffer.neckY[playerIndex] = 0; UBuffer.neckZ[playerIndex] = 0;

			UBuffer.lShoulderX[playerIndex] = 0; UBuffer.lShoulderY[playerIndex] = 0; UBuffer.lShoulderZ[playerIndex] = 0;
			UBuffer.lElbowX[playerIndex] = 0; UBuffer.lElbowY[playerIndex] = 0; UBuffer.lElbowZ[playerIndex] = 0;
			UBuffer.lHandX[playerIndex] = 0; UBuffer.lHandY[playerIndex] = 0; UBuffer.lHandZ[playerIndex] = 0;

			UBuffer.rShoulderX[playerIndex] = 0; UBuffer.rShoulderY[playerIndex] = 0; UBuffer.rShoulderZ[playerIndex] = 0;
			UBuffer.rElbowX[playerIndex] = 0; UBuffer.rElbowY[playerIndex] = 0; UBuffer.rElbowZ[playerIndex] = 0;
			UBuffer.rHandX[playerIndex] = 0; UBuffer.rHandY[playerIndex] = 0; UBuffer.rHandZ[playerIndex] = 0;

			UBuffer.pelvisX[playerIndex] = 0; UBuffer.pelvisY[playerIndex] = 0; UBuffer.pelvisZ[playerIndex] = 0;
			
			UBuffer.lKneeX[playerIndex] = 0; UBuffer.lKneeY[playerIndex] = 0; UBuffer.lKneeZ[playerIndex] = 0;
			UBuffer.lFootX[playerIndex] = 0; UBuffer.lFootY[playerIndex] = 0; UBuffer.lFootZ[playerIndex] = 0;

			UBuffer.rKneeX[playerIndex] = 0; UBuffer.rKneeY[playerIndex] = 0; UBuffer.rKneeZ[playerIndex] = 0;
			UBuffer.rFootX[playerIndex] = 0; UBuffer.rFootY[playerIndex] = 0; UBuffer.rFootZ[playerIndex] = 0;

			uintptr_t list_entry;
			KeReadProcessMemory(gameProcess, (PVOID)(entity_list + (8 * (playerIndex & 0x7FFF) >> 9) + 16), &list_entry, sizeof(list_entry));
			//if (!list_entry) 
			//{	/*DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "List Entry: %d, %d\n", (int)list_entry, playerIndex);
			//	UBuffer.x[playerIndex] = 0; UBuffer.y[playerIndex] = 0; UBuffer.z[playerIndex] = 0;*/ continue; }
			
			uintptr_t player;
			KeReadProcessMemory(gameProcess, (PVOID)(list_entry + 120 * (playerIndex & 0x1FF)), &player, sizeof(player));
			if (!player) 
			{	//DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Player: %d, %d\n", (int)player, playerIndex);
				 continue; }
			
			//DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Player: %d, %d\n", (int)player, playerIndex);
			

			int playerTeam;
			KeReadProcessMemory(gameProcess, (PVOID)(player + 0x3BF)/*m_iTeamNum*/, &playerTeam, sizeof(playerTeam));
			if (playerTeam == localTeam || playerTeam == 0) 
			{	//DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Team: %d, ID: %d\n", playerTeam, playerIndex);
				 continue; }

			DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Team: %d, ID: %d\n", playerTeam, playerIndex);
			

			//print out team
			//DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Team: %d, ID: %d\n", playerTeam, playerIndex);
			
			
			
			uintptr_t playerPawn;
			KeReadProcessMemory(gameProcess, (PVOID)(player + 0x7EC /*m_hPlayerPawn*/), &playerPawn, sizeof(playerPawn));

			uintptr_t list_entry2;
			KeReadProcessMemory(gameProcess, (PVOID)(entity_list + 0x8 * ((playerPawn & 0x7FFF) >> 9) + 16), &list_entry2, sizeof(list_entry2));
			if (!list_entry2) 
			{	//DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "List Entry2: %d, %d\n", (int)list_entry2, playerIndex);
				 continue; }

			uintptr_t pCSPlayerPawn;
			KeReadProcessMemory(gameProcess, (PVOID)(list_entry2 + 120 * (playerPawn & 0x1FF)), &pCSPlayerPawn, sizeof(pCSPlayerPawn));
			if (!pCSPlayerPawn) 
			{	//DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "PlayerPawn: %d, %d\n", (int)pCSPlayerPawn, playerIndex);
				 continue; }

			DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "PlayerPawn: %d\n", (int)pCSPlayerPawn);

			

			int playerHealth;
			KeReadProcessMemory(gameProcess, (PVOID)(pCSPlayerPawn + 0x32C)/*m_iHealth*/, &playerHealth, sizeof(playerHealth));

			//DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Health: %d\n", playerHealth);

			if (playerHealth <= 0) 
			{	//DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Health: %d, %d\n", playerHealth, playerIndex);
				 continue; }

			if (pCSPlayerPawn == localPlayer) 
			{	//DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "LocalPlayer: %d, %d\n", (int)pCSPlayerPawn, playerIndex);
				 continue; }
			
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Health: %d\n", (int)playerHealth);

			Vector3 origin;
			//old offset always seems to work
			KeReadProcessMemory(gameProcess, (PVOID)(pCSPlayerPawn + 0x1224/*current but broken is 0x80, old 0x1224*/)/*m_vecOrigin*/, &origin, sizeof(origin));

			//sets up the new player positions
			UBuffer.x[playerIndex] = origin.x; UBuffer.y[playerIndex] = origin.y; UBuffer.z[playerIndex] = origin.z;

			uintptr_t gamescene;
			KeReadProcessMemory(gameProcess, (PVOID)(pCSPlayerPawn + 0x310 /*m_pGameSceneNode*/), &gamescene, sizeof(gamescene));
			uintptr_t bonearray;
			KeReadProcessMemory(gameProcess, (PVOID)(gamescene + 0x160 /*m_modelState */ + 0x80 /*m_vecOrigin*/), &bonearray, sizeof(bonearray));

			//6 -> 5 -> (8->9->10)(13->14->15)(1->(23->24)(26->27))
			Vector3 head;
			KeReadProcessMemory(gameProcess, (PVOID)(bonearray + 6 * 32), &head, sizeof(head));
			UBuffer.headX[playerIndex] = head.x; UBuffer.headY[playerIndex] = head.y; UBuffer.headZ[playerIndex] = head.z;

			Vector3 neck;
			KeReadProcessMemory(gameProcess, (PVOID)(bonearray + 5 * 32), &neck, sizeof(neck));
			UBuffer.neckX[playerIndex] = neck.x; UBuffer.neckY[playerIndex] = neck.y; UBuffer.neckZ[playerIndex] = neck.z;

			Vector3 lShoulder;
			KeReadProcessMemory(gameProcess, (PVOID)(bonearray + 8 * 32), &lShoulder, sizeof(lShoulder));
			UBuffer.lShoulderX[playerIndex] = lShoulder.x; UBuffer.lShoulderY[playerIndex] = lShoulder.y; UBuffer.lShoulderZ[playerIndex] = lShoulder.z;
			Vector3 lElbow;
			KeReadProcessMemory(gameProcess, (PVOID)(bonearray + 9 * 32), &lElbow, sizeof(lElbow));
			UBuffer.lElbowX[playerIndex] = lElbow.x; UBuffer.lElbowY[playerIndex] = lElbow.y; UBuffer.lElbowZ[playerIndex] = lElbow.z;
			Vector3 lHand;
			KeReadProcessMemory(gameProcess, (PVOID)(bonearray + 10 * 32), &lHand, sizeof(lHand));
			UBuffer.lHandX[playerIndex] = lHand.x; UBuffer.lHandY[playerIndex] = lHand.y; UBuffer.lHandZ[playerIndex] = lHand.z;

			Vector3 rShoulder;
			KeReadProcessMemory(gameProcess, (PVOID)(bonearray + 13 * 32), &rShoulder, sizeof(rShoulder));
			UBuffer.rShoulderX[playerIndex] = rShoulder.x; UBuffer.rShoulderY[playerIndex] = rShoulder.y; UBuffer.rShoulderZ[playerIndex] = rShoulder.z;
			Vector3 rElbow;
			KeReadProcessMemory(gameProcess, (PVOID)(bonearray + 14 * 32), &rElbow, sizeof(rElbow));
			UBuffer.rElbowX[playerIndex] = rElbow.x; UBuffer.rElbowY[playerIndex] = rElbow.y; UBuffer.rElbowZ[playerIndex] = rElbow.z;
			Vector3 rHand;
			KeReadProcessMemory(gameProcess, (PVOID)(bonearray + 15 * 32), &rHand, sizeof(rHand));
			UBuffer.rHandX[playerIndex] = rHand.x; UBuffer.rHandY[playerIndex] = rHand.y; UBuffer.rHandZ[playerIndex] = rHand.z;

			Vector3 pelvis;
			KeReadProcessMemory(gameProcess, (PVOID)(bonearray + 1 * 32), &pelvis, sizeof(pelvis));
			UBuffer.pelvisX[playerIndex] = pelvis.x; UBuffer.pelvisY[playerIndex] = pelvis.y; UBuffer.pelvisZ[playerIndex] = pelvis.z;

			Vector3 lKnee;
			KeReadProcessMemory(gameProcess, (PVOID)(bonearray + 23 * 32), &lKnee, sizeof(lKnee));
			UBuffer.lKneeX[playerIndex] = lKnee.x; UBuffer.lKneeY[playerIndex] = lKnee.y; UBuffer.lKneeZ[playerIndex] = lKnee.z;
			Vector3 lFoot;
			KeReadProcessMemory(gameProcess, (PVOID)(bonearray + 24 * 32), &lFoot, sizeof(lFoot));
			UBuffer.lFootX[playerIndex] = lFoot.x; UBuffer.lFootY[playerIndex] = lFoot.y; UBuffer.lFootZ[playerIndex] = lFoot.z;

			Vector3 rKnee;
			KeReadProcessMemory(gameProcess, (PVOID)(bonearray + 26 * 32), &rKnee, sizeof(rKnee));
			UBuffer.rKneeX[playerIndex] = rKnee.x; UBuffer.rKneeY[playerIndex] = rKnee.y; UBuffer.rKneeZ[playerIndex] = rKnee.z;
			Vector3 rFoot;
			KeReadProcessMemory(gameProcess, (PVOID)(bonearray + 27 * 32), &rFoot, sizeof(rFoot));
			UBuffer.rFootX[playerIndex] = rFoot.x; UBuffer.rFootY[playerIndex] = rFoot.y; UBuffer.rFootZ[playerIndex] = rFoot.z;

			UBuffer.health[playerIndex] = playerHealth;
			/*DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "X:%d Y:%d Z:%d\n", (int)origin.x, (int)origin.y, (int)origin.z);
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "%d %d %d\n", (int)head.x, (int)head.y, (int)head.z);

			DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "gamescene: %d\n", (int)gamescene);
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "bonearray: %d\n", (int)bonearray);
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "%d %d %d\n", (int)neck.x, (int)neck.y, (int)neck.z);
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "%d %d %d\n", (int)lShoulder.x, (int)lShoulder.y, (int)lShoulder.z);
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "%d %d %d\n", (int)lElbow.x, (int)lElbow.y, (int)lElbow.z);
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "%d %d %d\n", (int)lHand.x, (int)lHand.y, (int)lHand.z);
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "%d %d %d\n", (int)rShoulder.x, (int)rShoulder.y, (int)rShoulder.z);
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "%d %d %d\n", (int)rElbow.x, (int)rElbow.y, (int)rElbow.z);
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "%d %d %d\n", (int)rHand.x, (int)rHand.y, (int)rHand.z);
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "%d %d %d\n", (int)pelvis.x, (int)pelvis.y, (int)pelvis.z);
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "%d %d %d\n", (int)lKnee.x, (int)lKnee.y, (int)lKnee.z);
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "%d %d %d\n", (int)lFoot.x, (int)lFoot.y, (int)lFoot.z);
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "%d %d %d\n", (int)rKnee.x, (int)rKnee.y, (int)rKnee.z);
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "%d %d %d\n", (int)rFoot.x, (int)rFoot.y, (int)rFoot.z);*/

			
			//DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "X:%d\n", (int)origin);
			//DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "X:%d Y:%d Z:%d\n", (int)origin.x, (int)origin.y, (int)origin.z);


		}//end sudo for loop
		
	

		


		


		////write to usermode
		//int UBuffer = 567;		//write
		//IVector3 UBuffer = { 1,2,3 };
        // position
		KeWriteProcessMemory(Process, &UBuffer, vecAddr, sizeof(playerPOS));
		//view matrix
		KeWriteProcessMemory(Process, &view_matrix, viewMatrixAddr, sizeof(view_matrix_t));
		//DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "(W) Value written: %d\n", UBuffer.x[0]);
		
		//break;
	} //end while

} //end function

//triggerbot logic
// read PID from usermode
// get if process NULL then get process and baseAddr, otherwise, skip
// read from csgo: baseAddr + LocalPlayerPawn
// read enityID
// read entity list
// check if not -1 (may need some testing but should work, else CSGO method)


NTSTATUS DriverEntry(
	_In_ PDRIVER_OBJECT DriverObject,
	_In_ PUNICODE_STRING RegistryPath
) {
	UNREFERENCED_PARAMETER(DriverObject);
	UNREFERENCED_PARAMETER(RegistryPath);

	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "entered--------------\n");


	//makes handle
	HANDLE hThread;
	NTSTATUS status = PsCreateSystemThread(&hThread, THREAD_ALL_ACCESS, NULL, NULL, NULL, MyThreadFunction, NULL);
	if (!NT_SUCCESS(status)) { //checks if handle was created
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "failed\n");
		return status;
	}

	//closes the handle
	ObReferenceObjectByHandle(hThread, THREAD_ALL_ACCESS, NULL, KernelMode, &hThread, NULL);
	ZwClose(hThread);

	return STATUS_SUCCESS;
}


