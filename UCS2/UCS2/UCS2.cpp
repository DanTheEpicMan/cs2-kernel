#include <iostream>
#include <Windows.h>
#include "memory.hpp"

float width = 2560; 
float height = 1440;

#include <objidl.h>
#include <gdiplus.h>

// dont dont dont
#include <fstream>
#include <string>
// dont dont dont

using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")

//edit if you need to pass through more
struct KBufferStruct
{
    unsigned long long KBuffer;
    int PID;
};

//edit if you meed to get through more
struct UBufferStruct
{
    int x, y, z;
};

struct Vector3
{
	float x, y, z;
};

struct playerPOS {
    int x[64], y[64], z[64];
};

struct view_matrix_t {
    float* operator[ ](int index) {
        return matrix[index];
    }

    float matrix[4][4];
};

struct Vector2
{
    float x, y;
};

Vector2 WorldToScreen(const Vector3 pos, view_matrix_t matrix, float width, float height) {
    //world space to clip space
    float _x = matrix[0][0] * pos.x + matrix[0][1] * pos.y + matrix[0][2] * pos.z + matrix[0][3];
    //std::cout << "x: " << _x << std::endl;
    float _y = matrix[1][0] * pos.x + matrix[1][1] * pos.y + matrix[1][2] * pos.z + matrix[1][3];
    //std::cout << "y: " << _y << std::endl;

    float w = matrix[3][0] * pos.x + matrix[3][1] * pos.y + matrix[3][2] * pos.z + matrix[3][3];
    //std::cout << "w: " << w << std::endl;

    if (w < 0.01f)
        return Vector2(0.0f, 0.0f);

    float inv_w = 1.f / w;
    _x *= inv_w;
    _y *= inv_w;
    //clip space to screen space
    //std::cout << "x2: " << _x << std::endl;
    //std::cout << "y2: " << _y << std::endl;

    float x = width * .5f;
    float y = height * .5f;
    //std::cout << "x3: " << x << std::endl;
    //std::cout << "y3: " << y << std::endl;

    x += 0.5f * _x * width + 0.5f;
    y -= 0.5f * _y * height + 0.5f;
    //std::cout << "x4: " << x << std::endl;
    //std::cout << "y4: " << y << std::endl;

    return Vector2(x, y);
}

