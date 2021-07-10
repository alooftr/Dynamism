// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <ShlObj_core.h>
#include <unordered_map>
#include "menu.h"
#include "../ImGui/code_editor.h"
#include "../constchars.h"
#include "../cheats/visuals/player_esp.h"
#include "../Kit_parser.h"
#include "../cheats/misc/logs.h"


#include <ImGui\implot.h>
#include "misc/misc.h"

#define ALPHA (ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaBar| ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_Float)
#define NOALPHA (ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_Float)

std::vector <std::string> files;
std::vector <std::string> scripts;
std::string editing_script;

auto selected_script = 0;
static auto loaded_editing_script = false;

static auto menu_setupped = false;
static auto should_update = true;

IDirect3DTexture9* mapCachedImages[4000];
IDirect3DTexture9* all_skins[35];
IDirect3DTexture9* skin_knife[100];
IDirect3DTexture9* skins_to_preview[50];
IDirect3DTexture9* skins_to_edit = nullptr;

#include <iostream>
#include <fstream>  
std::vector<int> gun_definitioni = { 0, 1, 9, 7, 16, 61, 40, 60, 8, 4, 38, 10, 13, 30, 11, 24, 19, 3, 36, 39, 33, 32, 63, 17, 2, 26, 27, 34, 23, 64, 35, 25, 28, 14, 29,
								515, 514, 500, 512, 505, 506, 509, 507, 508, 520, 516, 522, 523, 519, 503, 517, 518, 521, 525,
								5030,5033,5034,5031,5032,5027,5035,

}; // 503 - > 519
std::vector<int> knife_definition = { 0, 507 };
int isdownloaded = 0;
int getwep(int a) {
	int i = 0;
	for (auto p : gun_definitioni) {
		if (p == a) {
			return i;
		}
		++i;
	}
}


std::map<int, int> knifesid = { {6, 8}, {7, 7}, {8, 0}, {9, 6}, {10, 1}, {11, 2}, {12, 5}, {13, 3}, {14, 4}, {15, 10}, {16, 9}, {17, 11}, {18, 13}, {19, 12 }, {20, 14}, {21, 15}, {22, 16}, {23, 17}, {24, 18} }; //14 -> 13
std::map<int, int> knifesidn = { {8, 6}, {7, 7}, {0, 8}, {6, 9}, {1, 10}, {2, 11}, {5, 12}, {3, 13}, {4, 14}, {10, 15}, {9, 16}, {11, 17}, {13, 18}, {12, 19}, {14, 20}, {15, 21}, {16, 22}, {17, 23}, {18, 24} };


const int skins_weapon_count = 35;
const int skins_knife_count = 19;
const int skins_glove_count = 7;
const int skinsCount = skins_weapon_count + skins_knife_count + skins_glove_count;
int gunLen[skinsCount] = { 0, 25, 27, 30, 28, 23, 17, 24, 25, 28, 19, 22, 24, 29, 21, 26, 27, 27, 34, 21, 24, 22, 22, 27, 23, 25, 22, 24, 6, 10, 23, 23, 14, 12, 25,
						   18, 18, 24, 18, 24, 24, 18, 24, 24, 12, 18, 12, 12, 12, 12, 12, 12, 12, 12,
						   8,8,8,8,8,4,4 };
std::map<int, IDirect3DTexture9*> alltextures[skinsCount];

std::vector<std::vector<int>> vskns = {
	{0},
	{757, 232, 17, 37, 40, 61, 90, 185, 231, 237, 273, 296, 328, 347, 351, 397, 425, 468, 469, 527, 509, 603, 645, 711, 805, 841}, /*deagle*/  /*25*/ /* 1 */
	{887, 756, 736, 917, 30, 51, 72, 84, 174, 181, 212, 227, 251, 259, 279, 344, 395, 424, 446, 451, 475, 525, 584, 640, 662, 691, 718, 788, 803, 838}, /*awp*/  /*27*/ /* 9*/
	{724, 14, 44, 72, 122, 170, 172, 180, 226, 282, 300, 316, 302, 340, 341, 380, 394, 422, 456, 474, 490, 506, 524, 600, 639, 656, 675, 707, 795, 801, 836}, /*ak47*/  /*30*/ /* 7*/
	{8, 16, 17, 101, 155, 164, 167, 176, 187, 215, 255, 309, 336, 384, 400, 449, 471, 480, 512, 533, 588, 632, 664, 695, 780, 793, 811, 844}, /*m4a4*/  /*28*/ /* 16 */

	{25, 60, 183, 217, 221, 236, 277, 290, 313, 318, 332, 339, 364, 454, 489, 504, 540, 637, 653, 657, 705, 796, 817}, /*usp*/  /*23*/ /* 61 */
	{899, 868, 26, 60, 96, 99, 200, 222, 233, 253, 304, 319, 361, 503, 538, 554, 624, 670, 751}, /*ssg08*/  /*17*/ /* 40 */
	{60, 77, 189, 217, 235, 254, 257, 301, 321, 326, 360, 383, 430, 440, 445, 497, 548, 587, 631, 644, 663, 681, 714, 792}, /*m4a1s*/  /*24*/ /* 60 */
	{758, 727, 913, 9, 10, 197, 33, 46, 47, 73, 100, 110, 375, 280, 305, 444, 455, 507, 541, 583, 601, 674, 690, 708, 779, 794, 845}, /*aug*/  /*25*/ /* 8 */

	{732, 918, 2, 3, 38, 40, 48, 159, 208, 230, 278, 293, 353, 367, 381, 399, 437, 479, 495, 532, 586, 607, 623, 680, 694, 713, 789, 799, 808}, /*glock*/  /*28*/ /* 4 */
	{914, 46, 70, 100, 116, 157, 165, 196, 232, 298, 312, 391, 406, 502, 518, 597, 612, 641, 642}, /*scar*/  /*19*/ /* 38 */
	{904, 919, 22, 47, 92, 154, 178, 194, 218, 244, 260, 288, 371, 429, 477, 492, 529, 604, 626, 659, 723, 835}, /*famas*/  /*22*/ /* 10 */
	{76, 83, 119, 192, 216, 235, 237, 241, 264, 265, 297, 379, 398, 428, 460, 478, 494, 546, 629, 647, 661, 790, 807, 842}, /*galil*/  /*24*/ /* 13 */

	{889, 905, 2, 17, 36, 159, 179, 206, 216, 235, 242, 248, 264, 272, 303, 374, 439, 459, 463, 520, 539, 555, 599, 614, 671, 684, 722, 791, 816, 839}, /*tec9*/  /*29*/ /* 30 */
	{6, 8, 46, 72, 74, 147, 195, 229, 235, 294, 382, 438, 465, 493, 511, 545, 606, 628, 677, 712, 806}, /*g3sg1*/  /*21*/ /* 11*/
	{916, 15, 17, 37, 70, 90, 93, 169, 175, 193, 281, 333, 362, 392, 436, 441, 488, 556, 615, 652, 672, 688, 704, 778, 802, 851}, /*ump45*/  /*26*/ /* 24 */
	{759, 911, 20, 67, 100, 111, 124, 156, 169, 175, 182, 228, 234, 244, 283, 311, 335, 342, 359, 486, 516, 593, 611, 636, 669, 717, 776, 849}, /*p90*/  /*27*/ /* 19 */

	{906, 3, 44, 46, 78, 141, 151, 210, 223, 252, 254, 265, 274, 352, 377, 387, 427, 464, 510, 530, 585, 605, 646, 660, 693, 784, 837}, /*fiveseven*/  /*27*/ /*3*/
	{907, 15, 27, 34, 77, 99, 102, 162, 164, 168, 207, 219, 230, 258, 271, 295, 358, 373, 388, 404, 426, 466, 467, 501, 551, 592, 650, 668, 678, 749, 777, 786, 813, 848}, /*p250*/  /*34*/ /* 36 */
	{897, 28, 39, 98, 101, 136, 186, 243, 247, 287, 298, 363, 378, 487, 519, 553, 598, 613, 686, 702, 750, 815}, /*sg553*/  /*21*/ /* 39 */
	{5, 11, 15, 28, 102, 141, 209, 213, 245, 250, 354, 365, 423, 442, 481, 500, 536, 627, 649, 696, 719, 752, 782, 847}, /*mp7*/  /*24*/ /* 33 */

	{894, 21, 32, 71, 95, 104, 184, 211, 246, 275, 327, 338, 346, 357, 389, 443, 485, 515, 550, 591, 635, 667, 700}, /*p2000*/  /*22*/ /* 32*/
	{12, 218, 268, 269, 270, 297, 298, 315, 322, 325, 334, 350, 366, 435, 453, 476, 543, 602, 622, 643, 687, 709}, /*cz75*/  /*22*/ /* 63 */
	{898, 742, 908, 3, 17, 32, 38, 98, 101, 157, 188, 246, 284, 310, 333, 337, 343, 372, 402, 433, 498, 534, 550, 589, 651, 665, 748, 812, 840}, /*mac10*/  /*27*/ /*17 */
	{903, 28, 43, 46, 47, 153, 190, 220, 249, 261, 276, 307, 330, 396, 447, 450, 491, 528, 544, 625, 658, 710, 747}, /*dualberettas*/  /*23*/ /* 2 */

	{3, 13, 25, 70, 148, 149, 159, 164, 171, 203, 224, 236, 267, 306, 349, 376, 457, 508, 526, 542, 594, 641, 676, 692, 775}, /*bizon*/  /*25*/ /* 26 */
	{737, 909, 32, 34, 39, 99, 100, 171, 177, 198, 291, 385, 431, 462, 473, 499, 535, 608, 633, 666, 703, 754, 787}, /*mag7*/  /*22*/ /* 27 */
	{867, 734, 910, 33, 39, 61, 100, 141, 148, 199, 262, 329, 366, 368, 386, 403, 448, 482, 549, 609, 630, 679, 697, 715, 755, 804}, /*mp9*/  /*24*/ /* 34 */
	{915, 753, 781, 800, 810, 846}, /*mp5*/  /*5*/ /* 23 */

	{12, 27, 522, 523, 595, 683, 701, 721, 798, 843}, /*revolver*/  /*10*/ /* 64 */
	{746, 3, 25, 62, 99, 107, 158, 164, 166, 170, 191, 214, 225, 263, 286, 294, 299, 356, 450, 484, 537, 590, 634, 699, 716, 785, 809}, /*nova*/  /*26*/ /* 35 */
	{42, 95, 96, 135, 166, 169, 205, 238, 240, 314, 320, 348, 370, 393, 407, 505, 521, 557, 616, 654, 689, 706, 850}, /*xm*/  /*23*/ /* 25 */
	{763, 28, 157, 240, 285, 298, 317, 336, 369, 432, 483, 514, 610, 698, 783}, /*negev*/  /*14*/

	{902, 22, 75, 202, 243, 266, 401, 452, 472, 496, 547, 648}, /*m249*/  /*12*/
	{5, 30, 41, 83, 119, 171, 203, 204, 246, 250, 256, 345, 390, 405, 434, 458, 517, 552, 596, 638, 655, 673, 720, 797, 814}, /*sawedoff*/  /*25*/

	{5, 12, 38, 40, 42, 43, 44, 59, 72, 77, 98, 143, 175, 409, 413, 414, 416, 411}, /*babochka*/ /*18*/
	{5, 12, 38, 40, 42, 43, 44, 59, 72, 77, 98, 143, 175, 409, 413, 414, 416, 411}, /*bowie*/    /*18*/
	{5, 12, 38, 40, 42, 43, 44, 59, 72, 77, 98, 143, 175, 409, 410, 413, 414, 416, 558, 563, 568, 573, 578, 580}, /*bayonet*/  /*24*/
	{5, 12, 38, 40, 42, 43, 44, 59, 72, 77, 98, 143, 175, 409, 413, 414, 416, 411}, /*falshion*/  /*18*/
	{5, 12, 38, 40, 42, 43, 44, 59, 72, 77, 98, 143, 175, 409, 410, 413, 414, 416, 559, 564, 568, 574, 579, 580}, /*flip*/  /*24*/
	{5, 12, 38, 40, 42, 43, 44, 59, 72, 77, 98, 143, 175, 409, 568, 413, 414, 416, 410, 560, 565, 575, 578, 580}, /*gut*/  /*24*/
	{5, 12, 38, 40, 42, 43, 44, 59, 72, 77, 98, 143, 175, 409, 413, 414, 416, 411}, /*hunstman*/  /*18*/
	{5, 12, 38, 40, 42, 43, 44, 59, 72, 77, 98, 143, 175, 409, 410, 413, 414, 416, 561, 566, 576, 578, 568, 582}, /*karambit*/  /*24*/
	{5, 12, 38, 40, 42, 43, 44, 59, 72, 77, 98, 143, 175, 409, 568, 413, 414, 416, 411, 562, 567, 577, 579, 581}, /*m9bay*/  /*24*/
	{5, 12, 38, 40, 42, 43, 44, 59, 72, 77, 143, 175}, /*navaja*/  /*12*/
	{5, 12, 38, 40, 42, 43, 44, 59, 72, 77, 98, 143, 175, 409, 413, 414, 416, 411}, /*shadowdragger*/  /*18*/
	{5, 12, 38, 40, 42, 43, 44, 59, 72, 77, 143, 175}, /*stiletto*/  /*12*/
	{5, 12, 38, 40, 42, 43, 44, 59, 72, 77, 143, 175}, /*talon*/  /*12*/
	{5, 12, 38, 40, 42, 43, 44, 59, 72, 77, 143, 175},/*ursus*/  /*12*/
	{5, 12, 38, 40, 42, 43, 44, 59, 72, 77, 143, 175},
	{5, 12, 38, 42, 43, 44, 59, 72, 77, 143, 175, 735},
	{5, 12, 38, 42, 43, 44, 59, 72, 77, 143, 175, 735},
	{5, 12, 38, 42, 43, 44, 59, 72, 77, 143, 175, 735},
	{5, 12, 38, 42, 43, 44, 59, 72, 77, 143, 175, 735},/*classic*/


	{10018, 10037, 10038, 10019,10048, 10047, 10046, 10045},
	{10024, 10026, 10027, 10028,10052, 10051, 10050, 10049},
	{10033, 10034, 10035, 10030, 10064, 10063, 10062, 10061 },
	{10013, 10015, 10016, 10040, 10044, 10043,10042, 10041 },
	{10009, 10010, 10036, 10021, 10056,	10055,10054, 10053},
	{10006, 10007, 10008, 10039},
	{10057, 10058, 10059, 10060},
};

__forceinline void padding(float x, float y)
{
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + x * c_menu::get().dpi_scale);
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + y * c_menu::get().dpi_scale);
}


Vector2D g_mouse;

static Vector RotatePoint(Vector EntityPos, Vector LocalPlayerPos, int posX, int posY, int sizeX, int sizeY, float angle, float zoom, bool* viewCheck)
{
	float r_1, r_2;
	float x_1, y_1;

	r_1 = -(EntityPos.y - LocalPlayerPos.y);
	r_2 = EntityPos.x - LocalPlayerPos.x;
	float Yaw = angle - 90.0f;

	float yawToRadian = Yaw * (float)(M_PI / 180.0F);
	x_1 = (float)(r_2 * (float)cos((double)(yawToRadian)) - r_1 * sin((double)(yawToRadian))) / 20;
	y_1 = (float)(r_2 * (float)sin((double)(yawToRadian)) + r_1 * cos((double)(yawToRadian))) / 20;

	*viewCheck = y_1 < 0;

	x_1 *= zoom;
	y_1 *= zoom;

	int sizX = sizeX / 2;
	int sizY = sizeY / 2;

	x_1 += sizX;
	y_1 += sizY;

	if (x_1 < 5)
		x_1 = 5;

	if (x_1 > sizeX - 5)
		x_1 = sizeX - 5;

	if (y_1 < 5)
		y_1 = 5;

	if (y_1 > sizeY - 5)
		y_1 = sizeY - 5;


	x_1 += posX;
	y_1 += posY;


	return Vector(x_1, y_1, 0);
}


void c_menu::DrawRadar()
{

	if (config_system.g_cfg.misc.ingame_radar)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 4.f);
		ImGui::SetNextWindowSize(ImVec2(200, 200));

		if (ImGui::Begin(("Radar"), &config_system.g_cfg.misc.ingame_radar, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize))
		{
			padding(8, 8);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (ImGui::GetWindowWidth() / 2) - 105);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 8);
			ImGui::Image(hooks::LogoImg, ImVec2(25, 25));
			ImGui::SameLine(90);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 8);
			ImGui::Text("Radar");

			ImDrawList* draw_list = ImGui::GetWindowDrawList();

			ImGui::PushStyleColor(ImGuiCol_ChildBg, config_system.g_cfg.menu.menu_theme == 1 ? ImVec4(34 / 255.f, 35 / 255.f, 39 / 255.f, 0.55f) : ImVec4(45 / 255.f, 49 / 255.f, 60 / 255.f, 0.55f));

			ImGui::BeginChild("eRadar", ImVec2(-1, -1));
			{
				ImVec2 winpos = ImGui::GetWindowPos();
				ImVec2 winsize = ImGui::GetWindowSize();

				draw_list->AddLine(ImVec2(winpos.x + winsize.x * 0.5f, winpos.y), ImVec2(winpos.x + winsize.x * 0.5f, winpos.y + winsize.y), ImColor(70, 70, 70, 255), 1.f);
				draw_list->AddLine(ImVec2(winpos.x, winpos.y + winsize.y * 0.5f), ImVec2(winpos.x + winsize.x, winpos.y + winsize.y * 0.5f), ImColor(70, 70, 70, 255), 1.f);
				if (m_engine()->IsInGame() && m_engine()->IsConnected())
				{

					Vector LocalPos = g_ctx.local()->get_shoot_position();
					Vector ang;
					m_engine()->GetViewAngles(ang);

					for (int i = 0; i < m_engine()->GetMaxClients(); i++) {
						player_t* pBaseEntity = (player_t*)m_entitylist()->GetClientEntity(i);

						bool enemy_only;

						if (!pBaseEntity)
							continue;

						if (!pBaseEntity->m_iHealth() > 0)
							continue;

						if (!config_system.g_cfg.radar.render_team)
							if (g_ctx.local()->m_iTeamNum() == pBaseEntity->m_iTeamNum())
								continue;

						if (!config_system.g_cfg.radar.render_enemy)
							if (g_ctx.local()->m_iTeamNum() != pBaseEntity->m_iTeamNum())
								continue;

						bool viewCheck = false;
						Vector EntityPos = RotatePoint(pBaseEntity->GetRenderOrigin(), LocalPos, winpos.x, winpos.y, winsize.x, winsize.y, ang.y, 5.f, &viewCheck);


						ImU32 clr1 = ImGui::GetColorU32(ImVec4(255, 0, 0, 255));
						ImU32 clr2 = ImGui::GetColorU32(ImVec4(0, 255, 0, 255));

						int s = 3;

						if (config_system.g_cfg.radar.render_enemy || config_system.g_cfg.radar.render_team)
							draw_list->AddCircleFilled(ImVec2(EntityPos.x, EntityPos.y), config_system.g_cfg.radar.size, g_ctx.local()->m_iTeamNum() == pBaseEntity->m_iTeamNum() ? clr2 : clr1);

						draw_list->AddCircleFilled(ImVec2(winpos.x + 99, winpos.y + 81), config_system.g_cfg.radar.size, ImColor(130, 130, 130, 255));
					}

				}
			}
			ImGui::EndChild();
			ImGui::PopStyleColor();
		}
		ImGui::End();
		ImGui::PopStyleVar();
	}

}

void c_menu::draw_keybinds()
{

	if (config_system.g_cfg.misc.keybinds)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 4.f);
		ImVec2 MenuP;
		ImGui::SetNextWindowSize(ImVec2(200, 15));
		ImGui::Begin("Key2", &config_system.g_cfg.misc.keybinds, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollbar);
		MenuP = ImGui::GetCurrentWindow()->Pos;

		padding(8, 8);
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (ImGui::GetWindowWidth() / 2) - 105);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 8);
		ImGui::SameLine(90);

		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 8);
		ImGui::Text("Binds");
		auto pos = ImGui::GetWindowPos();
		ImDrawList* draw_list = ImGui::GetWindowDrawList();

		ImGui::End();

		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
		ImGui::SetNextWindowPos({ MenuP.x, MenuP.y + 32 });
		if (ImGui::Begin("key", &config_system.g_cfg.misc.keybinds, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollbar))
		{
			std::string keys;
			std::vector<std::tuple<std::string, bool, int>> KeysToText;

			if (config_system.g_cfg.misc.thirdperson_toggle.key > KEY_NONE && config_system.g_cfg.misc.thirdperson_toggle.key < KEY_MAX)
			{
				keys += "Third Person";
				KeysToText.push_back(std::tuple<std::string, bool, int>("Third Person", config_system.g_cfg.misc.thirdperson_toggle.holdortoggle, config_system.g_cfg.misc.thirdperson_toggle.type));
				keys += "\n";
			}


			if (config_system.g_cfg.antiaim.flip_desync.key > KEY_NONE && config_system.g_cfg.antiaim.flip_desync.key < KEY_MAX)
			{
				keys += "Inverter";
				KeysToText.push_back(std::tuple<std::string, bool, int>("Inverter", config_system.g_cfg.antiaim.flip_desync.holdortoggle, config_system.g_cfg.antiaim.flip_desync.type));
				keys += "\n";

			}
			if (config_system.g_cfg.ragebot.body_aim_key.key > KEY_NONE && config_system.g_cfg.ragebot.body_aim_key.key < KEY_MAX)
			{
				keys += "Force body";
				KeysToText.push_back(std::tuple<std::string, bool, int>("Force Body", config_system.g_cfg.ragebot.body_aim_key.holdortoggle, config_system.g_cfg.ragebot.body_aim_key.type));
				keys += "\n";

			}
			if (config_system.g_cfg.ragebot.safe_point_key.key > KEY_NONE && config_system.g_cfg.ragebot.safe_point_key.key < KEY_MAX)
			{
				keys += "Safe point";
				KeysToText.push_back(std::tuple<std::string, bool, int>("Safe point", config_system.g_cfg.ragebot.safe_point_key.holdortoggle, config_system.g_cfg.ragebot.safe_point_key.type));
				keys += "\n";

			}
			if (config_system.g_cfg.ragebot.resolver_override.key > KEY_NONE && config_system.g_cfg.ragebot.resolver_override.key < KEY_MAX)
			{
				keys += "Override resolver";
				KeysToText.push_back(std::tuple<std::string, bool, int>("Override Resolver", config_system.g_cfg.ragebot.resolver_override.holdortoggle, config_system.g_cfg.ragebot.resolver_override.type));
				keys += "\n";

			}

			if (config_system.g_cfg.misc.automatic_peek.key > KEY_NONE && config_system.g_cfg.misc.automatic_peek.key < KEY_MAX)
			{
				keys += "Autopeek";
				KeysToText.push_back(std::tuple<std::string, bool, int>("Auto peek", config_system.g_cfg.misc.automatic_peek.holdortoggle, config_system.g_cfg.misc.automatic_peek.type));
				keys += "\n";

			}

			if (config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].damage_override_key.key > KEY_NONE && config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].damage_override_key.key < KEY_MAX)
			{
				keys += "Damage override";

				KeysToText.push_back(std::tuple<std::string, bool, int>("Override dmg", config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].damage_override_key.holdortoggle, config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].damage_override_key.type));
				keys += "\n";

			}

			if (config_system.g_cfg.misc.slowwalk_key.key > KEY_NONE && config_system.g_cfg.misc.slowwalk_key.key < KEY_MAX)
			{
				keys += "Slow walk";
				KeysToText.push_back(std::tuple<std::string, bool, int>("Slow walk", config_system.g_cfg.misc.slowwalk_key.holdortoggle, config_system.g_cfg.misc.slowwalk_key.type));
				keys += "\n";
			}
			if (config_system.g_cfg.misc.fakeduck_key.key > KEY_NONE && config_system.g_cfg.misc.fakeduck_key.key < KEY_MAX)
			{
				keys += "Fake duck";
				KeysToText.push_back(std::tuple<std::string, bool, int>("Fake duck", config_system.g_cfg.misc.fakeduck_key.holdortoggle, config_system.g_cfg.misc.fakeduck_key.type));
				keys += "\n";
			}


			if (config_system.g_cfg.ragebot.double_tap_key.key > KEY_NONE && config_system.g_cfg.ragebot.double_tap_key.key < KEY_MAX)
			{
				keys += "Double tap";
				KeysToText.push_back(std::tuple<std::string, bool, int>("Double tap", misc::get().double_tap_enabled, 1));
				keys += "\n";
			}
			if (config_system.g_cfg.antiaim.hide_shots_key.key > KEY_NONE && config_system.g_cfg.antiaim.hide_shots_key.key < KEY_MAX)
			{
				keys += "Hide shot";
				KeysToText.push_back(std::tuple<std::string, bool, int>("Hide shot", misc::get().hide_shots_key, 1));
				keys += "\n";
			}
			auto size = ImGui::CalcTextSize(keys.c_str()); // 16 на размер шрифта меняете и Menu::Get().globalFont на ваш шрифт

			ImGui::SetWindowSize(ImVec2(200, size.y + 35));


			for (int i = 0; i < KeysToText.size(); i++) {
				if (!std::get<1>(KeysToText[i]))
					continue;

				ImGui::Columns(2, NULL, false);
				ImGui::Text(std::get<0>(KeysToText[i]).c_str());
				ImGui::NextColumn();
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 50.f);
				if (std::get<2>(KeysToText[i]) == 1)
				{
					ImGui::TextColored(ImVec4(0.4f, 1.f, 0.4f, 1.f), "Toggled");
				}
				else if (std::get<2>(KeysToText[i]) == 0)
				{
					ImGui::TextColored(ImVec4(0.4f, 1.f, 0.4f, 1.f), "Holded");
				}


				ImGui::Columns(1);
			}


		}
		ImGui::End();
		ImGui::PopStyleColor();
		ImGui::PopStyleVar();
	}

}


