#ifndef __XR_WEAPON_MAG_H__
#define __XR_WEAPON_MAG_H__
#pragma once

#include "weapon.h"
#include "hudsound.h"
#include "ai_sounds.h"
#include "GameObject.h"

class ENGINE_API CMotionDef;

//������ ������� ��������� �������������
//����������� ��������, ������, ���� ��������� �������
#define WEAPON_ININITE_QUEUE -1


class CWeaponMagazined: public CWeapon
{
	friend class CWeaponScript;
private:
	typedef CWeapon inherited;
protected:
	// Media :: sounds
	HUD_SOUND		sndShow;
	HUD_SOUND		sndHide;
	HUD_SOUND		sndShot;
	HUD_SOUND		sndEmptyClick;
	HUD_SOUND		sndReload;
	HUD_SOUND		sndReload_jammed;
	HUD_SOUND		sndSightsUp;		//added by Daemonion for iron sight audio parameter - sights being raised
	HUD_SOUND		sndSightsDown;		//added by Daemonion for iron sight audio parameter - sights being lowered
	//���� �������� ��������
	HUD_SOUND*		m_pSndShotCurrent;

	virtual void	StopHUDSounds		();

	//�������������� ���������� � ���������
	LPCSTR			m_sSilencerFlameParticles;
	LPCSTR			m_sSilencerSmokeParticles;
	HUD_SOUND		sndSilencerShot;

	ESoundTypes		m_eSoundShow;
	ESoundTypes		m_eSoundHide;
	ESoundTypes		m_eSoundShot;
	ESoundTypes		m_eSoundEmptyClick;
	ESoundTypes		m_eSoundReload;
	ESoundTypes		m_eSoundReload_jammed;
	ESoundTypes		m_eSoundSightsUp;		//added by Daemonion for iron sight audio parameter - sights being raised
	ESoundTypes		m_eSoundSightsDown;		//added by Daemonion for iron sight audio parameter - sights being lowered
	struct SWMmotions{
		MotionSVec		mhud_idle;
		MotionSVec		mhud_idle_aim;
		MotionSVec		mhud_reload;	//
		MotionSVec		mhud_reload_jammed;	//
		MotionSVec		mhud_hide;		//
		MotionSVec		mhud_show;		//
		MotionSVec		mhud_shots;		//
		MotionSVec		mhud_idle_sprint;
	};
	SWMmotions			mhud;	
	
	// General
	//���� ������� ��������� UpdateSounds
	u32				dwUpdateSounds_Frame;
protected:
	virtual void	OnMagazineEmpty	();

	virtual void	switch2_Idle	();
	virtual void	switch2_Fire	();
	virtual void	switch2_Fire2	(){}
	virtual void	switch2_Empty	();
	virtual void	switch2_Reload	();
	virtual void	switch2_ReloadJammed	();
	virtual void	switch2_Hiding	();
	virtual void	switch2_Hidden	();
	virtual void	switch2_Showing	();
	
	virtual void	OnShot			();	
	
	virtual void	OnEmptyClick	();

	virtual void	OnAnimationEnd	(u32 state);
	virtual void	OnStateSwitch	(u32 S);

	virtual void	UpdateSounds	();

	bool			TryReload		();
	bool			TryPlayAnimIdle	();

protected:
	virtual void	ReloadMagazine	();
	virtual void	RestoreMagazineJammed();
			void	ApplySilencerKoeffs	();

	virtual void	state_Fire		(float dt);
	virtual void	state_MagEmpty	(float dt);
	virtual void	state_Misfire	(float dt);
public:
					CWeaponMagazined	(LPCSTR name="AK74",ESoundTypes eSoundType=SOUND_TYPE_WEAPON_SUBMACHINEGUN);
	virtual			~CWeaponMagazined	();

	virtual void	Load			(LPCSTR section);
	virtual CWeaponMagazined*cast_weapon_magazined	()		 {return this;}

	virtual void	SetDefaults		();
	virtual void	FireStart		();
	virtual void	FireEnd			();
	virtual void	Reload			();
	

	virtual	void	UpdateCL		();
	virtual void	net_Destroy		();
	virtual void			net_Export			(NET_Packet& P);
	virtual void			net_Import			(NET_Packet& P);

	virtual void	OnH_A_Chield		();

