#include "pch_script.h"
#include "UIInventoryWnd.h"

#include "xrUIXmlParser.h"
#include "UIXmlInit.h"
#include "../string_table.h"

#include "../actor.h"
#include "../uigamesp.h"
#include "../hudmanager.h"

#include "../CustomOutfit.h"

#include "../weapon.h"

#include "../script_process.h"

#include "../eatable_item_object.h"
#include "../inventory.h"

#include "UIInventoryUtilities.h"
using namespace InventoryUtilities;


#include "../InfoPortion.h"
#include "../level.h"
#include "../game_base_space.h"
#include "../entitycondition.h"

#include "../game_cl_base.h"
#include "../ActorCondition.h"
#include "UIDragDropListEx.h"
#include "UIOutfitSlot.h"
#include "UI3tButton.h"
#include "../../../build_config_defines.h"

#define				INVENTORY_ITEM_XML		"inventory_item.xml"
#define				INVENTORY_XML			"inventory_new.xml"

//#undef INV_NEW_SLOTS_SYSTEM



CUIInventoryWnd*	g_pInvWnd = NULL;

CUIInventoryWnd::CUIInventoryWnd()
{
	m_iCurrentActiveSlot				= NO_ACTIVE_SLOT;
	UIOutfitBackgroundSize.set(0.0f, 0.0f);
	UIRank								= NULL;
	Init								();
	SetCurrentItem						(NULL);

	g_pInvWnd							= this;	
	m_b_need_reinit						= false;
	Hide								();	
}