void c_menu::draw_indicators()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 4.f);
	static float fakelagfloat = 0.f;
	static float doubletaprecharge = 0.f;
	static float desyncamount = 0.f;
	static float Height = 0.f;
	static float velocity = 0.f;
	static float stamina = 0.f;

	if (g_ctx.local() && g_ctx.local()->is_alive() && g_ctx.local()->get_animation_state())
	{
		velocity = g_ctx.local()->m_vecVelocity().Length2D();

		desyncamount = !config_system.g_cfg.antiaim.enable ? 0.f : g_ctx.local()->get_max_desync_delta();

		Height = 1 - g_ctx.local()->m_flDuckAmount();

		stamina = g_ctx.local()->stamina();
	}

	if (config_system.g_cfg.misc.indicators) {
		if (ImGui::Begin("Indicators", &config_system.g_cfg.misc.indicators, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollbar))
		{
			padding(8, 8);
			ImGui::SetWindowSize(ImVec2(200, 105));
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (ImGui::GetWindowWidth() / 2) - 105);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 8);
			//	ImGui::Image(hooks::LogoImg, ImVec2(25, 25));
			ImGui::SameLine(80);
			ImGui::PushFont(c_menu::get().futura_small);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 8);
			ImGui::Text("Indicators");
			ImGui::Columns(2, NULL, false);

			ImGui::SetColumnWidth(0, 80);

			ImGui::Text("Fakelag");
			ImGui::Text("Desync");
			ImGui::Text("Velocity");
			ImGui::Text("Stamina");
			ImGui::NextColumn();
			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
			ImGui::PushItemWidth(120); padding(0, 8);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 20); padding(0, 8);
			ImGui::SliderInt("###fakelag", &fakelag::get().max_choke, 0, 16, "%1.f"); padding(0, 8);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 23); padding(0, 8);
			ImGui::SliderFloat("###desync", &desyncamount, 0, 120, "%1.f", 1.f); padding(0, 8);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 23); padding(0, 8);
			ImGui::SliderFloat("###velocity", &velocity, 0, 400, "%1.f", 1.f); padding(0, 8);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 23); padding(0, 8);
			ImGui::SliderFloat("###stamina", &stamina, 0, 40, "%1.f", 1.f); padding(0, 8);
			ImGui::PopItemWidth();
			ImGui::PopItemFlag();
			ImGui::Columns(1);
			ImGui::PopFont();
		}
		ImGui::End();
	}
	ImGui::PopStyleVar();
}

namespace ImGui
{
	struct circle_button {
		float alpha;
		float radius;
		bool active;
		ImVec2 pos;
		bool pos_set;
	};

	bool CircleButton(const char* label, const char* label_id, const ImVec2& size_arg, bool use_icon = false, IDirect3DTexture9* texture = nullptr, const char* icon_label = nullptr)
	{
		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label_id);
		const ImVec2 label_size = CalcTextSize(label, NULL, true);



		auto flags = 0;

		ImVec2 pos = window->DC.CursorPos;
		if ((flags & ImGuiButtonFlags_AlignTextBaseLine) && style.FramePadding.y < window->DC.CurrLineTextBaseOffset) // Try to vertically align buttons that are smaller/have no padding so that text baseline matches (bit hacky, since it shouldn't be a flag)
			pos.y += window->DC.CurrLineTextBaseOffset - style.FramePadding.y;
		ImVec2 size = CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

		const ImRect bb(pos, pos + size);
		ItemSize(size, style.FramePadding.y);
		if (!ItemAdd(bb, id))
			return false;

		if (window->DC.ItemFlags & ImGuiItemFlags_ButtonRepeat)
			flags |= ImGuiButtonFlags_Repeat;
		bool hovered, held;
		bool pressed = ButtonBehavior(bb, id, &hovered, &held, flags);

		static std::map<ImGuiID, float> hover_animation;
		auto it_hover = hover_animation.find(id);
		if (it_hover == hover_animation.end())
		{
			hover_animation.insert({ id, 0.f });
			it_hover = hover_animation.find(id);
		}
		it_hover->second = math::clamp(it_hover->second + (3.f * ImGui::GetIO().DeltaTime * (hovered ? 1.f : -1.f)), 0.0f, 1.f);

		static std::map<ImGuiID, circle_button> circle_animation;
		auto it_circle = circle_animation.find(id);
		if (it_circle == circle_animation.end())
		{
			circle_animation.insert({ id, {0.8f, 0.f, false, ImVec2(0, 0), false} });
			it_circle = circle_animation.find(id);
		}

		// Render
		const ImU32 col = GetColorU32((held && hovered) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);

		const ImVec4 text_act = ImVec4(255 / 255.f, 255 / 255.f, 255 / 255.f, 1.f);
		const ImVec4 icon_act = ImVec4(56 / 255.f, 51 / 255.f, 219 / 255.f, 1.f);

		RenderFrame(bb.Min, bb.Max, col, true, 25.f * c_menu::get().dpi_scale);

		/*  window->DrawList->PushClipRect(bb.Min, bb.Max, false);
		  ImGui::GetWindowDrawList()->AddCircleFilled(it_circle->second.pos, it_circle->second.radius, GetColorU32(ImVec4(56 / 255.f, 51 / 255.f, 219 / 255.f, it_circle->second.alpha)), 60);
		  window->DrawList->PopClipRect();*/

		ImGui::GetWindowDrawList()->AddRect(bb.Min, bb.Max, GetColorU32(ImVec4(14 / 255.f, 246 / 255.f, 250 / 255.f, it_hover->second)), 25.f * c_menu::get().dpi_scale);

		const float check_sz = ImMin(bb.GetWidth(), bb.GetHeight());
		const float pad = ImMax(1.0f, (float)(int)(check_sz / 6.0f));


		if (!use_icon)
		{
			PushStyleColor(ImGuiCol_Text, text_act);
			RenderTextClipped(bb.Min, bb.Max, label, NULL, &label_size, ImVec2(0.5f, 0.5f), &bb);
			PopStyleColor();
		}
		else
		{
			if (texture != nullptr)
				ImGui::GetWindowDrawList()->AddImage(texture, bb.Min + ImVec2(pad, pad), bb.Max - ImVec2(pad, pad));

		}

		// Automatically close popups
		if (pressed && !(flags & ImGuiButtonFlags_DontClosePopups) && (window->Flags & ImGuiWindowFlags_Popup))
			//    CloseCurrentPopup();

			IMGUI_TEST_ENGINE_ITEM_INFO(id, label, window->DC.LastItemStatusFlags);
		return pressed;

	}

	bool ColoredButton(const char* label, const ImVec2& size_arg, const bool selected, ImColor color)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return false;

		static float sizeplus = 0.f;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);
		const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);

		ImVec2 pos = window->DC.CursorPos;

		ImVec2 size = ImGui::CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

		const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
		ImGui::ItemSize(size, style.FramePadding.y);
		if (!ImGui::ItemAdd(bb, id))
			return false;

		bool hovered, held;
		bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, 0);

		float t = selected ? 1.0f : 0.0f;


		float ANIM_SPEED = 0.30f;
		if (g.LastActiveId == g.CurrentWindow->GetID(label))
		{
			float t_anim = ImSaturate(g.LastActiveIdTimer / ANIM_SPEED);
			t = selected ? (t_anim) : (1.0f - t_anim);
		}

		ImU32 col_bg;
		col_bg = ImGui::GetColorU32(ImLerp(ImVec4(0.f / 255.f, 0.f / 255.f, 0.f / 255.f, 0.f / 255.f), ImVec4(14 / 255.f, 246 / 255.f, 250 / 255.f, 1.00f), t));


		ImGui::PushFont(c_menu::get().futura_large);
		if (selected)
			window->DrawList->AddRect(ImVec2(bb.Min.x - 1, bb.Min.y - 1), { bb.Max.x + 1,bb.Max.y + 1 }, col_bg, 360.f);

		window->DrawList->AddRectFilled(ImVec2(bb.Min.x, bb.Min.y), { bb.Max.x,bb.Max.y }, color, 360.f);

		ImGui::PopFont();

		return pressed;
	}

	bool ColorEdit4(const char* label, Color* v, bool show_alpha = true, bool is_solo = false)
	{
		auto clr = ImVec4{
			v->r() / 255.f,
			v->g() / 255.f,
			v->b() / 255.f,
			v->a() / 255.f

		};

		if (ImGui::ColorEdit4(label, &clr.x, show_alpha, is_solo)) {
			v->SetColor(clr.x, clr.y, clr.z, clr.w);
			return true;
		}
		return false;
	}
	bool ColorEdit3(const char* label, Color* v)
	{
		return ColorEdit4(label, v, true);
	}

	bool Tab(const char* label, const ImVec2& size_arg, const bool selected)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return false;

		static float sizeplus = 0.f;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);
		const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);

		ImVec2 pos = window->DC.CursorPos;

		ImVec2 size = ImGui::CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

		const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
		ImGui::ItemSize(size, style.FramePadding.y);
		if (!ImGui::ItemAdd(bb, id))
			return false;

		bool hovered, held;
		bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, 0);

		float t = selected ? 1.0f : 0.0f;


		float ANIM_SPEED = 0.30f;
		if (g.LastActiveId == g.CurrentWindow->GetID(label))
		{
			float t_anim = ImSaturate(g.LastActiveIdTimer / ANIM_SPEED);
			t = selected ? (t_anim) : (1.0f - t_anim);
		}

		ImU32 col_bg;
		col_bg = ImGui::GetColorU32(ImLerp(ImVec4(145 / 255.f, 144 / 255.f, 148 / 255.f, 255.f / 255.f), ImVec4(255 / 255.f, 255 / 255.f, 255 / 255.f, 255.f / 255.f), t));


		ImGui::PushFont(c_menu::get().futura_large);
		if (selected)
			window->DrawList->AddText(ImVec2(bb.Min.x, bb.Min.y + 11), col_bg, label);
		if (!selected)
			window->DrawList->AddText(ImVec2(bb.Min.x, bb.Min.y + 11), ImColor(145 / 255.f, 144 / 255.f, 148 / 255.f, 255.f / 255.f), label);

		ImGui::PopFont();

		return pressed;
	}


	bool Tab2(const char* label, const char* desc, const ImVec2& size_arg, const bool selected)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return false;

		static float sizeplus = 0.f;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);
		const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);

		ImVec2 pos = window->DC.CursorPos;

		ImVec2 size = ImGui::CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

		const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
		ImGui::ItemSize(size, style.FramePadding.y);
		if (!ImGui::ItemAdd(bb, id))
			return false;

		bool hovered, held;
		bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, 0);

		float t = selected ? 1.0f : 0.0f;


		float ANIM_SPEED = 0.30f;
		if (g.LastActiveId == g.CurrentWindow->GetID(label))
		{
			float t_anim = ImSaturate(g.LastActiveIdTimer / ANIM_SPEED);
			t = selected ? (t_anim) : (1.0f - t_anim);
		}

		ImU32 col_bg;
		if (!hovered)
			col_bg = ImGui::GetColorU32(ImLerp(ImVec4(145 / 255.f, 144 / 255.f, 148 / 255.f, 255.f / 255.f), ImVec4(255 / 255.f, 255 / 255.f, 255 / 255.f, 255.f / 255.f), t));
		if (hovered)
			col_bg = ImGui::GetColorU32(ImLerp(ImVec4(145 / 255.f, 144 / 255.f, 148 / 255.f, 255.f / 255.f), ImVec4(225 / 255.f, 225 / 255.f, 225 / 255.f, 255.f / 255.f), t));

		ImGui::PushFont(c_menu::get().futura_large);
		if (selected || hovered)
			window->DrawList->AddText(ImVec2(bb.Min.x + 45, bb.Min.y + 11), col_bg, label);
		if (!selected)
			window->DrawList->AddText(ImVec2(bb.Min.x + 45, bb.Min.y + 11), ImColor(145 / 255.f, 144 / 255.f, 148 / 255.f, 255.f / 255.f), label);

		ImGui::PopFont();

		ImGui::SameLine();
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 115.f);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 7.f);
		ImGui::PushFont(c_menu::get().futura);
		if (selected || hovered)
			window->DrawList->AddText(ImVec2(bb.Min.x, bb.Min.y + size_arg.y / 2 - ImGui::CalcTextSize(desc).y / 2), col_bg, desc);
		if (!selected)
			window->DrawList->AddText(ImVec2(bb.Min.x, bb.Min.y + size_arg.y / 2 - ImGui::CalcTextSize(desc).y / 2), ImColor(145 / 255.f, 144 / 255.f, 148 / 255.f, 255.f / 255.f), desc);
		ImGui::PopFont();

		return pressed;
	}

	bool SubTab(const char* label, const ImVec2& size_arg, const bool selected)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return false;

		static float sizeplus = 0.f;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);
		const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);

		ImVec2 pos = window->DC.CursorPos;

		ImVec2 size = ImGui::CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

		const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
		ImGui::ItemSize(size, style.FramePadding.y);
		if (!ImGui::ItemAdd(bb, id))
			return false;

		bool hovered, held;
		bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, 0);

		if (selected)
			window->DrawList->AddRectFilled({ bb.Min.x,bb.Min.y }, { bb.Max.x,bb.Max.y }, ImColor(37 / 255.f, 43 / 255.f, 69 / 255.f, 1.00f));

		if (!selected)
			window->DrawList->AddRectFilled({ bb.Min.x,bb.Max.y }, { bb.Min.x - 5,bb.Min.y }, ImColor(40 / 255.f, 40 / 255.f, 40 / 255.f, 255.f / 255.f), 6.f);

		if (selected)
			window->DrawList->AddRectFilled({ bb.Min.x,bb.Max.y }, { bb.Min.x - 4,bb.Min.y }, ImColor(53 / 255.f, 78 / 255.f, 181 / 255.f, 255.f / 255.f), 6.f);


		window->DrawList->AddText(ImVec2(bb.Min.x + 7, bb.Min.y + size_arg.y / 2 - ImGui::CalcTextSize(label).y / 2), ImColor(255 / 255.f, 255 / 255.f, 255 / 255.f, 255.f / 255.f), label);
		//ImGui::PopFont();


		return pressed;
	}
}

void c_menu::AboutDyn(bool menu_open)
{
	if (menu_open)
	{
		if (getConfigs)
		{
			ImGui::PushStyleColor(ImGuiCol_WindowBg, config_system.g_cfg.menu.menu_theme == 1 ? ImVec4(21 / 255.f, 22 / 255.f, 26 / 255.f, 0.8f) : ImVec4(27 / 255.f, 31 / 255.f, 40 / 255.f, 0.8f));

			ImGui::SetNextWindowSize(ImVec2(300, 305));
			if (ImGui::Begin("Configs", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollbar))
			{
				ImVec2 pos = ImGui::GetWindowPos();
				ImGui::GetWindowDrawList()->AddImage(hooks::LogoTwo, ImVec2(pos.x + 25, pos.y - 40), ImVec2(pos.x + 275, pos.y + 180));
				ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(pos.x, pos.y + 139), ImVec2(pos.x + 300, pos.y + 140), ImColor(120, 120, 120, 40));

				ImGui::SetCursorPos({ 6, 145 });

				constexpr auto& config_items = config_system.get_configs();
				static int current_config = -1;

				if (static_cast<size_t>(current_config) >= config_items.size())
					current_config = -1;

				static char buffer[16];

				if (ImGui::ListBox("", &current_config, [](void* data, int idx, const char** out_text) {
					auto& vector = *static_cast<std::vector<std::string>*>(data);
					*out_text = vector[idx].c_str();
					return true;
				}, &config_items, config_items.size(), 6) && current_config != -1)
					strcpy(buffer, config_items[current_config].c_str());

				ImGui::SetCursorPos({ 210, 195 });
				if (ImGui::CustomButton("Reload", "##RELOAD", ImVec2(80, 22), true, c_menu::get().arrow, "C"))
				{
					config_system.get_configs() = { "" };
					config_system.run(buffer);
				}
				ImGui::SetCursorPos({ 210, 145 });
				if (ImGui::CustomButton("Load", "##LOAD", ImVec2(80, 22), true, c_menu::get().load_font, "l"))
					config_system.load(current_config);

				ImGui::SetCursorPos({ 210, 170 });
				if (ImGui::CustomButton("Delete", "##DELETE", ImVec2(80, 22), true, c_menu::get().misc_font, "F"))
					config_system.remove(current_config);

				ImGui::SetCursorPos({ 210, 220 });
				if (ImGui::CustomButton("Save", "##Save", ImVec2(80, 22), true, c_menu::get().load_font, "m"))
					config_system.save(current_config);

				ImGui::SetCursorPos({ 210, 253 });
				if (ImGui::CustomButton("Create", "##Create", ImVec2(80, 20), true, c_menu::get().misc_font, "E"))
					config_system.add(buffer);

				ImGui::SetCursorPos({ 6, 255 });
				ImGui::InputText("Put config name here", buffer, ARRAYSIZE(buffer));

				ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(pos.x, pos.y + 274), ImVec2(pos.x + 300, pos.y + 275), ImColor(120, 120, 120, 40));
				ImGui::SetCursorPos({ 6, 280 });
				ImGui::Text("Menu style");
				ImGui::SameLine(240);

				if (ImGui::ColoredButton("##STYLE1", ImVec2(15, 15), config_system.g_cfg.menu.menu_theme == 0 ? true : false, ImColor(67, 71, 102, 255)))
					config_system.g_cfg.menu.menu_theme = 0;

				ImGui::SameLine(0, 10);

				if (ImGui::ColoredButton("##STYLE2", ImVec2(15, 15), config_system.g_cfg.menu.menu_theme == 1 ? true : false, ImColor(11, 10, 11, 255)))
					config_system.g_cfg.menu.menu_theme = 1;

			}
			ImGui::End();
			ImGui::PopStyleColor();
		}
	}
}

std::string get_wep(int id, int custom_index = -1, bool knife = true)
{
	if (custom_index > -1)
	{
		if (knife)
		{
			switch (custom_index)
			{
			case 0: return crypt_str("weapon_knife");
			case 1: return crypt_str("weapon_bayonet");
			case 2: return crypt_str("weapon_knife_css");
			case 3: return crypt_str("weapon_knife_skeleton");
			case 4: return crypt_str("weapon_knife_outdoor");
			case 5: return crypt_str("weapon_knife_cord");
			case 6: return crypt_str("weapon_knife_canis");
			case 7: return crypt_str("weapon_knife_flip");
			case 8: return crypt_str("weapon_knife_gut");
			case 9: return crypt_str("weapon_knife_karambit");
			case 10: return crypt_str("weapon_knife_m9_bayonet");
			case 11: return crypt_str("weapon_knife_tactical");
			case 12: return crypt_str("weapon_knife_falchion");
			case 13: return crypt_str("weapon_knife_survival_bowie");
			case 14: return crypt_str("weapon_knife_butterfly");
			case 15: return crypt_str("weapon_knife_push");
			case 16: return crypt_str("weapon_knife_ursus");
			case 17: return crypt_str("weapon_knife_gypsy_jackknife");
			case 18: return crypt_str("weapon_knife_stiletto");
			case 19: return crypt_str("weapon_knife_widowmaker");
			}
		}
		else
		{
			switch (custom_index)
			{
			case 0: return crypt_str("ct_gloves"); //-V1037
			case 1: return crypt_str("studded_bloodhound_gloves");
			case 2: return crypt_str("t_gloves");
			case 3: return crypt_str("ct_gloves");
			case 4: return crypt_str("sporty_gloves");
			case 5: return crypt_str("slick_gloves");
			case 6: return crypt_str("leather_handwraps");
			case 7: return crypt_str("motorcycle_gloves");
			case 8: return crypt_str("specialist_gloves");
			case 9: return crypt_str("studded_hydra_gloves");
			}
		}
	}
	else
	{
		switch (id)
		{
		case 0: return crypt_str("knife");
		case 1: return crypt_str("gloves");
		case 2: return crypt_str("weapon_ak47");
		case 3: return crypt_str("weapon_aug");
		case 4: return crypt_str("weapon_awp");
		case 5: return crypt_str("weapon_cz75a");
		case 6: return crypt_str("weapon_deagle");
		case 7: return crypt_str("weapon_elite");
		case 8: return crypt_str("weapon_famas");
		case 9: return crypt_str("weapon_fiveseven");
		case 10: return crypt_str("weapon_g3sg1");
		case 11: return crypt_str("weapon_galilar");
		case 12: return crypt_str("weapon_glock");
		case 13: return crypt_str("weapon_m249");
		case 14: return crypt_str("weapon_m4a1_silencer");
		case 15: return crypt_str("weapon_m4a1");
		case 16: return crypt_str("weapon_mac10");
		case 17: return crypt_str("weapon_mag7");
		case 18: return crypt_str("weapon_mp5sd");
		case 19: return crypt_str("weapon_mp7");
		case 20: return crypt_str("weapon_mp9");
		case 21: return crypt_str("weapon_negev");
		case 22: return crypt_str("weapon_nova");
		case 23: return crypt_str("weapon_hkp2000");
		case 24: return crypt_str("weapon_p250");
		case 25: return crypt_str("weapon_p90");
		case 26: return crypt_str("weapon_bizon");
		case 27: return crypt_str("weapon_revolver");
		case 28: return crypt_str("weapon_sawedoff");
		case 29: return crypt_str("weapon_scar20");
		case 30: return crypt_str("weapon_ssg08");
		case 31: return crypt_str("weapon_sg556");
		case 32: return crypt_str("weapon_tec9");
		case 33: return crypt_str("weapon_ump45");
		case 34: return crypt_str("weapon_usp_silencer");
		case 35: return crypt_str("weapon_xm1014");
		default: return crypt_str("unknown");
		}
	}
}

IDirect3DTexture9* get_skin_preview(const char* weapon_name, const std::string& skin_name, IDirect3DDevice9* device)
{
	IDirect3DTexture9* skin_image = nullptr;
	std::string vpk_path;

	if (strcmp(weapon_name, crypt_str("unknown")) && strcmp(weapon_name, crypt_str("knife")) && strcmp(weapon_name, crypt_str("gloves"))) //-V526
	{
		if (skin_name.empty() || skin_name == crypt_str("default"))
			vpk_path = crypt_str("resource/flash/econ/weapons/base_weapons/") + std::string(weapon_name) + crypt_str(".png");
		else
			vpk_path = crypt_str("resource/flash/econ/default_generated/") + std::string(weapon_name) + crypt_str("_") + std::string(skin_name) + crypt_str("_light_large.png");
	}
	else
	{
		if (!strcmp(weapon_name, crypt_str("knife")))
			vpk_path = crypt_str("resource/flash/econ/weapons/base_weapons/weapon_knife.png");
		else if (!strcmp(weapon_name, crypt_str("gloves")))
			vpk_path = crypt_str("resource/flash/econ/weapons/base_weapons/ct_gloves.png");
		else if (!strcmp(weapon_name, crypt_str("unknown")))
			vpk_path = crypt_str("resource/flash/econ/weapons/base_weapons/weapon_snowball.png");

	}
	const auto handle = m_basefilesys()->Open(vpk_path.c_str(), crypt_str("r"), crypt_str("GAME"));
	if (handle)
	{
		int file_len = m_basefilesys()->Size(handle);
		char* image = new char[file_len]; //-V121

		m_basefilesys()->Read(image, file_len, handle);
		m_basefilesys()->Close(handle);

		D3DXCreateTextureFromFileInMemory(device, image, file_len, &skin_image);
		delete[] image;
	}

	if (!skin_image)
	{
		std::string vpk_path;

		if (strstr(weapon_name, crypt_str("bloodhound")) != NULL || strstr(weapon_name, crypt_str("hydra")) != NULL)
			vpk_path = crypt_str("resource/flash/econ/weapons/base_weapons/ct_gloves.png");
		else
			vpk_path = crypt_str("resource/flash/econ/weapons/base_weapons/") + std::string(weapon_name) + crypt_str(".png");

		const auto handle = m_basefilesys()->Open(vpk_path.c_str(), crypt_str("r"), crypt_str("GAME"));

		if (handle)
		{
			int file_len = m_basefilesys()->Size(handle);
			char* image = new char[file_len]; //-V121

			m_basefilesys()->Read(image, file_len, handle);
			m_basefilesys()->Close(handle);

			D3DXCreateTextureFromFileInMemory(device, image, file_len, &skin_image);
			delete[] image;
		}
	}

	return skin_image;
}

