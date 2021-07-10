
#include "../../ImGui/imgui.h"
#include "player_esp.h"
#include "../menu.h"
#include "../../Render.h"
CEsp* GP_Esp = nullptr;

#define IMAGE_X 110
#define IMAGE_Y 245

#define IMAGE_OUTLINE 50

#define LINES_X 70
#define LINES_Y 80

#define BOX_POS_X 45
#define BOX_POS_Y 20

#define BOX_SIZE_X 160
#define BOX_SIZE_Y 310

#define MV_HEALTH_TEXT "90HP"
#define MV_ARMOR_TEXT "90A"
#define MV_NAME_TEXT "Name"
#define MV_WEAPON_TEXT "M4A4"
#define MV_WEAPON_TEXT_ICO "S"
#define MV_DISTANCE_TEXT "5m"
#define MV_MONEY_TEXT "1000$"
#define MV_SCOPE_TEXT "SCOPED"
#define MV_FLASHED_TEXT "FLASHED"
#define MV_DEFUSING_TEXT "DEFUSING"

#define FULL_BACK_SIZE_X 100
#define FULL_BACK_SIZE_Y 100

#define SELECTED_BORDER_SIZE 4

#define VIS_PREW_ZONE_X 350 
#define VIS_PREW_ZONE_Y 400 

bool IsMousPress = false;
bool IsMousClick = false;

int BoxSize_Y = 0;
int BoxSize_X = 0;

bool IsBlockMove = false;

int CEsp::MoveStruct::ActiveIdx = -1;
int CEsp::MoveStruct::LastActive = -1;
bool CEsp::MoveStruct::InitMItems = false;

