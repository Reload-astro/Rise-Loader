#include <iostream>
#include <windows.h>
#include <fstream>
#include <filesystem>
#include <string>
#include "ui.hpp"
#include "../globals.hpp"
#include "../imgui/imgui.hpp"
#include "../imgui/imgui_internal.hpp"
#include <urlmon.h>
#pragma comment(lib, "urlmon.lib")

std::string ui::_GetCurrentDirectory()
{
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    std::string::size_type pos = std::string(buffer).find_last_of("\\/");

    return std::string(buffer).substr(0, pos);
}

size_t WriteCallbackD(void* contents, size_t size, size_t nmemb, void* userp) {
    // Calculate the total size of the downloaded data
    size_t totalSize = size * nmemb;

    // Cast the user pointer as a std::ofstream pointer
    std::ofstream* fileStream = static_cast<std::ofstream*>(userp);

    // Write the downloaded data to the file stream
    fileStream->write(static_cast<const char*>(contents), totalSize);

    // Return the total size of the downloaded data
    return totalSize;
}


size_t WriteCallback(void* contents, size_t size, size_t nmemb, FILE* file) {
    return fwrite(contents, size, nmemb, file);
}

// Function to download a file from a URL
void downloadFile(const std::string& url, const std::string& filepath) {
    HRESULT hr = URLDownloadToFileA(NULL, url.c_str(), filepath.c_str(), 0, NULL);
    if (hr != S_OK)
        throw std::runtime_error("Failed to download file");
}

void ui::DownloadConfig(const std::string& url, const std::string& name)
{
    std::string filepath = ui::_GetCurrentDirectory() + "\\save\\Rise\\configs\\"+ name + ".json";

    try {
        downloadFile(url, filepath);
        std::cout << "File downloaded successfully." << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

void ui::OpenFolder(const std::string& path)
{
    std::wstring wpath(path.begin(), path.end());
    ShellExecuteW(NULL, L"open", wpath.c_str(), NULL, NULL, SW_SHOWNORMAL);
}

void ui::http(const std::string& url)
{
#ifdef _WIN32
    ShellExecuteA(nullptr, "open", url.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
#else
    // Add platform-specific code here for Linux or macOS
    std::cout << "Opening website: " << url << std::endl;
#endif
}

void ui::start_rise() {
    // Get the current directory
    std::string path = ui::_GetCurrentDirectory();
    std::string save_load = path + "\\save";

    // Specify the path to the Java executable and its arguments
    std::string java_path = path + "\\files\\azul-1.8.9_345\\bin\\java.exe";
    std::string java_arguments = "-noverify -Xms4096m -Xmx4g -Djava.library.path=" + path + "\\files\\1.8.9-natives-win -cp \"" + path + "\\files\\RiseCompressed.jar;lwjgl.jar;lwjgl_util.jar\" net.minecraft.client.main.Main -uuid fc5bc365-aedf-30a8-8b89-04e462e29bde -accessToken yes -version 1";

    STARTUPINFOA startupInfo;
    PROCESS_INFORMATION processInfo;

    ZeroMemory(&startupInfo, sizeof(startupInfo));
    startupInfo.cb = sizeof(startupInfo);
    ZeroMemory(&processInfo, sizeof(processInfo));
    std::string commandLine = java_path + " " + java_arguments;

    // Launch the Java program
    if (!CreateProcessA(
        NULL,
        const_cast<LPSTR>(commandLine.c_str()),
        NULL,
        NULL,
        FALSE,
        0x08000000,
        NULL,
        save_load.c_str(), // Set the working directory to save_load
        &startupInfo,
        &processInfo
    )) {
        MessageBox(NULL, "Rise failed to start!", ui::window_title, MB_ICONERROR | MB_OK);;
        return;
    }

    // Close the handles to the Java process and thread to avoid resource leaks
    CloseHandle(processInfo.hProcess);
    CloseHandle(processInfo.hThread);
}

void ui::init(LPDIRECT3DDEVICE9 device) {
    dev = device;
	
    // colors
    ImGui::StyleColorsDark();

	if (window_pos.x == 0) {
		RECT screen_rect{};
		GetWindowRect(GetDesktopWindow(), &screen_rect);
		screen_res = ImVec2(float(screen_rect.right), float(screen_rect.bottom));
		window_pos = (screen_res - window_size) * 0.5f;

		// init images here
	}
}