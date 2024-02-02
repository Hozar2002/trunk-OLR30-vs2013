// ручной фонарь аля doom 3
// hi_flyer 15.12.19

#pragma once
#include "WeaponMagazined.h"
#include "script_export_space.h"
#include "WeaponKnife.h"

class CWeaponHandTorch: public CWeaponKnife {
private:
	typedef CWeaponKnife inherited;
protected:

		void					Light_Create();
		void					Light_Destroy();
		virtual void			Strike					(const Fvector& pos, const Fvector& dir);
		bool                    m_switched_on;
		ref_light				light_torch;

public:
		virtual void			OnH_B_Independent(bool just_before_destroy);
		virtual void					OnAnimationEnd		(u32 state);
		void				Load							(LPCSTR section);
		virtual void		OnMoveToRuck		();
							CWeaponHandTorch(); 
		virtual BOOL		net_Spawn			(CSE_Abstract* DC);
		virtual				~CWeaponHandTorch(); 
		virtual void			UpdateCL();

protected:
	shared_str		                light_trace_bone;
	float							range;
	//Fvector							color;
	float							cone;
	u16					torch_material_idx;

		DECLARE_SCRIPT_REGISTER_FUNCTION
};
add_to_type_list(CWeaponHandTorch)
#undef script_type_list
#define script_type_list save_type_list(CWeaponHandTorch)
