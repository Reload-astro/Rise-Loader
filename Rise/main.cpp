#include "main.hpp"
#include "utill/ui/ui.hpp"
#include "utill/globals.hpp"
#include "utill/addons.hpp"

#define FLOATERS_LIMIT 300
#define WINDOW_WIDTH  1920
#define WINDOW_HEIGHT 1080
#define IMGUI_L 200
#define IMGUI_R 100
std::vector<addons::addons> floaters;
auto window_flags = ImGuiWindowFlags_WindowDecoration;

int APIENTRY WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, ui::window_title, NULL };
    RegisterClassEx(&wc);
    main_hwnd = CreateWindow(wc.lpszClassName, ui::window_title, WS_POPUP, 0, 0, 5, 5, NULL, NULL, wc.hInstance, NULL);

    if (!CreateDeviceD3D(main_hwnd)) {
        CleanupDeviceD3D();
        UnregisterClass(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    ShowWindow(main_hwnd, SW_HIDE);
    UpdateWindow(main_hwnd);

    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    ImGui_ImplWin32_Init(main_hwnd);
    ImGui_ImplDX9_Init(g_pd3dDevice);

    POINT mouse;
    RECT rc = { 0 };
    GetWindowRect(main_hwnd, &rc);

    addons::CreateFloater(floaters, FLOATERS_LIMIT, 5.f, 25.f, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, addons::vec3(0.f, 0.005f), IM_COL32_BLUE);

    MSG msg;
    ZeroMemory(&msg, sizeof(msg));
    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            continue;
        }

        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        {

            static int init = false;
            if (!init) {
                ui::init(g_pd3dDevice);
                init = true;
            }
            else {

                if (globals.active)
                {
                    ImGui::SetNextWindowPos(ImVec2(WINDOW_WIDTH/2, WINDOW_HEIGHT/2), ImGuiCond_Once);
                    ImGui::SetNextWindowSize(ImVec2(IMGUI_L, IMGUI_R));
                    ImGui::SetNextWindowBgAlpha(1.0f);
                    ImGui::Begin("Rise Loader", &globals.active, window_flags);
                    {
                        auto draw = ImGui::GetWindowDrawList();
                        auto pos = ImGui::GetWindowPos();
                        auto size = ImGui::GetWindowSize();
                        GetCursorPos(&mouse);
 
                        // render this before anything else so it is the background
                        addons::Update(floaters, addons::vec3(mouse.x, mouse.y)/*mouse x and y*/, addons::vec3(rc.left, rc.top)/*hWnd x and y positions*/); // you can change a few things inside the update function

                        if (ImGui::Button("       Load Client       ")) {
                            ui::start_rise();
                        }
                        if (ImGui::Button("       Open Vantage      ")) {
                            ui::http("https://vantage.rip");
                        }
                        if (ImGui::Button("       Rise Folder      ")) {
                            ui::OpenFolder(ui::_GetCurrentDirectory());
                        }
                    }
                    ImGui::End();
                }
            }
        }
        ImGui::EndFrame();

        g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
        if (g_pd3dDevice->BeginScene() >= 0)
        {
            ImGui::Render();
            ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
            g_pd3dDevice->EndScene();
        }

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }

        HRESULT result = g_pd3dDevice->Present(NULL, NULL, NULL, NULL);

        if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET) {
            ResetDevice();
        }
        if (!globals.active) {
            msg.message = WM_QUIT;
        }
    }

    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    DestroyWindow(main_hwnd);
    UnregisterClass(wc.lpszClassName, wc.hInstance);

    return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
        {
            g_d3dpp.BackBufferWidth = LOWORD(lParam);
            g_d3dpp.BackBufferHeight = HIWORD(lParam);
            ResetDevice();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU)
            return 0;
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}