IDirect3DTexture9* get_sticker(const char* sticker_name, IDirect3DDevice9* device)
{
	IDirect3DTexture9* skin_image = nullptr;

	const auto handle = m_basefilesys()->Open(sticker_name, crypt_str("r"), crypt_str("GAME"));
	if (handle)
	{
		int file_len = m_basefilesys()->Size(handle);
		char* image = new char[file_len]; //-V121

		m_basefilesys()->Read(image, file_len, handle);
		m_basefilesys()->Close(handle);

		D3DXCreateTextureFromFileInMemory(device, image, file_len, &skin_image);
		delete[] image;
	}

	if (!skin_image)
	{
		const auto handle = m_basefilesys()->Open(sticker_name, crypt_str("r"), crypt_str("GAME"));

		if (handle)
		{
			int file_len = m_basefilesys()->Size(handle);
			char* image = new char[file_len]; //-V121

			m_basefilesys()->Read(image, file_len, handle);
			m_basefilesys()->Close(handle);

			D3DXCreateTextureFromFileInMemory(device, image, file_len, &skin_image);
			delete[] image;
		}
	}

	return skin_image;
}


// setup some styles and colors, window size and bg alpha
// dpi setup
void c_menu::menu_setup(ImGuiStyle& style) //-V688
{
	if (config_system.g_cfg.menu.menu_theme == 0)
		ImGui::StyleColorsClassic();
	else if (config_system.g_cfg.menu.menu_theme == 1)
		ImGui::StyleColorsDark();

	ImGui::SetNextWindowSize(ImVec2(width, height), ImGuiCond_Once); // window pos setup
	ImGui::SetNextWindowBgAlpha(min(style.Alpha, 0.94f)); // window bg alpha setup

	styles.WindowPadding = style.WindowPadding;
	styles.WindowRounding = style.WindowRounding;
	styles.WindowMinSize = style.WindowMinSize;
	styles.ChildRounding = style.ChildRounding;
	styles.PopupRounding = style.PopupRounding;
	styles.FramePadding = style.FramePadding;
	styles.FrameRounding = style.FrameRounding;
	styles.ItemSpacing = style.ItemSpacing;
	styles.ItemInnerSpacing = style.ItemInnerSpacing;
	styles.TouchExtraPadding = style.TouchExtraPadding;
	styles.IndentSpacing = style.IndentSpacing;
	styles.ColumnsMinSpacing = style.ColumnsMinSpacing;
	styles.ScrollbarSize = style.ScrollbarSize;
	styles.ScrollbarRounding = style.ScrollbarRounding;
	styles.GrabMinSize = style.GrabMinSize;
	styles.GrabRounding = style.GrabRounding;
	styles.TabRounding = style.TabRounding;
	styles.TabMinWidthForUnselectedCloseButton = style.TabMinWidthForUnselectedCloseButton;
	styles.DisplayWindowPadding = style.DisplayWindowPadding;
	styles.DisplaySafeAreaPadding = style.DisplaySafeAreaPadding;
	styles.MouseCursorScale = style.MouseCursorScale;


	menu_setupped = true; // we dont want to setup menu again
}

// resize current style sizes
void c_menu::dpi_resize(float scale_factor, ImGuiStyle& style) //-V688
{
	style.WindowPadding = (styles.WindowPadding * scale_factor);
	style.WindowRounding = (styles.WindowRounding * scale_factor);
	style.WindowMinSize = (styles.WindowMinSize * scale_factor);
	style.ChildRounding = (styles.ChildRounding * scale_factor);
	style.PopupRounding = (styles.PopupRounding * scale_factor);
	style.FramePadding = (styles.FramePadding * scale_factor);
	style.FrameRounding = (styles.FrameRounding * scale_factor);
	style.ItemSpacing = (styles.ItemSpacing * scale_factor);
	style.ItemInnerSpacing = (styles.ItemInnerSpacing * scale_factor);
	style.TouchExtraPadding = (styles.TouchExtraPadding * scale_factor);
	style.IndentSpacing = (styles.IndentSpacing * scale_factor);
	style.ColumnsMinSpacing = (styles.ColumnsMinSpacing * scale_factor);
	style.ScrollbarSize = (styles.ScrollbarSize * scale_factor);
	style.ScrollbarRounding = (styles.ScrollbarRounding * scale_factor);
	style.GrabMinSize = (styles.GrabMinSize * scale_factor);
	style.GrabRounding = (styles.GrabRounding * scale_factor);
	style.TabRounding = (styles.TabRounding * scale_factor);
	if (styles.TabMinWidthForUnselectedCloseButton != FLT_MAX) //-V550
		style.TabMinWidthForUnselectedCloseButton = (styles.TabMinWidthForUnselectedCloseButton * scale_factor);
	style.DisplayWindowPadding = (styles.DisplayWindowPadding * scale_factor);
	style.DisplaySafeAreaPadding = (styles.DisplaySafeAreaPadding * scale_factor);
	style.MouseCursorScale = (styles.MouseCursorScale * scale_factor);
}


std::string get_config_dir()
{
	std::string folder;
	static TCHAR path[MAX_PATH];


	CreateDirectory(folder.c_str(), NULL);
	return folder;
}


void draw_combo(const char* name, int& variable, const char* labels[], int count)
{
	ImGui::Combo(name, &variable, labels, count);
}

void draw_combo(const char* name, int& variable, bool (*items_getter)(void*, int, const char**), void* data, int count)
{

	ImGui::Combo(name, &variable, items_getter, data, count);
}

void draw_multicombo(std::string name, std::vector<int>& variable, const char* labels[], int count, std::string& preview)
{
	auto hashname = name; // we dont want to render name of combo

	for (auto i = 0, j = 0; i < count; i++)
	{
		if (variable[i])
		{
			if (j)
				preview += crypt_str(", ") + (std::string)labels[i];
			else
				preview = labels[i];

			j++;
		}
	}

	if (ImGui::BeginCombo(hashname.c_str(), preview.c_str())) // draw start
	{
		ImGui::Spacing();
		ImGui::BeginGroup();
		{

			for (auto i = 0; i < count; i++)
				ImGui::Selectable(labels[i], (bool*)&variable[i], ImGuiSelectableFlags_DontClosePopups);

		}
		ImGui::EndGroup();
		ImGui::Spacing();

		ImGui::EndCombo();
	}

	preview = crypt_str("None"); // reset preview to use later
}

bool LabelClick(const char* label, bool* v, const char* unique_id)
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems)
		return false;

	// The concatoff/on thingies were for my weapon config system so if we're going to make that, we still need this aids.
	char Buf[64];
	_snprintf(Buf, 62, crypt_str("%s"), label);

	char getid[128];
	sprintf_s(getid, 128, crypt_str("%s%s"), label, unique_id);


	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(getid);
	const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);

	const ImRect check_bb(window->DC.CursorPos, ImVec2(label_size.y + style.FramePadding.y * 2 + window->DC.CursorPos.x, window->DC.CursorPos.y + label_size.y + style.FramePadding.y * 2));
	ImGui::ItemSize(check_bb, style.FramePadding.y);

	ImRect total_bb = check_bb;

	if (label_size.x > 0)
	{
		ImGui::SameLine(0, style.ItemInnerSpacing.x);
		const ImRect text_bb(ImVec2(window->DC.CursorPos.x, window->DC.CursorPos.y + style.FramePadding.y), ImVec2(window->DC.CursorPos.x + label_size.x, window->DC.CursorPos.y + style.FramePadding.y + label_size.y));

		ImGui::ItemSize(ImVec2(text_bb.GetWidth(), check_bb.GetHeight()), style.FramePadding.y);
		total_bb = ImRect(ImMin(check_bb.Min, text_bb.Min), ImMax(check_bb.Max, text_bb.Max));
	}

	if (!ImGui::ItemAdd(total_bb, id))
		return false;

	bool hovered, held;
	bool pressed = ImGui::ButtonBehavior(total_bb, id, &hovered, &held);
	if (pressed)
		*v = !(*v);

	float t = *v ? 1.0f : 0.0f;

	float ANIM_SPEED = 0.30f;
	if (g.LastActiveId == g.CurrentWindow->GetID(label))
	{
		float t_anim = ImSaturate(g.LastActiveIdTimer / ANIM_SPEED);
		t = *v ? (t_anim) : (1.0f - t_anim);
	}

	ImU32 col_bg;
	col_bg = ImGui::GetColorU32(ImLerp(ImVec4(145 / 255.f, 144 / 255.f, 148 / 255.f, 255.f / 255.f), ImVec4(255 / 255.f, 255 / 255.f, 255 / 255.f, 255.f / 255.f), t));


	if (!*v)
		window->DrawList->AddText(ImVec2(check_bb.GetTL().x + 12, check_bb.GetTL().y), ImColor(145 / 255.f, 144 / 255.f, 148 / 255.f, 255.f / 255.f), Buf);

	if (*v)
		ImGui::GetWindowDrawList()->AddText(ImVec2(check_bb.GetTL().x + 12, check_bb.GetTL().y), col_bg, Buf);


	return pressed;

}


void draw_keybind(const char* label, key_bind* key_bind, const char* unique_id)
{
	// reset bind if we re pressing esc
	if (key_bind->key == KEY_ESCAPE)
		key_bind->key = KEY_NONE;

	auto clicked = false;
	auto text = (std::string)m_inputsys()->ButtonCodeToString(key_bind->key);

	if (key_bind->key <= KEY_NONE || key_bind->key >= KEY_MAX)
		text = crypt_str("...");

	// if we clicked on keybind
	if (hooks::input_shouldListen && hooks::input_receivedKeyval == &key_bind->key)
	{
		clicked = true;
		text = crypt_str("...");
	}

	auto textsize = ImGui::CalcTextSize(text.c_str()).x + 8 * c_menu::get().dpi_scale;
	auto labelsize = ImGui::CalcTextSize(label);

	ImGui::Text(label);
	ImGui::SameLine();

	ImGui::SetCursorPosX(ImGui::GetWindowSize().x - (ImGui::GetWindowSize().x - ImGui::CalcItemWidth()) + 35 * c_menu::get().dpi_scale);
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 3 * c_menu::get().dpi_scale);

	if (ImGui::CustomButton(text.c_str(), unique_id, ImVec2(max(50 * c_menu::get().dpi_scale, textsize), 15 * c_menu::get().dpi_scale)))
		ImGui::OpenPopup(unique_id);


	static auto hold = false, toggle = false;

	switch (key_bind->mode)
	{
	case HOLD:
		hold = true;
		toggle = false;
		break;
	case TOGGLE:
		toggle = true;
		hold = false;
		break;
	}

	ImGui::SetNextWindowSize(ImVec2(134, 50));
	if (ImGui::BeginPopup(unique_id))
	{
		padding(8, 2);

		ImGui::Combo("Choose", &key_bind->type, "Hold\0Toggle\0", -1, true);

		if (key_bind->type == 0)
		{
			hold = true;
			if (hold)
			{
				toggle = false;
				key_bind->mode = HOLD;
			}
			else if (toggle)
			{
				hold = false;
				key_bind->mode = TOGGLE;
			}
			else
			{
				toggle = false;
				key_bind->mode = HOLD;
			}

		}
		if (key_bind->type == 1)
		{
			toggle = true;
			if (toggle)
			{
				hold = false;
				key_bind->mode = TOGGLE;
			}
			else if (hold)
			{
				toggle = false;
				key_bind->mode = HOLD;
			}
			else
			{
				hold = false;
				key_bind->mode = TOGGLE;
			}

		}

		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ((ImGui::GetCurrentWindow()->Size.x / 2) - (ImGui::CalcTextSize(crypt_str("Togg")).x / 2)));
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 11);

		if (ImGui::KeybindButton(text.c_str(), unique_id, ImVec2(max(50 * c_menu::get().dpi_scale, textsize), 18 * c_menu::get().dpi_scale), clicked))
			clicked = true;

		if (clicked)
		{
			hooks::input_shouldListen = true;
			hooks::input_receivedKeyval = &key_bind->key;
		}

		ImGui::EndPopup();
	}
}

void draw_semitabs(const char* labels[], int count, int& tab, ImGuiStyle& style)
{
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() - (2 * c_menu::get().dpi_scale));

	// center of main child
	float offset = 343 * c_menu::get().dpi_scale;

	// text size padding + frame padding
	for (int i = 0; i < count; i++)
		offset -= (ImGui::CalcTextSize(labels[i]).x) / 2 + style.FramePadding.x * 2;

	// set new padding
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset);
	ImGui::BeginGroup();

	for (int i = 0; i < count; i++)
	{
		// switch current tab
		if (ImGui::ContentTab(labels[i], tab == i))
			tab = i;

		// continue drawing on same line 
		if (i + 1 != count)
		{
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + style.ItemSpacing.x);
		}
	}

	ImGui::EndGroup();
}


__forceinline void tab_start()
{
	ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + (20 * c_menu::get().dpi_scale), ImGui::GetCursorPosY() + (5 * c_menu::get().dpi_scale)));
}

__forceinline void tab_end()
{
	ImGui::PopStyleVar();
	ImGui::SetWindowFontScale(c_menu::get().dpi_scale);
}
std::string file_path;
static TextEditor editor;

void lua_edit(std::string window_name)
{

	auto get_dir = [&]() -> void
	{
		static TCHAR path[MAX_PATH];


		SHGetFolderPathA(NULL, CSIDL_PERSONAL, NULL, 0, path);
		file_path = std::string(path) + "\\Dynamism.space\\Luas\\";

		CreateDirectory(file_path.c_str(), NULL);
		file_path += window_name + crypt_str(".lua");
	};

	get_dir();


	ImGui::PushStyleVar(ImGuiStyleVar_ScrollbarSize, 5.f);


	if (!loaded_editing_script)
	{
		static auto lang = TextEditor::LanguageDefinition::Lua();

		editor.SetLanguageDefinition(lang);
		editor.SetReadOnly(false);

		std::ifstream t(file_path);

		if (t.good()) // does while exist?
		{
			std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
			editor.SetText(str); // setup script content
		}

		loaded_editing_script = true;
	}

	ImGui::SetWindowFontScale(1.f + ((c_menu::get().dpi_scale - 1.0) * 0.5f));

	padding(8, 8);
	editor.Render("##EDITOR", ImVec2(568, 500));


	//if (ImGui::CustomButton(crypt_str("Save"), (crypt_str("Save") + window_name).c_str(), ImVec2(125 * (1.f + ((c_menu::get().dpi_scale - 1.0) * 0.5f)), 0), true, c_menu::get().ico_bottom, crypt_str("S")))
	//{
	//	std::ofstream out;

	//	out.open(file_path);
	//	out << editor.GetText() << std::endl;
	//	out.close();
	//}

	//ImGui::SameLine();

	//// TOOD: close button will close window (return in start of function)
	//if (ImGui::CustomButton(crypt_str("Close"), (crypt_str("Close") + window_name).c_str(), ImVec2(125 * (1.f + ((c_menu::get().dpi_scale - 1.0) * 0.5f)), 0)))
	//{
	//	g_ctx.globals.focused_on_input = false;
	//	loaded_editing_script = false;
	//	editing_script.clear();
	//}


	ImGui::PopStyleVar();

}

void c_menu::DrawRageTab()
{
	if (subtab2 == 0)
	{
		ImGui::PushStyleColor(ImGuiCol_ChildBg, config_system.g_cfg.menu.menu_theme == 1 ? ImVec4(34 / 255.f, 35 / 255.f, 39 / 255.f, 0.55f) : ImVec4(45 / 255.f, 49 / 255.f, 60 / 255.f, 0.55f));

		ImGui::NewLine();  ImGui::NewLine(); ImGui::SameLine(10);
		ImGui::BeginChild("Tabs", ImVec2(560, 60), false);
		{
			ImGui::PushFont(c_menu::get().ico_menu);
			if (ImGui::BlssButton("Y", "Auto", c_menu::get().futura_small, hooks::rage_weapon == 4 ? true : false, ImVec2(90, 60))) hooks::rage_weapon = 4; ImGui::SameLine(0, 0);
			if (ImGui::BlssButton("a", "Scout", c_menu::get().futura_small, hooks::rage_weapon == 5 ? true : false, ImVec2(90, 60))) hooks::rage_weapon = 5; ImGui::SameLine(0, 0);
			if (ImGui::BlssButton("Z", "AWP", c_menu::get().futura_small, hooks::rage_weapon == 6 ? true : false, ImVec2(100, 60))) hooks::rage_weapon = 6; ImGui::SameLine(0, 0);
			if (ImGui::BlssButton("W", "Rifle", c_menu::get().futura_small, hooks::rage_weapon == 3 ? true : false, ImVec2(100, 60))) hooks::rage_weapon = 3; ImGui::SameLine(0, 0);
			if (ImGui::BlssButton("J", "Heavy pistols", c_menu::get().futura_small, hooks::rage_weapon == 0 ? true : false, ImVec2(90, 60))) hooks::rage_weapon = 0; ImGui::SameLine(0, 0);
			if (ImGui::BlssButton("B", "Pistols", c_menu::get().futura_small, hooks::rage_weapon == 1 ? true : false, ImVec2(90, 60))) hooks::rage_weapon = 1; ImGui::SameLine(0, 0);

			ImGui::PopFont();

		}
		ImGui::EndChild();
		ImGui::PopStyleColor();

		ImGui::Columns(2, NULL, false);
		ImGui::NewLine();
		ImGui::BeginChild(crypt_str("RageBot"), ImVec2(280, 220), true);
		padding(8, 8);
		ImGui::Checkbox(crypt_str("Enable"), &config_system.g_cfg.ragebot.enable); padding(8, 2);


		ImGui::Checkbox(crypt_str("Automatic fire"), &config_system.g_cfg.ragebot.autoshoot); padding(8, 2);
		if (hooks::rage_weapon == 4 || hooks::rage_weapon == 6 || hooks::rage_weapon == 5)
		{
			ImGui::Checkbox(crypt_str("Automatic scope"), &config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].autoscope); padding(8, 2);
		}
		ImGui::Checkbox("Resolver", &config_system.g_cfg.ragebot.resolver); padding(8, 2);
		if (config_system.g_cfg.ragebot.resolver)
		{
			draw_keybind("Resolver Override", &config_system.g_cfg.ragebot.resolver_override, "##RESOLVEROVERRIDE"); padding(8, 2);
			ImGui::Checkbox(crypt_str("Pitch Resolver"), &config_system.g_cfg.ragebot.pitch_antiaim_correction); padding(8, 2);
		}
		draw_keybind(crypt_str("Auto peek"), &config_system.g_cfg.misc.automatic_peek, crypt_str("##AUTOPEEK__HOTKEY")); padding(8, 2);

		if (ImGui::BeginCombo(crypt_str("Hit scan"), "Choose"))
		{
			ImGui::Spacing();
			ImGui::BeginGroup();
			{
				ImGui::MenuItem("Head", nullptr, &config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].HEAD, ImGuiButtonFlags_DontClosePopups);
				ImGui::MenuItem("Neck", nullptr, &config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].NECK, ImGuiButtonFlags_DontClosePopups);
				ImGui::MenuItem("Upper chest", nullptr, &config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].UPERR_CHEST, ImGuiButtonFlags_DontClosePopups);
				ImGui::MenuItem("Chest", nullptr, &config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].CHEST, ImGuiButtonFlags_DontClosePopups);
				ImGui::MenuItem("Lower chest", nullptr, &config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].LOWER_CHEST, ImGuiButtonFlags_DontClosePopups);
				ImGui::MenuItem("Stomach", nullptr, &config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].STOMACH, ImGuiButtonFlags_DontClosePopups);
				ImGui::MenuItem("Pelvis", nullptr, &config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].PELVIS, ImGuiButtonFlags_DontClosePopups);
				ImGui::MenuItem("Arms", nullptr, &config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].RIGHT_UPPER_ARM, ImGuiButtonFlags_DontClosePopups);
				ImGui::MenuItem("Legs", nullptr, &config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].RIGHT_THIGH, ImGuiButtonFlags_DontClosePopups);
				ImGui::MenuItem("Foot", nullptr, &config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].FOOT, ImGuiButtonFlags_DontClosePopups);
			}
			ImGui::EndGroup();
			ImGui::Spacing();
			ImGui::EndCombo();
		}padding(8, 2);


		ImGui::Checkbox("Dynamic Body Aim", &config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].body_if.enable); padding(8, 2);
		if (config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].body_if.enable) {
			if (ImGui::BeginCombo(crypt_str("Force body if"), "Choose"))
			{
				ImGui::Spacing();
				ImGui::BeginGroup();
				{
					ImGui::Selectable("In air", &config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].body_if.air, ImGuiSelectableFlags_DontClosePopups);
					ImGui::Selectable("Lethal", &config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].body_if.lethal, ImGuiSelectableFlags_DontClosePopups);
					ImGui::Selectable("Under HP", &config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].body_if.if_under, ImGuiSelectableFlags_DontClosePopups);
				}
				ImGui::EndGroup();
				ImGui::Spacing();
				ImGui::EndCombo();
			}padding(8, 2);
			if (config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].body_if.if_under)
			{
				padding(8, 6); ImGui::SliderInt("Under x HP", &config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].body_if.under_hp, 0, 100); padding(8, 2);
			}
		}
		ImGui::Checkbox("Dynamic Head Aim", &config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].head_if.enable); padding(8, 2);
		if (config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].head_if.enable) {
			if (ImGui::BeginCombo(crypt_str("Force head if"), "Choose"))
			{
				ImGui::Spacing();
				ImGui::BeginGroup();
				{
					ImGui::Selectable("In air", &config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].head_if.air, ImGuiSelectableFlags_DontClosePopups);
					ImGui::Selectable("Running", &config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].head_if.running, ImGuiSelectableFlags_DontClosePopups);
				}
				ImGui::EndGroup();
				ImGui::Spacing();
				ImGui::EndCombo();
			}padding(8, 2);
		}


		ImGui::EndChild();

		ImGui::NewLine();
		ImGui::BeginChild(crypt_str("Override"), ImVec2(280, 220), true);
		padding(8, 8);
		ImGui::Checkbox(crypt_str("Automatic penetration"), &config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].autowall); padding(8, 2);

		if (hooks::rage_weapon == 4 || hooks::rage_weapon == 6 || hooks::rage_weapon == 5)
		{
			ImGui::Checkbox(crypt_str("No scope hitchance"), &config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].noscope_ht); padding(8, 2);
		}
		ImGui::Checkbox(crypt_str("Quick stop"), &config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].autostop); padding(8, 2);
		if (config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].autostop)
		{
			if (ImGui::BeginCombo(crypt_str("Modifiers"), "Choose"))
			{
				ImGui::Spacing();
				ImGui::MenuItem("Between shots", nullptr, &config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].AUTOSTOP_BETWEEN_SHOTS, ImGuiSelectableFlags_DontClosePopups);
				ImGui::MenuItem("On lethal", nullptr, &config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].AUTOSTOP_LETHAL, ImGuiSelectableFlags_DontClosePopups);
				ImGui::MenuItem("On visible", nullptr, &config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].AUTOSTOP_VISIBLE, ImGuiSelectableFlags_DontClosePopups);
				ImGui::Spacing();
				ImGui::EndCombo();
			}padding(8, 2);
		}

		ImGui::Checkbox(crypt_str("Prefer safe points"), &config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].prefer_safe_points); padding(8, 2);
		ImGui::Checkbox(crypt_str("Prefer body aim"), &config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].prefer_body_aim); padding(8, 2);
		draw_keybind(crypt_str("Damage override"), &config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].damage_override_key, crypt_str("##HOTKEY__DAMAGE_OVERRIDE")); padding(8, 2);

		draw_keybind(crypt_str("Force safe points"), &config_system.g_cfg.ragebot.safe_point_key, crypt_str("##HOKEY_FORCE_SAFE_POINTS")); padding(8, 2);
		draw_keybind(crypt_str("Force body aim"), &config_system.g_cfg.ragebot.body_aim_key, crypt_str("##HOKEY_FORCE_BODY_AIM")); padding(8, 2);



		ImGui::EndChild();
		ImGui::NextColumn();
		{
			ImGui::NewLine();
			ImGui::BeginChild(crypt_str("Configuration"), ImVec2(280, 280), true);
			padding(8, 15);
			ImGui::SliderInt(crypt_str("Hitchance amount"), &config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].hitchance_amount, 1, 100); padding(8, 2);
			if (config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].noscope_ht)
			{
				ImGui::SliderInt(crypt_str("Hitchance amount [not scoped]"), &config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].noscope_hitchance, 1, 100); padding(8, 2);
			}
			ImGui::SliderInt(crypt_str("Minimum damage"), &config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].minimum_visible_damage, 1, 120, true); padding(8, 2);

			if (config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].autowall)
			{
				ImGui::SliderInt(crypt_str("Autowall min damage"), &config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].minimum_damage, 1, 120, true); padding(8, 2);
			}
			if (config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].damage_override_key.key > KEY_NONE && config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].damage_override_key.key < KEY_MAX)
			{
				ImGui::SliderInt(crypt_str("Override min damage"), &config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].minimum_override_damage, 1, 120, "%s.2f hp"); padding(8, 2);
			}

			ImGui::Checkbox(crypt_str("Enable multipoint"), &config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].static_point_scale); padding(8, 2);

			if (config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].static_point_scale)
			{
				padding(6, 8); ImGui::SliderFloat(crypt_str("Head scale"), &config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].head_scale, 0.0f, 100.f, config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].head_scale ? crypt_str("%.2f") : crypt_str("None")); padding(8, 2);
				padding(6, 8); ImGui::SliderFloat(crypt_str("Body scale"), &config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].body_scale, 0.0f, 100.f, config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].body_scale ? crypt_str("%.2f") : crypt_str("None")); padding(8, 2);
			}

			ImGui::Checkbox(crypt_str("Enable max misses"), &config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].max_misses); padding(8, 2);

			if (config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].max_misses)
			{
				padding(6, 8); ImGui::SliderInt(crypt_str("Max misses"), &config_system.g_cfg.ragebot.weapon[hooks::rage_weapon].max_misses_amount, 0, 6); padding(8, 2);
			}

			ImGui::EndChild();

			ImGui::NewLine();
			ImGui::BeginChild(crypt_str("Exploits"), ImVec2(280, 160), true);
			padding(8, 8);
			ImGui::Checkbox(crypt_str("Double tap"), &config_system.g_cfg.ragebot.double_tap); padding(8, 2);
			if (config_system.g_cfg.ragebot.double_tap)
			{
				ImGui::SameLine();
				draw_keybind(crypt_str(""), &config_system.g_cfg.ragebot.double_tap_key, crypt_str("##HOTKEY_DOUBLE")); padding(8, 2);
			}
			ImGui::Checkbox(crypt_str("Hide shots"), &config_system.g_cfg.antiaim.hide_shots); padding(8, 2);

			if (config_system.g_cfg.antiaim.hide_shots)
			{
				ImGui::SameLine();
				draw_keybind(crypt_str(""), &config_system.g_cfg.antiaim.hide_shots_key, crypt_str("##HOTKEY_HIDESHOTS")); padding(8, 2);
			}
			ImGui::EndChild();
		}
		ImGui::Columns(1);
	}
	else if (subtab2 == 1)
	{
		ImGui::Columns(2, NULL, false);
		ImGui::NewLine();
		ImGui::BeginChild(crypt_str("Anti aim"), ImVec2(280, 530), true);
		padding(8, 8);
		static auto type = 0;
		ImGui::PushFont(c_menu::get().antiaim_icons);

		ImGui::PushStyleColor(ImGuiCol_ChildBg, config_system.g_cfg.menu.menu_theme == 1 ? ImVec4(34 / 255.f, 35 / 255.f, 39 / 255.f, 0.55f) : ImVec4(45 / 255.f, 49 / 255.f, 60 / 255.f, 0.55f));
		ImGui::BeginChild("AntiAim", ImVec2(260, 60), false);
		{
			if (ImGui::BlssButton("i", "Static", c_menu::get().futura_small, type == 0 ? true : false, ImVec2(93, 60))) type = 0; ImGui::SameLine(0, 0);
			if (ImGui::BlssButton("h", "Moving", c_menu::get().futura_small, type == 2 ? true : false, ImVec2(93, 60))) type = 2; ImGui::SameLine(0, 0);
			if (ImGui::BlssButton("g", "Air", c_menu::get().futura_small, type == 3 ? true : false, ImVec2(93, 60))) type = 3; ImGui::SameLine(0, 0);
		}
		ImGui::EndChild();
		ImGui::PopFont();
		ImGui::PopStyleColor();
		ImGui::NewLine();
		padding(8, 2);
		ImGui::Checkbox(crypt_str("Enable"), &config_system.g_cfg.antiaim.enable); padding(8, 2);
		ImGui::Checkbox("At targets", &config_system.g_cfg.antiaim.at_target); padding(8, 2);
		ImGui::Checkbox("Manual", &config_system.g_cfg.antiaim.manual); padding(8, 2);
		if (config_system.g_cfg.antiaim.manual)
		{

			draw_keybind("Manual back", &config_system.g_cfg.antiaim.manual_back, "##HOTKEY_INVERT_BACK"); padding(8, 2);
			draw_keybind(crypt_str("Manual left"), &config_system.g_cfg.antiaim.manual_left, crypt_str("##HOTKEY_INVERT_LEFT")); padding(8, 2);
			draw_keybind(crypt_str("Manual right"), &config_system.g_cfg.antiaim.manual_right, crypt_str("##HOTKEY_INVERT_RIGHT")); padding(8, 2);
		}
		draw_combo(crypt_str("Pitch"), config_system.g_cfg.antiaim.pitch, pitch, ARRAYSIZE(pitch));
		padding(8, 2);
		draw_combo(crypt_str("Yaw"), config_system.g_cfg.antiaim.yaw, yaw, ARRAYSIZE(yaw));
		padding(8, 2);
		if (config_system.g_cfg.antiaim.yaw == 1)
		{
			ImGui::SliderInt(crypt_str("Jitter range"), &config_system.g_cfg.antiaim.range, 1, 180); padding(8, 2);
		}

		draw_combo(crypt_str("Desync"), config_system.g_cfg.antiaim.desynch, desync, ARRAYSIZE(desync)); padding(8, 2);

		if (config_system.g_cfg.antiaim.desync != 3 && config_system.g_cfg.antiaim.desync != 0)
		{
			draw_keybind(crypt_str("Invert desync"), &config_system.g_cfg.antiaim.flip_desync, "##INVERT_DESYNC"); padding(8, 2);
		}


		ImGui::EndChild();
		ImGui::NextColumn();
		{
			ImGui::NewLine();
			ImGui::BeginChild("Others", ImVec2(280, 280), true);
			padding(8, 8);
			ImGui::Checkbox(crypt_str("Slow walk"), &config_system.g_cfg.misc.slowwalk); padding(8, 2);

			if (config_system.g_cfg.misc.slowwalk)
			{
				ImGui::SameLine(); draw_keybind(crypt_str(""), &config_system.g_cfg.misc.slowwalk_key, "##SLOWWALK"); padding(8, 2);
				padding(6, 8);	ImGui::SliderInt(crypt_str("Slow walk speed"), &config_system.g_cfg.misc.slowwalk_speed, 0, 100); padding(8, 2);
			}
			draw_keybind(crypt_str("Fake duck"), &config_system.g_cfg.misc.fakeduck_key, "##FAKEDUCK__HOTKEY"); padding(8, 2);

			ImGui::Checkbox(crypt_str("Automatic Inverter"), &config_system.g_cfg.antiaim.auto_inverter); padding(8, 2);
			if (config_system.g_cfg.antiaim.auto_inverter) {
				draw_combo(crypt_str("Inverter Type"), config_system.g_cfg.antiaim.inverter_type, inverters, ARRAYSIZE(inverters)); padding(8, 2);
			}


			ImGui::EndChild();
			ImGui::NewLine();
			ImGui::BeginChild("FakeLag", ImVec2(280, 230), true);
			{
				padding(8, 8);
				ImGui::Checkbox(crypt_str("Fakelag"), &config_system.g_cfg.antiaim.fakelag); padding(8, 2);
				if (config_system.g_cfg.antiaim.fakelag)
				{
					draw_combo(crypt_str("Fake lag type"), config_system.g_cfg.move_type[1].fakelag_type, fakelags, ARRAYSIZE(fakelags)); padding(8, 2);
					ImGui::SliderInt(crypt_str("Fakelag amount"), &config_system.g_cfg.antiaim.fakelag_amount, 1, 16); padding(8, 2);
					ImGui::SliderInt(crypt_str("Triggers amount"), &config_system.g_cfg.antiaim.triggers_fakelag_amount, 1, 16);
				}
			}



			ImGui::EndChild();
			ImGui::Columns(1);
		}
	}
}


