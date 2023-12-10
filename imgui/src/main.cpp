#include <iostream>
#include <format>

#include "memory.h"

#include <Windows.h>
#include <dwmapi.h>
#include <d3d11.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_dx11.h>
#include <imgui/imgui_impl_win32.h>
#include <windowsx.h>

#include <thread>

#include <fstream>
#include <string>

#include <chrono>
#include <thread>

#include <WinUser.h>

//#include "memory.hpp"

//
//                                      Cheat definition stuctures
// 
#pragma once
#include <windows.h>
#include <TlHelp32.h>
#define WIN32_LEAN_AND_MEAN




namespace VARS {
    DWORD GetProcess(const wchar_t* Target) {
        HANDLE snapshotHandle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

        if (snapshotHandle == INVALID_HANDLE_VALUE) {
            return NULL;
        }

        PROCESSENTRY32W processEntry = { };
        processEntry.dwSize = sizeof(PROCESSENTRY32W);

        if (Process32FirstW(snapshotHandle, &processEntry)) {

            do {

                if (_wcsicmp(processEntry.szExeFile, Target) == 0) {
                    CloseHandle(snapshotHandle);
                    return processEntry.th32ProcessID;
                }



            } while (Process32NextW(snapshotHandle, &processEntry));



        }

        CloseHandle(snapshotHandle);
        return NULL;

    }



    uintptr_t GetModuleBaseAddress(DWORD processId, const wchar_t* ModuleTarget) {

        HANDLE snapshotHandle = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processId);

        if (snapshotHandle == INVALID_HANDLE_VALUE) {
            return NULL;
        }

        MODULEENTRY32W moduleEntry = { };
        moduleEntry.dwSize = sizeof(MODULEENTRY32W);

        if (Module32FirstW(snapshotHandle, &moduleEntry)) {

            do {

                if (_wcsicmp(moduleEntry.szModule, ModuleTarget) == 0) {
                    CloseHandle(snapshotHandle);
                    return reinterpret_cast<uintptr_t>(moduleEntry.modBaseAddr);
                }



            } while (Module32NextW(snapshotHandle, &moduleEntry));



        }

        CloseHandle(snapshotHandle);
        return NULL;

    }

}
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
    //make 33 positions
    int x[64], y[64], z[64];
    int headX[64], headY[64], headZ[64];
    int neckX[64], neckY[64], neckZ[64];

    int lShoulderX[64], lShoulderY[64], lShoulderZ[64];
    int lElbowX[64], lElbowY[64], lElbowZ[64];
    int lHandX[64], lHandY[64], lHandZ[64];

    int rShoulderX[64], rShoulderY[64], rShoulderZ[64];
    int rElbowX[64], rElbowY[64], rElbowZ[64];
    int rHandX[64], rHandY[64], rHandZ[64];

    int pelvisX[64], pelvisY[64], pelvisZ[64];

    int lKneeX[64], lKneeY[64], lKneeZ[64];
    int lFootX[64], lFootY[64], lFootZ[64];

    int rKneeX[64], rKneeY[64], rKneeZ[64];
    int rFootX[64], rFootY[64], rFootZ[64];
    int health[64];
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
//
//                                      End of cheat definition structures
//

