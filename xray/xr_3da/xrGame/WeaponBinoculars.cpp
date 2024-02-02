#include "stdafx.h"
#include "WeaponBinoculars.h"

#include "xr_level_controller.h"

#include "level.h"
#include "ui\UIFrameWindow.h"
#include "object_broker.h"
#include "hudmanager.h"
CWeaponBinoculars::CWeaponBinoculars() : CWeaponCustomPistol("BINOCULARS")
{
	SetSlot (APPARATUS_SLOT);
}

CWeaponBinoculars::~CWeaponBinoculars()
{
	HUD_SOUND::DestroySound	(sndZoomIn);
	HUD_SOUND::DestroySound	(sndZoomOut);
	HUD_SOUND::DestroySound	(sndIdle);
}

void CWeaponBinoculars::Load	(LPCSTR section)
{
	inherited::Load(section);

	// Sounds
	HUD_SOUND::LoadSound(section, "snd_zoomin",  sndZoomIn,		SOUND_TYPE_ITEM_USING);
	HUD_SOUND::LoadSound(section, "snd_zoomout", sndZoomOut,	SOUND_TYPE_ITEM_USING);
	HUD_SOUND::LoadSound(section, "snd_idle_binocle", sndIdle,	SOUND_TYPE_ITEM_USING);
}


bool CWeaponBinoculars::Action(s32 cmd, u32 flags) 
{
	switch(cmd) 
	{
	case kWPN_FIRE : 
		return inherited::Action(kWPN_ZOOM, flags);
	}

	return inherited::Action(cmd, flags);
}

void CWeaponBinoculars::OnZoomIn		()
{
	if(H_Parent() && !IsZoomed())
	{
		HUD_SOUND::StopSound(sndZoomOut);
		bool b_hud_mode = (Level().CurrentEntity() == H_Parent());
		HUD_SOUND::PlaySound(sndZoomIn, H_Parent()->Position(), H_Parent(), b_hud_mode);
		HUD_SOUND::PlaySound(sndIdle, H_Parent()->Position(), H_Parent(), b_hud_mode);
	}

	inherited::OnZoomIn();
	m_fZoomFactor = m_fRTZoomFactor;

}

void CWeaponBinoculars::OnZoomOut		()
{
	if(H_Parent() && IsZoomed() && !IsRotatingToZoom())
	{
		HUD_SOUND::StopSound(sndZoomIn);
		HUD_SOUND::StopSound(sndIdle);
		bool b_hud_mode = (Level().CurrentEntity() == H_Parent());	
		HUD_SOUND::PlaySound(sndZoomOut, H_Parent()->Position(), H_Parent(), b_hud_mode);
	
		m_fRTZoomFactor = m_fZoomFactor;//store current
	}


	inherited::OnZoomOut();
}

BOOL	CWeaponBinoculars::net_Spawn			(CSE_Abstract* DC)
{
	m_fRTZoomFactor = m_fScopeZoomFactor;
	inherited::net_Spawn(DC);
	return TRUE;
}

void	CWeaponBinoculars::net_Destroy()
{
	inherited::net_Destroy();
}

void	CWeaponBinoculars::UpdateCL()
{
	inherited::UpdateCL();
	UpdateIdleSnd			(Device.dwTimeDelta);
}

#define SND_UPDATE_TIME 2800u
void CWeaponBinoculars::UpdateIdleSnd		(u32 dt) 
{
	static u32 snd_update_time = 0;
	if ( snd_update_time > SND_UPDATE_TIME ) 
	{
	snd_update_time = 0;
	bool b_hud_mode = (Level().CurrentEntity() == H_Parent());
	if(H_Parent() && IsZoomed() && b_hud_mode)
	{
		HUD_SOUND::PlaySound	(sndIdle, H_Parent()->Position(), this, true, true);
	};
	}
	else
	{
		snd_update_time +=dt;
	};
}

void CWeaponBinoculars::OnDrawUI()
{
	inherited::OnDrawUI	();
}

void CWeaponBinoculars::save(NET_Packet &output_packet)
{
	inherited::save(output_packet);
	save_data		(m_fRTZoomFactor,output_packet);
}

void CWeaponBinoculars::load(IReader &input_packet)
{
	inherited::load(input_packet);
	load_data		(m_fRTZoomFactor,input_packet);
}

void CWeaponBinoculars::GetBriefInfo(xr_string& str_name, xr_string& icon_sect_name, xr_string& str_count, xr_string& ammo_sect_name)
{
	str_name		= NameShort();
	str_count		= "";
	icon_sect_name	= *cNameSect();
	ammo_sect_name = "";
}

void CWeaponBinoculars::net_Relcase	(CObject *object)
{

}