void c_menu::DrawMainTab()
{
	static char MESSAGE[255] = "";

	if (subtab == 0)
	{

		ImGui::NewLine();
		ImGui::PushStyleColor(ImGuiCol_ChildBg, config_system.g_cfg.menu.menu_theme == 1 ? ImVec4(34 / 255.f, 35 / 255.f, 39 / 255.f, 0.55f) : ImVec4(45 / 255.f, 49 / 255.f, 60 / 255.f, 0.55f));
			ImGui::BeginChild(crypt_str("Changelogs"), ImVec2(280, 520), false);
			padding(8, 4);
			ImGui::Text("Changelog to that shit paste");
			ImGui::Separator();
			padding(8, 8);
			ImGui::Text("[+] New resolver"); padding(8, 2);
			ImGui::Text("[+] Better ragebot"); padding(8, 2);
			ImGui::Text("[+] Better prediction system"); padding(8, 2);
			ImGui::Text("[-] Removed scripts"); padding(8, 2);
			ImGui::Text("[+] Better animationfix"); padding(8, 2);
			ImGui::Text("[+] Fixed nightmode"); padding(8, 2);

			ImGui::EndChild();

		ImGui::PopStyleColor();
	}
}

void c_menu::draw_players()
{
	static int map = 0;
	if (subtab4 < 3)
	{
		auto player = subtab4;

		ImGui::Columns(2, NULL, false);
		ImGui::NewLine();
		ImGui::BeginChild(crypt_str("ESP"), ImVec2(280, 280), true);

		padding(8, 8);
		ImGui::Checkbox(crypt_str("Enable"), &config_system.g_cfg.player.enable); padding(8, 2);
		ImGui::Checkbox(crypt_str("Glow"), &config_system.g_cfg.player.type[player].glow); padding(8, 2);
		ImGui::SameLine(100); ImGui::ColorEdit4("##glow", &config_system.g_cfg.player.type[player].glow_color); padding(8, 2);

		if (player == ENEMY)
		{
			ImGui::Checkbox(crypt_str("Fov arrows"), &config_system.g_cfg.player.arrows); padding(8, 2);
			ImGui::SameLine(100); ImGui::ColorEdit4("##fov_arrow", &config_system.g_cfg.player.arrows_color); padding(8, 2);
		}

		ImGui::Checkbox(crypt_str("Box"), &config_system.g_cfg.player.type[player].box); padding(8, 2);
		ImGui::SameLine(100); ImGui::ColorEdit4("##box_color", &config_system.g_cfg.player.type[player].box_color); padding(8, 2);

		if (config_system.g_cfg.player.type[player].box)
		{
			ImGui::Combo("Box type", &config_system.g_cfg.player.type[player].box_type, "Normal\0Corners\0Rouding\0"); padding(8, 2);
			if (config_system.g_cfg.player.type[player].box_type == 2)
			{
				padding(6, 8); ImGui::SliderFloat("Rouding", &config_system.g_cfg.player.type[player].rouding, 0, 20); padding(8, 2);
			}

			ImGui::Checkbox("Filled box", &config_system.g_cfg.player.type[player].filledbox); padding(8, 2);
			ImGui::SameLine(100); ImGui::ColorEdit4("##filled_color", &config_system.g_cfg.player.type[player].filledboxcolor); padding(8, 2);

		}

		ImGui::Checkbox(crypt_str("Health bar"), &config_system.g_cfg.player.type[player].health); padding(8, 2);
		ImGui::Checkbox(crypt_str("Ammo bar"), &config_system.g_cfg.player.type[player].ammo); padding(8, 2);
		ImGui::Checkbox(crypt_str("Skeleton"), &config_system.g_cfg.player.type[player].skeleton); padding(8, 2);
		ImGui::Checkbox(crypt_str("Name"), &config_system.g_cfg.player.type[player].name); padding(8, 2);

		if (ImGui::BeginCombo(crypt_str("Weapon"), "Choose"))
		{
			ImGui::Spacing();
			ImGui::BeginGroup();
			{
				ImGui::Selectable("Icon", &config_system.g_cfg.player.type[player].weapon_icon, ImGuiSelectableFlags_DontClosePopups);
				//				ImGui::Selectable("Text", &config_system.g_cfg.player.type[player].weapon_text, ImGuiSelectableFlags_DontClosePopups);
			}
			ImGui::EndGroup();
			ImGui::Spacing();
			ImGui::EndCombo();
		}padding(8, 2);


		if (ImGui::BeginCombo(crypt_str("Flags"), "Choose"))
		{
			ImGui::Spacing();
			ImGui::BeginGroup();
			{
				ImGui::Selectable("Money", &config_system.g_cfg.player.type[player].FLAGS_MONEY, ImGuiSelectableFlags_DontClosePopups);
				ImGui::Selectable("Armor", &config_system.g_cfg.player.type[player].FLAGS_ARMOR, ImGuiSelectableFlags_DontClosePopups);
				ImGui::Selectable("Scoped", &config_system.g_cfg.player.type[player].FLAGS_SCOPED, ImGuiSelectableFlags_DontClosePopups);
				ImGui::Selectable("Defuse kit", &config_system.g_cfg.player.type[player].FLAGS_KIT, ImGuiSelectableFlags_DontClosePopups);
				ImGui::Selectable("C4 Carrier", &config_system.g_cfg.player.type[player].FLAGS_C4, ImGuiSelectableFlags_DontClosePopups);
			}
			ImGui::EndGroup();
			ImGui::Spacing();
			ImGui::EndCombo();
		}padding(8, 2);

		ImGui::EndChild();
		ImGui::NewLine();
		ImGui::BeginChild("Others", ImVec2(280, 220), true);
		padding(8, 8);
		ImGui::Checkbox(crypt_str("Footsteps"), &config_system.g_cfg.player.type[player].footsteps); padding(8, 2);
		ImGui::SameLine(100); ImGui::ColorEdit4("##footsteps_color", &config_system.g_cfg.player.type[player].footsteps_color); padding(8, 2);

		ImGui::Checkbox(crypt_str("DLight"), &config_system.g_cfg.player.type[player].dLight); padding(8, 2);
		ImGui::SameLine(100); ImGui::ColorEdit4("##dlight_color", &config_system.g_cfg.player.type[player].dLight_color); padding(8, 2);

		if (config_system.g_cfg.player.type[player].dLight)
		{
			padding(6, 8); ImGui::SliderFloat("Dlight radius", &config_system.g_cfg.player.type[player].dLightR, 0, 1000); padding(8, 2);
			padding(6, 8); ImGui::SliderFloat("Dlight decay", &config_system.g_cfg.player.type[player].dLightDecay, 0, 400); padding(8, 2);
			padding(6, 8); ImGui::SliderInt("Dlight style", &config_system.g_cfg.player.type[player].dLightStyle, 0, 10); padding(8, 2);
			padding(6, 8); ImGui::SliderInt("Dlight expoent", &config_system.g_cfg.player.type[player].dLightExpoent, 0, 5); padding(8, 2);

		}
		ImGui::EndChild();
		ImGui::NextColumn();
		{
			ImGui::NewLine();
			ImGui::BeginChild("Chams", ImVec2(280, 520), true);

			padding(8, 8);
			if (player == ENEMY || player == TEAM)
			{
				ImGui::Checkbox("Enable chams", &config_system.g_cfg.player.type[player].enable_chams); padding(8, 2);
				ImGui::SameLine(100); ImGui::ColorEdit4("##visible.color", &config_system.g_cfg.player.type[player].chams_color); padding(8, 2);

				if (config_system.g_cfg.player.type[player].enable_chams)
				{
					draw_combo(crypt_str("Chams material"), config_system.g_cfg.player.type[player].chams_type, chamstype, ARRAYSIZE(chamstype)); padding(8, 2);
				}
				ImGui::Checkbox("Enable XQZ", &config_system.g_cfg.player.type[player].xqz_enable); padding(8, 2);
				ImGui::SameLine(100); ImGui::ColorEdit4("##xqz.color", &config_system.g_cfg.player.type[player].xqz_color); padding(8, 2);

				if (config_system.g_cfg.player.type[player].xqz_enable)
				{
					draw_combo(crypt_str("Chams xqz material"), config_system.g_cfg.player.type[player].chams_xqz, chamstype, ARRAYSIZE(chamstype)); padding(8, 2);

				}
				ImGui::Separator(); padding(8, 2);
				ImGui::Checkbox(crypt_str("Second material"), &config_system.g_cfg.player.type[player].double_material); padding(8, 2);
				ImGui::SameLine(100); ImGui::ColorEdit4("##second.color", &config_system.g_cfg.player.type[player].double_material_color); padding(8, 2);

				if (config_system.g_cfg.player.type[player].double_material)
				{
					draw_combo(crypt_str("Second chams material"), config_system.g_cfg.player.type[player].double_material_m, chamstype, ARRAYSIZE(chamstype)); padding(8, 2);
				}
				ImGui::Checkbox(crypt_str("Second xqz material"), &config_system.g_cfg.player.type[player].double_material_xqz); padding(8, 2);
				ImGui::SameLine(100); ImGui::ColorEdit4("##second.xqz.material", &config_system.g_cfg.player.type[player].double_material_color_xqz); padding(8, 2);

				if (config_system.g_cfg.player.type[player].double_material_xqz)
				{
					draw_combo(crypt_str("Second xqz chams material"), config_system.g_cfg.player.type[player].double_material_m_xqz, chamstype, ARRAYSIZE(chamstype)); padding(8, 2);
				}

				if (player == ENEMY)
				{
					ImGui::Separator(); padding(8, 2);
					ImGui::Checkbox(crypt_str("Backtrack chams"), &config_system.g_cfg.player.backtrack_chams); padding(8, 2);
					ImGui::SameLine(100); ImGui::ColorEdit4("##backtrack.color", &config_system.g_cfg.player.backtrack_chams_color); padding(8, 2);
					if (config_system.g_cfg.player.backtrack_chams)
					{
						draw_combo(crypt_str("Backtrack chams material"), config_system.g_cfg.player.backtrack_chams_material, chamstype, ARRAYSIZE(chamstype)); padding(8, 2);

					}
					/*ImGui::Separator(); padding(8, 2);
					ImGui::Checkbox(crypt_str("Shot record chams"), &config_system.g_cfg.player.lag_hitbox); padding(8, 2);
					ImGui::SameLine(100); ImGui::ColorEdit4("##lagcom.color", &config_system.g_cfg.player.lagcomp_chams_color); padding(8, 2);
					if (config_system.g_cfg.player.lag_hitbox)
					{
						draw_combo(crypt_str("Shot record chams material"), config_system.g_cfg.player.lagcomp_chams_material, chamstype, ARRAYSIZE(chamstype)); padding(8, 2);

					}*/
				}
				ImGui::Separator(); padding(8, 2);
				ImGui::Checkbox(crypt_str("Ragdoll chams"), &config_system.g_cfg.player.type[player].ragdoll_chams); padding(8, 2);
				ImGui::SameLine(100); ImGui::ColorEdit4("##ragdoll.color", &config_system.g_cfg.player.type[player].ragdoll_chams_color); padding(8, 2);
				if (config_system.g_cfg.player.type[player].ragdoll_chams)
				{
					draw_combo(crypt_str("Ragdoll chams material"), config_system.g_cfg.player.type[player].ragdoll_chams_material, chamstype, ARRAYSIZE(chamstype)); padding(8, 2);
				}
			}
			if (player == LOCAL)
			{
				static int chamspg = 0;

				ImGui::Combo("Chams type", &chamspg, "Local\0ViewModel\0"); padding(8, 2);
				switch (chamspg)
				{
				case 0:
					ImGui::Checkbox("Enable Chams", &config_system.g_cfg.player.type[LOCAL].enable_chams); padding(8, 2);
					ImGui::SameLine(100); ImGui::ColorEdit4("##local.color", &config_system.g_cfg.player.type[player].chams_color); padding(8, 2);
					if (config_system.g_cfg.player.type[LOCAL].enable_chams)
					{
						draw_combo(crypt_str("Chams material"), config_system.g_cfg.player.type[player].chams_type, chamstype, ARRAYSIZE(chamstype)); padding(8, 2);

					}
					ImGui::Separator(); padding(8, 2);
					ImGui::Checkbox(crypt_str("Second material"), &config_system.g_cfg.player.type[player].double_material); padding(8, 2);
					ImGui::SameLine(100); ImGui::ColorEdit4("##Second local color", &config_system.g_cfg.player.type[player].double_material_color); padding(8, 2);
					if (config_system.g_cfg.player.type[player].double_material)
					{
						draw_combo(crypt_str("Second chams material"), config_system.g_cfg.player.type[player].double_material_m, chamstype, ARRAYSIZE(chamstype)); padding(8, 2);
					}

					ImGui::Separator(); padding(8, 2);
					ImGui::Checkbox(crypt_str("Desync chams"), &config_system.g_cfg.player.fake_chams_enable); padding(8, 2);
					ImGui::SameLine(100); ImGui::ColorEdit4("##desync.color", &config_system.g_cfg.player.fake_chams_color); padding(8, 2);
					if (config_system.g_cfg.player.fake_chams_enable)
					{
						draw_combo(crypt_str("Desync chams material"), config_system.g_cfg.player.fake_chams_type, chamstype, ARRAYSIZE(chamstype)); padding(8, 2);

						ImGui::Checkbox(crypt_str("Second desync material"), &config_system.g_cfg.player.fake_double_material); padding(8, 2);
						ImGui::SameLine(100); ImGui::ColorEdit4("##second.desync.color", &config_system.g_cfg.player.fake_double_material_color); padding(8, 2);
						if (config_system.g_cfg.player.fake_double_material)
						{
							draw_combo(crypt_str("Second desync chams material"), config_system.g_cfg.player.fake_double_material_m, chamstype, ARRAYSIZE(chamstype)); padding(8, 2);
						}
					}
					ImGui::Separator(); padding(8, 2);
					ImGui::Checkbox(crypt_str("FakeLag chams"), &config_system.g_cfg.player.visualize_lag); padding(8, 2);
					ImGui::Separator(); padding(8, 2);
					ImGui::Checkbox(crypt_str("Transparency in scope"), &config_system.g_cfg.player.transparency_in_scope); padding(8, 2);
					padding(6, 8);
					if (config_system.g_cfg.player.transparency_in_scope)
						ImGui::SliderFloat(crypt_str("Set your alpha"), &config_system.g_cfg.player.transparency_in_scope_amount, 0.f, 100.f);

					break;
				case 1:
					ImGui::Checkbox(crypt_str("Arms chams"), &config_system.g_cfg.esp.arms_chams); padding(8, 2);
					ImGui::SameLine(100); ImGui::ColorEdit4("##arms.color", &config_system.g_cfg.esp.arms_chams_color); padding(8, 2);
					if (config_system.g_cfg.esp.arms_chams)
					{
						draw_combo(crypt_str("Arms chams material"), config_system.g_cfg.esp.arms_chams_type, chamstype, ARRAYSIZE(chamstype)); padding(8, 2);
					}
					ImGui::Checkbox(crypt_str("Second material "), &config_system.g_cfg.esp.arms_double_material); padding(8, 2);
					ImGui::SameLine(100); ImGui::ColorEdit4("##Second arms color", &config_system.g_cfg.esp.arms_double_material_color, ALPHA); padding(8, 2);
					if (config_system.g_cfg.esp.arms_double_material)
					{
						draw_combo(crypt_str("Second arms chams material"), config_system.g_cfg.esp.arms_double_material_m, chamstype, ARRAYSIZE(chamstype)); padding(8, 2);
						ImGui::ColorEdit4(crypt_str("Second arms color"), &config_system.g_cfg.esp.arms_double_material_color, ALPHA); padding(8, 2);
					}
					ImGui::Separator(); padding(8, 2);

					ImGui::Checkbox(crypt_str("Weapon chams"), &config_system.g_cfg.esp.weapon_chams); padding(8, 2);
					ImGui::SameLine(100); ImGui::ColorEdit4("##weapon.color", &config_system.g_cfg.esp.weapon_chams_color, ALPHA); padding(8, 2);
					if (config_system.g_cfg.esp.weapon_chams)
					{
						draw_combo(crypt_str("Weapon chams material"), config_system.g_cfg.esp.weapon_chams_type, chamstype, ARRAYSIZE(chamstype)); padding(8, 2);
					}


					break;
				}

			}

			ImGui::EndChild();
		}
		ImGui::Columns(1);
	}
	else if (subtab4 == 3)
	{
		ImGui::Columns(2, NULL, false);
		ImGui::NewLine();
		ImGui::BeginChild("World", ImVec2(280, 280), true);
		padding(8, 8);

		ImGui::Checkbox(crypt_str("Night mode"), &config_system.g_cfg.esp.nightmode); padding(8, 2);
		if (config_system.g_cfg.esp.nightmode)
		{
			ImGui::ColorEdit4(crypt_str("World color"), &config_system.g_cfg.esp.worlds_color, true, true); padding(8, 2);
			ImGui::ColorEdit4(crypt_str("Props color"), &config_system.g_cfg.esp.pops_color, true, true); padding(8, 2);
		}
		ImGui::Checkbox(crypt_str("Transparency"), &config_system.g_cfg.esp.asus); padding(8, 2);
		if (config_system.g_cfg.esp.asus)
		{
			padding(6, 8); ImGui::SliderInt("World transparency", &config_system.g_cfg.esp.world_transparency, 0, 100); padding(8, 2);
			padding(6, 8);	ImGui::SliderInt("Props transparency", &config_system.g_cfg.esp.props_transparency, 0, 100); padding(8, 2);

		}
		//padding(8, 2); ImGui::SliderInt(crypt_str("Field of view"), &config_system.g_cfg.esp.fov, 0, 89);

		ImGui::Checkbox("Sky changer", &config_system.g_cfg.esp.skyboxch);
		if (config_system.g_cfg.esp.skyboxch)
		{

			padding(8, 2); ImGui::Combo("Skybox changer", &config_system.g_cfg.esp.skybox, "cs_baggage_skybox_\0cs_tibet\0embassy\0italy\0jungle\0nukeblank\0office\0sky_cs15_daylight01_hdr\0sky_cs15_daylight02_hdr\0sky_cs15_daylight03_hdr\0sky_cs15_daylight04_hdr\0sky_csgo_cloudy01\0sky_csgo_night_flat\0sky_csgo_night02\0sky_day02_05_hdr\0sky_day02_05\0sky_dust\0sky_l4d_rural02_ldr\0sky_venice\0vertigo_hdr\0vertigo\0vertigoblue_hdr\0vietnam\0custom\0");

			padding(8, 2); ImGui::ColorEdit4(crypt_str("Sky color"), &config_system.g_cfg.esp.skybox_color, true, true); padding(8, 2);

			/*	padding(8, 2); ImGui::ColorEdit4(crypt_str("Skybox color"), &config_system.g_cfg.esp.skybox_color); */

			if (config_system.g_cfg.esp.skybox == 23)
			{
				static char sky_custom[64] = "\0";

				if (!config_system.g_cfg.esp.custom_skybox.empty())
					strcpy_s(sky_custom, sizeof(sky_custom), config_system.g_cfg.esp.custom_skybox.c_str());


				padding(8, 2);	if (ImGui::InputText(crypt_str("Search"), sky_custom, sizeof(sky_custom)))
					config_system.g_cfg.esp.custom_skybox = sky_custom;

			}
		}padding(8, 2);
		ImGui::Checkbox(crypt_str("Fog modulation"), &config_system.g_cfg.esp.fog); padding(8, 2);
		if (config_system.g_cfg.esp.fog)
		{
			ImGui::ColorEdit4(crypt_str("Fog color"), &config_system.g_cfg.esp.fog_color, true, true); padding(8, 2);
		}

		if (ImGui::BeginCombo(crypt_str("Removals"), "Remove"))
		{
			ImGui::Spacing();
			ImGui::BeginGroup();
			{
				ImGui::MenuItem("Scope", nullptr, &config_system.g_cfg.esp.REMOVALS_SCOPE, ImGuiSelectableFlags_DontClosePopups);
				ImGui::MenuItem("Zoom", nullptr, &config_system.g_cfg.esp.REMOVALS_ZOOM, ImGuiSelectableFlags_DontClosePopups);
				ImGui::MenuItem("Smoke", nullptr, &config_system.g_cfg.esp.REMOVALS_SMOKE, ImGuiSelectableFlags_DontClosePopups);
				ImGui::MenuItem("Flash", nullptr, &config_system.g_cfg.esp.REMOVALS_FLASH, ImGuiSelectableFlags_DontClosePopups);
				ImGui::MenuItem("Recoil", nullptr, &config_system.g_cfg.esp.REMOVALS_RECOIL, ImGuiSelectableFlags_DontClosePopups);
				ImGui::MenuItem("Postprocessing", nullptr, &config_system.g_cfg.esp.REMOVALS_POSTPROCESSING, ImGuiSelectableFlags_DontClosePopups);
			}
			ImGui::EndGroup();
			ImGui::Spacing();

			ImGui::EndCombo();
		}padding(8, 2);
		ImGui::Checkbox(crypt_str("Lighting on shot"), &config_system.g_cfg.misc.lightingonshot); padding(8, 2);

		ImGui::NewLine();
		ImGui::EndChild();

		ImGui::NewLine();
		ImGui::BeginChild("Effects", ImVec2(280, 220), true);
		padding(8, 8);
		ImGui::Checkbox(crypt_str("Impacts"), &config_system.g_cfg.esp.client_bullet_impacts); padding(8, 2);
		ImGui::SameLine(100); ImGui::ColorEdit4(crypt_str("##clientbulletimpacts"), &config_system.g_cfg.esp.client_bullet_impacts_color, ALPHA); padding(8, 2);

		ImGui::Checkbox(crypt_str("Bullet tracers"), &config_system.g_cfg.esp.bullet_tracer); padding(8, 2);
		ImGui::SameLine(100); ImGui::ColorEdit4(crypt_str("##bulltracecolor"), &config_system.g_cfg.esp.bullet_tracer_color, ALPHA); padding(8, 2);

		ImGui::Checkbox(crypt_str("Enemies bullet tracers"), &config_system.g_cfg.esp.enemy_bullet_tracer); padding(8, 2);
		ImGui::SameLine(100); ImGui::ColorEdit4(crypt_str("##enemiesbulltracecolor"), &config_system.g_cfg.esp.enemy_bullet_tracer_color, ALPHA); padding(8, 2);

		ImGui::Checkbox(crypt_str("Grenade prediction"), &config_system.g_cfg.esp.grenade_prediction); padding(8, 2);
		if (config_system.g_cfg.esp.grenade_prediction)
		{
			ImGui::ColorEdit4(crypt_str("Tracer color"), &config_system.g_cfg.esp.grenade_prediction_tracer_color, ALPHA, true); padding(8, 2);
			ImGui::ColorEdit4(crypt_str("Predict color"), &config_system.g_cfg.esp.grenade_prediction_color, ALPHA, true); padding(8, 2);
		}
		ImGui::EndChild();
		ImGui::NextColumn();
		{
			ImGui::NewLine();
			ImGui::BeginChild("World-ESP", ImVec2(280, 520), true);
			padding(8, 8);
			ImGui::Checkbox(crypt_str("Fire timer"), &config_system.g_cfg.esp.molotov_timer); padding(8, 2);

			ImGui::Checkbox(crypt_str("Smoke timer"), &config_system.g_cfg.esp.smoke_timer); padding(8, 2);
			ImGui::Checkbox(crypt_str("Bomb ESP"), &config_system.g_cfg.esp.bright); padding(8, 2);

			if (ImGui::BeginCombo(crypt_str("Weapon ESP"), "Choose")) // draw start
			{
				ImGui::Spacing();
				ImGui::BeginGroup();
				{
					ImGui::Selectable("Box", &config_system.g_cfg.esp.WEAPON_BOX, ImGuiSelectableFlags_DontClosePopups);
					ImGui::Selectable("Icon", &config_system.g_cfg.esp.WEAPON_ICON, ImGuiSelectableFlags_DontClosePopups);
					ImGui::Selectable("Text", &config_system.g_cfg.esp.WEAPON_TEXT, ImGuiSelectableFlags_DontClosePopups);
					ImGui::Selectable("Ammo", &config_system.g_cfg.esp.WEAPON_AMMO, ImGuiSelectableFlags_DontClosePopups);
				}
				ImGui::EndGroup();
				ImGui::Spacing();

				ImGui::EndCombo();
			}padding(8, 2);

			if (config_system.g_cfg.esp.WEAPON_BOX)
			{
				ImGui::ColorEdit4(crypt_str("Weapon box color"), &config_system.g_cfg.esp.box_color, true, true); padding(8, 2);
			}
			if (config_system.g_cfg.esp.WEAPON_TEXT || config_system.g_cfg.esp.WEAPON_ICON)
			{
				ImGui::ColorEdit4(crypt_str("Weapon text color"), &config_system.g_cfg.esp.weapon_color, true, true); padding(8, 2);
			}
			if (config_system.g_cfg.esp.WEAPON_AMMO)
			{
				ImGui::ColorEdit4(crypt_str("Weapon ammo color"), &config_system.g_cfg.esp.weapon_ammo_color, true, true); padding(8, 2);
			}
			//ImGui::PlotLines();
			if (ImGui::BeginCombo(crypt_str("Grenade ESP"), "Choose")) // draw start
			{
				ImGui::Spacing();
				ImGui::BeginGroup();
				{


					ImGui::Selectable("Box", &config_system.g_cfg.esp.GRENADE_BOX, ImGuiSelectableFlags_DontClosePopups);
					ImGui::Selectable("Icon", &config_system.g_cfg.esp.GRENADE_ICON, ImGuiSelectableFlags_DontClosePopups);
					ImGui::Selectable("Text", &config_system.g_cfg.esp.GRENADE_TEXT, ImGuiSelectableFlags_DontClosePopups);
				}
				ImGui::EndGroup();
				ImGui::Spacing();

				ImGui::EndCombo();
			}padding(8, 2);
			if (config_system.g_cfg.esp.GRENADE_BOX)
			{
				ImGui::ColorEdit4(crypt_str("Grenade box color"), &config_system.g_cfg.esp.grenade_box_color, true, true); padding(8, 2);
			}

			if (config_system.g_cfg.esp.GRENADE_TEXT || config_system.g_cfg.esp.GRENADE_ICON)
			{
				ImGui::ColorEdit4("Grenade text color", &config_system.g_cfg.esp.projectiles_color, true, true); padding(8, 2);
			}
			if (config_system.g_cfg.esp.GRENADE_TRAIL)
			{
				ImGui::ColorEdit4("Grenade trail color", &config_system.g_cfg.esp.grenade_trail_color, true, true); padding(8, 2);
			}
			ImGui::Checkbox("Grenade helper", &config_system.g_cfg.grenadehelper.enable); padding(8, 2);

			ImGui::Combo("Choose the map", &map, "de_mirage\0de_inferno\0de_overpass\0de_dust2\0de_cache\0de_nuke\0de_cbble\0de_train"); padding(8, 2);
			if (map == 0)
			{
				static char pos_name[256] = "";
				ImGui::InputText("Put name here", pos_name, 256);
				std::string opt_pos_name = pos_name;

				padding(8, 2); if (ImGui::CustomButton("Add new pos", "##1"))
				{
					if (!m_engine()->IsInGame())
						return;

					mirage new_str;
					new_str.info = opt_pos_name;
					new_str.pos = g_ctx.local()->GetAbsOrigin();
					new_str.ang = g_ctx.local()->get_shoot_position();
					m_engine()->GetViewAngles(new_str.ang);

					config_system.g_cfg.grenadehelper.MirageGrenade.push_back(new_str);
				}
			}
			if (map == 1)
			{
				static char pos_name[256] = "";
				ImGui::InputText("Put name here", pos_name, 256);
				std::string opt_pos_name = pos_name;

				padding(8, 2);	if (ImGui::CustomButton("Add new pos", "##1"))
				{
					if (!m_engine()->IsInGame())
						return;

					inferno new_str;
					new_str.info = opt_pos_name;
					new_str.pos = g_ctx.local()->GetAbsOrigin();
					new_str.ang = g_ctx.local()->get_shoot_position();
					m_engine()->GetViewAngles(new_str.ang);

					config_system.g_cfg.grenadehelper.InfernoGrenade.push_back(new_str);
				}
			}

			if (map == 7)
			{
				static char pos_name[256] = "";
				ImGui::InputText("Put name here", pos_name, 256);
				std::string opt_pos_name = pos_name;

				padding(8, 2); if (ImGui::CustomButton("Add new pos", "##1"))
				{
					if (!m_engine()->IsInGame())
						return;

					train new_str;
					new_str.info = opt_pos_name;
					new_str.pos = g_ctx.local()->GetAbsOrigin();
					new_str.ang = g_ctx.local()->get_shoot_position();
					m_engine()->GetViewAngles(new_str.ang);

					config_system.g_cfg.grenadehelper.trainGrenade.push_back(new_str);
				}
			}

			if (map == 3)
			{
				static char pos_name[256] = "";
				ImGui::InputText("Put name here", pos_name, 256);
				std::string opt_pos_name = pos_name;

				padding(8, 2); if (ImGui::CustomButton("Add new pos", "##1"))
				{
					if (!m_engine()->IsInGame())
						return;

					dust2 new_str;
					new_str.info = opt_pos_name;
					new_str.pos = g_ctx.local()->GetAbsOrigin();
					new_str.ang = g_ctx.local()->get_shoot_position();
					m_engine()->GetViewAngles(new_str.ang);

					config_system.g_cfg.grenadehelper.dust2Grenade.push_back(new_str);
				}
			}
			if (map == 2)
			{
				static char pos_name[256] = "";
				ImGui::InputText("Put name here", pos_name, 256);
				std::string opt_pos_name = pos_name;

				padding(8, 2);	if (ImGui::CustomButton("Add new pos", "##1"))
				{
					if (!m_engine()->IsInGame())
						return;

					overpass new_str;
					new_str.info = opt_pos_name;
					new_str.pos = g_ctx.local()->GetAbsOrigin();
					new_str.ang = g_ctx.local()->get_shoot_position();
					m_engine()->GetViewAngles(new_str.ang);

					config_system.g_cfg.grenadehelper.overpassGrenade.push_back(new_str);
				}
			}
			if (map == 5)
			{
				static char pos_name[256] = "";
				ImGui::InputText("Put name here", pos_name, 256);
				std::string opt_pos_name = pos_name;

				padding(8, 2);	if (ImGui::CustomButton("Add new pos", "##1"))
				{
					if (!m_engine()->IsInGame())
						return;

					nuke new_str;
					new_str.info = opt_pos_name;
					new_str.pos = g_ctx.local()->GetAbsOrigin();
					new_str.ang = g_ctx.local()->get_shoot_position();
					m_engine()->GetViewAngles(new_str.ang);

					config_system.g_cfg.grenadehelper.nukeGrenade.push_back(new_str);
				}
			}
			if (map == 4)
			{
				static char pos_name[256] = "";
				ImGui::InputText("Put name here", pos_name, 256);
				std::string opt_pos_name = pos_name;

				padding(8, 2);	if (ImGui::CustomButton("Add new pos", "##1"))
				{
					if (!m_engine()->IsInGame())
						return;

					cache new_str;
					new_str.info = opt_pos_name;
					new_str.pos = g_ctx.local()->GetAbsOrigin();
					new_str.ang = g_ctx.local()->get_shoot_position();
					m_engine()->GetViewAngles(new_str.ang);

					config_system.g_cfg.grenadehelper.cacheGrenade.push_back(new_str);
				}
			}
			if (map == 6)
			{
				static char pos_name[256] = "";
				ImGui::InputText("Put name here", pos_name, 256);
				std::string opt_pos_name = pos_name;

				padding(8, 2);	if (ImGui::CustomButton("Add new pos", "##1"))
				{
					if (!m_engine()->IsInGame())
						return;

					cbble new_str;
					new_str.info = opt_pos_name;
					new_str.pos = g_ctx.local()->GetAbsOrigin();
					new_str.ang = g_ctx.local()->get_shoot_position();
					m_engine()->GetViewAngles(new_str.ang);

					config_system.g_cfg.grenadehelper.cbbleGrenade.push_back(new_str);
				}
			}

			ImGui::EndChild();
		}
		ImGui::Columns(1);
	}
}
using json = nlohmann::json;
CHAR my_documents[MAX_PATH];
HRESULT result = SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, my_documents);