void CUIInventoryWnd::Init()
{
	//CUIXml								uiXml;
	//bool xml_result						= uiXml.Init(CONFIG_PATH, UI_PATH, INVENTORY_XML);
	//R_ASSERT3							(xml_result, "file parsing error ", uiXml.m_xml_file_name);
	static CUIXml uiXml;
	static bool is_xml_ready = false;
	if ( !is_xml_ready ) {
	  bool xml_result = uiXml.Init(CONFIG_PATH, UI_PATH, INVENTORY_XML);
	  R_ASSERT3(xml_result, "file parsing error ", uiXml.m_xml_file_name);
	  is_xml_ready = true;
	}

	CUIXmlInit							xml_init;

	xml_init.InitWindow					(uiXml, "main", 0, this);

	AttachChild							(&UIBeltSlots);
	xml_init.InitStatic					(uiXml, "belt_slots", 0, &UIBeltSlots);

	AttachChild							(&UIBack);
	xml_init.InitStatic					(uiXml, "back", 0, &UIBack);

	AttachChild							(&UIStaticBottom);
	xml_init.InitStatic					(uiXml, "bottom_static", 0, &UIStaticBottom);

	AttachChild							(&UIBagWnd);
	xml_init.InitStatic					(uiXml, "bag_static", 0, &UIBagWnd);
	
	AttachChild							(&UIMoneyWnd);
	xml_init.InitStatic					(uiXml, "money_static", 0, &UIMoneyWnd);

	AttachChild							(&UIDescrWnd);
	xml_init.InitStatic					(uiXml, "descr_static", 0, &UIDescrWnd);


	UIDescrWnd.AttachChild				(&UIItemInfo);
	UIItemInfo.Init						(0, 0, UIDescrWnd.GetWidth(), UIDescrWnd.GetHeight(), INVENTORY_ITEM_XML);
	
#ifdef INV_NEW_SLOTS_SYSTEM
	if (GameID() == GAME_SINGLE){
		AttachChild							(&UISleepWnd);
		UISleepWnd.Init();
		UISleepWnd.SetWindowName("sleep_wnd");  // ��� ������� ���������� ����� GetStatic
	}
#endif
	
	AttachChild							(&UIPersonalWnd);
	xml_init.InitFrameWindow			(uiXml, "character_frame_window", 0, &UIPersonalWnd);

	AttachChild							(&UIProgressBack);
	xml_init.InitStatic					(uiXml, "progress_background", 0, &UIProgressBack);

	if (GameID() != GAME_SINGLE){
		AttachChild						(&UIProgressBack_rank);
		xml_init.InitStatic				(uiXml, "progress_back_rank", 0, &UIProgressBack_rank);

		UIProgressBack_rank.AttachChild	(&UIProgressBarRank);
		xml_init.InitProgressBar		(uiXml, "progress_bar_rank", 0, &UIProgressBarRank);
		UIProgressBarRank.SetProgressPos(100);

	}
	

	UIProgressBack.AttachChild (&UIProgressBarHealth);
	xml_init.InitProgressBar (uiXml, "progress_bar_health", 0, &UIProgressBarHealth);
	
	UIProgressBack.AttachChild	(&UIProgressBarPsyHealth);
	xml_init.InitProgressBar (uiXml, "progress_bar_psy", 0, &UIProgressBarPsyHealth);

	UIProgressBack.AttachChild	(&UIProgressBarRadiation);
	xml_init.InitProgressBar (uiXml, "progress_bar_radiation", 0, &UIProgressBarRadiation);

	UIProgressBack.AttachChild	(&UIProgressBarPower);
	xml_init.InitProgressBar (uiXml, "progress_bar_power", 0, &UIProgressBarPower);

#ifdef INV_NEW_SLOTS_SYSTEM
	if (GameID() == GAME_SINGLE){
		UIProgressBack.AttachChild	(&UIProgressBarSatiety);
		xml_init.InitProgressBar (uiXml, "progress_bar_satiety", 0, &UIProgressBarSatiety);
	}
#endif

	UIPersonalWnd.AttachChild			(&UIStaticPersonal);
	xml_init.InitStatic					(uiXml, "static_personal",0, &UIStaticPersonal);
//	UIStaticPersonal.Init				(1, UIPersonalWnd.GetHeight() - 175, 260, 260);

//	AttachChild							(&UIOutfitInfo);
//	UIOutfitInfo.InitFromXml			(uiXml);
//.	xml_init.InitStatic					(uiXml, "outfit_info_window",0, &UIOutfitInfo);

	//�������� ��������������� ����������
	xml_init.InitAutoStatic				(uiXml, "auto_static", this);
	
	
	UIOutfitBackground					= xr_new<CUI3DActorStatic>();
	AttachChild							(UIOutfitBackground);
	float x = uiXml.ReadAttribFlt("outfit_background_static", 0, "x");
	float y = uiXml.ReadAttribFlt("outfit_background_static", 0, "y");
	float width = uiXml.ReadAttribFlt("outfit_background_static", 0, "width");
	float height = uiXml.ReadAttribFlt("outfit_background_static", 0, "height");
	
	UIOutfitBackground->Init(x, y, width, height);
	UIOutfitBackgroundSize.set(width, height);

	OutfitStaticUpdate();

	if (GameID() != GAME_SINGLE){
		UIRankFrame = xr_new<CUIStatic> (); UIRankFrame->SetAutoDelete(true);
		UIRank = xr_new<CUIStatic> (); UIRank->SetAutoDelete(true);

		CUIXmlInit::InitStatic(uiXml, "rank", 0, UIRankFrame);
		CUIXmlInit::InitStatic(uiXml, "rank:pic", 0, UIRank);
		AttachChild(UIRankFrame);
		UIRankFrame->AttachChild(UIRank);		
	}

	m_pUIBagList						= xr_new<CUIDragDropListEx>(); UIBagWnd.AttachChild(m_pUIBagList); m_pUIBagList->SetAutoDelete(true);
	xml_init.InitDragDropListEx			(uiXml, "dragdrop_bag", 0, m_pUIBagList);
	BindDragDropListEnents				(m_pUIBagList);

	m_pUIBeltList						= xr_new<CUIDragDropListEx>(); AttachChild(m_pUIBeltList); m_pUIBeltList->SetAutoDelete(true);
	xml_init.InitDragDropListEx			(uiXml, "dragdrop_belt", 0, m_pUIBeltList);
	BindDragDropListEnents				(m_pUIBeltList);

	m_pUIOutfitList = xr_new<CUIOutfitDragDropList>();
	AttachChild(m_pUIOutfitList);
	m_pUIOutfitList->SetAutoDelete(true);

	xml_init.InitDragDropListEx(uiXml, "dragdrop_outfit", 0, m_pUIOutfitList);
	BindDragDropListEnents(m_pUIOutfitList);
	//m_pUIOutfitList->OnInit(uiXml);
	ZeroMemory(&m_slots_array, sizeof(m_slots_array));

#ifdef INV_NEW_SLOTS_SYSTEM	
	m_pUIPistolList = xr_new<CUIDragDropListEx>(); AttachChild(m_pUIPistolList); m_pUIPistolList->SetAutoDelete(true);
	xml_init.InitDragDropListEx(uiXml, "dragdrop_slot_weapon_1", 0, m_pUIPistolList);
	BindDragDropListEnents(m_pUIPistolList);

	m_pUIAutomaticList = xr_new<CUIDragDropListEx>(); AttachChild(m_pUIAutomaticList); m_pUIAutomaticList->SetAutoDelete(true);
	xml_init.InitDragDropListEx(uiXml, "dragdrop_slot_weapon_2", 0, m_pUIAutomaticList);
	BindDragDropListEnents(m_pUIAutomaticList);


	m_pUIGrenList = xr_new<CUIDragDropListEx>(); AttachChild(m_pUIGrenList); m_pUIGrenList->SetAutoDelete(true);
	xml_init.InitDragDropListEx(uiXml, "dragdrop_slot_grenade", 0, m_pUIGrenList);
	BindDragDropListEnents(m_pUIGrenList);


	if (GameID() == GAME_SINGLE){
		m_pUIKnifeList						= xr_new<CUIDragDropListEx>(); AttachChild(m_pUIKnifeList); m_pUIKnifeList->SetAutoDelete(true);
		xml_init.InitDragDropListEx			(uiXml, "dragdrop_slot_weapon_0", 0, m_pUIKnifeList);
		BindDragDropListEnents				(m_pUIKnifeList);	
		
		m_pUIBinocularList					= xr_new<CUIDragDropListEx>(); AttachChild(m_pUIBinocularList); m_pUIBinocularList->SetAutoDelete(true);
		xml_init.InitDragDropListEx			(uiXml, "dragdrop_slot_weapon_3", 0, m_pUIBinocularList);
		BindDragDropListEnents				(m_pUIBinocularList);
		
		m_pUITorchList						= xr_new<CUIDragDropListEx>(); AttachChild(m_pUITorchList); m_pUITorchList->SetAutoDelete(true);
		xml_init.InitDragDropListEx			(uiXml, "dragdrop_slot_torch", 0, m_pUITorchList);
		BindDragDropListEnents				(m_pUITorchList);
#	if !defined(OLR_SLOTS)
		m_pUIDetectorList					= xr_new<CUIDragDropListEx>(); AttachChild(m_pUIDetectorList); m_pUIDetectorList->SetAutoDelete(true);
		xml_init.InitDragDropListEx			(uiXml, "dragdrop_slot_detector", 0, m_pUIDetectorList);
		BindDragDropListEnents				(m_pUIDetectorList);

		m_pUIPDAList						= xr_new<CUIDragDropListEx>(); AttachChild(m_pUIPDAList); m_pUIPDAList->SetAutoDelete(true);
		xml_init.InitDragDropListEx			(uiXml, "dragdrop_slot_pda", 0, m_pUIPDAList);
		BindDragDropListEnents				(m_pUIPDAList);		
		
		m_pUIHelmetList						= xr_new<CUIDragDropListEx>(); AttachChild(m_pUIHelmetList); m_pUIHelmetList->SetAutoDelete(true);
		xml_init.InitDragDropListEx			(uiXml, "dragdrop_slot_helmet", 0, m_pUIHelmetList);
		BindDragDropListEnents				(m_pUIHelmetList);
		
		m_pUISlotQuickAccessList_0			= xr_new<CUIDragDropListEx>(); AttachChild(m_pUISlotQuickAccessList_0); m_pUISlotQuickAccessList_0->SetAutoDelete(true);
		xml_init.InitDragDropListEx			(uiXml, "dragdrop_slot_quick_access_0", 0, m_pUISlotQuickAccessList_0);
		BindDragDropListEnents				(m_pUISlotQuickAccessList_0);
		
		m_pUISlotQuickAccessList_1			= xr_new<CUIDragDropListEx>(); AttachChild(m_pUISlotQuickAccessList_1); m_pUISlotQuickAccessList_1->SetAutoDelete(true);
		xml_init.InitDragDropListEx			(uiXml, "dragdrop_slot_quick_access_1", 0, m_pUISlotQuickAccessList_1);
		BindDragDropListEnents				(m_pUISlotQuickAccessList_1);
		
		m_pUISlotQuickAccessList_2			= xr_new<CUIDragDropListEx>(); AttachChild(m_pUISlotQuickAccessList_2); m_pUISlotQuickAccessList_2->SetAutoDelete(true);
		xml_init.InitDragDropListEx			(uiXml, "dragdrop_slot_quick_access_2", 0, m_pUISlotQuickAccessList_2);
		BindDragDropListEnents				(m_pUISlotQuickAccessList_2);
		
		m_pUISlotQuickAccessList_3			= xr_new<CUIDragDropListEx>(); AttachChild(m_pUISlotQuickAccessList_3); m_pUISlotQuickAccessList_3->SetAutoDelete(true);
		xml_init.InitDragDropListEx			(uiXml, "dragdrop_slot_quick_access_3", 0, m_pUISlotQuickAccessList_3);
		BindDragDropListEnents				(m_pUISlotQuickAccessList_3);
#	endif
		
		m_slots_array[KNIFE_SLOT]				= m_pUIKnifeList;
		m_slots_array[APPARATUS_SLOT]			= m_pUIBinocularList;
		m_slots_array[TORCH_SLOT]				= m_pUITorchList;
		
#	if !defined(OLR_SLOTS)
		m_slots_array[DETECTOR_SLOT]			= m_pUIDetectorList;
		m_slots_array[PDA_SLOT]					= m_pUIPDAList;
		m_slots_array[OUTFIT_SLOT]				= m_pUIOutfitList;
		m_slots_array[HELMET_SLOT]				= m_pUIHelmetList;
		m_slots_array[SLOT_QUICK_ACCESS_0]		= m_pUISlotQuickAccessList_0;
		m_slots_array[SLOT_QUICK_ACCESS_1]		= m_pUISlotQuickAccessList_1;
		m_slots_array[SLOT_QUICK_ACCESS_2]		= m_pUISlotQuickAccessList_2;
		m_slots_array[SLOT_QUICK_ACCESS_3]		= m_pUISlotQuickAccessList_3;
#	endif
	}
#else
	m_pUIPistolList = xr_new<CUIDragDropListEx>(); AttachChild(m_pUIPistolList); m_pUIPistolList->SetAutoDelete(true);
	xml_init.InitDragDropListEx(uiXml, "dragdrop_pistol", 0, m_pUIPistolList);
	BindDragDropListEnents(m_pUIPistolList);

	m_pUIAutomaticList = xr_new<CUIDragDropListEx>(); AttachChild(m_pUIAutomaticList); m_pUIAutomaticList->SetAutoDelete(true);
	xml_init.InitDragDropListEx(uiXml, "dragdrop_automatic", 0, m_pUIAutomaticList);
	BindDragDropListEnents(m_pUIAutomaticList);
#endif	
	m_slots_array[OUTFIT_SLOT]				= m_pUIOutfitList;
	m_slots_array[PISTOL_SLOT]				= m_pUIPistolList;
	m_slots_array[GREN_SLOT]				= m_pUIGrenList;
	m_slots_array[RIFLE_SLOT]				= m_pUIAutomaticList;
	m_slots_array[GRENADE_SLOT]				= NULL;	
	m_slots_array[BOLT_SLOT]				= NULL;		
	m_slots_array[ARTEFACT_SLOT]		    = NULL;	

	//pop-up menu
	AttachChild							(&UIPropertiesBox);
	UIPropertiesBox.Init				(0,0,300,300);
	UIPropertiesBox.Hide				();

	//AttachChild							(&UIStaticTime);
	//xml_init.InitStatic					(uiXml, "time_static", 0, &UIStaticTime);

	//UIStaticTime.AttachChild			(&UIStaticTimeString);
	//xml_init.InitStatic					(uiXml, "time_static_str", 0, &UIStaticTimeString);

	UIExitButton						= xr_new<CUI3tButton>();UIExitButton->SetAutoDelete(true);
	AttachChild							(UIExitButton);
	xml_init.Init3tButton				(uiXml, "exit_button", 0, UIExitButton);

//Load sounds

	XML_NODE* stored_root				= uiXml.GetLocalRoot		();
	uiXml.SetLocalRoot					(uiXml.NavigateToNode		("action_sounds",0));
	::Sound->create						(sounds[eInvSndOpen],		uiXml.Read("snd_open",			0,	NULL),st_Effect,sg_SourceType);
	::Sound->create						(sounds[eInvSndClose],		uiXml.Read("snd_close",			0,	NULL),st_Effect,sg_SourceType);
	::Sound->create						(sounds[eInvItemToSlot],	uiXml.Read("snd_item_to_slot",	0,	NULL),st_Effect,sg_SourceType);
	::Sound->create						(sounds[eInvItemToBelt],	uiXml.Read("snd_item_to_belt",	0,	NULL),st_Effect,sg_SourceType);
	::Sound->create						(sounds[eInvItemToRuck],	uiXml.Read("snd_item_to_ruck",	0,	NULL),st_Effect,sg_SourceType);
	::Sound->create						(sounds[eInvProperties],	uiXml.Read("snd_properties",	0,	NULL),st_Effect,sg_SourceType);
	::Sound->create						(sounds[eInvDropItem],		uiXml.Read("snd_drop_item",		0,	NULL),st_Effect,sg_SourceType);
	::Sound->create						(sounds[eInvAttachAddon],	uiXml.Read("snd_attach_addon",	0,	NULL),st_Effect,sg_SourceType);
	::Sound->create						(sounds[eInvDetachAddon],	uiXml.Read("snd_detach_addon",	0,	NULL),st_Effect,sg_SourceType);
	::Sound->create						(sounds[eInvItemUse],		uiXml.Read("snd_item_use",		0,	NULL),st_Effect,sg_SourceType);

	uiXml.SetLocalRoot					(stored_root);
}