Vector2 WorldToScreen(const Vector3 pos, view_matrix_t matrix, float width, float height) {
    //world space to clip space
    float _x = matrix[0][0] * pos.x + matrix[0][1] * pos.y + matrix[0][2] * pos.z + matrix[0][3];
    float _y = matrix[1][0] * pos.x + matrix[1][1] * pos.y + matrix[1][2] * pos.z + matrix[1][3];
    float w = matrix[3][0] * pos.x + matrix[3][1] * pos.y + matrix[3][2] * pos.z + matrix[3][3];

    if (w < 0.01f)
        return Vector2(0.0f, 0.0f);

    float inv_w = 1.f / w;
    _x *= inv_w;
    _y *= inv_w;
    //clip space to screen space

    float x = width * .5f;
    float y = height * .5f;

    x += 0.5f * _x * width + 0.5f;
    y -= 0.5f * _y * height + 0.5f;

    return Vector2(x, y);
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK window_procedure(HWND window, UINT message, WPARAM w_param, LPARAM l_param) {
    if (ImGui_ImplWin32_WndProcHandler(window, message, w_param, l_param)) {
        return 0L;
    }

    if (message == WM_DESTROY) {
        PostQuitMessage(0);
        return 0L;
    }
    switch (message)
    {
    case WM_NCHITTEST:
    {
        // Allow the window to be moved by treating the title bar area as the caption
        const LONG borderWidth = GetSystemMetrics(SM_CXSIZEFRAME);
        const LONG titleBarHeight = GetSystemMetrics(SM_CYCAPTION);
        POINT cursorPos = { GET_X_LPARAM(w_param), GET_Y_LPARAM(l_param) };
        RECT windowRect;
        GetWindowRect(window, &windowRect);

        if (cursorPos.y >= windowRect.top && cursorPos.y < windowRect.top + titleBarHeight)
            return HTCAPTION;

        break;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(window, message, w_param, l_param);
}


int screenWidth = GetSystemMetrics(SM_CXSCREEN);
int screenHeight = GetSystemMetrics(SM_CYSCREEN);

INT APIENTRY WinMain(HINSTANCE instance, HINSTANCE, PSTR, INT cmd_show) {

    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = window_procedure;
    wc.hInstance = instance;
    wc.lpszClassName = L"External Overlay Class";

    RegisterClassExW(&wc);

    const HWND window = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED,
        wc.lpszClassName,
        L"Crosshair",
        WS_POPUP,
        0,
        0,
        screenWidth,
        screenHeight,
        nullptr,
        nullptr,
        wc.hInstance,
        nullptr
    );

    SetLayeredWindowAttributes(window, RGB(0, 0, 0), BYTE(255), LWA_ALPHA);

    {
        RECT client_area{};
        GetClientRect(window, &client_area);

        RECT window_area{};
        GetWindowRect(window, &window_area);

        POINT diff{};
        ClientToScreen(window, &diff);

        const MARGINS margins{
            window_area.left + (diff.x - window_area.left),
            window_area.top + (diff.y - window_area.top),
            client_area.right,
            client_area.bottom,

        };

        DwmExtendFrameIntoClientArea(window, &margins);
    }


    DXGI_SWAP_CHAIN_DESC sd{};
    sd.BufferDesc.RefreshRate.Numerator = 60U;
    sd.BufferDesc.RefreshRate.Denominator = 1U;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.SampleDesc.Count = 1U;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.BufferCount = 2U;
    sd.OutputWindow = window;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    constexpr D3D_FEATURE_LEVEL levels[2]{
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_0,
    };

    ID3D11Device* device{ nullptr };
    ID3D11DeviceContext* device_context{ nullptr };
    IDXGISwapChain* swap_chain{ nullptr };
    ID3D11RenderTargetView* render_target_view{ nullptr };
    D3D_FEATURE_LEVEL level{};

    //create device and that
    D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        0U,
        levels,
        2U,
        D3D11_SDK_VERSION,
        &sd,
        &swap_chain,
        &device,
        &level,
        &device_context
    );

    ID3D11Texture2D* back_buffer{ nullptr };
    swap_chain->GetBuffer(0U, IID_PPV_ARGS(&back_buffer));

    if (back_buffer) {
        device->CreateRenderTargetView(back_buffer, nullptr, &render_target_view);
        back_buffer->Release();
    }
    else {
        return 1;
    }

    ShowWindow(window, cmd_show);
    UpdateWindow(window);

    ImGui::CreateContext();

    //set up ImGui style andcontent
    ImGui::StyleColorsDark();

    //Display the additional window

    ImGui_ImplWin32_Init(window);
    ImGui_ImplDX11_Init(device, device_context);

    bool running = true;

    float centerX = screenWidth / 2.0f;
    float centerY = screenHeight / 2.0f;
    float crosshairSize = 10.0f;
    float lineWidth = 1.5f;
    float gapSize = 4.0f;
    float dotSize = 2.0f;

    //                                  defining our cheat variables
    // 
    //Buffer to be passed to kernel
    KBufferStruct KBuffer;
    KBuffer.KBuffer = 0x0000;
    KBuffer.PID = 0;

    //BUffer written to by kernel
    UBufferStruct UBuffer;
    UBuffer.x = 4321;

    //were player positions are stored
    playerPOS vec;
    for (int playerIndex = 0; playerIndex < 64; playerIndex++)
    {
        vec.x[playerIndex] = 0; vec.y[playerIndex] = 0; vec.z[playerIndex] = 0;
        vec.headX[playerIndex] = 0; vec.headY[playerIndex] = 0; vec.headZ[playerIndex] = 0;
        vec.neckX[playerIndex] = 0; vec.neckY[playerIndex] = 0; vec.neckZ[playerIndex] = 0;

        vec.lShoulderX[playerIndex] = 0; vec.lShoulderY[playerIndex] = 0; vec.lShoulderZ[playerIndex] = 0;
        vec.lElbowX[playerIndex] = 0; vec.lElbowY[playerIndex] = 0; vec.lElbowZ[playerIndex] = 0;
        vec.lHandX[playerIndex] = 0; vec.lHandY[playerIndex] = 0; vec.lHandZ[playerIndex] = 0;

        vec.rShoulderX[playerIndex] = 0; vec.rShoulderY[playerIndex] = 0; vec.rShoulderZ[playerIndex] = 0;
        vec.rElbowX[playerIndex] = 0; vec.rElbowY[playerIndex] = 0; vec.rElbowZ[playerIndex] = 0;
        vec.rHandX[playerIndex] = 0; vec.rHandY[playerIndex] = 0; vec.rHandZ[playerIndex] = 0;

        vec.pelvisX[playerIndex] = 0; vec.pelvisY[playerIndex] = 0; vec.pelvisZ[playerIndex] = 0;

        vec.lKneeX[playerIndex] = 0; vec.lKneeY[playerIndex] = 0; vec.lKneeZ[playerIndex] = 0;
        vec.lFootX[playerIndex] = 0; vec.lFootY[playerIndex] = 0; vec.lFootZ[playerIndex] = 0;

        vec.rKneeX[playerIndex] = 0; vec.rKneeY[playerIndex] = 0; vec.rKneeZ[playerIndex] = 0;
        vec.rFootX[playerIndex] = 0; vec.rFootY[playerIndex] = 0; vec.rFootZ[playerIndex] = 0;
    }

    //were out view matrix is stored
    view_matrix_t viewMatrix;

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 16; j++) {
            viewMatrix.matrix[i][j] = 0;
        }
    }

    const auto KBufferAddr = &KBuffer;
    const auto UBufferAddr = &UBuffer;
    const auto vecAddr = &vec;
    const auto viewMatrixAddr = &viewMatrix;
    DWORD pid = GetCurrentProcessId();

    //PID of CS2
    DWORD processId = VARS::GetProcess(L"cs2.exe");

    //base addr of the game
    long long int baseAddress = VARS::GetModuleBaseAddress(processId, L"client.dll");

    

    //write out "test" to C:\Users\Me\Desktop\addr.txt
    //                                  out replacement for the console
    std::string data = "PVOID KBufferAddr = (PVOID)" + std::to_string((long long int)KBufferAddr) + ";\n" + "PVOID UBufferAddr = (PVOID)" + std::to_string((long long int)UBufferAddr) + ";\n" + "PVOID vecAddr = (PVOID)" + std::to_string((long long int)vecAddr) + ";\n" + "PVOID viewMatrixAddr = (PVOID)" + std::to_string((long long int)viewMatrixAddr) + ";\n" + "int PID = " + std::to_string((int)pid) + ";";
    std::ofstream outfile("C:\\Users\\Me\\Desktop\\addr.txt");

    if (outfile.is_open()) {
        outfile << data;
        outfile.close();
    }

    //passed to kernel input


    int width = 2560;
    int height = 1440;

    bool startDriver = false; //not really but the driver wont start reading until this is true
    
    HWND TargetWindow = FindWindowA(NULL, "Counter-Strike 2");
    while (running) {
        /*RECT r;
        GetWindowRect(TargetWindow, &r);*/
        //SetWindowPos(window, nullptr, r.left, r.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
        //SetWindowPos(window, nullptr, 0, 0, r.right - r.left, r.bottom - r.top, SWP_NOMOVE | SWP_NOZORDER);

        //SetWindowPos(window, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
        //SetWindowPos(window, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);

        //SetWindowPos(TargetWindow, window, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
       

        MSG msg;
        while (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);

            if (msg.message == WM_QUIT) {
                running = false;
            }
        }

        if (!running) {
            break;
        }

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        ImDrawList* drawList = ImGui::GetBackgroundDrawList();
        

        //drawList->AddCircleFilled({ static_cast<float>(screenWidth) / 2, static_cast<float>(screenHeight) / 2 }, 5.f, ImColor(1.f, 0.f, 0.f));
        // Draw horizontal line
        //drawList->AddLine({ centerX - crosshairSize - gapSize, centerY }, { centerX - gapSize, centerY }, ImColor(255, 150, 150), lineWidth);
        //drawList->AddLine({ centerX + gapSize, centerY }, { centerX + crosshairSize + gapSize, centerY }, ImColor(255, 150, 150), lineWidth);

        // Draw vertical line
        //drawList->AddLine({ centerX, centerY - crosshairSize - gapSize }, { centerX, centerY - gapSize }, ImColor(255, 150, 150), lineWidth);
        //drawList->AddLine({ centerX, centerY + gapSize }, { centerX, centerY + crosshairSize + gapSize }, ImColor(255, 150, 150), lineWidth);



        if (startDriver)
        {
            for (int i = 0; i < 64; i++) {
                Vector3 pos = { (float)vec.x[i], (float)vec.y[i], (float)vec.z[i] }; //set localy for easy access
                if (pos.x == 0 && pos.y == 0 && pos.z == 0) { //check that player exists/not on same team/dead
                    continue;
                }

                //red       EE6352: (0.933, 0.388, 0.322)
                //green     59CD90 : (0.349, 0.804, 0.565)
                //blue      3FA7D6 : (0.247, 0.655, 0.839)
                //yellow    FAC05E : (0.980, 0.753, 0.369)
                //pink      F79D84 : (0.969, 0.616, 0.518)
                auto red = ImColor(0.933f, 0.388f, 0.322f);
                auto green = ImColor(0.349f, 0.804f, 0.565f);
                auto blue = ImColor(0.247f, 0.655f, 0.839f);
                auto yellow = ImColor(0.980f, 0.753f, 0.369f);
                auto pink = ImColor(0.969f, 0.616f, 0.518f);



                Vector3 head = { (float)vec.headX[i], (float)vec.headY[i], (float)vec.headZ[i] };
                Vector3 neck = { (float)vec.neckX[i], (float)vec.neckY[i], (float)vec.neckZ[i] };

                Vector3 lShoulder = { (float)vec.lShoulderX[i], (float)vec.lShoulderY[i], (float)vec.lShoulderZ[i] };
                Vector3 lElbow = { (float)vec.lElbowX[i], (float)vec.lElbowY[i], (float)vec.lElbowZ[i] };
                Vector3 lHand = { (float)vec.lHandX[i], (float)vec.lHandY[i], (float)vec.lHandZ[i] };

                Vector3 rShoulder = { (float)vec.rShoulderX[i], (float)vec.rShoulderY[i], (float)vec.rShoulderZ[i] };
                Vector3 rElbow = { (float)vec.rElbowX[i], (float)vec.rElbowY[i], (float)vec.rElbowZ[i] };
                Vector3 rHand = { (float)vec.rHandX[i], (float)vec.rHandY[i], (float)vec.rHandZ[i] };

                Vector3 pelvis = { (float)vec.pelvisX[i], (float)vec.pelvisY[i], (float)vec.pelvisZ[i] };

                Vector3 lKnee = { (float)vec.lKneeX[i], (float)vec.lKneeY[i], (float)vec.lKneeZ[i] };
                Vector3 lFoot = { (float)vec.lFootX[i], (float)vec.lFootY[i], (float)vec.lFootZ[i] };

                Vector3 rKnee = { (float)vec.rKneeX[i], (float)vec.rKneeY[i], (float)vec.rKneeZ[i] };
                Vector3 rFoot = { (float)vec.rFootX[i], (float)vec.rFootY[i], (float)vec.rFootZ[i] };


                //Vector2 screen = WorldToScreen(head, viewMatrix, 2560, 1440);
                Vector2 posScreen = WorldToScreen(pos, viewMatrix, width, height);
                Vector2 screenHead = WorldToScreen(head, viewMatrix, width, height);
                Vector2 screenNeck = WorldToScreen(neck, viewMatrix, width, height);

                Vector2 screenLShoulder = WorldToScreen(lShoulder, viewMatrix, width, height);
                Vector2 screenLElbow = WorldToScreen(lElbow, viewMatrix, width, height);
                Vector2 screenLHand = WorldToScreen(lHand, viewMatrix, width, height);

                Vector2 screenRShoulder = WorldToScreen(rShoulder, viewMatrix, width, height);
                Vector2 screenRElbow = WorldToScreen(rElbow, viewMatrix, width, height);
                Vector2 screenRHand = WorldToScreen(rHand, viewMatrix, width, height);

                Vector2 screenPelvis = WorldToScreen(pelvis, viewMatrix, width, height);

                Vector2 screenLKnee = WorldToScreen(lKnee, viewMatrix, width, height);
                Vector2 screenLFoot = WorldToScreen(lFoot, viewMatrix, width, height);

                Vector2 screenRKnee = WorldToScreen(rKnee, viewMatrix, width, height);
                Vector2 screenRFoot = WorldToScreen(rFoot, viewMatrix, width, height);


                drawList->AddCircle({ screenHead.x, screenHead.y }, (float)(screenNeck.y-screenHead.y)/*5.f*/, blue);

                //drawList->AddLine({ screenHead.x, screenHead.y }, { screenNeck.x, screenNeck.y }, ImColor(1.f, 0.f, 0.f));

                drawList->AddLine({ screenNeck.x, screenNeck.y }, { screenLShoulder.x, screenLShoulder.y }, blue);
                drawList->AddLine({ screenLShoulder.x, screenLShoulder.y }, { screenLElbow.x, screenLElbow.y }, blue);
                drawList->AddLine({ screenLElbow.x, screenLElbow.y }, { screenLHand.x, screenLHand.y }, blue);

                drawList->AddLine({ screenNeck.x, screenNeck.y }, { screenRShoulder.x, screenRShoulder.y }, blue);
                drawList->AddLine({ screenRShoulder.x, screenRShoulder.y }, { screenRElbow.x, screenRElbow.y }, blue);
                drawList->AddLine({ screenRElbow.x, screenRElbow.y }, { screenRHand.x, screenRHand.y }, blue);

                drawList->AddLine({ screenNeck.x, screenNeck.y }, { screenPelvis.x, screenPelvis.y }, blue);

                drawList->AddLine({ screenPelvis.x, screenPelvis.y }, { screenLKnee.x, screenLKnee.y }, blue);
                drawList->AddLine({ screenLKnee.x, screenLKnee.y }, { screenLFoot.x, screenLFoot.y }, blue);

                drawList->AddLine({ screenPelvis.x, screenPelvis.y }, { screenRKnee.x, screenRKnee.y }, blue);
                drawList->AddLine({ screenRKnee.x, screenRKnee.y }, { screenRFoot.x, screenRFoot.y }, blue);



                float height = screenHead.y - posScreen.y;
                float healthBarHeight = height * (vec.health[i] / 100.0f);
                float healthBarHeightNeg = height - healthBarHeight;

                int barOffset = height / 3;

                drawList->AddLine({ posScreen.x + barOffset, posScreen.y }, { posScreen.x + barOffset, posScreen.y + healthBarHeight }, green);
                drawList->AddLine({ posScreen.x + barOffset, posScreen.y + height }, { posScreen.x + barOffset, posScreen.y + healthBarHeight }, red);
                /*float height = screenHead.y - screenHealth.y;
                float healthBarHeight = height * (1 / 100.0f);

                drawList->AddRectFilled({ screenHealth.x, screenHealth.y }, { screenHealth.x, screenHealth.y + healthBarHeight }, ImColor(0.f, 1.f, 0.f));*/

                //conversts from 3d to 2d
                //Vector2 screen = WorldToScreen(pos, viewMatrix, 2560, 1440);

                //drawList->AddCircleFilled({ screen.x, screen.y }, 5.f, ImColor(1.f, 0.f, 0.f));
            }//while loop
        } else {//if startDriver = false
            std::string myText;
            std::ifstream MyReadFile("C:\\Users\\Me\\Desktop\\addr.txt");

            getline(MyReadFile, myText);

            MyReadFile.close();

            if (myText == "") {
				startDriver = true;

                std::ofstream MyWriteFile("C:\\Users\\Me\\Desktop\\addr.txt");

                if (MyWriteFile.is_open()) {
                    MyWriteFile << "recived";
                    MyWriteFile.close();
                }

                KBuffer.KBuffer = baseAddress;
                KBuffer.PID = processId;
			}
        }

        //Rendering :)
        ImGui::Render();


        constexpr float color[4]{ 0.f, 0.f, 0.f, 0.f };
        device_context->OMSetRenderTargets(1U, &render_target_view, nullptr);
        device_context->ClearRenderTargetView(render_target_view, color);

        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        swap_chain->Present(1U, 0U);

        
    }

    


    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();

    ImGui::DestroyContext();

    if (swap_chain) {
        swap_chain->Release();
    }

    if (device_context) {
        device_context->Release();
    }

    if (device) {
        device->Release();
    }

    if (render_target_view) {
        render_target_view->Release();
    }

    DestroyWindow(window);
    UnregisterClassW(wc.lpszClassName, wc.hInstance);

    return 0;
}