constexpr auto& config_items = config_system.get_configs();
static int current_config = -1;

constexpr auto& config_items2 = config_system.get_configs2();
static int current_config2 = -1;
bool OpenConfig = false;
#include <iostream>
#include <string>
#include <fstream>
#include <streambuf>
#include <archivex.h>

using namespace std;

void c_menu::DrawConfig()
{
	if (subtab5 == 0)
	{
		ImGui::SetCursorPosX(7);

		ImGui::BeginChild("Config", ImVec2(585, 520), false);
		{

			padding(8, 8);

			auto same_line_counter = 0;
			for (int i = 1; i < config_items.size(); i++)
			{


				ImGui::BeginGroup();
				ImGui::NewLine(); ImGui::SameLine();

				ImGui::PushFont(c_menu::get().compart_font);
				if (ImGui::CircleButton("e", std::string(std::string("##compart_") + config_items[i]).c_str(), ImVec2(20, 20)))
				{
					/*std::string pathToLogin = std::string(my_documents) + "\\Dynamism.space\\Configs\\" + config_items[i].c_str();
					std::ifstream a(pathToLogin);

					while (a)
					{
						string str((istreambuf_iterator<char>(a)), istreambuf_iterator<char>());
						eventlogs::get().add(str.c_str(), true);
					}
					a.close();*/
					//CHAT::get().SendConfig(str.str());

				//	eventlogs::get().add(std::string("Shared ") + std::string(config_items[i]).c_str(), false); //-V106

				}
				ImGui::PopFont();
				ImGui::PushFont(c_menu::get().misc_font);
				ImGui::NewLine(); ImGui::SameLine();
				if (ImGui::CircleButton("E", std::string(std::string("##save_") + config_items[i]).c_str(), ImVec2(20, 20)))
				{
					config_system.save(i);

					eventlogs::get().add(std::string("Saved ") + std::string(config_items[i]), false); //-V106

				}

				ImGui::NewLine(); ImGui::SameLine();
				if (ImGui::CircleButton("F", std::string(std::string("##removed_") + config_items[i].c_str()).c_str(), ImVec2(20, 20)))
				{
					config_system.remove(i);
					eventlogs::get().add(std::string("Removed ") + std::string(config_items[i]), false); //-V106
				}
				ImGui::EndGroup();
				ImGui::PopFont();
				ImGui::SameLine(0, 2);

				if (ImGui::CustomButton(config_items[i].c_str(), std::string(std::string("##") + config_items[i].c_str()).c_str(), ImVec2(100, 100)))
				{
					config_system.load(i);
					eventlogs::get().add(std::string("Loaded ") + std::string(config_items[i]), false); //-V106
				}


				if (same_line_counter < 3) {
					ImGui::SameLine();
					same_line_counter++;
				}
				else {
					padding(8, 2);
					same_line_counter = 0;
				}

			}


			/*		char username[255];
					sprintf(username, "Welcome, %s", USERNAME);
					char uid[255];
					sprintf(uid, "Your uid: %s", userid.c_str());
					char expires[255];
					sprintf(expires, "Subscription ends: %s", expiration_date.c_str());
					char joined[255];
					sprintf(joined, "Joined: %s", joined_date.c_str());
					ImGui::NewLine();
					padding(8, 2); ImGui::Text(username); padding(8, 2);
					ImGui::Text(uid); padding(8, 2);
					ImGui::Text(expires); padding(8, 2);
					ImGui::Text(joined); padding(8, 2);
					ImGui::SetCursorPos(ImVec2(195, 10));*/

					//if (ImGui::CustomButton(("Create"), "##1", ImVec2(80, 18))) {
					//	config_system.add(buffer);
					//}


					//if (current_config != -1)
					//{
					//	ImGui::SetCursorPos(ImVec2(195, 30));


					//	if (ImGui::CustomButton(("Load Config"), "##2", ImVec2(80, 18))) {
					//		config_system.load(current_config);
					//	}
					//	ImGui::SetCursorPos(ImVec2(195, 50));

					//	if (ImGui::CustomButton(("Save Config"), "##3", ImVec2(80, 18))) {
					//		config_system.save(current_config);
					//	}
					//	ImGui::SetCursorPos(ImVec2(195, 70));

					//	if (ImGui::CustomButton(("Remove Config"), "##4", ImVec2(80, 18))) {
					//		config_system.remove(current_config);
					//	}

					//}

		}
		ImGui::EndChild();
		static char buffer[16];
		ImGui::SetCursorPos({ 7, 25 });
		ImGui::PushStyleColor(ImGuiCol_FrameBg, config_system.g_cfg.menu.menu_theme == 1 ? ImVec4(34 / 255.f, 35 / 255.f, 39 / 255.f, 1.f) : ImVec4(45 / 255.f, 49 / 255.f, 60 / 255.f, 0.55f));
		if (ImGui::InputText("Put config name ( press enter to create )", buffer, IM_ARRAYSIZE(buffer), ImGuiInputTextFlags_EnterReturnsTrue)) {
			config_system.add(buffer);
		}
		ImGui::PopStyleColor();
		ImGui::SetCursorPos({ 535, 5 });

		ImGui::PushFont(c_menu::get().arrow_font);

		if (ImGui::CircleButton("C", "##SAVE_ARROW", ImVec2(40, 40)))
		{
			config_system.get_configs() = { "" };
			config_system.run(buffer);
		}

		ImGui::PopFont();
	}

}