EListType CUIInventoryWnd::GetType(CUIDragDropListEx* l)
{
	if(l==m_pUIBagList)			return iwBag;
	if(l==m_pUIBeltList)		return iwBelt;

	for (u32 i = 0; i < SLOTS_TOTAL; i++)
		if (m_slots_array[i] == l)
			return iwSlot;
#pragma todo("alpet: ����� ����� �������")
/* 
	if(l==m_pUIAutomaticList)	return iwSlot;
	if(l==m_pUIPistolList)		return iwSlot;
	if(l==m_pUIOutfitList)		return iwSlot;

#ifdef INV_NEW_SLOTS_SYSTEM
	if(l==m_pUIKnifeList)				return iwSlot;
	if(l==m_pUIBinocularList)			return iwSlot;
	if(l==m_pUIDetectorList)			return iwSlot;
	if(l==m_pUITorchList)				return iwSlot;
	if(l==m_pUIPDAList)					return iwSlot;
	if(l==m_pUIHelmetList)				return iwSlot;
	if(l==m_pUISlotQuickAccessList_0)	return iwSlot;
	if(l==m_pUISlotQuickAccessList_1)	return iwSlot;
	if(l==m_pUISlotQuickAccessList_2)	return iwSlot;
	if(l==m_pUISlotQuickAccessList_3)	return iwSlot;
#endif
	*/
	NODEFAULT;
#ifdef DEBUG
	return iwSlot;
#endif // DEBUG
}