	virtual bool	Attach(PIItem pIItem, bool b_send_event);
	virtual bool	Detach(const char* item_section_name, bool b_spawn_item);
	virtual bool	CanAttach(PIItem pIItem);
	virtual bool	CanDetach(const char* item_section_name);

	virtual void	InitAddons();

	virtual bool	Action			(s32 cmd, u32 flags);
	virtual void	onMovementChanged	(ACTOR_DEFS::EMoveCommand cmd);
	bool			IsAmmoAvailable	();
	virtual void	UnloadMagazine	(bool spawn_ammo = true);

	virtual void	GetBriefInfo				(xr_string& str_name, xr_string& icon_sect_name, xr_string& str_count, xr_string& ammo_sect_name);


	//////////////////////////////////////////////
	// ��� �������� ��������� ��� ����������
	//////////////////////////////////////////////
public:
	virtual bool	SwitchMode				();
	virtual bool	SingleShotMode			()			{return 1 == m_iQueueSize;}
	virtual void	SetQueueSize			(int size);
	IC		int		GetQueueSize			() const	{return m_iQueueSize;};
	virtual bool	StopedAfterQueueFired	()			{return m_bStopedAfterQueueFired; }
	virtual void	StopedAfterQueueFired	(bool value){m_bStopedAfterQueueFired = value; }

protected:
	//������������ ������ �������, ������� ����� ����������
	int				m_iQueueSize;
	//���������� ������� ����������� ��������
	int				m_iShotNum;
	//  [7/20/2005]
	//����� ������ �������, ��� ����������� ��������, ���������� ������ (������� ��-�� �������)
	int				m_iShootEffectorStart;
	Fvector			m_vStartPos, m_vStartDir;
	//  [7/20/2005]
	//���� ����, ��� �� ������������ ����� ���� ��� ����������
	//����� ������� ��������, ������� ���� ������ � m_iQueueSize
	bool			m_bStopedAfterQueueFired;
	//���� ����, ��� ���� �� ���� ������� �� ������ �������
	//(���� ���� ����� ������ ������ �� ����� � ��������� FireEnd)
	bool			m_bFireSingleShot;
	//������ ��������
	bool			m_bHasDifferentFireModes;
	xr_vector<int>	m_aFireModes;
	int				m_iCurFireMode;
	string16		m_sCurFireMode;
	int				m_iPrefferedFireMode;

	//���������� ��������� �������������
	//������ ������ ����� ��������
	bool m_bLockType;

	//////////////////////////////////////////////
	// ����� �����������
	//////////////////////////////////////////////
public:
	virtual void	OnZoomIn			();
	virtual void	OnZoomOut			();
	virtual	void	OnNextFireMode		();
	virtual	void	OnPrevFireMode		();
	virtual bool	HasFireModes		() { return m_bHasDifferentFireModes; };
	virtual	int		GetCurrentFireMode	() { return m_aFireModes[m_iCurFireMode]; };	
	virtual LPCSTR	GetCurrentFireModeStr	() {return m_sCurFireMode;};

	virtual const	xr_vector<int>&	GetFireModes() const				{return m_aFireModes;}
	virtual	void					SetCurFireMode(int fire_mode)		{m_iCurFireMode = fire_mode;}

	virtual void	save				(NET_Packet &output_packet);
	virtual void	load				(IReader &input_packet);

protected:
	virtual bool	AllowFireWhileWorking() {return false;}

	//����������� ������� ��� ������������ �������� HUD
	virtual void	PlayAnimShow();
	virtual void	PlayAnimHide();
	virtual void	PlayAnimReload();
	virtual void	PlayAnimReloadJammed();
	virtual void	PlayAnimIdle();
	virtual void	PlayAnimShoot();
	virtual void	PlayReloadSound		();
	virtual void	PlayReloadJammedSound();

	virtual void	StartIdleAnim		();
	virtual	int		ShotsFired			() { return m_iShotNum; }
	virtual float	GetWeaponDeterioration	();

	// Callback function added by Cribbledirge.
	virtual IC void	StateSwitchCallback(GameObject::ECallbackType actor_type, GameObject::ECallbackType npc_type);

	// Real Wolf.20.01.15
	IC			bool TryToGetAmmo(u32);
};

#endif //__XR_WEAPON_MAG_H__