void c_menu::DrawLegitTab()
{
	static int WeaponGroup = hooks::legit_weapon;
	ImGui::Columns(2, NULL, false);
	ImGui::NewLine();
	ImGui::BeginChild("Legit", ImVec2(280, 300), true);
	padding(8, 8);
	ImGui::Checkbox("Enable", &config_system.g_cfg.legitbot.enabled); padding(8, 2);

	ImGui::SameLine(); draw_keybind("", &config_system.g_cfg.legitbot.key, "##LEGITKEY"); padding(8, 2);
	ImGui::Checkbox("Auto pistol", &config_system.g_cfg.legitbot.autopistol); padding(8, 2);

	//ImGui::Combo("Aim mode", &config_system.g_cfg.legitbot.legitmode, "Simple\0Advancede\0"); padding(8, 2);
	ImGui::Separator(); padding(8, 2);
	ImGui::Checkbox("Friendly fire", &config_system.g_cfg.legitbot.friendly_fire); padding(8, 2);
	ImGui::Checkbox("Smoke check", &config_system.g_cfg.legitbot.do_if_enemy_in_smoke); padding(8, 2);
	ImGui::Checkbox("Flash check ", &config_system.g_cfg.legitbot.do_if_local_flashed); padding(8, 2);
	ImGui::Checkbox("Jump check", &config_system.g_cfg.legitbot.do_if_local_in_air); padding(8, 2);
	ImGui::Separator(); padding(8, 2);
	ImGui::Checkbox("Auto stop", &config_system.g_cfg.legitbot.weapon[WeaponGroup].auto_stop); padding(8, 2);
	ImGui::Checkbox("Silent Aim", &config_system.g_cfg.legitbot.weapon[WeaponGroup].silent); padding(8, 2);

	ImGui::Checkbox("Draw fov", &config_system.g_cfg.legitbot.draw_fov); padding(8, 2);

	ImGui::EndChild(); ImGui::NewLine();
	ImGui::BeginChild("RCS", ImVec2(280, 200), true);
	padding(8, 16);
	ImGui::SliderFloat("RCS Standalone X", &config_system.g_cfg.legitbot.weapon[WeaponGroup].rcs, 0, 100); padding(8, 10);
	ImGui::SliderFloat("RCS Standalone Y", &config_system.g_cfg.legitbot.weapon[WeaponGroup].custom_rcs_fov, 0, 100); padding(8, 10);

	ImGui::EndChild();
	const char* hitbox_legit[2] = { crypt_str("Nearest"), crypt_str("Hitscan") };
	ImGui::NextColumn();
	{
		ImGui::NewLine();
		ImGui::BeginChild("Weapons", ImVec2(280, 300), true);
		{
			padding(8, 8);
			ImGui::PushFont(c_menu::get().ico_menu);
			ImGui::PushStyleColor(ImGuiCol_ChildBg, config_system.g_cfg.menu.menu_theme == 1 ? ImVec4(34 / 255.f, 35 / 255.f, 39 / 255.f, 0.55f) : ImVec4(45 / 255.f, 49 / 255.f, 60 / 255.f, 0.55f));

			ImGui::BeginChild(":D", ImVec2(260, 50));
			{
				if (ImGui::BlssButton("G", "", c_menu::get().futura_small, WeaponGroup == 1 ? true : false, ImVec2(55, 50))) WeaponGroup = 1; ImGui::SameLine(0, 0);
				if (ImGui::BlssButton("W", "", c_menu::get().futura_small, WeaponGroup == 2 ? true : false, ImVec2(55, 50))) WeaponGroup = 2; ImGui::SameLine(0, 0);
				if (ImGui::BlssButton("Z", "", c_menu::get().futura_small, WeaponGroup == 4 ? true : false, ImVec2(56, 50))) WeaponGroup = 4; ImGui::SameLine(0, 0);
				if (ImGui::BlssButton("L", "", c_menu::get().futura_small, WeaponGroup == 3 ? true : false, ImVec2(50, 50))) WeaponGroup = 3; ImGui::SameLine(0, 0);
				if (ImGui::BlssButton("b", "", c_menu::get().futura_small, WeaponGroup == 5 ? true : false, ImVec2(55, 50))) WeaponGroup = 5; ImGui::SameLine(0, 0);
			}
			ImGui::EndChild();
			ImGui::PopFont();
			ImGui::PopStyleColor(); padding(8, 12);
			ImGui::SliderFloat("Fov", &config_system.g_cfg.legitbot.weapon[WeaponGroup].fov, 0.0f, 20.0f, "%.2f"); padding(8, 12);
			ImGui::SliderFloat("Smooth", &config_system.g_cfg.legitbot.weapon[WeaponGroup].smooth, 1, 40, "%.2f"); padding(8, 2);
			ImGui::Separator(); padding(8, 2);
			draw_combo(crypt_str("Hitbox mode"), config_system.g_cfg.legitbot.weapon[hooks::legit_weapon].priority, hitbox_legit, ARRAYSIZE(hitbox_legit)); padding(8, 2);
			if (config_system.g_cfg.legitbot.weapon[hooks::legit_weapon].priority == 1)
			{
				ImGui::Combo("Hitbox", &config_system.g_cfg.legitbot.weapon[WeaponGroup].autofire_hitchance, "Head\0Neck\0Chest\0Pelvis\0Legs\0"); padding(8, 2);
			}
			ImGui::Separator(); padding(8, 2);
			ImGui::SliderInt("Kill delay", &config_system.g_cfg.legitbot.weapon[WeaponGroup].target_switch_delay, 0, 300, "%.3f ms"); padding(8, 2);
		}

		ImGui::EndChild(); ImGui::NewLine();
		ImGui::BeginChild("Trigger", ImVec2(280, 200), true);
		padding(8, 8);
		ImGui::Checkbox("Enable Trigger", &config_system.g_cfg.triggerbot.trigger_enable); padding(8, 2);
		if (config_system.g_cfg.triggerbot.trigger_enable) {
			ImGui::SameLine();  draw_keybind("", &config_system.g_cfg.triggerbot.trigger_key, "##TRIGERKEY"); padding(8, 2);
			ImGui::Separator(); padding(8, 2);
			ImGui::Checkbox("Head", &config_system.g_cfg.triggerbot.trigger_hitbox_head); padding(8, 2);
			ImGui::Checkbox("Body", &config_system.g_cfg.triggerbot.trigger_hitbox_body); padding(8, 2);
			ImGui::Checkbox("Arms", &config_system.g_cfg.triggerbot.trigger_hitbox_arms); padding(8, 2);
			ImGui::Checkbox("Legs", &config_system.g_cfg.triggerbot.trigger_hitbox_legs); padding(8, 2);
			ImGui::Separator(); padding(8, 2);

			ImGui::SliderInt("Trigger delay", &config_system.g_cfg.triggerbot.trigger_delay, 1, 50); padding(8, 12);

			ImGui::SliderFloat("Trigger hitchance", &config_system.g_cfg.triggerbot.trigger_hitchance, 1, 100, "%.2f"); padding(8, 2);

			ImGui::Separator(); padding(8, 2);
			ImGui::Checkbox("Trigger recoil", &config_system.g_cfg.triggerbot.trigger_recoil); padding(8, 2);

			ImGui::Checkbox("Smoke check", &config_system.g_cfg.triggerbot.triggercheck); padding(8, 2);
			ImGui::Checkbox("Flash check", &config_system.g_cfg.triggerbot.triggercheck2); padding(8, 2);
			ImGui::Checkbox("Jump check", &config_system.g_cfg.triggerbot.triggercheck3); padding(8, 2);
			ImGui::NewLine();
		}
		ImGui::EndChild();
	}
	ImGui::Columns(1);
}

std::vector<std::tuple<int, int>> SkinToMenu;
std::vector<std::tuple<int, int>> GloveToMenu;
std::vector<std::tuple<int, int>> KnifeToMenu;


struct GlovesSkins_s
{
	int ItemIndex;
	int PaintKit;
};
GlovesSkins_s GlovesSkin_Array[] =
{
{ 5030,10018 },
{ 5030,10037 },
{ 5030,10038 },
{ 5030,10019 },
{ 5030,10048 },
{ 5030,10047 },
{ 5030,10046 },
{ 5030,10045 },

{ 5033,10024 },
{ 5033,10026 },
{ 5033,10027 },
{ 5033,10028 },
{ 5033,10052 },
{ 5033,10051 },
{ 5033,10050 },
{ 5033,10049 },

{ 5034,10033 },
{ 5034,10034 },
{ 5034,10035 },
{ 5034,10030 },
{ 5034,10064 },
{ 5034,10063 },
{ 5034,10062 },
{ 5034,10061 },

{ 5031,10013 },
{ 5031,10015 },
{ 5031,10016 },
{ 5031,10040 },
{ 5031,10044 },
{ 5031,10043 },
{ 5031,10042 },
{ 5031,10041 },

{ 5032,10009},
{ 5032,10010},
{ 5032,10036},
{ 5032,10021},
{ 5032,10056},
{ 5032,10055},
{ 5032,10054},
{ 5032,10053},

{ 5027,10006 },
{ 5027,10007 },
{ 5027,10008 },
{ 5027,10039 },

{ 5035,10057 },
{ 5035,10058 },
{ 5035,10059 },
{ 5035,10060 },
};

const char* GetWeaponNameById(int id)
{
	switch (id)
	{
	case 1:
		return "deagle";
	case 2:
		return "elite";
	case 3:
		return "fiveseven";
	case 4:
		return "glock";
	case 7:
		return "ak47";
	case 8:
		return "aug";
	case 9:
		return "awp";
	case 10:
		return "famas";
	case 11:
		return "g3sg1";
	case 13:
		return "galilar";
	case 14:
		return "m249";
	case 60:
		return "m4a1_silencer";
	case 16:
		return "m4a1";
	case 17:
		return "mac10";
	case 19:
		return "p90";
	case 23:
		return "mp5sd";
	case 24:
		return "ump45";
	case 25:
		return "xm1014";
	case 26:
		return "bizon";
	case 27:
		return "mag7";
	case 28:
		return "negev";
	case 29:
		return "sawedoff";
	case 30:
		return "tec9";
	case 32:
		return "hkp2000";
	case 33:
		return "mp7";
	case 34:
		return "mp9";
	case 35:
		return "nova";
	case 36:
		return "p250";
	case 38:
		return "scar20";
	case 39:
		return "sg556";
	case 40:
		return "ssg08";
	case 61:
		return "usp_silencer";
	case 63:
		return "cz75a";
	case 64:
		return "revolver";
	case 508:
		return "knife_m9_bayonet";
	case 500:
		return "bayonet";
	case 505:
		return "knife_flip";
	case 506:
		return "knife_gut";
	case 507:
		return "knife_karambit";
	case 509:
		return "knife_tactical";
	case 512:
		return "knife_falchion";
	case 514:
		return "knife_survival_bowie";
	case 515:
		return "knife_butterfly";
	case 516:
		return "knife_push";

	case 519:
		return "knife_ursus";
	case 520:
		return "knife_gypsy_jackknife";
	case 522:
		return "knife_stiletto";
	case 523:
		return "knife_widowmaker";

	case WEAPON_KNIFE_CSS:
		return "knife_css";

	case WEAPON_KNIFE_CORD:
		return "knife_cord";

	case WEAPON_KNIFE_CANIS:
		return "knife_canis";

	case WEAPON_KNIFE_OUTDOOR:
		return "knife_outdoor";

	case WEAPON_KNIFE_SKELETON:
		return "knife_skeleton";

	case 5027:
		return "studded_bloodhound_gloves";
	case 5028:
		return "t_gloves";
	case 5029:
		return "ct_gloves";
	case 5030:
		return "sporty_gloves";
	case 5031:
		return "slick_gloves";
	case 5032:
		return "leather_handwraps";
	case 5033:
		return "motorcycle_gloves";
	case 5034:
		return "specialist_gloves";
	case 5035:
		return "studded_hydra_gloves";

	default:
		return "";
	}
}

const char* GetWeaponName(int id)
{
	switch (id)
	{
	case 1:
		return "Desert Eagle";
	case 2:
		return "Dual Berettas";
	case 3:
		return "Five-SeveN";
	case 4:
		return "Glock-18";
	case 7:
		return "AK-47";
	case 8:
		return "AUG";
	case 9:
		return "AWP";
	case 10:
		return "FAMAS";
	case 11:
		return "G3SG1";
	case 13:
		return "Galil AR";
	case 14:
		return "M249";
	case 60:
		return "M4A1-S";
	case 16:
		return "M4A4";
	case 17:
		return "MAC-10";
	case 19:
		return "P90";
	case 23:
		return "MP5-SD";
	case 24:
		return "UMP-45";
	case 25:
		return "XM1014";
	case 26:
		return "PP-Bizon";
	case 27:
		return "MAG-7";
	case 28:
		return "Negev";
	case 29:
		return "Sawed-Off";
	case 30:
		return "Tec-9";
	case 32:
		return "P2000";
	case 33:
		return "MP7";
	case 34:
		return "MP9";
	case 35:
		return "Nova";
	case 36:
		return "p250";
	case 38:
		return "scar20";
	case 39:
		return "SG 553";
	case 40:
		return "SSG 08";
	case 61:
		return "USP-S";
	case 63:
		return "CZ75 Auto";
	case 64:
		return "R8 Revolver";
	case 508:
		return "M9 Bayonet";
	case 500:
		return "Bayonet";
	case 505:
		return "Flip Knife";
	case 506:
		return "Gut Knife";
	case 507:
		return "Karambit";
	case 509:
		return "Huntsman Knife";
	case 512:
		return "Falchion Knife";
	case 514:
		return "Bowie Knife";
	case 515:
		return "Butterfly Knife";
	case 516:
		return "Shadow Daggers";
	case 519:
		return "Ursus Knife";
	case 520:
		return "Navaja Knife";
	case 522:
		return "Stiletto Knife";
	case 523:
		return "Talon Knife";

	case WEAPON_KNIFE_CSS:
		return "Classic Knife";

	case WEAPON_KNIFE_CORD:
		return "Paracord Knife";

	case WEAPON_KNIFE_CANIS:
		return "Survival Knife";

	case WEAPON_KNIFE_OUTDOOR:
		return "Nomad Knife";

	case WEAPON_KNIFE_SKELETON:
		return "Skeleton Knife";
	case 5027:
		return "studded_bloodhound_gloves";
	case 5028:
		return "t_gloves";
	case 5029:
		return "ct_gloves";
	case 5030:
		return "sporty_gloves";
	case 5031:
		return "slick_gloves";
	case 5032:
		return "leather_handwraps";
	case 5033:
		return "motorcycle_gloves";
	case 5034:
		return "specialist_gloves";
	case 5035:
		return "studded_hydra_gloves";

	default:
		return "";
	}
}

void c_menu::DrawSkinTab()
{

	static auto GetColRar = [&](int rar) -> ImVec4 {
		switch (rar)
		{
		case 1:
			return ImVec4(150 / 255.f, 150 / 255.f, 150 / 255.f, 255 / 255.f);
			break;
		case 2:
			return ImVec4(100 / 255.f, 100 / 255.f, 255 / 255.f, 255 / 255.f);
			break;
		case 3:
			return ImVec4(50 / 255.f, 50 / 255.f, 255 / 255.f, 255 / 255.f);
			break;
		case 4:
			return ImVec4(255 / 255.f, 64 / 255.f, 242 / 255.f, 255 / 255.f);
			break;
		case 5:
			return ImVec4(255 / 255.f, 50 / 255.f, 50 / 255.f, 255 / 255.f);
			break;
		case 6:
			return ImVec4(255 / 255.f, 50 / 255.f, 50 / 255.f, 255 / 255.f);
			break;
		case 7:
			return ImVec4(255 / 255.f, 196 / 255.f, 46 / 255.f, 255 / 255.f);
			break;
		default:
			return ImVec4(150 / 255.f, 150 / 255.f, 150 / 255.f, 255 / 255.f);
			break;
		}
	};

	static bool need_update = false;
	auto same_line_counter = 0;
	std::string SkinName[65];
	std::string SkinToMenu[65];
	static int gloves = 0;

	ImGui::SetCursorPosX(7);

	ImGui::PushStyleColor(ImGuiCol_ChildBg, config_system.g_cfg.menu.menu_theme == 1 ? ImVec4(34 / 255.f, 35 / 255.f, 39 / 255.f, 0.55f) : ImVec4(45 / 255.f, 49 / 255.f, 60 / 255.f, 0.55f));
	ImGui::BeginChild("Skin changer", ImVec2(585, 520), false);
	{
		padding(8, 8);
		if (subtab7 == 0)
		{
			if (!need_update)
			{
				if (current_profile == -1)
				{
					for (auto i = 1; i < skins_weapon_count; i++)
					{
						std::string skinName = GetWeaponNameById(gun_definitioni[i]);
						std::string skinName2 = GetWeaponName(gun_definitioni[i]);


						if (!all_skins[i])
						{

							all_skins[i] = get_skin_preview(std::string(std::string("weapon_").c_str() + skinName).c_str(), "", device);
						}
						ImGui::PushStyleColor(ImGuiCol_Button, config_system.g_cfg.menu.menu_theme == 1 ? ImVec4(34 / 255.f, 35 / 255.f, 39 / 255.f, 1.f) : ImVec4(45 / 255.f, 49 / 255.f, 60 / 255.f, 1.f));
						ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));
						ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0));
						ImGui::BeginGroup();

						if (ImGui::ImageButton(all_skins[i], ImVec2(100 * dpi_scale, 76 * dpi_scale))) {
							current_profile = i;

						}
						ImGui::Text(skinName2.c_str());
						ImGui::EndGroup();
						ImGui::PopStyleColor(3);

						if (same_line_counter < 4) {
							ImGui::SameLine();
							same_line_counter++;
						}
						else {
							padding(8, 2);
							same_line_counter = 0;
						}

					}
					for (int i = skins_weapon_count; i < skins_weapon_count + skins_knife_count; ++i)
					{
						auto& selected_entry = config_system.g_cfg.skins.skinChanger[WEAPON_KNIFE];
						selected_entry.itemIdIndex = WEAPON_KNIFE;

						std::string skinName = GetWeaponNameById(gun_definitioni[i]);
						std::string skinName2 = GetWeaponName(gun_definitioni[i]);

						if (!skin_knife[i])
						{
							skin_knife[i] = get_skin_preview(std::string(std::string("weapon_").c_str() + skinName).c_str(), "", device);
						}
						ImGui::PushStyleColor(ImGuiCol_Button, config_system.g_cfg.menu.menu_theme == 1 ? ImVec4(34 / 255.f, 35 / 255.f, 39 / 255.f, 1.f) : ImVec4(45 / 255.f, 49 / 255.f, 60 / 255.f, 1.f));
						ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));
						ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0));
						ImGui::BeginGroup();
						if (ImGui::ImageButton(skin_knife[i], ImVec2(100 * dpi_scale, 76 * dpi_scale))) {
							current_profile = i;
							selected_entry.definition_override_vector_index = gun_definitioni[current_profile];
							SkinChanger::scheduleHudUpdate();
							config_system.g_cfg.skins.skinChanger[WEAPON_KNIFE].update();

						}
						ImGui::Text(skinName2.c_str());
						ImGui::EndGroup();
						ImGui::PopStyleColor(3);

						if (same_line_counter < 4) {
							ImGui::SameLine();
							same_line_counter++;
						}
						else {
							padding(8, 2);
							same_line_counter = 0;
						}
					}

				}
				else
				{


					for (int i = 0; i < gunLen[current_profile] && current_profile != 0; ++i) {

						auto& selected_entry = config_system.g_cfg.skins.skinChanger[current_profile >= 35 ? WEAPON_KNIFE : gun_definitioni[current_profile]];
						selected_entry.itemIdIndex = current_profile >= 35 ? WEAPON_KNIFE : gun_definitioni[current_profile];

						if (gunLen[current_profile] > i) {
							static std::string validFname;
							std::string skinName = GetWeaponNameById(gun_definitioni[current_profile]);
							static int skin = 0;
							ImVec4 color;
							for (auto s : g_SkinData.weaponSkins[skinName]) {
								auto gg = g_SkinData.skinMap[s];
								if (gg.paintkit == vskns[current_profile][i]) {
									validFname = s;
									skin = gg.paintkit;
									color = current_profile >= 35 ? GetColRar(6) : GetColRar(gg.rarity);
									SkinName[i] = g_SkinData.skinNames[gg.tagName];

									break;
								}
							}

							if (!(alltextures[current_profile])[i])
							{
								(alltextures[current_profile])[i] = get_skin_preview(std::string(std::string("weapon_").c_str() + skinName).c_str(), validFname.c_str(), device);
							}
							ImGui::PushStyleColor(ImGuiCol_Button, config_system.g_cfg.menu.menu_theme == 1 ? ImVec4(34 / 255.f, 35 / 255.f, 39 / 255.f, 1.f) : ImVec4(45 / 255.f, 49 / 255.f, 60 / 255.f, 1.f));
							ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));
							ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0));

							ImGui::BeginGroup();
							if (ImGui::ImageButton((alltextures[current_profile])[i], ImVec2(100 * dpi_scale, 76 * dpi_scale), true, color)) {

								SkinChanger::scheduleHudUpdate();
								config_system.g_cfg.skins.skinChanger[current_profile >= 35 ? WEAPON_KNIFE : gun_definitioni[current_profile]].update();

								selected_entry.paintKit = skin;
								need_update = true;

							}


							ImGui::Text(SkinName[i].c_str());
							ImGui::EndGroup();
							ImGui::PopStyleColor(3);
							if (same_line_counter < 4) {
								ImGui::SameLine();
								same_line_counter++;
							}
							else {
								padding(8, 2);
								same_line_counter = 0;
							}
						}
					}
				}
			}
			else
			{
				ImGui::Columns(2, NULL, false);
				auto& selected_entry = config_system.g_cfg.skins.skinChanger[current_profile >= 35 ? WEAPON_KNIFE : gun_definitioni[current_profile]];
				selected_entry.itemIdIndex = current_profile >= 35 ? WEAPON_KNIFE : gun_definitioni[current_profile];
				std::string skinName = GetWeaponNameById(gun_definitioni[current_profile]);
				static std::string validFname;
				ImVec4 color;
				for (auto s : g_SkinData.weaponSkins[skinName]) {
					auto gg = g_SkinData.skinMap[s];
					if (gg.paintkit == selected_entry.paintKit) {
						validFname = s;
						color = current_profile >= 35 ? GetColRar(6) : GetColRar(gg.rarity);
						break;
					}
				}

				if (!skins_to_edit || skins_to_edit == nullptr)
				{
					skins_to_edit = get_skin_preview(std::string(std::string("weapon_").c_str() + skinName).c_str(), validFname.c_str(), device);
				}
				ImGui::BeginChild("##Skin", ImVec2(280, 500), false);
				padding(8, 8);
				ImGui::PushStyleColor(ImGuiCol_Button, config_system.g_cfg.menu.menu_theme == 1 ? ImVec4(34 / 255.f, 35 / 255.f, 39 / 255.f, 1.f) : ImVec4(45 / 255.f, 49 / 255.f, 60 / 255.f, 1.f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0));

				ImGui::ImageButton(skins_to_edit, ImVec2(260, 200), true, color);
				ImGui::PopStyleColor(3);
				if (current_profile < 35)
				{
					auto& selected_sticker0 = selected_entry.stickers[0];
					auto& selected_sticker1 = selected_entry.stickers[1];
					auto& selected_sticker2 = selected_entry.stickers[2];
					auto& selected_sticker3 = selected_entry.stickers[3];
				}
				auto same_line = 0;
				ImGui::SetNextWindowSize(ImVec2(275, 200));
				if (ImGui::BeginPopup("##ADDSTICKER1"))
				{
					auto& selected_sticker = selected_entry.stickers[0];

					ImGui::NewLine();
					padding(8, 8);
					for (int i = 0; i < game_data::sticker_kits.size(); i++)
					{
						const auto& stickerObject = game_data::sticker_kits.at(i);

						if (!mapCachedImages[i] || mapCachedImages[i] == nullptr)
						{
							mapCachedImages[i] = get_sticker(stickerObject.szImagePath.c_str(), device);
						}

						ImGui::PushStyleColor(ImGuiCol_Button, config_system.g_cfg.menu.menu_theme == 1 ? ImVec4(34 / 255.f, 35 / 255.f, 39 / 255.f, 1.f) : ImVec4(45 / 255.f, 49 / 255.f, 60 / 255.f, 1.f));
						ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));
						ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0));

						if (mapCachedImages[i] != nullptr)
							if (ImGui::ImageButton(mapCachedImages[i], ImVec2(50, 40), true, GetColRar(stickerObject.rarity)))
							{
								SkinChanger::scheduleHudUpdate();
								selected_sticker.kit = stickerObject.id;
								selected_sticker.CircleButton1 = mapCachedImages[i];
							}
						ImGui::PopStyleColor(3);

						if (same_line_counter < 4) {
							ImGui::SameLine();
							same_line_counter++;
						}
						else {
							padding(8, 2);
							same_line_counter = 0;
						}
					}

					ImGui::EndPopup();
				}
				ImGui::SetNextWindowSize(ImVec2(275, 200));
				if (ImGui::BeginPopup("##ADDSTICKER2"))
				{
					auto& selected_sticker = selected_entry.stickers[1];

					ImGui::NewLine();
					padding(8, 8);
					for (int i = 0; i < game_data::sticker_kits.size(); i++)
					{
						const auto& stickerObject = game_data::sticker_kits.at(i);

						if (!mapCachedImages[i] || mapCachedImages[i] == nullptr)
						{
							mapCachedImages[i] = get_sticker(stickerObject.szImagePath.c_str(), device);
						}

						ImGui::PushStyleColor(ImGuiCol_Button, config_system.g_cfg.menu.menu_theme == 1 ? ImVec4(34 / 255.f, 35 / 255.f, 39 / 255.f, 1.f) : ImVec4(45 / 255.f, 49 / 255.f, 60 / 255.f, 1.f));
						ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));
						ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0));

						if (mapCachedImages[i] != nullptr)
							if (ImGui::ImageButton(mapCachedImages[i], ImVec2(50, 40), true, GetColRar(stickerObject.rarity)))
							{
								SkinChanger::scheduleHudUpdate();
								selected_sticker.kit = stickerObject.id;
								selected_sticker.CircleButton1 = mapCachedImages[i];
							}
						ImGui::PopStyleColor(3);

						if (same_line_counter < 4) {
							ImGui::SameLine();
							same_line_counter++;
						}
						else {
							padding(8, 2);
							same_line_counter = 0;
						}
					}

					ImGui::EndPopup();
				}
				ImGui::SetNextWindowSize(ImVec2(275, 200));
				if (ImGui::BeginPopup("##ADDSTICKER3"))
				{
					auto& selected_sticker = selected_entry.stickers[2];

					ImGui::NewLine();
					padding(8, 8);
					for (int i = 0; i < game_data::sticker_kits.size(); i++)
					{
						const auto& stickerObject = game_data::sticker_kits.at(i);

						if (!mapCachedImages[i] || mapCachedImages[i] == nullptr)
						{
							mapCachedImages[i] = get_sticker(stickerObject.szImagePath.c_str(), device);
						}

						ImGui::PushStyleColor(ImGuiCol_Button, config_system.g_cfg.menu.menu_theme == 1 ? ImVec4(34 / 255.f, 35 / 255.f, 39 / 255.f, 1.f) : ImVec4(45 / 255.f, 49 / 255.f, 60 / 255.f, 1.f));
						ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));
						ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0));

						if (mapCachedImages[i] != nullptr)
							if (ImGui::ImageButton(mapCachedImages[i], ImVec2(50, 40), true, GetColRar(stickerObject.rarity)))
							{
								SkinChanger::scheduleHudUpdate();
								selected_sticker.kit = stickerObject.id;
								selected_sticker.CircleButton1 = mapCachedImages[i];
							}
						ImGui::PopStyleColor(3);

						if (same_line_counter < 4) {
							ImGui::SameLine();
							same_line_counter++;
						}
						else {
							padding(8, 2);
							same_line_counter = 0;
						}
					}

					ImGui::EndPopup();
				}
				ImGui::SetNextWindowSize(ImVec2(275, 200));
				if (ImGui::BeginPopup("##ADDSTICKER4"))
				{
					auto& selected_sticker = selected_entry.stickers[3];

					ImGui::NewLine();
					padding(8, 8);
					for (int i = 0; i < game_data::sticker_kits.size(); i++)
					{
						const auto& stickerObject = game_data::sticker_kits.at(i);

						if (!mapCachedImages[i] || mapCachedImages[i] == nullptr)
						{
							mapCachedImages[i] = get_sticker(stickerObject.szImagePath.c_str(), device);
						}

						ImGui::PushStyleColor(ImGuiCol_Button, config_system.g_cfg.menu.menu_theme == 1 ? ImVec4(34 / 255.f, 35 / 255.f, 39 / 255.f, 1.f) : ImVec4(45 / 255.f, 49 / 255.f, 60 / 255.f, 1.f));
						ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));
						ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0));

						if (mapCachedImages[i] != nullptr)
							if (ImGui::ImageButton(mapCachedImages[i], ImVec2(50, 40), true, GetColRar(stickerObject.rarity)))
							{
								SkinChanger::scheduleHudUpdate();
								selected_sticker.kit = stickerObject.id;
								selected_sticker.CircleButton1 = mapCachedImages[i];
							}
						ImGui::PopStyleColor(3);

						if (same_line_counter < 4) {
							ImGui::SameLine();
							same_line_counter++;
						}
						else {
							padding(8, 2);
							same_line_counter = 0;
						}
					}

					ImGui::EndPopup();
				}
				ImGui::EndChild();
				ImGui::NextColumn();
				ImGui::BeginChild("##Config", ImVec2(280, 500), false);
				bool drugs = false;
				padding(8, 8);
				if (ImGui::InputText(crypt_str("Name-Tag"), selected_entry.custom_name, sizeof(selected_entry.custom_name)))
				{
					config_system.g_cfg.skins.custom_name_tag[gun_definitioni[current_profile]] = selected_entry.custom_name;
					SkinChanger::scheduleHudUpdate();
				}
				padding(8, 8);
				if (ImGui::InputInt(crypt_str("Seed"), &selected_entry.seed, 1, 100))
					SkinChanger::scheduleHudUpdate();
				padding(8, 8);
				if (ImGui::InputInt(crypt_str("StatTrak"), &selected_entry.stat_trak, 1, 15))
					SkinChanger::scheduleHudUpdate();
				padding(8, 8);
				if (ImGui::SliderFloat(crypt_str("Wear"), &selected_entry.wear, 0, 1, "%.8f"))
					drugs = true;

				else if (drugs)
				{
					SkinChanger::scheduleHudUpdate();
					drugs = false;
				}

				ImGui::EndChild();
				ImGui::Columns(1);
			}
		}
		else
		{
			for (int i = skins_weapon_count + skins_knife_count; i < skinsCount; ++i)
			{
				for (int a = 0; a < gunLen[i] && i != 0; ++a) {
					auto& selected_entry = config_system.g_cfg.skins.skinChanger[99];
					selected_entry.itemIdIndex = 99;
					static std::string validFname;
					std::string skinName = GetWeaponNameById(gun_definitioni[i]);
					static int skin = 0;
					ImVec4 color;
					for (auto s : g_SkinData.weaponSkins[skinName]) {
						auto gg = g_SkinData.skinMap[s];
						if (gg.paintkit == vskns[i][a]) {
							validFname = s;
							skin = gg.paintkit;
							color = a >= 35 ? GetColRar(6) : GetColRar(gg.rarity);
							SkinName[a] = g_SkinData.skinNames[gg.tagName];
							break;
						}
					}
					if (!(alltextures[i])[a])
					{
						(alltextures[i])[a] = get_skin_preview(std::string(skinName).c_str(), validFname.c_str(), device);
					}
					if (gunLen[i] > a) {


						ImGui::PushStyleColor(ImGuiCol_Button, config_system.g_cfg.menu.menu_theme == 1 ? ImVec4(34 / 255.f, 35 / 255.f, 39 / 255.f, 1.f) : ImVec4(45 / 255.f, 49 / 255.f, 60 / 255.f, 1.f));
						ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));
						ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0));
						ImGui::BeginGroup();
						if (ImGui::ImageButton((alltextures[i])[a], ImVec2(100 * dpi_scale, 76 * dpi_scale), true, color)) {
							SkinChanger::scheduleHudUpdate();
							config_system.g_cfg.skins.skinChanger[99].update();
							selected_entry.definition_override_index = gun_definitioni[i];
							selected_entry.paintKit = skin;
						}
						ImGui::Text(SkinName[a].c_str());
						ImGui::EndGroup();
						ImGui::PopStyleColor(3);
						if (same_line_counter < 4) {
							ImGui::SameLine();
							same_line_counter++;
						}
						else {
							padding(8, 2);
							same_line_counter = 0;
						}
					}
				}
			}

		}
	}
	ImGui::EndChild();
	if (subtab7 == 0)
	{
		if (!need_update)
		{
			if (current_profile > -1)
			{
				ImGui::SetCursorPos({ 535, 597 });
				ImGui::PushFont(c_menu::get().arrow_font);
				if (ImGui::CircleButton("B", "##SKINBUTTON_ARROW", ImVec2(40, 40)))
				{
					current_profile = -1;
				}
				ImGui::PopFont();
			}
		}
		else
		{
			ImGui::SetCursorPos({ 535, 597 });
			ImGui::PushFont(c_menu::get().arrow_font);
			if (ImGui::CircleButton("B", "##SKINBUTTON_ARROW", ImVec2(40, 40)))
			{
				skins_to_edit = nullptr;
				current_profile = -1;
				need_update = false;
			}
			ImGui::PopFont();
		}
	}

}