void CUIInventoryWnd::PlaySnd(eInventorySndAction a)
{
	if (sounds[a]._handle())
        sounds[a].play					(NULL, sm_2D);
}

CUIInventoryWnd::~CUIInventoryWnd()
{
//.	ClearDragDrop(m_vDragDropItems);
	ClearAllLists						();
}

bool CUIInventoryWnd::OnMouse(float x, float y, EUIMessages mouse_action)
{
	if(m_b_need_reinit)
		return true;

	//����� ��������������� ���� �� ������ ������
	if(mouse_action == WINDOW_RBUTTON_DOWN)
	{
		if(UIPropertiesBox.IsShown())
		{
			UIPropertiesBox.Hide		();
			return						true;
		}
	}

	CUIWindow::OnMouse					(x, y, mouse_action);

	return true; // always returns true, because ::StopAnyMove() == true;
}

void CUIInventoryWnd::Draw()
{
	CUIWindow::Draw						();
}


void CUIInventoryWnd::Update()
{
	if(m_b_need_reinit)
		InitInventory					();
	
	CActor*	m_pActor = smart_cast<CActor*>(Level().CurrentViewEntity());
	if (m_pActor) m_pActor->g_UpdateWalkAnimation();


	CEntityAlive *pEntityAlive			= smart_cast<CEntityAlive*>(Level().CurrentEntity());

	if(pEntityAlive) 
	{
		float v = pEntityAlive->conditions().GetHealth()*100.0f;
		UIProgressBarHealth.SetProgressPos		(v);

		v = pEntityAlive->conditions().GetPsyHealth()*100.0f;
		UIProgressBarPsyHealth.SetProgressPos	(v);

		v = pEntityAlive->conditions().GetRadiation()*100.0f;
		UIProgressBarRadiation.SetProgressPos	(v);

		v = pEntityAlive->conditions().GetPower()*100.0f;
		UIProgressBarPower.SetProgressPos	(v);
		
#ifdef INV_NEW_SLOTS_SYSTEM
		if (GameID() == GAME_SINGLE){
			CActor*	m_pActor = smart_cast<CActor*>(Level().CurrentViewEntity());
			
			v =(m_pActor->conditions().GetSatiety())*100.0f;
			UIProgressBarSatiety.SetProgressPos	(v);
		}

#endif

		CInventoryOwner* pOurInvOwner	= smart_cast<CInventoryOwner*>(pEntityAlive);
		u32 _money						= 0;

		if (GameID() != GAME_SINGLE){
			game_PlayerState* ps = Game().GetPlayerByGameID(pEntityAlive->ID());
			if (ps){
				UIProgressBarRank.SetProgressPos(ps->experience_D*100);
				_money							= ps->money_for_round;
			}
		}else
		{
			_money							= pOurInvOwner->get_money();
		}
		// update money
		string64						sMoney;
		//red_virus
		sprintf_s						(sMoney,"%d %s", _money, *CStringTable().translate("ui_st_money_regional"));
		UIMoneyWnd.SetText				(sMoney);

		// update outfit parameters
		//CCustomOutfit* outfit			= smart_cast<CCustomOutfit*>(pOurInvOwner->inventory().m_slots[OUTFIT_SLOT].m_pIItem);		
		//UIOutfitInfo.Update				(outfit);		
	}

	//UIStaticTimeString.SetText(*InventoryUtilities::GetGameTimeAsString(InventoryUtilities::etpTimeToMinutes));

	//Msg("invvvvv");
	const bool isActorEat = g_actor->IsEat();
	if (isActorEat)
	{
		GetHolder()->StartStopMenu(this, true);
	}

	CUIWindow::Update					();
}