int main()
{
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);


    HWND hWnd = FindWindow(NULL, TEXT("Non Existant Name"));
    HDC hdc;
    hdc = GetDC(hWnd);

    //Buffer to be passed to kernel
    KBufferStruct KBuffer;
    KBuffer.KBuffer = 0x0000;
    KBuffer.PID = 0;

    //BUffer written to by kernel
    UBufferStruct UBuffer;
    UBuffer.x = 4321;

    //were player positions are stored
    playerPOS vec;
    for (int i = 0; i < 64; i++)
    {
		vec.x[i] = 0;
		vec.y[i] = 0;
		vec.z[i] = 0;
	}


    //were out view matrix is stored
    view_matrix_t viewMatrix;

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 16; j++) {
            viewMatrix.matrix[i][j] = 0;
        }
    }

    //memory addreses
    const auto KBufferAddr = &KBuffer;
    const auto UBufferAddr = &UBuffer;
    const auto vecAddr = &vec;
    const auto viewMatrixAddr = &viewMatrix;
    DWORD pid = GetCurrentProcessId();

    // dont dont dont
    std::string data = "PVOID KBufferAddr = (PVOID)" + std::to_string((long long int)KBufferAddr) + ";\n" + "PVOID UBufferAddr = (PVOID)" + std::to_string((long long int)UBufferAddr) + ";\n" + "PVOID vecAddr = (PVOID)" + std::to_string((long long int)vecAddr) + ";\n" + "PVOID viewMatrixAddr = (PVOID)" + std::to_string((long long int)viewMatrixAddr) + ";\n" + "int PID = " + std::to_string((int)pid) + ";";
    std::ofstream outfile("C:\\Users\\Me\\Desktop\\addr.txt");

    if (outfile.is_open()) {
        outfile << data;
        outfile.close();
    }
    // dont dont dont

    //to be pasted into kernel, driver.cpp
    
    std::cout << "PVOID KBufferAddr = (PVOID)0x" << KBufferAddr << ";" << std::endl;
    std::cout << "PVOID UBufferAddr = (PVOID)0x" << UBufferAddr << ";" << std::endl;
    std::cout << "PVOID vecAddr = (PVOID)0x" << vecAddr << ";" << std::endl;
    std::cout << "PVOID viewMatrixAddr = (PVOID)0x" << viewMatrixAddr << ";" << std::endl;
    std::cout << "int PID = " << pid << ";" << std::endl;

    //used to hang until game is open
    int response;
    std::cout << "type any number when CS2 open: ";
    std::cin >> response;


    //PID of CS2
    DWORD processId = VARS::GetProcess(L"cs2.exe");

    //base addr of the game
    long long int baseAddress = VARS::GetModuleBaseAddress(processId, L"client.dll");

    std::cout << "Process ID: " << processId << std::endl;  
    std::cout << "Base Address: " << std::hex << baseAddress << std::endl;

    //passed to kernel input
    KBuffer.KBuffer = baseAddress;
    KBuffer.PID = processId;

    //debug
    //int bla = 0;
    //while (false)
    //{
    //    std::cout << "bla: "; std::cin >> bla;

    //    //value recived from driver
    //    for (int i = 0; i < 32; i++) {
    //        std::cout << "X: " << vec.x[i] << "  Y: " << vec.y[i] << "  Z: " << vec.z[i] << std::endl;

    //    }
    //    // value for view matrix from driver
    //    for (int i = 0; i < 4; i++) {
    //        std::cout << "View Matrix: " << viewMatrix.matrix[i][0] << "  " << viewMatrix.matrix[i][1] << "  " << viewMatrix.matrix[i][2] << "  " << viewMatrix.matrix[i][3] << std::endl;
    //    }
    //}

    while (true) {
       
        //go through all players
        for (int i = 0; i < 64; i++) {
            Vector3 pos = { (float)vec.x[i], (float)vec.y[i], (float)vec.z[i] }; //set localy for easy access
            if (pos.x == 0 && pos.y == 0 && pos.z == 0) { //check that player exists/not on same team/dead
                continue;
            }

            Vector3 head = { pos.x, pos.y, pos.z + 70.f }; //around the size from feet to head in the game

            //conversts from 3d to 2d
            Vector2 screen = WorldToScreen(pos, viewMatrix, width, height);
            Vector2 headScreen = WorldToScreen(head, viewMatrix, width, height);

            //draws line from head to feet
            Graphics graphics(hdc);
            Pen pen(Color(255, 255, 0, 0), 3);
            graphics.DrawLine(&pen, headScreen.x, headScreen.y, screen.x, screen.y);

		}
    }
        


    


}

/*
//were the 
struct C_UTL_VECTOR aimPunchCache stores to
{
    DWORD_PTR count = 0;
    DWORD_PTR data = 0;
};

* localplayer = class
* baseViewAnglesAddy = baseaddr + dwViewAngles
void aim::recoilControl(LocalPlayer localPlayer, DWORD_PTR baseViewAnglesAddy) {
    localPlayer.getAimPunchCache(); // aimPunchCache, writes to class
    localPlayer.getViewAngles(); // m_angEyeAngles  , writes to class

    static Vector3 oldPunch;
    Vector3 aimPunchAngle = MemMan.ReadMem<Vector3>(localPlayer.aimPunchCache.data + (localPlayer.aimPunchCache.count - 1) * sizeof(Vector3));

    if (localPlayer.getShotsFired() > 1) { //m_iShotsFired

        Vector3 recoilVector = {
            localPlayer.viewAngles.x + oldPunch.x - aimPunchAngle.x * 2.f,
            localPlayer.viewAngles.y + oldPunch.y - aimPunchAngle.y * 2.f
        };
        recoilVector = clampAngles(recoilVector);

        MemMan.WriteMem<Vector3>(baseViewAnglesAddy, recoilVector);
    }
    oldPunch.x = aimPunchAngle.x * 2.f;
    oldPunch.y = aimPunchAngle.y * 2.f;
}

*/