void c_menu::DrawMiscTab()
{
	if (subtab6 == 0)
	{
		ImGui::Columns(2, NULL, false);
		ImGui::NewLine();
		ImGui::BeginChild("Misc", ImVec2(280, 280), true);
		padding(8, 8);
		ImGui::Checkbox(crypt_str("Anti-untrusted"), &config_system.g_cfg.misc.anti_untrusted); padding(8, 2);
		ImGui::Checkbox(crypt_str("Rank reveal"), &config_system.g_cfg.misc.rank_reveal); padding(8, 2);
		ImGui::Checkbox(crypt_str("Watermark"), &config_system.g_cfg.menu.watermark); padding(8, 2);
		ImGui::Checkbox(crypt_str("Spectators list"), &config_system.g_cfg.misc.spectatos); padding(8, 2);
		ImGui::Checkbox(crypt_str("Preserve killfeed"), &config_system.g_cfg.esp.preserve_killfeed); padding(8, 2);
		ImGui::Checkbox(crypt_str("Indicators"), &config_system.g_cfg.misc.indicators); padding(8, 2);
		ImGui::Checkbox(crypt_str("Key list"), &config_system.g_cfg.misc.keybinds); padding(8, 2);
		ImGui::Checkbox(crypt_str("Events logs"), &config_system.g_cfg.misc.events_to_log); padding(8, 2);
		ImGui::SameLine(100); ImGui::ColorEdit4(crypt_str("##logcolor"), &config_system.g_cfg.misc.log_color); padding(8, 2);
		ImGui::Checkbox(crypt_str("Clantag"), &config_system.g_cfg.misc.clantag_spammer); padding(8, 2);
		ImGui::Checkbox(crypt_str("Hitmarker"), &config_system.g_cfg.esp.hit); padding(8, 2);
		if (config_system.g_cfg.esp.hit)
		{
			draw_combo(crypt_str("Hitsound"), config_system.g_cfg.esp.hitsound, sounds, ARRAYSIZE(sounds)); padding(8, 2);
			if (config_system.g_cfg.esp.hitsound == 4)
			{
				static char hs[256] = "";
				ImGui::InputText("File", hs, 256); padding(8, 2);
				config_system.g_cfg.esp.hitfile = hs;
			}
		}
		padding(0, 10);
		ImGui::SliderInt(crypt_str("Ratio Amount"), &config_system.g_cfg.misc.aspect_ratio, 0, 250); padding(8, 2);

		draw_keybind(crypt_str("Thirdperson"), &config_system.g_cfg.misc.thirdperson_toggle, crypt_str("##TPKEY__HOTKEY")); padding(8, 2);

		if (config_system.g_cfg.misc.thirdperson_toggle.key > KEY_NONE && config_system.g_cfg.misc.thirdperson_toggle.key < KEY_MAX)
		{
			padding(6, 8); ImGui::SliderInt(crypt_str("Distance"), &config_system.g_cfg.misc.thirdperson_distance, 100, 300); padding(8, 2);
		}

		ImGui::EndChild();
		ImGui::NewLine();
		ImGui::BeginChild("Others", ImVec2(280, 220), true);
		padding(8, 8);
		ImGui::Checkbox("Viewmodel editor", &config_system.g_cfg.esp.viewmodel); padding(8, 6);
		if (config_system.g_cfg.esp.viewmodel)
		{
			ImGui::SliderInt(crypt_str("View model fov"), &config_system.g_cfg.esp.viewmodel_fov, 0, 89); padding(8, 6);
			ImGui::SliderInt(crypt_str("View model x"), &config_system.g_cfg.esp.viewmodel_x, -50, 50); padding(8, 6);
			ImGui::SliderInt(crypt_str("View model y"), &config_system.g_cfg.esp.viewmodel_y, -50, 50); padding(8, 6);
			ImGui::SliderInt(crypt_str("View model z"), &config_system.g_cfg.esp.viewmodel_z, -50, 50); padding(8, 6);
		}
		ImGui::Checkbox(crypt_str("Enable buybot"), &config_system.g_cfg.misc.buybot_enable); padding(8, 6);

		if (config_system.g_cfg.misc.buybot_enable)
		{
			draw_combo(crypt_str("Main"), config_system.g_cfg.misc.buybot1, mainwep, ARRAYSIZE(mainwep)); padding(8, 6);
			draw_combo(crypt_str("Pistols"), config_system.g_cfg.misc.buybot2, secwep, ARRAYSIZE(secwep)); padding(8, 6);
			if (ImGui::BeginCombo("Other", "Choose")) // draw start
			{
				ImGui::Spacing();
				ImGui::BeginGroup();
				{

					ImGui::Selectable("Vest", &config_system.g_cfg.misc.vest, ImGuiSelectableFlags_DontClosePopups);
					ImGui::Selectable("Helmet", &config_system.g_cfg.misc.helm, ImGuiSelectableFlags_DontClosePopups);
					ImGui::Selectable("Zeus", &config_system.g_cfg.misc.zeus, ImGuiSelectableFlags_DontClosePopups);
					ImGui::Selectable("Defuse", &config_system.g_cfg.misc.defuse, ImGuiSelectableFlags_DontClosePopups);
					ImGui::Selectable("He", &config_system.g_cfg.misc.he, ImGuiSelectableFlags_DontClosePopups);
					ImGui::Selectable("Molotov", &config_system.g_cfg.misc.inc, ImGuiSelectableFlags_DontClosePopups);
					ImGui::Selectable("Smoke", &config_system.g_cfg.misc.smoke, ImGuiSelectableFlags_DontClosePopups);

				}
				ImGui::EndGroup();
				ImGui::Spacing();

				ImGui::EndCombo();
			}
		}
		ImGui::EndChild();

		ImGui::NextColumn();
		{
			ImGui::NewLine();
			ImGui::BeginChild("Radar", ImVec2(280, 520), true);
			padding(8, 8);
			ImGui::Checkbox(crypt_str("Enable radar"), &config_system.g_cfg.misc.ingame_radar); padding(8, 2);
			ImGui::Checkbox(crypt_str("Render local"), &config_system.g_cfg.radar.render_local); padding(8, 2);
			ImGui::Checkbox(crypt_str("Render enemy"), &config_system.g_cfg.radar.render_enemy); padding(8, 2);
			ImGui::Checkbox(crypt_str("Render team"), &config_system.g_cfg.radar.render_team); padding(8, 2);
			padding(6, 8); ImGui::SliderFloat("Circles size", &config_system.g_cfg.radar.size, 0.f, 3.5f); padding(8, 2);
			ImGui::EndChild();
		}
		ImGui::Columns(1);
	}
	if (subtab6 == 1)
	{
		ImGui::Columns(2, NULL, false);
		ImGui::NewLine();
		ImGui::BeginChild("Movement", ImVec2(280, 280), true);
		padding(8, 8);
		ImGui::Checkbox(crypt_str("Bunny hop"), &config_system.g_cfg.misc.bunnyhop); padding(8, 2);
		draw_combo(crypt_str("Auto strafes"), config_system.g_cfg.misc.airstrafe, strafes, ARRAYSIZE(strafes)); padding(8, 2);
		ImGui::Combo("Trail type", &config_system.g_cfg.misc.trail_types, "Glow\0Particles\0Line\0"); padding(8, 2);

		if (config_system.g_cfg.misc.trail_types == 0 || config_system.g_cfg.misc.trail_types == 2)
		{
			ImGui::Checkbox("Movement trail", &config_system.g_cfg.misc.movement_trails); padding(8, 2);
			ImGui::SameLine(100); ImGui::ColorEdit4("##movement_trail", &config_system.g_cfg.misc.trail_color); padding(8, 2);

			if (config_system.g_cfg.misc.movement_trails)
			{
				ImGui::Checkbox("Rainbow trail", &config_system.g_cfg.misc.rainbow_trails); padding(8, 2);
			}
		}
		if (config_system.g_cfg.misc.trail_types == 1)
		{
			ImGui::Checkbox("Movement trail", &config_system.g_cfg.misc.movement_trails); padding(8, 2);
		}
		ImGui::Checkbox(crypt_str("Show velocity"), &config_system.g_cfg.misc.speedindicatior); padding(8, 2);
		ImGui::Checkbox(crypt_str("Velocity graph"), &config_system.g_cfg.misc.velocitygraph); padding(8, 2);
		ImGui::Checkbox(crypt_str("Show keyboard"), &config_system.g_cfg.misc.wasd); padding(8, 2);

		ImGui::EndChild();
		ImGui::NewLine();
		ImGui::BeginChild("Others", ImVec2(280, 220), true);
		padding(8, 8);
		ImGui::Checkbox(crypt_str("Edge jump"), &config_system.g_cfg.misc.edge); padding(8, 2);
		if (config_system.g_cfg.misc.edge)
		{
			draw_keybind(crypt_str("Edge jump key"), &config_system.g_cfg.misc.edge_jump_key, crypt_str("##EJUMP_KEY")); padding(8, 2);
			ImGui::Checkbox(crypt_str("Duck in air"), &config_system.g_cfg.misc.duckinair); padding(8, 2);
		}
		ImGui::Checkbox(crypt_str("Jump bug"), &config_system.g_cfg.misc.jumpbug); padding(8, 2);
		if (config_system.g_cfg.misc.jumpbug)
		{
			draw_keybind(crypt_str("Jump bug key"), &config_system.g_cfg.misc.jumpbug_key, crypt_str("##JUMPBUG_KEY")); padding(8, 2);
		}
		ImGui::Checkbox(crypt_str("Edge bug"), &config_system.g_cfg.misc.edgebug); padding(8, 2);
		if (config_system.g_cfg.misc.edgebug)
		{
			draw_keybind(crypt_str("Edge bug key"), &config_system.g_cfg.misc.edgebug_key, crypt_str("##EBUG_KEY")); padding(8, 2);
		}
		ImGui::Checkbox(crypt_str("Long jump"), &config_system.g_cfg.misc.longjump); padding(8, 2);
		if (config_system.g_cfg.misc.longjump)
		{
			draw_keybind(crypt_str("Long jump key"), &config_system.g_cfg.misc.longjump_key, crypt_str("##LJUMP_KEY")); padding(8, 2);
		}
		ImGui::EndChild();

		ImGui::NextColumn();
		{
			ImGui::NewLine();
			ImGui::BeginChild("Recorder", ImVec2(280, 520), true);
			padding(8, 8);

			ImGui::Checkbox("Movement recorder", &config_system.g_cfg.misc.enable_movement); padding(8, 2);
			if (config_system.g_cfg.misc.enable_movement)
			{
				draw_keybind(crypt_str("Recorder"), &config_system.g_cfg.misc.recorder, crypt_str("##RECORDER_KEY")); padding(8, 2);
				draw_keybind(crypt_str("Player"), &config_system.g_cfg.misc.playing, crypt_str("##PLAYER_KEY")); padding(8, 2);

				ImGui::Checkbox("Show the first path", &config_system.g_cfg.misc.showfirstpath); padding(8, 2);
				ImGui::SameLine(100); ImGui::ColorEdit4("##teste", &config_system.g_cfg.misc.showfirstpath_color); padding(8, 2);

				ImGui::Checkbox("Show path", &config_system.g_cfg.misc.showpath); padding(8, 2);
				ImGui::SameLine(100); ImGui::ColorEdit4("##show_path", &config_system.g_cfg.misc.pathcolor); padding(8, 2);

				ImGui::Checkbox("Show 3D circle", &config_system.g_cfg.misc.show3dcircle); padding(8, 2);
				ImGui::SameLine(100); ImGui::ColorEdit4("##show_3dcircle", &config_system.g_cfg.misc.circle3d); padding(8, 2);

				if (config_system.g_cfg.misc.show3dcircle)
				{

					padding(6, 8); ImGui::SliderInt("Show circle distance", &config_system.g_cfg.misc.showcircle, 0, 5000); padding(8, 2);
				}

				padding(6, 8); ImGui::SliderFloat("Smooth", &config_system.g_cfg.misc.smooth, 10, 60); padding(8, 2);


				if (static_cast<size_t>(current_config2) >= config_items2.size())
					current_config2 = -1;

				static char buffer[16];
				padding(8, 2);
				if (ImGui::Combo("Configs", &current_config2, [](void* data, int idx, const char** out_text) {
					auto& vector = *static_cast<std::vector<std::string>*>(data);
					*out_text = vector[idx].c_str();
					return true;
				}, &config_items2, config_items2.size(), 5) && current_config2 != -1)
					strcpy(buffer, config_items2[current_config2].c_str());
				ImGui::PushID(0);
				ImGui::PushItemWidth(178);
				padding(8, 2); if (ImGui::InputText("", buffer, IM_ARRAYSIZE(buffer), ImGuiInputTextFlags_EnterReturnsTrue)) {
					if (current_config2 != -1)
						config_system.rename2(current_config2, buffer);
				}
				ImGui::PopID();
				ImGui::SameLine();
				if (ImGui::CustomButton(("Create"), "##1", ImVec2(80, 18))) {
					config_system.add2(buffer);
				}


				if (current_config2 != -1)
				{

					padding(8, 2);
					if (ImGui::CustomButton(("Load Config"), "##2", ImVec2(80, 18))) {
						config_system.load2(current_config2);
					}
					ImGui::SameLine();
					if (ImGui::CustomButton(("Save Config"), "##3", ImVec2(80, 18))) {
						config_system.save2(current_config2);
					}
					ImGui::SameLine();
					if (ImGui::CustomButton(("Remove Config"), "##4", ImVec2(80, 18))) {
						config_system.remove2(current_config2);
					}

				}
			}
			ImGui::EndChild();
		}
		ImGui::Columns(1);
	}


	if (subtab6 == 2)
	{
		ImGui::SetCursorPosX(7);

		ImGui::PushStyleColor(ImGuiCol_ChildBg, config_system.g_cfg.menu.menu_theme == 1 ? ImVec4(34 / 255.f, 35 / 255.f, 39 / 255.f, 0.55f) : ImVec4(45 / 255.f, 49 / 255.f, 60 / 255.f, 0.55f));
		ImGui::BeginChild("Lua", ImVec2(585, 520), false);
		{
			std::string lua_name;
			if (!OpenConfig)
			{
				if (!loaded_editing_script)
				{
					static auto should_update = true;

					if (should_update)
					{
						should_update = false;
						scripts = c_lua::get().scripts;

						for (auto& current : scripts)
						{
							if (current.size() >= 5 && current.at(current.size() - 1) == 'c')
								current.erase(current.size() - 5, 5);
							else if (current.size() >= 4)
								current.erase(current.size() - 4, 4);
						}
					}


					auto backup_scripts = scripts;
					padding(8, 8);
					int same_line_counter = 0;
					for (int i = 0; i < scripts.size(); i++)
					{
						auto script_id = c_lua::get().get_script_id(scripts[i] + crypt_str(".lua"));

						if (script_id == -1)
							continue;

						ImGui::PushFont(c_menu::get().misc_font);
						ImGui::BeginGroup();
						ImGui::NewLine(); ImGui::SameLine();
						if (ImGui::CircleButton("A", std::string(std::string("##reload_") + scripts[i].c_str()).c_str(), ImVec2(20, 20)))
						{
							c_lua::get().unload_script(script_id);
							c_lua::get().load_script(script_id);
							c_lua::get().refresh_scripts();

							scripts = c_lua::get().scripts;

							if (script_id >= scripts.size())
								script_id = scripts.size() - 1; //-V103

							for (auto& current : scripts)
							{
								if (current.size() >= 5 && current.at(current.size() - 1) == 'c')
									current.erase(current.size() - 5, 5);
								else if (current.size() >= 4)
									current.erase(current.size() - 4, 4);
							}

							eventlogs::get().add(crypt_str("Reloaded ") + scripts.at(script_id) + crypt_str(" script"), false); //-V106

						}
						ImGui::NewLine(); ImGui::SameLine();
						if (ImGui::CircleButton("E", std::string(std::string("##edit_") + scripts[i].c_str()).c_str(), ImVec2(20, 20)))
						{
							loaded_editing_script = true;
							editing_script = scripts.at(script_id);
						}
						ImGui::NewLine(); ImGui::SameLine();
						if (ImGui::CircleButton("F", std::string(std::string("##unload_") + scripts[i].c_str()).c_str(), ImVec2(20, 20)))
						{
							c_lua::get().unload_script(script_id);
							c_lua::get().refresh_scripts();

							scripts = c_lua::get().scripts;

							if (script_id >= scripts.size())
								script_id = scripts.size() - 1; //-V103

							for (auto& current : scripts)
							{
								if (current.size() >= 5 && current.at(current.size() - 1) == 'c')
									current.erase(current.size() - 5, 5);
								else if (current.size() >= 4)
									current.erase(current.size() - 4, 4);
							}

							eventlogs::get().add(crypt_str("Unloaded ") + scripts.at(script_id) + crypt_str(" script"), false); //-V106

						}
						ImGui::EndGroup();
						ImGui::PopFont();
						ImGui::SameLine(0, 2);

						if (ImGui::CustomButton(scripts[i].c_str(), std::string(std::string("##") + scripts[i].c_str()).c_str(), ImVec2(100, 100)))
						{
							c_lua::get().load_script(script_id);
							c_lua::get().refresh_scripts();

							scripts = c_lua::get().scripts;

							if (script_id >= scripts.size())
								script_id = scripts.size() - 1; //-V103

							for (auto& current : scripts)
							{
								if (current.size() >= 5 && current.at(current.size() - 1) == 'c')
									current.erase(current.size() - 5, 5);
								else if (current.size() >= 4)
									current.erase(current.size() - 4, 4);
							}
							eventlogs::get().add(crypt_str("Loaded ") + scripts.at(script_id) + crypt_str(" script"), false); //-V106
						}


						if (same_line_counter < 3) {
							ImGui::SameLine();
							same_line_counter++;
						}
						else {
							padding(8, 2);
							same_line_counter = 0;
						}

					}

				}
				else {
					lua_edit(editing_script);
				}
			}
			else
			{
				auto previous_check_box = false;
				padding(8, 8);
				for (auto& current : c_lua::get().scripts)
				{
					auto& items = c_lua::get().items.at(c_lua::get().get_script_id(current));

					for (auto& item : items)
					{
						std::string item_name;

						auto first_point = false;
						auto item_str = false;

						for (auto& c : item.first)
						{
							if (c == '.')
							{
								if (first_point)
								{
									item_str = true;
									continue;
								}
								else
									first_point = true;
							}

							if (item_str)
								item_name.push_back(c);
						}

						switch (item.second.type)
						{
						case NEXT_LINE:
							previous_check_box = false; padding(8, 2);
							break;
						case CHECK_BOX:
							previous_check_box = true;
							ImGui::Checkbox(item_name.c_str(), &item.second.check_box_value); padding(8, 2);
							break;
						case COMBO_BOX:
							previous_check_box = false;
							draw_combo(item_name.c_str(), item.second.combo_box_value, [](void* data, int idx, const char** out_text)
							{
								auto labels = (std::vector <std::string>*)data;
								*out_text = labels->at(idx).c_str(); //-V106
								return true;
							}, &item.second.combo_box_labels, item.second.combo_box_labels.size()); padding(8, 2);
							break;
						case SLIDER_INT:
							previous_check_box = false;
							padding(8, 10);	ImGui::SliderInt(item_name.c_str(), &item.second.slider_int_value, item.second.slider_int_min, item.second.slider_int_max); padding(8, 2);
							break;
						case SLIDER_FLOAT:
							previous_check_box = false;
							padding(8, 10); ImGui::SliderFloat(item_name.c_str(), &item.second.slider_float_value, item.second.slider_float_min, item.second.slider_float_max); padding(8, 2);
							break;
						case COLOR_PICKER:
							if (previous_check_box)
								previous_check_box = false;
							else
								ImGui::Text((item_name + ' ').c_str());

							ImGui::SameLine();
							ImGui::ColorEdit4((crypt_str("##") + item_name).c_str(), &item.second.color_picker_value, ALPHA, true); padding(8, 2);
							break;
						}
					}
				}
			}
		}
		ImGui::EndChild();
		ImGui::PopStyleColor();
		if (!OpenConfig)
		{
			if (!loaded_editing_script)
			{
				ImGui::PushFont(c_menu::get().ico_menu);


				ImGui::SetCursorPos({ 435, 597 });

				if (ImGui::CircleButton("v", "##CONFIG_ARROW", ImVec2(40, 40)))
				{
					OpenConfig = true;
				}
				ImGui::PopFont();
				ImGui::PushFont(c_menu::get().arrow_font);

				ImGui::SetCursorPos({ 485, 597 });

				if (ImGui::CircleButton("F", "##UNLOAD_ARROW", ImVec2(40, 40)))
				{
					c_lua::get().unload_all_scripts();
					c_lua::get().refresh_scripts();

					scripts = c_lua::get().scripts;

					if (selected_script >= scripts.size())
						selected_script = scripts.size() - 1; //-V103

					for (auto& current : scripts)
					{
						if (current.size() >= 5 && current.at(current.size() - 1) == 'c')
							current.erase(current.size() - 5, 5);
						else if (current.size() >= 4)
							current.erase(current.size() - 4, 4);
					}
				}

				ImGui::SetCursorPos({ 535, 597 });

				if (ImGui::CircleButton("C", "##REFRESH_ARROW", ImVec2(40, 40)))
				{
					c_lua::get().refresh_scripts();
					scripts = c_lua::get().scripts;

					if (selected_script >= scripts.size())
						selected_script = scripts.size() - 1; //-V103

					for (auto& current : scripts)
					{
						if (current.size() >= 5 && current.at(current.size() - 1) == 'c')
							current.erase(current.size() - 5, 5);
						else if (current.size() >= 4)
							current.erase(current.size() - 4, 4);
					}
				}
				ImGui::PopFont();
			}
			else {
				ImGui::PushFont(c_menu::get().arrow_font);

				ImGui::SetCursorPos({ 485, 597 });

				if (ImGui::CircleButton("E", "##SAVE_ARROW", ImVec2(40, 40)))
				{
					std::ofstream out;

					out.open(file_path);
					out << editor.GetText() << std::endl;
					out.close();
				}
				ImGui::SetCursorPos({ 535, 597 });

				if (ImGui::CircleButton("B", "##BACK_ARROW", ImVec2(40, 40)))
				{
					loaded_editing_script = false;
					editing_script.clear();
				}
				ImGui::PopFont();
			}
		}
		else
		{
			ImGui::PushFont(c_menu::get().arrow_font);

			ImGui::SetCursorPos({ 740, 550 });

			if (ImGui::CircleButton("B", "##BACK_ARROW", ImVec2(40, 40)))
			{
				OpenConfig = false;
			}
			ImGui::PopFont();
		}
	}

}




