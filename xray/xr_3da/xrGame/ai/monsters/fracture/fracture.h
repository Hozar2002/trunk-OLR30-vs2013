#pragma once
#include "../BaseMonster/base_monster.h"
#include "../../../script_export_space.h"

class CStateManagerFracture;

class CFracture : public CBaseMonster {
	typedef		CBaseMonster		inherited;
	
public:
					CFracture 			();
	virtual			~CFracture 			();	

	//virtual void	UpdateCL			();
	TTime			time_upd_timer_fracture;
	virtual void	reinit				();

	ref_sound			threaten_start_sound;

	virtual void	Load				(LPCSTR section);
	virtual void	CheckSpecParams		(u32 spec_params);

	virtual bool	check_start_conditions	(ControlCom::EControlType type);
	virtual void	on_activate_control		(ControlCom::EControlType);

	DECLARE_SCRIPT_REGISTER_FUNCTION

	public:

		bool	start_threaten;
};

add_to_type_list(CFracture)
#undef script_type_list
#define script_type_list save_type_list(CFracture)
