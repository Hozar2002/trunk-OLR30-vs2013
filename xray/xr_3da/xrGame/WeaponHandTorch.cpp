// ручной фонарь аля doom 3
// hi_flyer 15.12.19

#include "stdafx.h"
#include "pch_script.h"
#include "WeaponHandTorch.h"
#include "game_object_space.h"
#include "xrserver_objects_alife_items.h"
#include "game_cl_base.h"
#include "Entity.h"
#include "Actor.h"
#include "../skeletoncustom.h"
#include "../camerabase.h"
#include "../LightAnimLibrary.h"
#include "ai_sounds.h"
#include "PhysicsShell.h"
#include "PhysicsShellHolder.h"
#include "WeaponKnife.h"
#include "level_bullet_manager.h"
#include "gamemtllib.h"

#define TORCH_MATERIAL_NAME "objects\\clothes"		//   bullet

CWeaponHandTorch::CWeaponHandTorch() : CWeaponKnife() 
{
		m_class_name				= get_class_name<CWeaponHandTorch>(this);
		Light_Create();
		m_switched_on               = false;
		torch_material_idx		= (u16)-1;
}

void CWeaponHandTorch::Light_Create()
{
		light_torch				= ::Render->light_create();
		light_torch->set_type		(IRender_Light::SPOT);
		light_torch->set_virtual_size (0.1);
		light_torch->set_active	(false);
}

void CWeaponHandTorch::Light_Destroy()
{
	light_torch.destroy		();
}

void CWeaponHandTorch::Load	(LPCSTR section)
{
	inherited::Load		(section);

	////////////////////////////////////////////////////////////////////////////////////////////////

		light_trace_bone = pSettings->r_string(section, "light_trace_bone");
		VERIFY(light_trace_bone != BI_NONE);

		//color			= pSettings->r_fvector3	(section,"shell_point");
		range			= pSettings->r_float	(section,"light_range");
		cone			= pSettings->r_float	(section,"light_cone");

		torch_material_idx =  GMLib.GetMaterialIdx(TORCH_MATERIAL_NAME);
	////////////////////////////////////////////////////////////////////////////////////////////////
}

BOOL CWeaponHandTorch::net_Spawn(CSE_Abstract* DC) 
{	
	if (!inherited::net_Spawn(DC))
		return				(FALSE);

		CKinematics* K			= smart_cast<CKinematics*>(Visual());
		CInifile* pUserData		= K->LL_UserData(); 

		light_torch->set_range(range);
		light_torch->set_color(1,1,1);
		light_torch->set_cone		(deg2rad(cone));

	return					(TRUE);
}

void CWeaponHandTorch::OnAnimationEnd		(u32 state)
{
	switch (state)
	{
	case eHiding:
		{
			SwitchState(eHidden);
			Light_Destroy();
			m_switched_on               = false;
			//Msg("off");
		}break;
	case eShowing:
		{
			Light_Create();
			m_switched_on               = true;
			//Msg("on");
		}break;
	case eFire: 
	case eFire2: 
		{
            if(m_attackStart) 
			{
				m_attackStart = false;
				if(GetState()==eFire)
					m_pHUD->animPlay(random_anim(mhud_attack_e), TRUE, this, GetState());
				else
					m_pHUD->animPlay(random_anim(mhud_attack2_e), TRUE, this, GetState());

				Fvector	p1, d; 
				p1.set(get_LastFP()); 
				d.set(get_LastFD());

				if(H_Parent()) 
					smart_cast<CEntity*>(H_Parent())->g_fireParams(this, p1,d);
				else break;

				Strike(p1,d);
			} 
			else 
				SwitchState(eIdle);
				
			this->onMovementChanged(mcAnyMove);
		}break;
	case eIdle:	
		SwitchState(eIdle);		break;	
	}

}

void CWeaponHandTorch::Strike(const Fvector& pos, const Fvector& dir)
{
	CCartridge						cartridge; 
	cartridge.m_buckShot			= 1;				
	cartridge.m_impair				= 1;
	cartridge.m_kDisp				= 1;
	cartridge.m_kHit				= 1;
	cartridge.m_kImpulse			= 1;
	cartridge.m_kPierce				= 1;
	cartridge.m_flags.set			(CCartridge::cfTracer, FALSE);
	cartridge.m_flags.set			(CCartridge::cfRicochet, FALSE);
	cartridge.fWallmarkSize			= fWallmarkSize;
	cartridge.bullet_material_idx	= torch_material_idx;

	while(m_magazine.size() < 2)	m_magazine.push_back(cartridge);
	iAmmoElapsed					= m_magazine.size();
	bool SendHit					= SendHitAllowed(H_Parent());

	PlaySound						(m_sndShot,pos);

	Level().BulletManager().AddBullet(	pos, 
										dir, 
										m_fStartBulletSpeed, 
										fCurrentHit, 
										fHitImpulse, 
										H_Parent()->ID(), 
										ID(), 
										m_eHitType, 
										fireDistance, 
										cartridge, 
										SendHit);
}

void CWeaponHandTorch::OnH_B_Independent(bool just_before_destroy)
{
	RemoveShotEffector();

	inherited::OnH_B_Independent(just_before_destroy);

	if (m_pHUD)
		m_pHUD->Hide();

	//завершить принудительно все процессы что шли
	FireEnd();
	m_bPending = false;
	SwitchState(eIdle);

	m_strapped_mode = false;
	SetHUDmode(FALSE);
	m_bZoomMode = false;
	UpdateXForm();
	m_switched_on               = false;
	Light_Destroy();
}

void CWeaponHandTorch::OnMoveToRuck()
{
	inherited::OnMoveToRuck();
	m_switched_on               = false;
	Light_Destroy();
	//Msg("torch on belt");
}

void CWeaponHandTorch::UpdateCL()
{
	inherited::UpdateCL();

		CActor* pActor = smart_cast<CActor*>(H_Parent());
		if (pActor) 
		{

			CKinematics* pVisual = smart_cast<CKinematics*>(m_pHUD->Visual());
			u16	bone_id			 = pVisual->LL_BoneID(light_trace_bone);
			CBoneInstance& BI    = pVisual->LL_GetBoneInstance(bone_id);

				Fmatrix M;
				M.mul(m_pHUD->Transform(), BI.mTransform);

				if(!light_torch)		
					{
						//Msg("create new light update");
						Light_Create();
					};

				if (light_torch)
				{
					light_torch->set_active(m_switched_on);
					if (m_switched_on == true){
						light_torch->set_position	(M.c);		
						light_torch->set_rotation	(M.k.invert(),M.i);	
					}
				}
		}
}

CWeaponHandTorch::~CWeaponHandTorch()
{
	Light_Destroy();
}


using namespace luabind;

#pragma optimize("s",on)
void CWeaponHandTorch::script_register	(lua_State *L)
{
	module(L)
	[
		class_<CWeaponHandTorch, CWeaponKnife>("CWeaponHandTorch")
			.def(constructor<>())
	];
}