void CUIInventoryWnd::Show() 
{ 	

	InitInventory			();
	inherited::Show			();
	
	if (!IsGameTypeSingle())
	{
		CActor *pActor = smart_cast<CActor*>(Level().CurrentEntity());
		if(!pActor) return;

		pActor->SetWeaponHideState(INV_STATE_INV_WND, true);

		//rank icon		
		int team = Game().local_player->team;
		int rank = Game().local_player->rank;
		string256 _path;		
		if (GameID() != GAME_DEATHMATCH){
			if (1==team)
		        sprintf_s(_path, "ui_hud_status_green_0%d", rank+1);
			else
				sprintf_s(_path, "ui_hud_status_blue_0%d", rank+1);
		}
		else
		{
			sprintf_s(_path, "ui_hud_status_green_0%d", rank+1);
		}
		UIRank->InitTexture(_path);
	}

	CActor *pActor = smart_cast<CActor*>(Level().CurrentEntity());
	if (pActor) pActor->RepackAmmo();

	SendInfoToActor						("ui_inventory");

	Update								();
	PlaySnd								(eInvSndOpen);
}

void CUIInventoryWnd::Hide()
{
	PlaySnd								(eInvSndClose);
	inherited::Hide						();

	SendInfoToActor						("ui_inventory_hide");
	ClearAllLists						();
	UIItemInfo.SetItemImageDefaultValues();

	//������� ���� � �������� ����
	CActor *pActor = smart_cast<CActor*>(Level().CurrentEntity());
	if(pActor && m_iCurrentActiveSlot != NO_ACTIVE_SLOT && 
		pActor->inventory().m_slots[m_iCurrentActiveSlot].m_pIItem)
	{
		pActor->inventory().Activate(m_iCurrentActiveSlot);
		m_iCurrentActiveSlot = NO_ACTIVE_SLOT;
	}

	if (!IsGameTypeSingle())
	{
		CActor *pActor		= smart_cast<CActor*>(Level().CurrentEntity());
		if(!pActor)			return;

		pActor->SetWeaponHideState(INV_STATE_INV_WND, false);
	}
}