void CEsp::CustomVisuals(ImVec2 Start, int player)
{

	ImGuiWindow* window = ImGui::GetCurrentWindow();
	auto Draw = window->DrawList;
	ImGuiIO& g = ImGui::GetIO();

	static MoveStruct* MV_Hp = &MVItemList[MI_HEALTH_BAR];
	static MoveStruct* MV_Armor = &MVItemList[MI_ARMOR_BAR];

	static MoveStruct* MV_Hp_Text = &MVItemList[MI_HEALTH_TEXT];
	static MoveStruct* MV_Armor_Text = &MVItemList[MI_ARMOR_TEXT];

	static MoveStruct* MV_Name = &MVItemList[MI_NAME_TEXT];
	static MoveStruct* MV_Weapon = &MVItemList[MI_WEAPON_TEXT];
	static MoveStruct* MV_Ammo = &MVItemList[MI_AMMO_TEXT];
	static MoveStruct* MV_Distance = &MVItemList[MI_DISTANCE_TEXT];
	static MoveStruct* MV_Money = &MVItemList[MI_MONEY_TEXT];
	static MoveStruct* MV_Scope = &MVItemList[MI_SCOPE_TEXT];
	static MoveStruct* MV_Flashed = &MVItemList[MI_FLASHED_TEXT];
	static MoveStruct* MV_Defusing = &MVItemList[MI_DEFUSING_TEXT];

	MV_Hp_Text->Size = ImGui::CalcTextSize("HK");
	MV_Armor_Text->Size = ImGui::CalcTextSize("AK47");
	MV_Name->Size = ImGui::CalcTextSize(MV_NAME_TEXT);
	MV_Distance->Size = ImGui::CalcTextSize(MV_DISTANCE_TEXT);
	MV_Money->Size = ImGui::CalcTextSize("1000$");
	MV_Flashed->Size = ImGui::CalcTextSize("BOMB");
	MV_Scope->Size = ImGui::CalcTextSize(MV_SCOPE_TEXT);
	MV_Defusing->Size = ImGui::CalcTextSize(MV_DEFUSING_TEXT);
	MV_Weapon->Size = ImGui::CalcTextSize("V");

	MV_Hp->IsEnabled = true;
	MV_Armor_Text->IsEnabled = true;
	MV_Name->IsEnabled = true;
	MV_Weapon->IsEnabled = true;
	MV_Armor_Text->IsEnabled = config_system.g_cfg.player.type[player].ammo;
	MV_Money->IsEnabled = config_system.g_cfg.player.type[player].FLAGS_MONEY;
	MV_Scope->IsEnabled = config_system.g_cfg.player.type[player].FLAGS_SCOPED;
	MV_Defusing->IsEnabled = config_system.g_cfg.player.type[player].FLAGS_KIT;

	ImColor LinesColor = ImColor(1.f * 255.f, 1.f * 255.f, 1.f * 255.f, 0.1f * 255.f);
	Color OutLineColor = Color(1.f * 255.f, 1.f * 255.f, 1.f * 255.f, 0.3f * 255.f);
	ImColor BackColor = ImColor(0.5f * 255.f, 0.5f * 255.f, 0.5f * 255.f, 0.5f * 255.f);


	for (int i = 0; i < 6; i++)
		IsMousPress = g.MouseDownDuration[i] > 0.0f;

	IsMousClick = g.MouseClicked;

	ImVec2 MousePos = g.MousePos;

	auto FixPos = [](ImVec2 Max, ImVec2 Min, ImVec2& Cur)
	{
		if (Cur.x > Max.x - 5) { Cur.x = Max.x - 5; }
		if (Cur.y > Max.y - 5) { Cur.y = Max.y - 5; }
		if (Cur.x < Min.x + 5) { Cur.x = Min.x + 5; }
		if (Cur.y < Min.y + 5) { Cur.y = Min.y + 5; }
	};

	auto DrawVBox = [Draw](ImVec2 Pos, ImVec2 Size, Color Color)
	{
		Pos = ImVec2(int(Pos.x), int(Pos.y));
		Draw->AddLine(ImVec2(Pos.x - 0.5f, Pos.y), ImVec2(Pos.x + Size.x, Pos.y), Color.u32());
		Draw->AddLine(ImVec2(Pos.x, Pos.y - 0.5f), ImVec2(Pos.x, Pos.y + Size.y), Color.u32());
		Draw->AddLine(ImVec2(Pos.x, Pos.y + Size.y), ImVec2(Pos.x + Size.x + 0.5f, Pos.y + Size.y), Color.u32());
		Draw->AddLine(ImVec2(Pos.x + Size.x, Pos.y), ImVec2(Pos.x + Size.x, Pos.y + Size.y + 0.5f), Color.u32());
	};

	auto DrawVBoxCorner = [Draw](ImVec2 Pos, ImVec2 Size, Color Color)
	{
		Pos = ImVec2(int(Pos.x), int(Pos.y));
		/*

		01-----------11
		2             2
		|             |
		|             |
		1             1
		22-----------23
		*/

		int LSize_X = Size.x / 4;
		int LSize_Y = Size.y / 4;

		/*01*/
		Draw->AddLine(
			ImVec2(Pos.x, Pos.y),
			ImVec2(Pos.x + LSize_X, Pos.y),
			Color.u32());

		/*02*/
		Draw->AddLine(
			ImVec2(Pos.x, Pos.y),
			ImVec2(Pos.x, Pos.y + LSize_Y),
			Color.u32());

		/*11*/
		Draw->AddLine(
			ImVec2(Pos.x + Size.x - LSize_X, Pos.y),
			ImVec2(Pos.x + Size.x, Pos.y),
			Color.u32());

		/*12*/
		Draw->AddLine(
			ImVec2(Pos.x + Size.x, Pos.y),
			ImVec2(Pos.x + Size.x, Pos.y + LSize_Y),
			Color.u32());


		/*21*/
		Draw->AddLine(
			ImVec2(Pos.x, Pos.y + Size.y - LSize_Y),
			ImVec2(Pos.x, Pos.y + Size.y),
			Color.u32());

		/*22*/
		Draw->AddLine(
			ImVec2(Pos.x, Pos.y + Size.y),
			ImVec2(Pos.x + LSize_X, Pos.y + Size.y),
			Color.u32());

		/*31*/
		Draw->AddLine(
			ImVec2(Pos.x + Size.x, Pos.y + Size.y - LSize_Y),
			ImVec2(Pos.x + Size.x, Pos.y + Size.y),
			Color.u32());

		/*32*/
		Draw->AddLine(
			ImVec2(Pos.x + Size.x - LSize_X, Pos.y + Size.y),
			ImVec2(Pos.x + Size.x, Pos.y + Size.y),
			Color.u32());
	};

	auto Background = [Draw, DrawVBox, LinesColor, OutLineColor, BackColor](ImVec2  Min, ImVec2 Max, ImVec2 MenuPos)
	{

		Draw->AddRectFilled(Min, ImVec2(VIS_PREW_ZONE_X, VIS_PREW_ZONE_Y), BackColor);

		for (int i(1); i <= LINES_Y; i++)
			Draw->AddLine(ImVec2(Min.x, Min.y + (VIS_PREW_ZONE_Y / LINES_Y) * i), ImVec2(Min.x + VIS_PREW_ZONE_X, Min.y + (VIS_PREW_ZONE_Y / LINES_Y) * i), LinesColor);
		for (int i(1); i <= LINES_X; i++)
			Draw->AddLine(ImVec2(Min.x + (VIS_PREW_ZONE_X / LINES_X) * i, Min.y), ImVec2(Min.x + (VIS_PREW_ZONE_X / LINES_X) * i, Min.y + VIS_PREW_ZONE_Y), LinesColor);

		DrawVBox(Min, ImVec2(VIS_PREW_ZONE_X, VIS_PREW_ZONE_Y), OutLineColor);
	};

	auto DrawBarVertical = [Draw, DrawVBox](ImVec2 Pos, ImVec2 Size, Color MainColor, Color BackColor, Color OutLineColor, int Lines, MoveStruct* Curt)
	{
		if (Curt->boolParam1)
			Draw->AddRectFilled(ImVec2(Pos.x, Pos.y),
				ImVec2(Pos.x + 3, Pos.y + 268),
				MainColor.u32());

		if (!Curt->boolParam3)
			Draw->AddRectFilled(ImVec2(Pos.x, Pos.y),
				ImVec2(Pos.x + 3, Pos.y + 268),
				BackColor.u32());
		else
			Draw->AddRectFilled(ImVec2(Pos.x, Pos.y),
				ImVec2(Pos.x + 3, Pos.y + 268),
				BackColor.u32());

		if (Curt->boolParam2)
			DrawVBox(Pos, Size, OutLineColor);
	
	};

	auto DrawBarHorizontal = [Draw, DrawVBox](ImVec2 Pos, ImVec2 Size, Color MainColor, Color BackColor, Color OutLineColor, int Lines, MoveStruct* Curt)
	{
		Pos = ImVec2(int(Pos.x), int(Pos.y));
		if (Curt->boolParam1)
			Draw->AddRectFilled(ImVec2(Pos.x, Pos.y),
				ImVec2(Pos.x + 134, Pos.y + 3),
				MainColor.u32());

		if (Curt->boolParam3)
			Draw->AddRectFilled(ImVec2(Pos.x, Pos.y),
				ImVec2(Pos.x + 134, Pos.y + 3),
				BackColor.u32());
		else
			Draw->AddRectFilled(ImVec2(Pos.x, Pos.y),
				ImVec2(Pos.x + 134, Pos.y + 3),
				BackColor.u32());

		if (Curt->boolParam2)
			DrawVBox(Pos, Size, OutLineColor);

	};

	ImVec2 StartPos = ImGui::GetWindowPos();
	ImVec2 StartPosScreen = ImGui::GetWindowPos();


	ImVec2 TextOffset = ImVec2(StartPosScreen.x - StartPos.x, StartPosScreen.y - StartPos.y);
	auto _DrawText = [Draw](std::string Text, float size,ImVec2 Pos, MoveStruct* Curt, ImFont* font)
	{

		Draw->AddText(font, size, Pos, Curt->Color1.u32(), Text.c_str());
	};

	ImVec2 EndPosScreen = ImVec2(StartPosScreen.x + VIS_PREW_ZONE_X, StartPosScreen.y + VIS_PREW_ZONE_Y);

	if ((MousePos.x > StartPosScreen.x && MousePos.x < EndPosScreen.x) &&
		(MousePos.y > StartPosScreen.y && MousePos.y < EndPosScreen.y))
	{

	}

	int FixImgPos_X = ((VIS_PREW_ZONE_X - IMAGE_X) / 2);
	int FixImgPos_Y = ((VIS_PREW_ZONE_Y - IMAGE_Y) / 2);

	ImVec2 ImgePos = ImVec2(StartPos.x + FixImgPos_X, StartPos.y + FixImgPos_Y);

	ImVec2 ScreenImgePos = ImgePos;

	//Background(StartPosScreen, EndPosScreen, StartPos);

	/*	if (!VisualBodyImage)
			D3DXCreateTextureFromFileEx(GP_Render->m_pDevice, (CGlobal::SystemDisk + XorStr("X1N3\\Resources\\Images\\Body.png")).c_str(), 222, 579,
				D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &VisualBodyImage);

		if (VisualBodyImage)
			Draw->DrawImage(VisualBodyImage, ImgePos, ImgePos + ImVec2(IMAGE_X, IMAGE_Y));*/

	if (config_system.g_cfg.player.type[player].skeleton)
	{
		Draw->AddLine(ImVec2(StartPosScreen.x + 152 + 20, StartPosScreen.y + 122), ImVec2(StartPosScreen.x + 150 + 20, StartPosScreen.y + 103), ImColor(255, 255, 255));
		Draw->AddLine(ImVec2(StartPosScreen.x + 152 + 20, StartPosScreen.y + 122), ImVec2(StartPosScreen.x + 157 + 20, StartPosScreen.y + 129), ImColor(255, 255, 255));
		Draw->AddLine(ImVec2(StartPosScreen.x + 157 + 20, StartPosScreen.y + 129), ImVec2(StartPosScreen.x + 158 + 20, StartPosScreen.y + 163), ImColor(255, 255, 255));
		Draw->AddLine(ImVec2(StartPosScreen.x + 158 + 20, StartPosScreen.y + 163), ImVec2(StartPosScreen.x + 159 + 20, StartPosScreen.y + 194), ImColor(255, 255, 255));
		Draw->AddLine(ImVec2(StartPosScreen.x + 159 + 20, StartPosScreen.y + 194), ImVec2(StartPosScreen.x + 180 + 20, StartPosScreen.y + 219), ImColor(255, 255, 255));
		Draw->AddLine(ImVec2(StartPosScreen.x + 180 + 20, StartPosScreen.y + 219), ImVec2(StartPosScreen.x + 195 + 20, StartPosScreen.y + 273), ImColor(255, 255, 255));
		Draw->AddLine(ImVec2(StartPosScreen.x + 195 + 20, StartPosScreen.y + 273), ImVec2(StartPosScreen.x + 196 + 20, StartPosScreen.y + 306), ImColor(255, 255, 255));
		Draw->AddLine(ImVec2(StartPosScreen.x + 159 + 20, StartPosScreen.y + 194), ImVec2(StartPosScreen.x + 139 + 20, StartPosScreen.y + 218), ImColor(255, 255, 255));
		Draw->AddLine(ImVec2(StartPosScreen.x + 139 + 20, StartPosScreen.y + 216), ImVec2(StartPosScreen.x + 131 + 20, StartPosScreen.y + 267), ImColor(255, 255, 255));
		Draw->AddLine(ImVec2(StartPosScreen.x + 131 + 20, StartPosScreen.y + 267), ImVec2(StartPosScreen.x + 134 + 20, StartPosScreen.y + 286), ImColor(255, 255, 255));
		Draw->AddLine(ImVec2(StartPosScreen.x + 157 + 20, StartPosScreen.y + 132), ImVec2(StartPosScreen.x + 121 + 20, StartPosScreen.y + 142), ImColor(255, 255, 255));
		Draw->AddLine(ImVec2(StartPosScreen.x + 121 + 20, StartPosScreen.y + 142), ImVec2(StartPosScreen.x + 126 + 20, StartPosScreen.y + 146), ImColor(255, 255, 255));
		Draw->AddLine(ImVec2(StartPosScreen.x + 125 + 20, StartPosScreen.y + 145), ImVec2(StartPosScreen.x + 141 + 20, StartPosScreen.y + 134), ImColor(255, 255, 255));
		Draw->AddLine(ImVec2(StartPosScreen.x + 157 + 20, StartPosScreen.y + 132), ImVec2(StartPosScreen.x + 183 + 20, StartPosScreen.y + 135), ImColor(255, 255, 255));
		Draw->AddLine(ImVec2(StartPosScreen.x + 181 + 20, StartPosScreen.y + 135), ImVec2(StartPosScreen.x + 174 + 20, StartPosScreen.y + 141), ImColor(255, 255, 255));
		Draw->AddLine(ImVec2(StartPosScreen.x + 161 + 20, StartPosScreen.y + 122), ImVec2(StartPosScreen.x + 174 + 20, StartPosScreen.y + 141), ImColor(255, 255, 255));
	}
	ImVec2 StartBoxPos = ImVec2(int(ScreenImgePos.x + IMAGE_X / 2), int(ScreenImgePos.y + (IMAGE_Y / 13) - (10 * 2.9)));

	ImVec2 CLines = ImVec2(StartPosScreen.x + (VIS_PREW_ZONE_X / 2), StartBoxPos.y);

	BoxSize_Y = int(IMAGE_Y - (IMAGE_Y / 13) + (10 * 4));
	BoxSize_X = int(BoxSize_Y / 2);
	StartBoxPos.x -= int(BoxSize_X / 2.f);
	int HPTol = MV_Armor->NumParam2;
	int ATol = MV_Armor->NumParam2;

	for (int i(0); (MITypes)i < MI_MAX; i++)
		MVItemList[i].Position = ImVec2(int(StartPosScreen.x + MVItemList[i].ZonePos.x), int(StartPosScreen.y + MVItemList[i].ZonePos.y));

		//MoveStruct::InitMItems = true;
		MoveStruct::LastActive = MI_MAX;
		if (!MoveStruct::InitMItems)
		{
			MV_Hp_Text->Position = ImVec2(StartBoxPos.x + BoxSize_X, StartBoxPos.y);
			MV_Hp_Text->StartPos = MV_Hp_Text->Position;
			MV_Hp_Text->Id = MI_HEALTH_TEXT;
			MV_Hp_Text->Side = RIGHT_S;
			MV_Hp_Text->TextSize = 13;
			MV_Hp_Text->Offset = ImVec2(-7, -12);
			MV_Hp_Text->IsText = true;
			MV_Hp_Text->Name = "MV_HP_Text";
			MV_Hp_Text->Color1 = Color(255, 255, 255, 255);

			/*=====================================================================================*/
			MV_Armor_Text->Position = ImVec2(StartBoxPos.x + (BoxSize_X / 2) - (MV_Armor_Text->Size.x / 2), StartBoxPos.y + BoxSize_Y);
			MV_Armor_Text->StartPos = MV_Armor_Text->Position;
			MV_Armor_Text->Id = MI_ARMOR_TEXT;
			MV_Armor_Text->Side = DOWN_S;
			MV_Armor_Text->TextSize = 13;
			MV_Armor_Text->Offset = ImVec2(4, -15);
			MV_Armor_Text->IsText = true;
			MV_Armor_Text->Name = "MV_Arm_Text";
			MV_Armor_Text->Color1 = Color(255, 255, 255, 255);

			/*=====================================================================================*/
			MV_Hp->Position = ImVec2(StartBoxPos.x - HPTol, StartBoxPos.y);
			MV_Hp->StartPos = MV_Hp->Position;
			MV_Hp->Id = MI_HEALTH_BAR;
			MV_Hp->Side = LEFT_S;
			MV_Hp->Size = ImVec2(HPTol, BoxSize_Y);
			MV_Hp->Offset = ImVec2(5, 0);

			MV_Hp->Color1 = Color(10, 255, 10, 255);
			MV_Hp->Color2 = Color(255, 0, 0, 255);
			MV_Hp->Color3 = Color(0, 0, 0, 255);

			MV_Hp->NumParam1 = 3;
			MV_Hp->NumParam2 = 3;
			MV_Hp->Name = "MV_HPBar";
			MV_Hp->boolParam1 = true;
			MV_Hp->boolParam2 = true;
			MV_Hp->boolParam3 = false;

			/*=====================================================================================*/
			MV_Armor->Position = ImVec2(StartBoxPos.x - ATol, StartBoxPos.y + BoxSize_Y);
			MV_Armor->StartPos = MV_Armor->Position;
			MV_Armor->Id = MI_ARMOR_BAR;
			MV_Armor->Side = DOWN_S;
			MV_Armor->Size = ImVec2(ATol, BoxSize_Y);
			MV_Armor->Offset = ImVec2(0, -5);

			MV_Armor->Color1 = Color(255, 255, 255, 255);
			MV_Armor->Color2 = Color(255, 0, 0, 255);
			MV_Armor->Color3 = Color(0, 0, 0, 255);

			MV_Armor->NumParam1 = 3;
			MV_Armor->NumParam2 = 3;
			MV_Armor->Name = "MV_ArmorBar";
			MV_Armor->boolParam1 = true;
			MV_Armor->boolParam2 = true;
			MV_Armor->boolParam3 = false;

			/*=====================================================================================*/

			MV_Name->Position = ImVec2(StartBoxPos.x + (BoxSize_X / 2) - (MV_Name->Size.x / 2), StartBoxPos.y - MV_Name->Size.y);
			MV_Name->StartPos = MV_Name->Position;
			MV_Name->Id = MI_NAME_TEXT;
			MV_Name->Side = TOP_S;
			MV_Name->TextSize = 13;
			MV_Name->Offset = ImVec2(8, 4);
			MV_Name->IsText = true;
			MV_Name->Name = "MV_Name";
			MV_Name->boolParam1 = false;

			MV_Name->Color1 = Color(255, 255, 255, 255);

			/*=====================================================================================*/

			MV_Weapon->Position = ImVec2(StartBoxPos.x + (BoxSize_X / 2) - (MV_Weapon->Size.x / 2), StartBoxPos.y + BoxSize_Y);
			MV_Weapon->StartPos = MV_Weapon->Position;
			MV_Weapon->Id = MI_WEAPON_TEXT;
			MV_Weapon->Side = DOWN_S;
			MV_Weapon->TextSize = 20;
			MV_Weapon->Offset = ImVec2(-5, -10);
			MV_Weapon->IsText = true;
			MV_Weapon->Name = "MV_Weapon";
			MV_Weapon->Color1 = Color(255, 255, 255, 255);

			/*=====================================================================================*/

			MV_Ammo->Position = ImVec2(StartBoxPos.x + (BoxSize_X / 2) - (MV_Ammo->Size.x / 2), StartBoxPos.y + BoxSize_Y);
			MV_Ammo->StartPos = MV_Ammo->Position;
			MV_Ammo->Id = MI_AMMO_TEXT;
			MV_Ammo->Side = DOWN_S;
			MV_Ammo->TextSize = 13;
			MV_Ammo->Offset = ImVec2(0, -10);
			MV_Ammo->IsText = true;
			MV_Ammo->Color1 = Color(255, 255, 255, 255);

			/*=====================================================================================*/
			MV_Distance->Position = ImVec2(StartBoxPos.x + BoxSize_X, StartBoxPos.y + BoxSize_Y - MV_Distance->Size.y);
			MV_Distance->StartPos = MV_Distance->Position;
			MV_Distance->Id = MI_DISTANCE_TEXT;
			MV_Distance->Side = DOWN_RIGHT_DOWN_S;
			MV_Distance->TextSize = 13;
			MV_Distance->Offset = ImVec2(-7, -12);
			MV_Distance->IsText = true;
			MV_Distance->Name = "MV_Distance";
			MV_Distance->Color1 = Color(255, 255, 255, 255);

			/*=====================================================================================*/
			MV_Money->Position = ImVec2(StartBoxPos.x + BoxSize_X, StartBoxPos.y);
			MV_Money->StartPos = MV_Money->Position;
			MV_Money->Id = MI_MONEY_TEXT;
			MV_Money->Side = RIGHT_S;
			MV_Money->TextSize = 13;
			MV_Money->Offset = ImVec2(-7, 0);
			MV_Money->IsText = true;
			MV_Money->Name = "MV_Money";
			MV_Money->Color1 = Color(255, 255, 255, 255);

			/*=====================================================================================*/
			MV_Scope->Position = ImVec2(StartBoxPos.x + BoxSize_X, StartBoxPos.y);
			MV_Scope->StartPos = MV_Scope->Position;
			MV_Scope->Id = MI_SCOPE_TEXT;
			MV_Scope->Side = RIGHT_S;
			MV_Scope->TextSize = 13;
			MV_Scope->Offset = ImVec2(-7, -12);
			MV_Scope->IsText = true;
			MV_Scope->Name = "MV_Scope";
			MV_Scope->Color1 = Color(255, 255, 255, 255);

			/*=====================================================================================*/
			MV_Flashed->Position = ImVec2(StartBoxPos.x + BoxSize_X, StartBoxPos.y);
			MV_Flashed->StartPos = MV_Flashed->Position;
			MV_Flashed->Id = MI_FLASHED_TEXT;
			MV_Flashed->Side = RIGHT_S;
			MV_Flashed->TextSize = 13;
			MV_Flashed->Color1 = Color(255, 255, 255, 255);
			MV_Flashed->Offset = ImVec2(-7, -50);
			MV_Flashed->IsText = true;
			MV_Flashed->Name = "MV_Flashed";

			/*=====================================================================================*/
			MV_Defusing->Position = ImVec2(StartBoxPos.x + BoxSize_X, StartBoxPos.y);
			MV_Defusing->StartPos = MV_Defusing->Position;
			MV_Defusing->Id = MI_DEFUSING_TEXT;
			MV_Defusing->Side = RIGHT_S;
			MV_Defusing->TextSize = 13;
			MV_Defusing->Color1 = Color(255, 255, 255, 255);
			MV_Defusing->Offset = ImVec2(-7, -24);
			MV_Defusing->IsText = true;
			MV_Defusing->Name = "MV_Defusing";

			MoveStruct::InitMItems = true;
		}
		if (config_system.g_cfg.player.type[player].filledbox)
		{
			Color color = config_system.g_cfg.player.type[player].filledboxcolor;

			if (config_system.g_cfg.player.type[player].box_type < 2)
			{
				ImGuiWindow* window = ImGui::GetCurrentWindow();
				window->DrawList->AddRectFilledMultiColor(StartBoxPos, ImVec2(StartBoxPos.x + BoxSize_X, StartBoxPos.y + BoxSize_Y),
					Color(color.r(), color.g(), color.b(), 0).u32(), Color(color.r(), color.g(), color.b(), 0).u32(), color.u32(), color.u32());
			}
			if (config_system.g_cfg.player.type[player].box_type == 2)
			{
				Draw->AddRectFilled(StartBoxPos, ImVec2(StartBoxPos.x + BoxSize_X, StartBoxPos.y + BoxSize_Y), color.u32(), config_system.g_cfg.player.type[player].rouding);
			}
		}

	if (config_system.g_cfg.player.type[player].box)
	{
		Color BoxOutLineColor = Color(0.f, 0.f, 0.f, 255.f);
		if (config_system.g_cfg.player.type[player].box_type == 0)
		{
			Draw->AddRect(StartBoxPos, ImVec2(StartBoxPos.x + BoxSize_X, StartBoxPos.y + BoxSize_Y), config_system.g_cfg.player.type[player].box_color.u32());

			Draw->AddRect(ImVec2(StartBoxPos.x + 1, StartBoxPos.y + 1), ImVec2(StartBoxPos.x + BoxSize_X - 0.9f, StartBoxPos.y + BoxSize_Y - 0.9f), BoxOutLineColor.u32());
			Draw->AddRect(ImVec2(StartBoxPos.x - 1, StartBoxPos.y - 1), ImVec2(StartBoxPos.x + BoxSize_X + 0.9f, StartBoxPos.y + BoxSize_Y + 0.9f), BoxOutLineColor.u32());
		}
		else if (config_system.g_cfg.player.type[player].box_type == 1)
		{
			DrawVBoxCorner(StartBoxPos, ImVec2(BoxSize_X, BoxSize_Y), config_system.g_cfg.player.type[player].box_color);

			DrawVBoxCorner(ImVec2(StartBoxPos.x + 1, StartBoxPos.y + 1), ImVec2(BoxSize_X - 2, BoxSize_Y - 2), BoxOutLineColor);
			DrawVBoxCorner(ImVec2(StartBoxPos.x - 1, StartBoxPos.y - 1), ImVec2(BoxSize_X + 2, BoxSize_Y + 2), BoxOutLineColor);

			int LSize_X = BoxSize_X / 4;
			int LSize_Y = BoxSize_Y / 4;

			Draw->AddLine(ImVec2(StartBoxPos.x + LSize_X + 0.5f, StartBoxPos.y - 1), ImVec2(StartBoxPos.x + LSize_X + 0.5f, StartBoxPos.y + 1), BoxOutLineColor.u32());
			Draw->AddLine(ImVec2(StartBoxPos.x - 1, StartBoxPos.y + LSize_Y + 0.5f), ImVec2(StartBoxPos.x + 1, StartBoxPos.y + LSize_Y + 0.5f), BoxOutLineColor.u32());

			Draw->AddLine(ImVec2(StartBoxPos.x + BoxSize_X - LSize_X - 0.5f, StartBoxPos.y - 1), ImVec2(StartBoxPos.x + BoxSize_X - LSize_X - 0.5f, StartBoxPos.y + 1), BoxOutLineColor.u32());
			Draw->AddLine(ImVec2(StartBoxPos.x + BoxSize_X - 1, StartBoxPos.y + LSize_Y + 0.5f), ImVec2(StartBoxPos.x + BoxSize_X + 1, StartBoxPos.y + LSize_Y + 0.5f), BoxOutLineColor.u32());

			Draw->AddLine(ImVec2(StartBoxPos.x + LSize_X + 0.5f, StartBoxPos.y + BoxSize_Y - 1), ImVec2(StartBoxPos.x + LSize_X + 0.5f, StartBoxPos.y + BoxSize_Y + 1), BoxOutLineColor.u32());
			Draw->AddLine(ImVec2(StartBoxPos.x - 1, StartBoxPos.y + BoxSize_Y - LSize_Y - 0.5f), ImVec2(StartBoxPos.x + 1, StartBoxPos.y + BoxSize_Y - LSize_Y - 0.5f), BoxOutLineColor.u32());

			Draw->AddLine(ImVec2(StartBoxPos.x + BoxSize_X - LSize_X - 0.5f, StartBoxPos.y + BoxSize_Y - 1), ImVec2(StartBoxPos.x + BoxSize_X - LSize_X - 0.5f, StartBoxPos.y + BoxSize_Y + 1), BoxOutLineColor.u32());
			Draw->AddLine(ImVec2(StartBoxPos.x + BoxSize_X - 1, StartBoxPos.y + BoxSize_Y - LSize_Y - 0.5f), ImVec2(StartBoxPos.x + BoxSize_X + 1, StartBoxPos.y + BoxSize_Y - LSize_Y - 0.5f), BoxOutLineColor.u32());
		}
		else if (config_system.g_cfg.player.type[player].box_type == 2)
		{
			Draw->AddRect(StartBoxPos, ImVec2(StartBoxPos.x + BoxSize_X, StartBoxPos.y + BoxSize_Y), config_system.g_cfg.player.type[player].box_color.u32(), config_system.g_cfg.player.type[player].rouding);

			Draw->AddRect(ImVec2(StartBoxPos.x + 1, StartBoxPos.y + 1), ImVec2(StartBoxPos.x + BoxSize_X - 0.9f, StartBoxPos.y + BoxSize_Y - 0.9f), BoxOutLineColor.u32(), config_system.g_cfg.player.type[player].rouding);
			Draw->AddRect(ImVec2(StartBoxPos.x - 1, StartBoxPos.y - 1), ImVec2(StartBoxPos.x + BoxSize_X + 0.9f, StartBoxPos.y + BoxSize_Y + 0.9f), BoxOutLineColor.u32(), config_system.g_cfg.player.type[player].rouding);

		}
	}



	auto ChangeSidePos = [StartBoxPos](MoveStruct* Curt, int fix) -> ImVec2
	{
		switch (Curt->Side)
		{
		case LEFT_S:  return ImVec2(StartBoxPos.x - fix - Curt->CorrectPadding.x, StartBoxPos.y);
		case RIGHT_S: return ImVec2(StartBoxPos.x + BoxSize_X + Curt->CorrectPadding.x, StartBoxPos.y);
		case TOP_S:   return ImVec2(StartBoxPos.x, StartBoxPos.y - fix - Curt->CorrectPadding.y);
		case DOWN_S:  return ImVec2(StartBoxPos.x, StartBoxPos.y + BoxSize_Y + Curt->CorrectPadding.y);
		default: return ImVec2(0, 0);
		}
	};

	auto ChangeSidePosText = [StartBoxPos](MoveStruct* Curt, ImVec2 Size) -> ImVec2
	{
		switch (Curt->Side)
		{
		case LEFT_S: return ImVec2(StartBoxPos.x - Size.x - Curt->CorrectPadding.x, StartBoxPos.y + Curt->CorrectPadding.y);
		case RIGHT_S:return ImVec2(StartBoxPos.x + BoxSize_X + Curt->CorrectPadding.x, StartBoxPos.y + Curt->CorrectPadding.y);
		case TOP_S:  return ImVec2(StartBoxPos.x + (BoxSize_X / 2) - (Size.x / 2), StartBoxPos.y - Size.y - Curt->CorrectPadding.y);
		case DOWN_S: return ImVec2(StartBoxPos.x + (BoxSize_X / 2) - (Size.x / 2), StartBoxPos.y + BoxSize_Y + Curt->CorrectPadding.y);
		case LEFT_TOP_S:  return ImVec2(StartBoxPos.x, StartBoxPos.y - Size.y - Curt->CorrectPadding.y);
		case RIGHT_TOP_S: return ImVec2(StartBoxPos.x + BoxSize_X - Size.x, StartBoxPos.y - Size.y - Curt->CorrectPadding.y);
		case LEFT_DOWN_S: return ImVec2(StartBoxPos.x, StartBoxPos.y + BoxSize_Y + Curt->CorrectPadding.y);
		case RIGHT_DOWN_S:return ImVec2(StartBoxPos.x + BoxSize_X - Size.x, StartBoxPos.y + BoxSize_Y + Curt->CorrectPadding.y);
		case CENTER_LEFT_S:    return ImVec2(StartBoxPos.x - Size.x - Curt->CorrectPadding.x, StartBoxPos.y + (BoxSize_Y / 2) + Curt->CorrectPadding.y);
		case CENTER_RIGHT_S:   return ImVec2(StartBoxPos.x + BoxSize_X + Curt->CorrectPadding.x, StartBoxPos.y + (BoxSize_Y / 2) + Curt->CorrectPadding.y);
		case DOWN_LEFT_DOWN_S: return ImVec2(StartBoxPos.x - Size.x - Curt->CorrectPadding.x, StartBoxPos.y + BoxSize_Y - Size.y + Curt->CorrectPadding.y);
		case DOWN_RIGHT_DOWN_S:return ImVec2(StartBoxPos.x + BoxSize_X + Curt->CorrectPadding.x, StartBoxPos.y + BoxSize_Y - Size.y + Curt->CorrectPadding.y);
		default: return ImVec2(0, 0);
		}
	};

	ImVec2 HPStartPos = ChangeSidePos(MV_Hp, HPTol);
	ImVec2 ArmorStartPos = ChangeSidePos(MV_Armor, ATol);

	ImVec2 ArmorTextStartPos = ChangeSidePosText(MV_Armor_Text, MV_Armor_Text->Size);
	ImVec2 HPTextStartPos = ChangeSidePosText(MV_Hp_Text, MV_Hp_Text->Size);

	ImVec2 NameStartPos = ChangeSidePosText(MV_Name, MV_Name->Size);
	ImVec2 WeaponStartPos = ChangeSidePosText(MV_Weapon, MV_Weapon->Size);
	ImVec2 AmmoStartPos = ChangeSidePosText(MV_Ammo, MV_Ammo->Size);
	ImVec2 DistanceStartPos = ChangeSidePosText(MV_Distance, MV_Distance->Size);
	ImVec2 MoneyStartPos = ChangeSidePosText(MV_Money, MV_Money->Size);
	ImVec2 DefusingStartPos = ChangeSidePosText(MV_Defusing, MV_Defusing->Size);
	ImVec2 ScopeStartPos = ChangeSidePosText(MV_Scope, MV_Scope->Size);
	ImVec2 FlashedStartPos = ChangeSidePosText(MV_Flashed, MV_Flashed->Size);

	MV_Hp->SizeV = ImVec2(HPTol, BoxSize_Y);
	MV_Armor->SizeV = ImVec2(ATol, BoxSize_Y);

	MV_Hp->SizeH = ImVec2(BoxSize_X, HPTol);
	MV_Armor->SizeH = ImVec2(BoxSize_X, ATol);

	auto IsPosTouch = [](MoveStruct& Curt, ImVec2 Pos) -> bool
	{
		if ((Pos.x > Curt.Position.x && Pos.x < Curt.Position.x + Curt.Size.x) &&
			(Pos.y > Curt.Position.y && Pos.y < Curt.Position.y + Curt.Size.y))
		{
			return true;
		}
		return false;
	};

	auto CalcPadding = [&](MoveStruct& Curt) -> void
	{
		bool CheckVert = true;
		bool CheckHor = true;

		Curt.CorrectPadding = ImVec2(0, 0);

		int TraceLenX = 25;
		int TraceLenY = 30;

		if (Curt.Side == RIGHT_S || Curt.Side == CENTER_RIGHT_S || Curt.Side == DOWN_RIGHT_DOWN_S)
		{
			for (MoveStruct MV : MVItemList)
			{
				if (MV.IsEnabled && Curt.IsEnabled && MV.Id != Curt.Id && (CheckHor || CheckVert) &&
					(MoveStruct::LastActive == Curt.Id || MoveStruct::LastActive == MV.Id ? !IsPosTouch(MV, Curt.Center) : true) &&
					(MV.Side != TOP_S && MV.Side != LEFT_TOP_S && MV.Side != RIGHT_TOP_S))
				{
					if (CheckVert)
					{
						for (int i(-3); i < TraceLenY; i++)
						{
							if (IsPosTouch(MV, ImVec2(Curt.Center.x, Curt.Center.y - (Curt.Size.y / 2) - i)))
							{
								Curt.CorrectPadding.y = MV.CorrectPadding.y + MV.Size.y;
								CheckVert = false;
							}
						}
					}

					if (CheckHor && (!MV.IsText ? true : Curt.Center.y > MV.Center.y))
					{
						for (int i(CheckVert ? -3 : 1); i < TraceLenX; i++)
						{
							if (IsPosTouch(MV, ImVec2(Curt.Center.x - (Curt.Size.x / 2) - i, Curt.Center.y)))
							{
								Curt.CorrectPadding.x = MV.CorrectPadding.x + MV.Size.x;
								CheckHor = false;
							}
						}
					}
				}
			}
		}

		if (Curt.Side == LEFT_S || Curt.Side == CENTER_LEFT_S || Curt.Side == DOWN_LEFT_DOWN_S)
		{
			for (MoveStruct MV : MVItemList)
			{
				if (MV.IsEnabled && Curt.IsEnabled && MV.Id != Curt.Id && (CheckHor || CheckVert) &&
					(MoveStruct::LastActive == Curt.Id || MoveStruct::LastActive == MV.Id ? !IsPosTouch(MV, Curt.Center) : true) &&
					(MV.Side != TOP_S && MV.Side != LEFT_TOP_S && MV.Side != RIGHT_TOP_S))
				{
					if (CheckVert)
					{
						for (int i(-3); i < TraceLenY; i++)
						{
							if (IsPosTouch(MV, ImVec2(Curt.Center.x, Curt.Center.y - (Curt.Size.y / 2) - i)))
							{
								Curt.CorrectPadding.y = MV.CorrectPadding.y + MV.Size.y;
								CheckVert = false;
							}
						}
					}

					if (CheckHor && (!MV.IsText ? true : Curt.Center.y > MV.Center.y))
					{
						for (int i(CheckVert ? -3 : 1); i < TraceLenX; i++)
						{
							if (IsPosTouch(MV, ImVec2(Curt.Center.x + (Curt.Size.x / 2) + i, Curt.Center.y)))
							{
								Curt.CorrectPadding.x = MV.CorrectPadding.x + MV.Size.x;
								CheckHor = false;
							}
						}
					}
				}
			}
		}

		if (Curt.Side == TOP_S || Curt.Side == LEFT_TOP_S || Curt.Side == RIGHT_TOP_S)
		{
			for (MoveStruct MV : MVItemList)
			{
				if (MV.IsEnabled && Curt.IsEnabled && MV.Id != Curt.Id && !IsPosTouch(MV, Curt.Center) &&
					(MV.Side != LEFT_S && MV.Side != CENTER_LEFT_S && MV.Side != DOWN_LEFT_DOWN_S && MV.Side != RIGHT_S && MV.Side != CENTER_RIGHT_S && MV.Side != DOWN_RIGHT_DOWN_S))
				{
					for (int i(-3); i < TraceLenY; i++)
					{
						if (IsPosTouch(MV, ImVec2(Curt.Center.x, Curt.Center.y + (Curt.Size.y / 2) + i)))
						{
							Curt.CorrectPadding.y = MV.CorrectPadding.y + MV.Size.y;
							return;
						}
					}
				}
			}
		}

		if (Curt.Side == DOWN_S || Curt.Side == LEFT_DOWN_S || Curt.Side == RIGHT_DOWN_S)
		{
			for (MoveStruct MV : MVItemList)
			{
				if (MV.IsEnabled && Curt.IsEnabled && MV.Id != Curt.Id && !IsPosTouch(MV, Curt.Center) &&
					(MV.Side != LEFT_S && MV.Side != CENTER_LEFT_S && MV.Side != DOWN_LEFT_DOWN_S && MV.Side != RIGHT_S && MV.Side != CENTER_RIGHT_S && MV.Side != DOWN_RIGHT_DOWN_S))
				{
					for (int i(-3); i < TraceLenY; i++)
					{
						if (IsPosTouch(MV, ImVec2(Curt.Center.x, Curt.Center.y - (Curt.Size.y / 2) - i)))
						{
							Curt.CorrectPadding.y = MV.Size.y;
							return;
						}
					}
				}
			}
		}
	};

	if (config_system.g_cfg.player.type[player].health)
	{
		MV_Hp->Color1 = config_system.g_cfg.player.type[player].custom_health_color ? config_system.g_cfg.player.type[player].health_color : Color(30, 255, 30);
		MV_Hp->Color2 = Color(0, 0, 0, 100);
		MV_Hp->Color3 = Color(0, 0, 0, 100);
		Move(*MV_Hp, IsMousPress, MousePos, HPStartPos, CLines);
		if (MV_Hp->Side == LEFT_S || MV_Hp->Side == RIGHT_S)
			DrawBarVertical(MV_Hp->Position, MV_Hp->Size, MV_Hp->Color1, MV_Hp->Color2, MV_Hp->Color3, MV_Hp->NumParam1, MV_Hp);
		else
			DrawBarHorizontal(MV_Hp->Position, MV_Hp->Size, MV_Hp->Color1, MV_Hp->Color2, MV_Hp->Color3, MV_Hp->NumParam1, MV_Hp);
	}
	if (config_system.g_cfg.player.type[player].ammo)
	{
		MV_Armor->Color1 = config_system.g_cfg.player.type[player].ammobar_color;
		MV_Armor->Color2 = Color(0, 0, 0, 100);
		MV_Armor->Color3 = Color(0, 0, 0, 100);

			Move(*MV_Armor, IsMousPress, MousePos, ArmorStartPos, CLines);
			if (MV_Armor->Side == LEFT_S || MV_Armor->Side == RIGHT_S)
				DrawBarVertical(MV_Armor->Position, MV_Armor->Size, MV_Armor->Color1, MV_Armor->Color2, MV_Armor->Color3, MV_Armor->NumParam1, MV_Armor);
			else
				DrawBarHorizontal(MV_Armor->Position, MV_Armor->Size, MV_Armor->Color1, MV_Armor->Color2, MV_Armor->Color3, MV_Armor->NumParam1, MV_Armor);

	}

	if (config_system.g_cfg.player.type[player].name)
	{
		MV_Name->Color1 = config_system.g_cfg.player.type[player].name_color;

		Move(*MV_Name, IsMousPress, MousePos, NameStartPos, CLines, true);
		_DrawText("Name", 13, MV_Name->Position, MV_Name, c_menu::get().futura_large);
	}
	

	if (config_system.g_cfg.player.type[player].weapon_icon)
	{
		Move(*MV_Weapon, IsMousPress, MousePos, WeaponStartPos, CLines, true);

		MV_Weapon->Color1 = config_system.g_cfg.player.type[player].weapon_color;

	//	_DrawText("W", 20, MV_Weapon->Position, MV_Weapon, c_menu::get().ico_menu);
		g_Render->DrawString(MV_Weapon->Position.x, MV_Weapon->Position.y, MV_Weapon->Color1, render2::centered_x | render2::outline, c_menu::get().ico_menu, "W");
	}

	if (config_system.g_cfg.player.type[player].FLAGS_MONEY)
	{
		Move(*MV_Money, IsMousPress, MousePos, MoneyStartPos, CLines, true);
		MV_Money->Color1 = Color(70,200,70);
		g_Render->DrawString(MV_Money->Position.x, MV_Money->Position.y, MV_Money->Color1, render2::none | render2::outline, c_menu::get().futura_small, "1000$");
	}

	if (config_system.g_cfg.player.type[player].FLAGS_ARMOR)
	{
		Move(*MV_Hp_Text, IsMousPress, MousePos, HPTextStartPos, CLines, true);
		MV_Hp_Text->Color1 = config_system.g_cfg.player.type[player].Armor;

		g_Render->DrawString(MV_Hp_Text->Position.x, MV_Hp_Text->Position.y, MV_Hp_Text->Color1, render2::none | render2::outline, c_menu::get().futura_small, "HK");
	}

	if (config_system.g_cfg.player.type[player].FLAGS_KIT)
	{
		Move(*MV_Defusing, IsMousPress, MousePos, DefusingStartPos, CLines, true);
		MV_Defusing->Color1 = config_system.g_cfg.player.type[player].Defuse;

		g_Render->DrawString(MV_Defusing->Position.x, MV_Defusing->Position.y, MV_Defusing->Color1, render2::none | render2::outline, c_menu::get().futura_small, "DEFUSE");
	}

	if (config_system.g_cfg.player.type[player].FLAGS_SCOPED)
	{
		Move(*MV_Scope, IsMousPress, MousePos, ScopeStartPos, CLines, true);
		MV_Scope->Color1 = config_system.g_cfg.player.type[player].Scoped;

		g_Render->DrawString(MV_Scope->Position.x, MV_Scope->Position.y, MV_Scope->Color1, render2::none | render2::outline, c_menu::get().futura_small, MV_SCOPE_TEXT);
	}

	if (config_system.g_cfg.player.type[player].FLAGS_C4)
	{
		Move(*MV_Flashed, IsMousPress, MousePos, FlashedStartPos, CLines, true);
		MV_Flashed->Color1 = config_system.g_cfg.player.type[player].BombCarrie;

		g_Render->DrawString(MV_Flashed->Position.x, MV_Flashed->Position.y, MV_Flashed->Color1, render2::none | render2::outline, c_menu::get().futura_small, "BOMB");

	}

	if (MoveStruct::LastActive != -1 && (MITypes)MoveStruct::LastActive <= MI_MAX && MoveStruct::ActiveIdx != -1)
	{
		MoveStruct* Selected = &MVItemList[MoveStruct::LastActive];

		Color SelColor = Color(0, 255, 255, 102);
		ImVec2 SP = Selected->Position;

		int Correct = 0;

		if (!Selected->IsText)
			Correct = 1;

	}

	auto CalcVisOffset = [StartBoxPos](MoveStruct& Curt, bool IsText = false)
	{
		float DisttX = 0.000000001;
		float DisttY = 0.000000001;

		if (!IsText)
		{
			if (Curt.Side == LEFT_S)
			{
				DisttX = ((StartBoxPos.x - Curt.CorrectPadding.x) - (Curt.Position.x + Curt.Size.x));
				DisttY = (StartBoxPos.y - Curt.Position.y);
			}
			else if (Curt.Side == RIGHT_S)
			{
				DisttX = ((StartBoxPos.x + Curt.CorrectPadding.x) - Curt.Position.x);
				DisttY = (StartBoxPos.y - Curt.Position.y);
			}
			else if (Curt.Side == TOP_S)
			{
				DisttX = (StartBoxPos.x - Curt.Position.x);
				DisttY = ((StartBoxPos.y - Curt.CorrectPadding.y) - Curt.Position.y - Curt.Size.y);
			}
			else if (Curt.Side == DOWN_S)
			{
				DisttX = (StartBoxPos.x - Curt.Position.x);
				DisttY = ((StartBoxPos.y - Curt.CorrectPadding.y) - Curt.Position.y - Curt.Size.y);
			}
		}
		else
		{
			if (Curt.Side == LEFT_S)
			{
				DisttX = ((StartBoxPos.x - Curt.CorrectPadding.x - Curt.Size.x) - (Curt.Position.x));
				DisttY = ((StartBoxPos.y + Curt.CorrectPadding.y) - Curt.Position.y);
			}
			else if (Curt.Side == RIGHT_S)
			{
				DisttX = ((StartBoxPos.x + BoxSize_X + Curt.CorrectPadding.x) - Curt.Position.x);
				DisttY = ((StartBoxPos.y + Curt.CorrectPadding.y) - Curt.Position.y);
			}
			else if (Curt.Side == TOP_S)
			{
				DisttX = (StartBoxPos.x - Curt.Position.x + (BoxSize_X / 2) - (Curt.Size.x / 2));
				DisttY = ((StartBoxPos.y - Curt.CorrectPadding.y) - Curt.Position.y - Curt.Size.y);
			}
			else if (Curt.Side == DOWN_S)
			{
				DisttX = (StartBoxPos.x - Curt.Position.x + (BoxSize_X / 2) - (Curt.Size.x / 2));
				DisttY = ((StartBoxPos.y + BoxSize_Y + Curt.CorrectPadding.y) - Curt.Position.y);
			}
		}

		if (Curt.Side == LEFT_TOP_S)
		{
			DisttX = (StartBoxPos.x - Curt.Position.x);
			DisttY = ((StartBoxPos.y - Curt.Size.y - Curt.CorrectPadding.y) - Curt.Position.y);
		}
		else if (Curt.Side == RIGHT_TOP_S)
		{
			DisttX = ((StartBoxPos.x + BoxSize_X) - (Curt.Position.x + Curt.Size.x));
			DisttY = ((StartBoxPos.y - Curt.Size.y - Curt.CorrectPadding.y) - Curt.Position.y);
		}
		else if (Curt.Side == LEFT_DOWN_S)
		{
			DisttX = (StartBoxPos.x - Curt.Position.x);
			DisttY = ((StartBoxPos.y + BoxSize_Y + Curt.CorrectPadding.y) - Curt.Position.y);
		}
		else if (Curt.Side == RIGHT_DOWN_S)
		{
			DisttX = ((StartBoxPos.x + BoxSize_X - Curt.Size.x) - Curt.Position.x);
			DisttY = ((StartBoxPos.y + BoxSize_Y + Curt.CorrectPadding.y) - Curt.Position.y);
		}
		else if (Curt.Side == CENTER_LEFT_S)
		{
			DisttX = ((StartBoxPos.x - Curt.CorrectPadding.x - Curt.Size.x) - (Curt.Position.x));
			DisttY = ((StartBoxPos.y + (BoxSize_Y / 2) + Curt.CorrectPadding.y) - Curt.Position.y);
		}
		else if (Curt.Side == CENTER_RIGHT_S)
		{
			DisttX = ((StartBoxPos.x + BoxSize_X + Curt.CorrectPadding.y) - Curt.Position.x);
			DisttY = ((StartBoxPos.y + (BoxSize_Y / 2) + Curt.CorrectPadding.y) - Curt.Position.y);
		}
		else if (Curt.Side == DOWN_LEFT_DOWN_S)
		{
			DisttX = ((StartBoxPos.x - Curt.CorrectPadding.x - Curt.Size.x) - (Curt.Position.x));
			DisttY = ((StartBoxPos.y + BoxSize_Y - Curt.Size.y + Curt.CorrectPadding.y) - (Curt.Position.y));
		}
		else if (Curt.Side == DOWN_RIGHT_DOWN_S)
		{
			DisttX = ((StartBoxPos.x + BoxSize_X + Curt.CorrectPadding.x) - Curt.Position.x);
			DisttY = ((StartBoxPos.y + BoxSize_Y - Curt.Size.y + Curt.CorrectPadding.y) - (Curt.Position.y));
		}

		if (DisttX == 0)
			DisttX = 0.000000001;

		if (DisttY == 0)
			DisttY = 0.000000001;

		Curt.CorrectOfst.x = (BoxSize_X / DisttX);
		Curt.CorrectOfst.y = (BoxSize_Y / DisttY);
	};

	CalcPadding(*MV_Hp);
	CalcPadding(*MV_Armor);

	CalcPadding(*MV_Name);
	CalcPadding(*MV_Weapon);
	CalcPadding(*MV_Ammo);
	CalcPadding(*MV_Distance);
	CalcPadding(*MV_Money);
	CalcPadding(*MV_Scope);
	CalcPadding(*MV_Flashed);
	CalcPadding(*MV_Defusing);

	CalcVisOffset(*MV_Hp);
	CalcVisOffset(*MV_Armor);

	CalcVisOffset(*MV_Hp_Text, true);
	CalcVisOffset(*MV_Armor_Text, true);

	CalcVisOffset(*MV_Name, true);
	CalcVisOffset(*MV_Weapon, true);
	CalcVisOffset(*MV_Ammo, true);
	CalcVisOffset(*MV_Distance, true);
	CalcVisOffset(*MV_Money, true);
	CalcVisOffset(*MV_Scope, true);
	CalcVisOffset(*MV_Flashed, true);
	CalcVisOffset(*MV_Defusing, true);

	for (int i(0); (MITypes)i < MI_MAX; i++)
		MVItemList[i].ZonePos = ImVec2(MVItemList[i].Position.x - StartPosScreen.x, MVItemList[i].Position.y - StartPosScreen.y);

	ImGui::SetWindowPos(ImVec2(StartPos.x, StartPos.y + VIS_PREW_ZONE_Y + 3));
}