static int active_sidebar_tab = 0;

void c_menu::draw(bool is_open)
{
	static float x = 0, x2 = 0, y2 = 0;

	// animation related code
	static float m_alpha = 0.0002f;
	m_alpha = math::clamp(m_alpha + (3.f * ImGui::GetIO().DeltaTime * (is_open ? 1.f : -1.f)), 0.0001f, 1.f);

	// set alpha in class to use later in widgets
	public_alpha = m_alpha;

	if (m_alpha <= 0.0001f)
		return;

	// set new alpha
	ImGui::PushStyleVar(ImGuiStyleVar_Alpha, m_alpha);

	if (config_system.g_cfg.menu.menu_theme == 0)
		ImGui::StyleColorsClassic();
	else if (config_system.g_cfg.menu.menu_theme == 1)
		ImGui::StyleColorsDark();

	// last active tab to switch effect & reverse alpha & preview alpha
	// IMPORTANT: DO TAB SWITCHING BY LAST_TAB!!!!!
	static int last_tab = active_sidebar_tab;
	ImGuiIO& io = ImGui::GetIO();
	const ImVec2 vecScreenSize = io.DisplaySize;

	ImVec2 menu_pos;
	ImVec2 pos;
	ImVec2 pos2;

	std::string skin;
	std::string misc;
	std::string configs;
	std::string visuals;
	std::string rage;
	std::string legit;
	std::string home;
	char username[255];
	sprintf(username, "%s", USERNAME);
	char uid[255];
	sprintf(uid, "UID: %s", userid.c_str());



	if (is_open)
	{
		x2 += m_globals()->m_frametime * 850;
		y2 += m_globals()->m_frametime * 1000;

		if (x2 >= 200)
			x2 = 200;

		if (y2 >= 650)
			y2 = 650;

		if (y2 == 650 && x2 == 200)
			x += m_globals()->m_frametime * 3550;

		if (x >= 600)
			x = 600;
	}

	if (!is_open)
	{
		x2 = 0;
		y2 = 0;
		x = 20;
	}

//	if (menu)
//	{
		ImGui::PushStyleColor(ImGuiCol_WindowBg, config_system.g_cfg.menu.menu_theme == 1 ? ImVec4(21 / 255.f, 22 / 255.f, 26 / 255.f, 1.f) : ImVec4(27 / 255.f, 31 / 255.f, 40 / 255.f, 1.f));

		ImGui::SetNextWindowPos(ImVec2(vecScreenSize.x * 0.5f, vecScreenSize.y * 0.5f), ImGuiCond_Once, ImVec2(0.5f, 0.5f));
		ImGui::SetNextWindowSize(ImVec2(x, 650));
		ImGui::Begin("Dyn", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
		{
			pos2 = ImGui::GetWindowPos();
			menu_pos = ImGui::GetCurrentWindow()->Pos;

			ImGui::SetCursorPos({ 535, 8 });
			ImGui::PushFont(c_menu::get().futura);
			LabelClick("F", &getConfigs, "##F");
			ImGui::PopFont();
			ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(pos2.x, pos2.y + 49), ImVec2(pos2.x + 600, pos2.y + 50), ImColor(120, 120, 120, 35));

			ImGui::SetCursorPos({ 0, 60 });

			ImGui::BeginGroup();
			{
				switch (active_sidebar_tab)
				{
				case 0:
					DrawMainTab();
					break;
				case 1:
					DrawRageTab();
					break;
				case 2:
					DrawLegitTab();
					break;
				case 3:
					draw_players();
					break;
				case 5:
					DrawMiscTab();
					break;
				case 4:
					DrawSkinTab();
					break;

				}
			}
			ImGui::EndGroup();
		}
		ImGui::End();
		ImGui::PopStyleColor();

		ImGui::PushStyleColor(ImGuiCol_WindowBg, config_system.g_cfg.menu.menu_theme == 1 ? ImVec4(21 / 255.f, 22 / 255.f, 26 / 255.f, 0.8f) : ImVec4(27 / 255.f, 31 / 255.f, 40 / 255.f, 0.8f));

		ImGui::SetNextWindowPos(ImVec2(menu_pos.x - 200, menu_pos.y));
		ImGui::SetNextWindowSize(ImVec2(x2, y2));
		ImGui::Begin("Dynamism", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
		{
			pos = ImGui::GetWindowPos();
			ImGui::GetWindowDrawList()->AddImage(hooks::LogoImg, ImVec2(pos.x + 20, pos.y + 10), ImVec2(pos.x + 180, pos.y + 160));


			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0));
			ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
			ImGui::PushStyleColor(ImGuiCol_BorderShadow, ImVec4(0, 0, 0, 0));
			switch (active_sidebar_tab)
			{
			case 0:
				ImGui::SetCursorPos({ 1, 160 });
				if (ImGui::ImageButton(hooks::HomeTabs, ImVec2(45, 45)))
					subtab = 0;

				if (subtab == 0)
					ImGui::SameLine(); ImGui::GetWindowDrawList()->AddImage(hooks::LayerTabs, ImVec2(pos.x + 5, pos.y + 163.5), ImVec2(pos.x + 50, pos.y + 208.5), ImVec2(0, 0), ImVec2(1, 1), ImColor(61, 60, 51, 255));

				ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(pos.x, pos.y + 200), ImVec2(pos.x + 200, pos.y + 235), ImColor(35, 37, 45));
				ImGui::SameLine(0, 0); ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 45.f); ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10); ImGui::PushFont(futura_large); ImGui::Text("Home"); ImGui::PopFont();
				break;
			case 1:
				ImGui::SetCursorPos({ 1, 160 });

				if (ImGui::ImageButton(hooks::RageTabs, ImVec2(45, 45)))
					subtab2 = 0;

				if (subtab2 == 0)
				{
					ImGui::SameLine(); ImGui::GetWindowDrawList()->AddImage(hooks::LayerTabs, ImVec2(pos.x + 5, pos.y + 163.5), ImVec2(pos.x + 50, pos.y + 208.5), ImVec2(0, 0), ImVec2(1, 1), ImColor(61, 60, 51, 255));
					rage = "Rage";
				}

				ImGui::SameLine(0, 0); ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 10.f);
				if (ImGui::ImageButton(hooks::AntiAimTabs, ImVec2(45, 45)))
					subtab2 = 1;

				if (subtab2 == 1)
				{
					ImGui::SameLine(); ImGui::GetWindowDrawList()->AddImage(hooks::LayerTabs, ImVec2(pos.x + 46, pos.y + 163.5), ImVec2(pos.x + 93, pos.y + 208.5), ImVec2(0, 0), ImVec2(1, 1), ImColor(61, 60, 51));
					rage = "Anti Aim";
				}
				ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(pos.x, pos.y + 200), ImVec2(pos.x + 200, pos.y + 235), ImColor(35, 37, 45));
				ImGui::SameLine(0, 0); ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 45.f); ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 30.f); ImGui::PushFont(futura_large); ImGui::Text(rage.c_str()); ImGui::PopFont();

				break;
			case 2:
				ImGui::SetCursorPos({ 1, 160 });

				if (ImGui::ImageButton(hooks::LegitTabs, ImVec2(45, 45)))
					subtab3 = 0;

				if (subtab3 == 0)
				{
					ImGui::SameLine(); ImGui::GetWindowDrawList()->AddImage(hooks::LayerTabs, ImVec2(pos.x + 5, pos.y + 163.5), ImVec2(pos.x + 50, pos.y + 208.5), ImVec2(0, 0), ImVec2(1, 1), ImColor(61, 60, 51));
				}


				ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(pos.x, pos.y + 200), ImVec2(pos.x + 200, pos.y + 235), ImColor(35, 37, 45));
				ImGui::SameLine(0, 0); ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 45.f); ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10); ImGui::PushFont(futura_large); ImGui::Text("Legit"); ImGui::PopFont();

				break;
			case 3:
				ImGui::SetCursorPos({ 1, 160 });

				if (ImGui::ImageButton(hooks::EnemyTabs, ImVec2(45, 45)))
					subtab4 = 0;

				if (subtab4 == 0)
				{
					ImGui::SameLine(); ImGui::GetWindowDrawList()->AddImage(hooks::LayerTabs, ImVec2(pos.x + 5, pos.y + 163.5), ImVec2(pos.x + 50, pos.y + 208.5), ImVec2(0, 0), ImVec2(1, 1), ImColor(61, 60, 51));
					visuals = "Enemy";
				}

				ImGui::SameLine(0, 0); ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 10.f);
				if (ImGui::ImageButton(hooks::TeamTabs, ImVec2(45, 45)))
					subtab4 = 1;

				if (subtab4 == 1)
				{
					ImGui::SameLine(); ImGui::GetWindowDrawList()->AddImage(hooks::LayerTabs, ImVec2(pos.x + 46, pos.y + 163.5), ImVec2(pos.x + 93, pos.y + 208.5), ImVec2(0, 0), ImVec2(1, 1), ImColor(61, 60, 51));
					visuals = "Team";
				}

				ImGui::SameLine(0, 0); ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 10.f);
				if (ImGui::ImageButton(hooks::SelfTabs, ImVec2(45, 45)))
					subtab4 = 2;

				if (subtab4 == 2)
				{
					ImGui::SameLine(); ImGui::GetWindowDrawList()->AddImage(hooks::LayerTabs, ImVec2(pos.x + 89, pos.y + 163.5), ImVec2(pos.x + 136, pos.y + 208.5), ImVec2(0, 0), ImVec2(1, 1), ImColor(61, 60, 51));
					visuals = "Self";
				}

				ImGui::SameLine(0, 0); ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 10.f);
				if (ImGui::ImageButton(hooks::EyeTabs, ImVec2(45, 45)))
					subtab4 = 3;

				if (subtab4 == 3)
				{
					ImGui::SameLine(); ImGui::GetWindowDrawList()->AddImage(hooks::LayerTabs, ImVec2(pos.x + 134, pos.y + 163.5), ImVec2(pos.x + 179, pos.y + 208.5), ImVec2(0, 0), ImVec2(1, 1), ImColor(61, 60, 51));
					visuals = "World";
				}

				ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(pos.x, pos.y + 200), ImVec2(pos.x + 200, pos.y + 235), ImColor(35, 37, 45));
				ImGui::SameLine(0, 0); ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 45.f); ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 120.f); ImGui::PushFont(futura_large); ImGui::Text(visuals.c_str()); ImGui::PopFont();

				break;
			case 4:
				ImGui::SetCursorPos({ 1, 160 });

				if (ImGui::ImageButton(hooks::SkinChangerTabs, ImVec2(45, 45)))
					subtab7 = 0;

				if (subtab7 == 0)
				{
					ImGui::SameLine(); ImGui::GetWindowDrawList()->AddImage(hooks::LayerTabs, ImVec2(pos.x + 5, pos.y + 163.5), ImVec2(pos.x + 50, pos.y + 208.5), ImVec2(0, 0), ImVec2(1, 1), ImColor(61, 60, 51));
					skin = "Skins";
				}

				ImGui::SameLine(0, 0); ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 10.f);
				if (ImGui::ImageButton(hooks::SkinTabs, ImVec2(45, 45)))
					subtab7 = 1;

				if (subtab7 == 1)
				{
					ImGui::SameLine(); ImGui::GetWindowDrawList()->AddImage(hooks::LayerTabs, ImVec2(pos.x + 46, pos.y + 163.5), ImVec2(pos.x + 93, pos.y + 208.5), ImVec2(0, 0), ImVec2(1, 1), ImColor(61, 60, 51));

					skin = "Gloves";
				}
				ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(pos.x, pos.y + 200), ImVec2(pos.x + 200, pos.y + 235), ImColor(35, 37, 45));
				ImGui::SameLine(0, 0); ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 45.f); ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 30.f); ImGui::PushFont(futura_large); ImGui::Text(skin.c_str()); ImGui::PopFont();


				break;
			case 5:
				ImGui::SetCursorPos({ 1, 160 });

				if (ImGui::ImageButton(hooks::MiscTabs, ImVec2(45, 45)))
					subtab6 = 0;

				if (subtab6 == 0)
				{
					ImGui::SameLine(); ImGui::GetWindowDrawList()->AddImage(hooks::LayerTabs, ImVec2(pos.x + 5, pos.y + 163.5), ImVec2(pos.x + 50, pos.y + 208.5), ImVec2(0, 0), ImVec2(1, 1), ImColor(61, 60, 51));
					misc = "Misc";
				}

				ImGui::SameLine(0, 0); ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 10.f);
				if (ImGui::ImageButton(hooks::MovementTabs, ImVec2(45, 45)))
					subtab6 = 1;

				if (subtab6 == 1)
				{
					ImGui::SameLine(); ImGui::GetWindowDrawList()->AddImage(hooks::LayerTabs, ImVec2(pos.x + 46, pos.y + 163.5), ImVec2(pos.x + 93, pos.y + 208.5), ImVec2(0, 0), ImVec2(1, 1), ImColor(61, 60, 51));

					misc = "Movement";
				}

				ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(pos.x, pos.y + 200), ImVec2(pos.x + 200, pos.y + 235), ImColor(35, 37, 45));
				ImGui::SameLine(0, 0); ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 45.f); ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 65.f); ImGui::PushFont(futura_large); ImGui::Text(misc.c_str()); ImGui::PopFont();


				break;
			}
			ImGui::PopStyleColor(5);


			ImGui::SetCursorPosY(230);

			ImGui::NewLine();
			ImGui::SameLine(50); ImGui::NewLine(); ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 15.f);
			ImGui::SameLine(50); ImGui::NewLine(); ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 15.f);
			if (ImGui::Tab2("Ragebot", "C", ImVec2(200, 50), active_sidebar_tab == 1 ? true : false))
				active_sidebar_tab = 1;
			ImGui::SameLine(50); ImGui::NewLine(); ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 15.f);
			if (ImGui::Tab2("Legitbot", "A", ImVec2(200, 50), active_sidebar_tab == 2 ? true : false))
				active_sidebar_tab = 2;
			ImGui::SameLine(50); ImGui::NewLine(); ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 15.f);
			if (ImGui::Tab2("Visuals", "E", ImVec2(200, 50), active_sidebar_tab == 3 ? true : false))
				active_sidebar_tab = 3;
			ImGui::SameLine(50); ImGui::NewLine(); ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 15.f);
			if (ImGui::Tab2("Extras", "F", ImVec2(200, 50), active_sidebar_tab == 5 ? true : false))
				active_sidebar_tab = 5;
			ImGui::SameLine(50); ImGui::NewLine(); ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 15.f);
			if (ImGui::Tab2("Skins", "D", ImVec2(200, 50), active_sidebar_tab == 4 ? true : false))
				active_sidebar_tab = 4;
		}
		ImGui::End();
		ImGui::PopStyleColor();
		ImGui::PopStyleVar();

		static int x3, y3;

		if (active_sidebar_tab == 3)
		{
			x3 += m_globals()->m_frametime * 1300;
			y3 += m_globals()->m_frametime * 1300;

			if (x3 >= 340)
				x3 = 340;

			if (y3 >= 520)
				y3 = 520;
		}
		else if (!is_open || active_sidebar_tab != 3)
		{
			x3 = 0;
			y3 = 0;
		}
		if (active_sidebar_tab == 3 && subtab4 < 3 && is_open)
		{
			static int lastactive = -1;

			static CEsp::MoveStruct* MV_Hp = &GP_Esp->MVItemList[CEsp::MI_HEALTH_BAR];
			static CEsp::MoveStruct* MV_Armor = &GP_Esp->MVItemList[CEsp::MI_ARMOR_BAR];
			static CEsp::MoveStruct* MV_Hp_Text = &GP_Esp->MVItemList[CEsp::MI_HEALTH_TEXT];
			static CEsp::MoveStruct* MV_Armor_Text = &GP_Esp->MVItemList[CEsp::MI_ARMOR_TEXT];
			static CEsp::MoveStruct* MV_Name = &GP_Esp->MVItemList[CEsp::MI_NAME_TEXT];
			static CEsp::MoveStruct* MV_Weapon = &GP_Esp->MVItemList[CEsp::MI_WEAPON_TEXT];
			static CEsp::MoveStruct* MV_Ammo = &GP_Esp->MVItemList[CEsp::MI_AMMO_TEXT];
			static CEsp::MoveStruct* MV_Distance = &GP_Esp->MVItemList[CEsp::MI_DISTANCE_TEXT];
			static CEsp::MoveStruct* MV_Money = &GP_Esp->MVItemList[CEsp::MI_MONEY_TEXT];
			static CEsp::MoveStruct* MV_Scope = &GP_Esp->MVItemList[CEsp::MI_SCOPE_TEXT];
			static CEsp::MoveStruct* MV_Flashed = &GP_Esp->MVItemList[CEsp::MI_FLASHED_TEXT];
			static CEsp::MoveStruct* MV_Defusing = &GP_Esp->MVItemList[CEsp::MI_DEFUSING_TEXT];

			if ((CEsp::MITypes)CEsp::MoveStruct::LastActive == CEsp::MI_HEALTH_BAR)
				lastactive = CEsp::MI_HEALTH_BAR;

			else if ((CEsp::MITypes)CEsp::MoveStruct::LastActive == CEsp::MI_ARMOR_BAR)
				lastactive = CEsp::MI_ARMOR_BAR;

			else if ((CEsp::MITypes)CEsp::MoveStruct::LastActive == CEsp::MI_NAME_TEXT)
				lastactive = CEsp::MI_NAME_TEXT;

			else if ((CEsp::MITypes)CEsp::MoveStruct::LastActive == CEsp::MI_WEAPON_TEXT)
				lastactive = CEsp::MI_WEAPON_TEXT;

			else if ((CEsp::MITypes)CEsp::MoveStruct::LastActive == CEsp::MI_FLASHED_TEXT)
				lastactive = CEsp::MI_FLASHED_TEXT;

			else if ((CEsp::MITypes)CEsp::MoveStruct::LastActive == CEsp::MI_DEFUSING_TEXT)
				lastactive = CEsp::MI_DEFUSING_TEXT;

			else if ((CEsp::MITypes)CEsp::MoveStruct::LastActive == CEsp::MI_SCOPE_TEXT)
				lastactive = CEsp::MI_SCOPE_TEXT;

			else if ((CEsp::MITypes)CEsp::MoveStruct::LastActive == CEsp::MI_HEALTH_TEXT)
				lastactive = CEsp::MI_HEALTH_TEXT;

			ImGui::SetNextWindowPos(ImVec2(menu_pos.x + 645, menu_pos.y));
			ImGui::SetNextWindowSize(ImVec2(x3, y3));
			ImGui::Begin("Preview", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
			{
				GP_Esp->CustomVisuals(ImGui::GetWindowPos(), subtab4);
				//ImGui::Text(std::to_string(lastactive).c_str());
				ImGui::BeginChild("Types", ImVec2(-1, -1), false);
				if (lastactive == CEsp::MI_HEALTH_BAR)
				{
					padding(8, 8);
					ImGui::Text("Health bar"); padding(8, 2);
					ImGui::Checkbox("Override health bar", &config_system.g_cfg.player.type[subtab4].custom_health_color);
					padding(-16, 2);
					ImGui::ColorEdit4("Override color", &config_system.g_cfg.player.type[subtab4].health_color, true, true);
				}
				else if (lastactive == CEsp::MI_ARMOR_BAR)
				{
					padding(8, 8);
					ImGui::Text("Ammo bar"); padding(8, 2);
					padding(-16, 2);
					ImGui::ColorEdit4("Ammo bar color", &config_system.g_cfg.player.type[subtab4].ammobar_color, true, true);
				}
				else if (lastactive == CEsp::MI_NAME_TEXT)
				{
					padding(8, 8);
					ImGui::Text("Player name"); padding(8, 2);
					padding(-16, 2);
					ImGui::ColorEdit4("Name color", &config_system.g_cfg.player.type[subtab4].name_color, true, true);
				}
				else if (lastactive == CEsp::MI_WEAPON_TEXT)
				{
					padding(8, 8);
					ImGui::Text("Player weapon"); padding(8, 2);
					padding(-16, 2);
					ImGui::ColorEdit4("Weapon color", &config_system.g_cfg.player.type[subtab4].weapon_color, true, true);

				}
				else if (lastactive == CEsp::MI_FLASHED_TEXT)
				{
					padding(8, 8);
					ImGui::Text("Player bomb"); padding(8, 2);
					padding(-16, 2);
					ImGui::ColorEdit4("Bomb color", &config_system.g_cfg.player.type[subtab4].BombCarrie, true, true);
				}
				else if (lastactive == CEsp::MI_DEFUSING_TEXT)
				{
					padding(8, 8);
					ImGui::Text("Defuse kit"); padding(8, 2);
					padding(-16, 2);
					ImGui::ColorEdit4("Defuse color", &config_system.g_cfg.player.type[subtab4].Defuse, true, true);
				}
				else if (lastactive == CEsp::MI_SCOPE_TEXT)
				{
					padding(8, 8);
					ImGui::Text("Player scopped"); padding(8, 2);
					padding(-16, 2);
					ImGui::ColorEdit4("Scopped color", &config_system.g_cfg.player.type[subtab4].Scoped, true, true);
				}
				else if (lastactive == CEsp::MI_HEALTH_TEXT)
				{
					padding(8, 8);
					ImGui::Text("Player armor"); padding(8, 2);
					padding(-16, 2);
					ImGui::ColorEdit4("Armor color", &config_system.g_cfg.player.type[subtab4].Armor, true, true);
				}
				ImGui::EndChild();
			}
			ImGui::End();
		}
	}