void CUIInventoryWnd::AttachAddon(PIItem item_to_upgrade)
{
	PlaySnd										(eInvAttachAddon);
	R_ASSERT									(item_to_upgrade);
	if (OnClient())
	{
		NET_Packet								P;
		item_to_upgrade->object().u_EventGen	(P, GE_ADDON_ATTACH, item_to_upgrade->object().ID());
		P.w_u32									(CurrentIItem()->object().ID());
		item_to_upgrade->object().u_EventSend	(P);
	};

	item_to_upgrade->Attach						(CurrentIItem(), true);


	//�������� ���� �� ��������� ����� � ��������� �� ����� ������ �������
	CActor *pActor								= smart_cast<CActor*>(Level().CurrentEntity());
	if(pActor && item_to_upgrade == pActor->inventory().ActiveItem())
	{
			m_iCurrentActiveSlot				= pActor->inventory().GetActiveSlot();
			pActor->inventory().Activate(NO_ACTIVE_SLOT);
			pActor->inventory().Activate(m_iCurrentActiveSlot);
			m_iCurrentActiveSlot = NO_ACTIVE_SLOT;
	}
	SetCurrentItem								(NULL);
}

void CUIInventoryWnd::DetachAddon(const char* addon_name)
{
	PlaySnd										(eInvDetachAddon);
	if (OnClient())
	{
		NET_Packet								P;
		CurrentIItem()->object().u_EventGen		(P, GE_ADDON_DETACH, CurrentIItem()->object().ID());
		P.w_stringZ								(addon_name);
		CurrentIItem()->object().u_EventSend	(P);
	};
	CurrentIItem()->Detach						(addon_name, true);

	//�������� ���� �� ��������� ����� � ��������� �� ����� ������ �������
	CActor *pActor								= smart_cast<CActor*>(Level().CurrentEntity());
	if(pActor && CurrentIItem() == pActor->inventory().ActiveItem())
	{
			m_iCurrentActiveSlot				= pActor->inventory().GetActiveSlot();
			pActor->inventory().Activate(NO_ACTIVE_SLOT);
			pActor->inventory().Activate(m_iCurrentActiveSlot);
			m_iCurrentActiveSlot = NO_ACTIVE_SLOT;
	}

	SetCurrentItem(NULL);
}