void CEsp::Move(MoveStruct& Cont, bool IsPressed, ImVec2 MouseP, ImVec2 StartP, ImVec2 CLines, bool IsText)
{
	ImGuiIO g = ImGui::GetIO();
	bool ChangeSide = false;
	if (MoveStruct::ActiveIdx == Cont.Id || MoveStruct::ActiveIdx == -1)
	{
		if (IsPressed && Cont.IsHover && Cont.Set)
		{
			MoveStruct::ActiveIdx = Cont.Id;
			MoveStruct::LastActive = Cont.Id;

			Cont.Offset.x = StartP.x - (MouseP.x - Cont.Correct.x);
			Cont.Offset.y = StartP.y - (MouseP.y - Cont.Correct.y);

			if (!IsText)
			{
				Sides OSide = Cont.Side;

				if (Cont.Center.x > CLines.x) { Cont.Side = RIGHT_S; }
				if (Cont.Center.x < CLines.x) { Cont.Side = LEFT_S; }

				if (g.MousePos.y < CLines.y + (BoxSize_Y / 8)) { Cont.Side = TOP_S; }
				if (g.MousePos.y > CLines.y + BoxSize_Y - (BoxSize_Y / 8)) { Cont.Side = DOWN_S; }

				if (OSide != Cont.Side && ((OSide != LEFT_S || Cont.Side != RIGHT_S) && (OSide != RIGHT_S || Cont.Side != LEFT_S)))
					ChangeSide = true;
			}
			else
			{
				if (Cont.Center.x > CLines.x) { Cont.Side = RIGHT_S; }
				if (Cont.Center.x < CLines.x) { Cont.Side = LEFT_S; }

				if (Cont.Center.x > CLines.x && Cont.Center.y > CLines.y + (BoxSize_Y / 3)) { Cont.Side = CENTER_RIGHT_S; }
				if (Cont.Center.x < CLines.x && Cont.Center.y > CLines.y + (BoxSize_Y / 3)) { Cont.Side = CENTER_LEFT_S; }

				if (Cont.Center.x > CLines.x && Cont.Center.y > CLines.y + BoxSize_Y - (BoxSize_Y / 3)) { Cont.Side = DOWN_RIGHT_DOWN_S; }
				if (Cont.Center.x < CLines.x && Cont.Center.y > CLines.y + BoxSize_Y - (BoxSize_Y / 3)) { Cont.Side = DOWN_LEFT_DOWN_S; }

				if (Cont.Center.y < CLines.y) { Cont.Side = TOP_S; }
				if (Cont.Center.y < CLines.y && Cont.Center.x < CLines.x - (BoxSize_X / 4)) { Cont.Side = LEFT_TOP_S; }
				if (Cont.Center.y < CLines.y && Cont.Center.x > CLines.x + (BoxSize_X / 4)) { Cont.Side = RIGHT_TOP_S; }

				if (Cont.Center.y > CLines.y + BoxSize_Y) { Cont.Side = DOWN_S; }
				if (Cont.Center.y > CLines.y + BoxSize_Y && Cont.Center.x < CLines.x - (BoxSize_X / 4)) { Cont.Side = LEFT_DOWN_S; }
				if (Cont.Center.y > CLines.y + BoxSize_Y && Cont.Center.x > CLines.x + (BoxSize_X / 4)) { Cont.Side = RIGHT_DOWN_S; }
			}
		}
		else
			MoveStruct::ActiveIdx = -1;

		Cont.Position = ImVec2(StartP.x - Cont.Offset.x, StartP.y - Cont.Offset.y);

		if ((MouseP.x < (Cont.Position.x + Cont.Size.x) && MouseP.x > Cont.Position.x) && (MouseP.y < (Cont.Position.y + Cont.Size.y) && MouseP.y > Cont.Position.y))
			Cont.IsHover = true;
		else
			Cont.IsHover = false;

		if (IsPressed && Cont.IsHover)
		{
			if (!Cont.Set && !IsBlockMove)
			{
				Cont.Set = true;
				Cont.Correct.x = MouseP.x - Cont.Position.x;
				Cont.Correct.y = MouseP.y - Cont.Position.y;
			}
		}
		else
			Cont.Set = false;

		Cont.Center = ImVec2(Cont.Position.x + Cont.Size.x / 2, Cont.Position.y + Cont.Size.y / 2);
		if (!IsText)
		{
			if (Cont.Side == DOWN_S || Cont.Side == TOP_S)
				Cont.Size = Cont.SizeH;

			if (Cont.Side == LEFT_S || Cont.Side == RIGHT_S)
				Cont.Size = Cont.SizeV;

			if (ChangeSide)
			{
				Cont.Center = MouseP;

				Cont.Offset.x = StartP.x - (MouseP.x - Cont.Size.x / 2);
				Cont.Offset.y = StartP.y - (MouseP.y - Cont.Size.y / 2);

				Cont.Position = ImVec2(StartP.x - Cont.Offset.x, StartP.y - Cont.Offset.y);

				Cont.Correct.x = MouseP.x - Cont.Position.x;
				Cont.Correct.y = MouseP.y - Cont.Position.y;
			}
		}
	}
}

