#pragma once
#include <d3d9.h>
#include "../imgui/imgui.hpp"

namespace ui {
	void init(LPDIRECT3DDEVICE9);
	void start_rise();
	void http(const std::string& url);
	void DownloadConfig(const std::string& url, const std::string& name);
	void OpenFolder(const std::string& url);
	std::string _GetCurrentDirectory();
}

namespace ui {
	inline LPDIRECT3DDEVICE9 dev;
	inline const char* window_title = "Rise Loader";
}

namespace ui {
	inline ImVec2 screen_res{ 000, 000 };
	inline ImVec2 window_pos{ 0, 0 };
	inline ImVec2 window_size{ 400, 300 };
	inline DWORD  window_flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar;
}