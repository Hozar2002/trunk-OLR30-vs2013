// hi_flyer 28.11.19

#ifndef __XR_WEAPON_M134_H__
#define __XR_WEAPON_M134_H__
#pragma once

#include "WeaponMagazined.h"
#include "script_export_space.h"

class CWeaponM134: public CWeaponMagazined
{
	enum EM134State
	{
		eM134Idle,
		eM134Spinup,
		eM134Brake,
		eM134Fire
	};

	static void RotateCallback_hud	(CBoneInstance* B);

private:
	typedef CWeaponMagazined inherited;

	float					fTime;
	float					fRotateSpeed;
	float					fRotateAngle;

	float					fRotateMaxSpeed;
	float					fRotateSpinupAccel;
	float					fRotateBreakAccel;

	u32						dwServoMaxFreq;
	u32						dwServoMinFreq;

	BYTE					bRotAxisHUD;

	u16						iHUDRotBone;

	EM134State				st_current, st_target;

protected:
	HUD_SOUND			m_sndStart;
	HUD_SOUND			m_sndSpin;
	HUD_SOUND			m_sndEnd;
	HUD_SOUND			sndServo;
	virtual void		OnStateSwitch				(u32 S);

public:

					CWeaponM134		();
	virtual			~CWeaponM134		();

	virtual void	FireStart		();
	virtual void	FireEnd			();
	virtual void	Reload			();
	virtual	void	UpdateCL		();

	void			Load(LPCSTR section);

	DECLARE_SCRIPT_REGISTER_FUNCTION
};
add_to_type_list(CWeaponM134)
#undef script_type_list
#define script_type_list save_type_list(CWeaponM134)

#endif //__XR_WEAPON_M134_H__
