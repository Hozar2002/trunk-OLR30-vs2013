#pragma once

#include "WeaponCustomPistol.h"
#include "script_export_space.h"

class CUIFrameWindow;
class CUIStatic;

class CWeaponBinoculars: public CWeaponCustomPistol
{
private:
	typedef CWeaponCustomPistol inherited;
protected:
	HUD_SOUND		sndZoomIn;
	HUD_SOUND		sndZoomOut;
	HUD_SOUND		sndIdle;
	float			m_fRTZoomFactor; //run-time zoom factor
public:
					CWeaponBinoculars	(); 
	virtual			~CWeaponBinoculars	();

	void			Load				(LPCSTR section);

	virtual void	OnZoomIn			();
	virtual void	OnZoomOut			();

	virtual void	net_Destroy			();
	virtual BOOL	net_Spawn			(CSE_Abstract* DC);

	virtual void	save				(NET_Packet &output_packet);
	virtual void	load				(IReader &input_packet);

	virtual bool	Action				(s32 cmd, u32 flags);
	virtual void	UpdateCL			();
	void			UpdateIdleSnd		(u32 dt);
	virtual void	OnDrawUI			();
	virtual bool	use_crosshair		()	const {return false;}
	virtual void	GetBriefInfo		(xr_string& str_name, xr_string& icon_sect_name, xr_string& str_count, xr_string& ammo_sect_name);
	virtual void	net_Relcase			(CObject *object);
protected:

	DECLARE_SCRIPT_REGISTER_FUNCTION
};
add_to_type_list(CWeaponBinoculars)
#undef script_type_list
#define script_type_list save_type_list(CWeaponBinoculars)
