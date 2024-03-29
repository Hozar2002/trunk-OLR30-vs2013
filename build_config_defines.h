#pragma once
// ==================================== ������ �� alpet ======================================= 
#define HLAMP_AFFECT_IMMUNITIES					// �������� ��������� ����������� ��� ������� ���� (CHangingLamp)
// #define LUAICP_COMPAT					// �������� ������������� � ������������� Lua (luaicp.dll). �������� ���� � ��������� 
#define SCRIPT_ICONS_CONTROL		    // �������� ������� ������� setup_game_icon � Lua
#define SCRIPT_EZI_CONTROL				// �������� ������� ������ CEffectorZoomInertion ��� ���������� ��������� ������� 
#define SPAWN_ANTIFREEZE				// ������������ �������� ����� �������� �� ������, ����� ��������� ��������������� ����� 
#define ECO_RENDER						// ��������� ��������� �������� ����� ����������� ������, ����� ��� �� ���������� ���������� ��� ������� FPS
//#define MT_OPT							// ������ ���������� ���� �� ��� ������� ������
//#define AF_SHOW_DYNAMIC_PARAMS			// ��� ���������� ����� ���������� ����������� ��������, � �� �� ����� ������������
#define SCRIPT_VARS_STORAGE				// �������� ��������� ���������� ���������� � �����

//#define OBJECTS_RADIOACTIVE // ����� ��� ����������������
// ==================================== ������ �� K.D. =======================================
#define KD_DETAIL_RADIUS				// alpet: �������� ������ ��������� ��������� ����� � ���������. ����� ������ �� ������������������ ��� ������ ������.

// ==================================== ������ �� Real Wolf ======================================= 
//#define INV_RUCK_UNLIMITED_FIX		// (������ �������� ��� ��� ������ � ����!!!) Real Wolf: ��������� ������������� ������������ ������� unlimited � ���� dragdrop_bag ��� �������� ��������� ��������� (������ �������� ��� ��� ������ � ����!!!)
#define INV_NEW_SLOTS_SYSTEM			// Real Wolf, Red Virus: �������� �����.
#define SUN_DIR_NOT_DEBUG			    // Real Wolf: ���������� ������ � ��� ���������� ���� CurrentEnv.sun_dir...
// #define ARTEFACTS_FROM_RUCK				// Real Wolf: ��������� ����� �������� �� ����� ��������� (������, �����, �������).
#define KNIFE_SPRINT_FIX				// Real Wolf: ��������� ������� ��� ��� �����2.
#define KNIFE_SPRINT_MOTION				// Real Wolf: �������������� �������� ���� ��� ����.
#define MISSILE_THREAT_FIX				// Real Wolf: ��������� ������� ��� ������ �������, ����� � �.�.
#define GRENADE_FROM_BELT					// Real Wolf: ������� � �����.
// #define LOCK_RELOAD_IN_SPRINT			// Real Wolf: ���������� ����������� �� ����� �������.
#define HIDE_WEAPON_IN_CAR				// Real Wolf: ������� ��� ������ � ������.
// #define BM16_ANIMS_FIX					// Real Wolf: ���������� �������� ��� BM16.
// #define INV_OUTFIT_FULL_ICON_HIDE		// Real Wolf: ������ ������ � ������� �������� �� ������ �������.
// #define UI_LOCK_PDA_WITHOUT_PDA_IN_SLOT // Real Wolf: ����������� ������ ���, ���� ������ �������� ���� � �����.
#define CAR_SAVE_FUEL					// Real Wolf: ���������� �������� ������ �������, ������������� ������ � ����������� ������.
//#define R1_EXCLUDE						// Real Wolf: ��������� ������ ������, �������� ������ ������ ������������ ���������.

// ==================================== ������ �� Red Virus ======================================= 
#define INV_NO_ACTIVATE_APPARATUS_SLOT  // Red Virus: ������� ������������� ������� ������ ��������� ��� �������� ������:  ��������, ��������� � ��
#define INV_MOVE_ITM_INTO_QUICK_SLOTS	// Red Virus: ��������� ������ ������� �������� � ������� ������

//#define WPN_BOBBING						// Red Virus: bobbing effect from lost alpha
#define AMMO_FROM_BELT					// Karlan
// ==================================== ������ �� denzor ======================================= 
#define DEAD_BODY_COLLISION				// �������� ������� ���
// #define DRAW_ACTOR_VISUAL_IN_CAR		// ���������� ������ ������ � ����������, ��� ���� �� ������� ����
#define CIRCLE_CROSSHAIR_IN_ALL_WEAPON	// ������� ������ � ����� ������
#define OFF_DRAW_NPC_INFO_IN_HUD		// ��������� ����� ���������� � ������� ��� �� ����
#define HIDE_WEAPON_IN_LADDER			// ������� ������ �� ��������
#define OLR_SLOTS						// ��������� ������ �����, ����������� ���� OLR (��� ������ INV_NEW_SLOTS_SYSTEM)
#define OFF_CAM_3_IN_CAR				// ��������� ��� �� �������� ���� � ������


//#define DEBUG_FLYER				// debug_flyer (���������� ��������) (����� ���������� ��� - ��������)