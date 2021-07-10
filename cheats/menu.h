#pragma once
#include "../includes.hpp"
#include <wininet.h>
#pragma comment (lib, "Wininet.lib")

class c_menu : public singleton<c_menu> {
public:
	void DrawRadar();
	void AboutDyn(bool menu_open);
	void draw(bool is_open);
	void draw_indicators();
	void draw_keybinds();
	void menu_setup(ImGuiStyle& style);
	void DrawSkinTab();
	void DrawMiscTab();
	void DrawLegitTab();
	ImFont* super_ico;
	void DrawConfig();
	bool* menu;
	float dpi_scale = 1.f;
	char USERNAME[255];
	ImFont* futura;
	ImFont* futura_large;
	ImFont* arrow_font;
	ImFont* arrow;
	ImFont* load_font;
	ImFont* misc_font;
	ImFont* compart_font;
	ImFont* futura_small;
	ImFont* menu_font;
	ImFont* antiaim_icons;
	ImFont* gotham;

	ImFont* ico_menu;
	ImFont* speed_font;
	ImFont* stamina;

	float public_alpha;
	IDirect3DDevice9* device;

	int current_profile = -1;
	std::string getConfig;
	std::string c_menu::DownloadSkinBytes()
	{
		HINTERNET hOpen = NULL;
		HINTERNET hFile = NULL;
		char* lpBuffer = NULL;
		DWORD dwBytesRead = 0;
		//Pointer to dynamic buffer.
		char* data = 0;
		//Dynamic data size.
		DWORD dataSize = 0;

		hOpen = InternetOpenA("Dynamism", NULL, NULL, NULL, NULL);
		if (!hOpen) return (char*)"";

		char image_url[255];
		sprintf(image_url, "http://51.210.45.242/forum/uploads/%s", image);

		if (!image.empty())
			hFile = InternetOpenUrlA(hOpen, image_url, NULL, NULL, INTERNET_FLAG_RELOAD | INTERNET_FLAG_DONT_CACHE, NULL);
		else
			hFile = InternetOpenUrlA(hOpen, "https://cdn.discordapp.com/attachments/528431612220932127/750524573199499374/valence-ID-3cb0175b-87f3-48ba-ab06-f9bfdf2a4e35.png", NULL, NULL, INTERNET_FLAG_RELOAD | INTERNET_FLAG_DONT_CACHE, NULL);

		if (!hFile) {
			InternetCloseHandle(hOpen);
			return (char*)"";
		}

		std::string output;
		do {
			char buffer[2000];
			InternetReadFile(hFile, (LPVOID)buffer, _countof(buffer), &dwBytesRead);
			output.append(buffer, dwBytesRead);
		} while (dwBytesRead);

		InternetCloseHandle(hFile);
		InternetCloseHandle(hOpen);

		return output;
	}
private:

	bool __LOGGED = false;
	std::string username;

	std::string user_id;
	std::string userid;
	std::string joined_date;
	std::string expiration_date;
	std::string token;
	std::string image;
	std::string reponse_out;

	char PASSWORD[255];
	std::string MessageToSay;
	int subtab = 0; //Home
	int subtab2 = 0; //Rage
	int subtab3 = 0; //Legit
	int subtab4 = 0; // Visuals
	int subtab5 = 0; //Skin changer
	int subtab6 = 0; //Misc
	int subtab7 = 0;
private:

	struct {
		ImVec2 WindowPadding;
		float  WindowRounding;
		ImVec2 WindowMinSize;
		float  ChildRounding;
		float  PopupRounding;
		ImVec2 FramePadding;
		float  FrameRounding;
		ImVec2 ItemSpacing;
		ImVec2 ItemInnerSpacing;
		ImVec2 TouchExtraPadding;
		float  IndentSpacing;
		float  ColumnsMinSpacing;
		float  ScrollbarSize;
		float  ScrollbarRounding;
		float  GrabMinSize;
		float  GrabRounding;
		float  TabRounding;
		float  TabMinWidthForUnselectedCloseButton;
		ImVec2 DisplayWindowPadding;
		ImVec2 DisplaySafeAreaPadding;
		float  MouseCursorScale;
	} styles;

	bool update_dpi = false;
	bool update_scripts = false;
	void dpi_resize(float scale_factor, ImGuiStyle& style);

	int active_tab_index;
	ImGuiStyle style;
	int width = 850, height = 560;
	float child_height;

	float preview_alpha = 1.f;

	void DrawRageTab();
	void DrawMainTab();
	void draw_players();


	std::string preview = crypt_str("None");

public:
	std::string chatmessage;
	bool getConfigs = false;
};