void	CUIInventoryWnd::SendEvent_ActivateSlot	(PIItem	pItem)
{
	NET_Packet						P;
	pItem->object().u_EventGen		(P, GEG_PLAYER_ACTIVATE_SLOT, pItem->object().H_Parent()->ID());
	P.w_u32							(pItem->GetSlot());
	pItem->object().u_EventSend		(P);
}

void	CUIInventoryWnd::SendEvent_Item2Slot			(PIItem	pItem)
{
	NET_Packet						P;
	pItem->object().u_EventGen		(P, GEG_PLAYER_ITEM2SLOT, pItem->object().H_Parent()->ID());
	P.w_u16							(pItem->object().ID());
	pItem->object().u_EventSend		(P);
	g_pInvWnd->PlaySnd				(eInvItemToSlot);
};

void	CUIInventoryWnd::SendEvent_Item2Belt			(PIItem	pItem)
{
	NET_Packet						P;
	pItem->object().u_EventGen		(P, GEG_PLAYER_ITEM2BELT, pItem->object().H_Parent()->ID());
	P.w_u16							(pItem->object().ID());
	pItem->object().u_EventSend		(P);
	g_pInvWnd->PlaySnd				(eInvItemToBelt);
};

void	CUIInventoryWnd::SendEvent_Item2Ruck			(PIItem	pItem)
{
	NET_Packet						P;
	pItem->object().u_EventGen		(P, GEG_PLAYER_ITEM2RUCK, pItem->object().H_Parent()->ID());
	P.w_u16							(pItem->object().ID());
	pItem->object().u_EventSend		(P);

	g_pInvWnd->PlaySnd				(eInvItemToRuck);
};