void CEsp::InitVisuals()
{
	for (int i(0); (MITypes)i < MI_MAX; i++)
		MVItemList.push_back(MoveStruct());


	auto LInitVisuals = [&]()->void
	{

		static MoveStruct* MV_Hp = &MVItemList[MI_HEALTH_BAR];
		static MoveStruct* MV_Armor = &MVItemList[MI_ARMOR_BAR];

		static MoveStruct* MV_Hp_Text = &MVItemList[MI_HEALTH_TEXT];
		static MoveStruct* MV_Armor_Text = &MVItemList[MI_ARMOR_TEXT];

		static MoveStruct* MV_Name = &MVItemList[MI_NAME_TEXT];
		static MoveStruct* MV_Weapon = &MVItemList[MI_WEAPON_TEXT];
		static MoveStruct* MV_Ammo = &MVItemList[MI_AMMO_TEXT];
		static MoveStruct* MV_Distance = &MVItemList[MI_DISTANCE_TEXT];
		static MoveStruct* MV_Money = &MVItemList[MI_MONEY_TEXT];
		static MoveStruct* MV_Scope = &MVItemList[MI_SCOPE_TEXT];
		static MoveStruct* MV_Flashed = &MVItemList[MI_FLASHED_TEXT];
		static MoveStruct* MV_Defusing = &MVItemList[MI_DEFUSING_TEXT];

		if (!MoveStruct::InitMItems)
		{
			//MoveStruct::InitMItems = true;
			MoveStruct::LastActive = MI_MAX;

			/*=====================================================================================*/
			MV_Hp->StartPos = MV_Hp->Position;
			MV_Hp->Id = MI_HEALTH_BAR;
			MV_Hp->Side = LEFT_S;
			MV_Hp->Offset = ImVec2(5, 0);

			MV_Hp->Color1[0] = 0.1f;
			MV_Hp->Color1[1] = 1.0f;
			MV_Hp->Color1[2] = 0.1f;

			MV_Hp->Color2[0] = 1.0f;
			MV_Hp->Color2[1] = 0.0f;
			MV_Hp->Color2[2] = 0.0f;

			MV_Hp->Color3[0] = 0.0f;
			MV_Hp->Color3[1] = 0.0f;
			MV_Hp->Color3[2] = 0.0f;

			MV_Hp->NumParam1 = 4;
			MV_Hp->NumParam2 = 5;
			MV_Hp->Name = "MV_HPBar";
			MV_Hp->boolParam1 = true;
			MV_Hp->boolParam2 = true;
			MV_Hp->boolParam3 = false;

			/*=====================================================================================*/
			MV_Armor->StartPos = MV_Armor->Position;
			MV_Armor->Id = MI_ARMOR_BAR;
			MV_Armor->Side = DOWN_S;
			MV_Armor->Offset = ImVec2(-5, 0);

			MV_Armor->Color1[0] = 1.0f;
			MV_Armor->Color1[1] = 1.0f;
			MV_Armor->Color1[2] = 1.0f;

			MV_Armor->Color2[0] = 1.0f;
			MV_Armor->Color2[1] = 0.0f;
			MV_Armor->Color2[2] = 0.0f;

			MV_Armor->Color3[0] = 0.0f;
			MV_Armor->Color3[1] = 0.0f;
			MV_Armor->Color3[2] = 0.0f;

			MV_Armor->NumParam1 = 4;
			MV_Armor->NumParam2 = 5;
			MV_Armor->Name = "MV_ArmorBar";
			MV_Armor->boolParam1 = true;
			MV_Armor->boolParam2 = true;
			MV_Armor->boolParam3 = false;

			/*=====================================================================================*/
			MV_Name->StartPos = MV_Name->Position;
			MV_Name->Id = MI_NAME_TEXT;
			MV_Name->Side = TOP_S;
			MV_Name->TextSize = 17;
			MV_Name->Offset = ImVec2(0, 4);
			MV_Name->IsText = true;
			MV_Name->Name = "MV_Name";
			MV_Name->boolParam1 = false;
			MV_Name->Color2[0] = 0.09f;
			MV_Name->Color2[1] = 0.09f;
			MV_Name->Color2[2] = 0.09f;

			/*=====================================================================================*/
			MV_Weapon->StartPos = MV_Weapon->Position;
			MV_Weapon->Id = MI_WEAPON_TEXT;
			MV_Weapon->Side = DOWN_S;
			MV_Weapon->TextSize = 17;
			MV_Weapon->Offset = ImVec2(0, -1);
			MV_Weapon->IsText = true;
			MV_Weapon->Name = "MV_Weapon";

			/*=====================================================================================*/
			MV_Ammo->StartPos = MV_Ammo->Position;
			MV_Ammo->Id = MI_AMMO_TEXT;
			MV_Ammo->Side = DOWN_S;
			MV_Ammo->TextSize = 17;
			MV_Ammo->Offset = ImVec2(0, -10);
			MV_Ammo->IsText = true;
			MV_Ammo->Name = "MV_Ammo";
			MV_Ammo->Color1[0] = 1.0f;
			MV_Ammo->Color1[1] = 1.0f;
			MV_Ammo->Color1[2] = 1.0f;

			/*=====================================================================================*/
			MV_Distance->StartPos = MV_Distance->Position;
			MV_Distance->Id = MI_DISTANCE_TEXT;
			MV_Distance->Side = DOWN_RIGHT_DOWN_S;
			MV_Distance->TextSize = 17;
			MV_Distance->Offset = ImVec2(-7, 0);
			MV_Distance->IsText = true;
			MV_Distance->Name = "MV_Distance";

			/*=====================================================================================*/
			MV_Money->StartPos = MV_Money->Position;
			MV_Money->Id = MI_MONEY_TEXT;
			MV_Money->Side = RIGHT_S;
			MV_Money->TextSize = 17;
			MV_Money->Color1[0] = 1.0f;
			MV_Money->Color1[1] = 1.0f;
			MV_Money->Color1[2] = 1.0f;
			MV_Money->Offset = ImVec2(-7, 0);
			MV_Money->IsText = true;
			MV_Money->Name = "MV_Money";

			/*=====================================================================================*/
			MV_Scope->StartPos = MV_Scope->Position;
			MV_Scope->Id = MI_SCOPE_TEXT;
			MV_Scope->Side = RIGHT_S;
			MV_Scope->TextSize = 17;
			MV_Scope->Color1[0] = 1.0f;
			MV_Scope->Color1[1] = 1.0f;
			MV_Scope->Color1[2] = 1.0f;
			MV_Scope->Offset = ImVec2(-7, -7);
			MV_Scope->IsText = true;
			MV_Scope->Name = "MV_Scope";

			/*=====================================================================================*/
			MV_Flashed->StartPos = MV_Flashed->Position;
			MV_Flashed->Id = MI_FLASHED_TEXT;
			MV_Flashed->Side = CENTER_RIGHT_S;
			MV_Flashed->TextSize = 17;
			MV_Flashed->Color1[0] = 1.0f;
			MV_Flashed->Color1[1] = 1.0f;
			MV_Flashed->Color1[2] = 1.0f;
			MV_Flashed->Offset = ImVec2(-7, 0);
			MV_Flashed->IsText = true;
			MV_Flashed->Name = "MV_Flashed";

			/*=====================================================================================*/
			MV_Defusing->StartPos = MV_Defusing->Position;
			MV_Defusing->Id = MI_DEFUSING_TEXT;
			MV_Defusing->Side = CENTER_RIGHT_S;
			MV_Defusing->TextSize = 17;
			MV_Defusing->Color1[0] = 1.0f;
			MV_Defusing->Color1[1] = 1.0f;
			MV_Defusing->Color1[2] = 1.0f;
			MV_Defusing->Offset = ImVec2(-7, -7);
			MV_Defusing->IsText = true;
			MV_Defusing->Name = "MV_Defusing";
		}
	};
	LInitVisuals();
}