void	CUIInventoryWnd::SendEvent_Item_Drop(PIItem	pItem)
{
	pItem->SetDropManual			(TRUE);

	if( OnClient() )
	{
		NET_Packet					P;
		pItem->object().u_EventGen	(P, GE_OWNERSHIP_REJECT, pItem->object().H_Parent()->ID());
		P.w_u16						(pItem->object().ID());
		pItem->object().u_EventSend(P);
	}
	g_pInvWnd->PlaySnd				(eInvDropItem);
};

void	CUIInventoryWnd::SendEvent_Item_Eat			(PIItem	pItem)
{
	R_ASSERT						(pItem->m_pCurrentInventory==m_pInv);
	NET_Packet						P;
	pItem->object().u_EventGen		(P, GEG_PLAYER_ITEM_EAT, pItem->object().H_Parent()->ID());
	P.w_u16							(pItem->object().ID());
	pItem->object().u_EventSend		(P);
};


void CUIInventoryWnd::BindDragDropListEnents(CUIDragDropListEx* lst)
{
	lst->m_f_item_drop				= CUIDragDropListEx::DRAG_DROP_EVENT(this,&CUIInventoryWnd::OnItemDrop);
	lst->m_f_item_start_drag		= CUIDragDropListEx::DRAG_DROP_EVENT(this,&CUIInventoryWnd::OnItemStartDrag);
	lst->m_f_item_db_click			= CUIDragDropListEx::DRAG_DROP_EVENT(this,&CUIInventoryWnd::OnItemDbClick);
	lst->m_f_item_selected			= CUIDragDropListEx::DRAG_DROP_EVENT(this,&CUIInventoryWnd::OnItemSelected);
	lst->m_f_item_rbutton_click		= CUIDragDropListEx::DRAG_DROP_EVENT(this,&CUIInventoryWnd::OnItemRButtonClick);
}


#include "../xr_level_controller.h"
#include <dinput.h>

bool CUIInventoryWnd::OnKeyboard(int dik, EUIMessages keyboard_action)
{
	CEatableItemObject* pEatable = smart_cast<CEatableItemObject*>(CurrentIItem());
	if(m_b_need_reinit) {
		return true;
	}

	if (UIPropertiesBox.GetVisible()) {
		UIPropertiesBox.OnKeyboard(dik, keyboard_action);
	}


	//const bool is_eat = pEatable && pEatable->IsEat();
	//if (!is_eat && is_binded(kDROP, dik) ) {
	//	if(WINDOW_KEY_PRESSED==keyboard_action)
	//		DropCurrentItem(false);
	//	return true;
	//}



	if (WINDOW_KEY_PRESSED == keyboard_action)
	{
#ifdef DEBUG
		if(DIK_NUMPAD7 == dik && CurrentIItem())
		{
			CurrentIItem()->ChangeCondition(-0.05f);
			UIItemInfo.InitItem(CurrentIItem());
		}
		else if(DIK_NUMPAD8 == dik && CurrentIItem())
		{
			CurrentIItem()->ChangeCondition(0.05f);
			UIItemInfo.InitItem(CurrentIItem());
		}
#endif
	}
	if( inherited::OnKeyboard(dik,keyboard_action) )return true;

	return false;
}

void CUIInventoryWnd::OutfitStaticUpdate() {
	SStaticParamsFromGameObject params;
	params.m_rotate.set(0.0f,0.0f,0.0f);
	params.m_scale = 1.f;
	auto pActor = smart_cast<CGameObject*>(g_actor);

	if (pActor) {

		UIOutfitBackground->SetWndSize(UI()->is_16_9_mode() ? UIOutfitBackgroundSize.x / 1.328f : UIOutfitBackgroundSize.x, UIOutfitBackgroundSize.y);
		
		UIOutfitBackground->SetGameObject(pActor, params);
		UIOutfitBackground->SetModelRadius(1.f);
		UIOutfitBackground->SetModelConstPosition(true);
		UIOutfitBackground->SetRotate(0.0, 3.6, 0.0);
	}
}

void CUIInventoryWnd::Draw3DStatic()
{
	if (::Render->get_generation() != ::Render->GENERATION_R1)
	{
		UIItemInfo.Draw3DStatic();
		if (this->IsShown())
		{
			if (UIOutfitBackground)
				UIOutfitBackground->Draw3D();
		}
	}
}