// hi_flyer 28.11.19
// миниган с раскруткой ствола

#include "pch_script.h"
#include "WeaponM134.h"
#include "game_object_space.h"

CWeaponM134::CWeaponM134		() : CWeaponMagazined("M134",SOUND_TYPE_WEAPON_SUBMACHINEGUN)
{
	SetSlot(RIFLE_SLOT);
	//Msg("create M134");
	iHUDRotBone		= 0;
}

CWeaponM134::~CWeaponM134		()
{
	//Msg("destroy M134");
	HUD_SOUND::DestroySound	(m_sndStart);
	HUD_SOUND::DestroySound	(m_sndSpin);
	HUD_SOUND::DestroySound	(m_sndEnd);
	HUD_SOUND::DestroySound	(sndServo);
}

void CWeaponM134::Load	(LPCSTR section)
{
		inherited::Load		(section);

		HUD_SOUND::LoadSound(section,"snd_start"		, m_sndStart		, ESoundTypes(SOUND_TYPE_WEAPON_SHOOTING)		);   // звук старта стрельбы
		HUD_SOUND::LoadSound(section,"snd_spin"		, m_sndSpin		, ESoundTypes(SOUND_TYPE_WEAPON_SHOOTING)		);   // звук раскрутки
		HUD_SOUND::LoadSound(section,"snd_end"		, m_sndEnd		, ESoundTypes(SOUND_TYPE_WEAPON_SHOOTING)		);   // звук раскрутки
		HUD_SOUND::LoadSound(section,"snd_servo"		, sndServo		, ESoundTypes(SOUND_TYPE_WEAPON_SHOOTING)		);

		LPCSTR rot_bone = pSettings->r_string	(section,"rot_bone");
		iHUDRotBone		= PKinematics(m_pHUD->Visual())->LL_BoneID(rot_bone);

		fRotateMaxSpeed		= pSettings->r_float	(section,"rot_max_speed");
		fRotateSpinupAccel	= pSettings->r_float	(section,"rot_spinup_accel");
		fRotateBreakAccel	= pSettings->r_float	(section,"rot_break_accel");

		PKinematics(m_pHUD->Visual())->LL_GetBoneInstance(iHUDRotBone).set_callback	(bctCustom,RotateCallback_hud,this);

		fRotateSpeed	= 0;
		fRotateAngle	= 0;

		bRotAxisHUD		= 0;

		dwServoMaxFreq	= 10000;
		dwServoMinFreq	= 22050;

		st_current=st_target=eM134Idle;
}


void CWeaponM134::RotateCallback_hud(CBoneInstance *B)
{
	//CWeaponM134*W		= (CWeaponM134*)B->Callback_Param;
	//Fmatrix&	xform	= B->mTransform;
	//Fmatrix		spin;

	//switch (W->bRotAxisHUD){
	//case 0: spin.rotateX	(W->fRotateAngle); break;		
	//case 1: spin.rotateY	(W->fRotateAngle); break;		
	//case 2: spin.rotateZ	(W->fRotateAngle); break;		
	//default: NODEFAULT;
	//}

	// mulB_44  -- как на видео
	// mulA_43  --  колесом
	// mul_43 - не компилит
	// mulA_44 -- также колесом
	//xform.mulB_43(spin);  // .mulB	(spin);

	CWeaponM134*	C			= (CWeaponM134*)B->Callback_Param;
	Fmatrix m;


	//m.rotateZ(C->fRotateAngle);   // ориг
	//m.rotateX(C->fRotateAngle);   // no
	m.rotateY(C->fRotateAngle);

	B->mTransform.mulB_43	(m);
}


void CWeaponM134::OnStateSwitch	(u32 S)
{
	inherited::OnStateSwitch(S);
	//switch (S)
	//{
	//case eFire:			// единично при выстреле 
	//	{
	//		HUD_SOUND::PlaySound(m_sndStart, H_Parent()->Position(), H_Parent(), true);
	//		st_target = eM134Spinup;
	//	}break;
	//}
}


void CWeaponM134::FireStart()
{
	if (!IsWorking() && IsValid())
	{
		//inherited::FireStart(); // если комент то не стреляет тут, сделать 
		CShootingObject::FireStart();  
		HUD_SOUND::PlaySound(m_sndStart, H_Parent()->Position(), H_Parent(), true);
		st_target = eM134Spinup;
		//Msg("FireStart M134");
	}
}


void CWeaponM134::FireEnd(){
	inherited::FireEnd();
	CShootingObject::FireEnd();
	//Msg(" is working %b", IsWorking());
	if(st_target == eM134Spinup || IsWorking())
	{
		HUD_SOUND::PlaySound(m_sndEnd, H_Parent()->Position(), H_Parent(), true);
		st_target = eM134Brake;
	}
	//Msg("end shoot M134");
	st_target = eM134Brake;
}

void CWeaponM134::Reload(){
	inherited::Reload();
	//Msg("end reload M134");
	st_target = eM134Brake;
}


void CWeaponM134::UpdateCL()
{
	inherited::UpdateCL();
	float dt = Device.fTimeDelta;

	//Msg(" state current [%f] -- state target - [%f] ", st_current, st_target);

		// on state change
	if (st_target!=st_current)
	{
		switch(st_target){
		case eM134Spinup:
			//Msg("spinup");
			fTime = 0;
			HUD_SOUND::PlaySound(sndServo, H_Parent()->Position(), H_Parent(), true);
			break;
		case eM134Brake:
			//Msg("brake");
			HUD_SOUND::PlaySound(sndServo, H_Parent()->Position(), H_Parent(), true);
			break;
		case eM134Idle:
			//Msg("idel");
			if (st_current==eM134Brake) {
				HUD_SOUND::StopSound(sndServo);
			}
			break;
		case eM134Fire:
			//Msg("fire");
			break;
		}
		st_current=st_target;
	}

	// cycle update
	switch (st_current)
	{
	case eM134Idle:
		break;
	case eM134Spinup:
		fRotateSpeed += fRotateSpinupAccel*dt;
		if (sndServo.playing()){
			float k = _sqrt(fRotateSpeed/fRotateMaxSpeed);
			//float freq=iFloor((dwServoMaxFreq-dwServoMinFreq)*k+dwServoMinFreq);
			sndServo.set_frequency(k);
			//Msg(" set frequency1 %f", freq);		// тут улетает на 1.00000f
		}
		//
		if (fRotateSpeed>=fRotateMaxSpeed){
			fRotateSpeed = fRotateMaxSpeed;	   	
			st_target = eM134Fire;  //тут залипает
		}
		//
		break;
	case eM134Brake:  
		fRotateSpeed -= fRotateBreakAccel*dt;
		if (sndServo.playing()){
			float k = _sqrt(fRotateSpeed/fRotateMaxSpeed);
			//float freq=iFloor((dwServoMaxFreq-dwServoMinFreq)*k+dwServoMinFreq);
			sndServo.set_frequency(k);
			//Msg(" set frequency2 %f", freq);
		}
		if (fRotateSpeed<=0){
			st_target		= eM134Idle;
			fRotateSpeed	= 0;
		}
		break;
	case eM134Fire:
		{
			fTime-=dt;

			while ((fTime<0)  && iAmmoElapsed)
			{
				fTime			+=fTimeToFire;

				// Fire
				Fvector						p1, d; 
				p1.set	(get_LastFP()); 
				d.set	(get_LastFD());
				//выстрел
				FireTrace					(p1,d);
				HUD_SOUND::PlaySound(sndShot, H_Parent()->Position(), H_Parent(), true);

				//Msg(" fTime>0 -- shooottt after spin [%f]", fTime);
			}

			if (0==iAmmoElapsed) st_target = eM134Brake;
		}
		break;
	}
	fRotateAngle	+= fRotateSpeed*dt;		// !! постоянно увеличивается не уменьшается

	//(" set fRotateAngle %f", fRotateAngle);
	//Msg(" set fRotateSpeed %f", fRotateSpeed);   // fRotateSpeed залипает на !!! rot_max_speed            = 73
	//Msg(" update 1");
}


using namespace luabind;

#pragma optimize("s",on)
void CWeaponM134::script_register	(lua_State *L)
{
	module(L)
	[
		class_<CWeaponM134, CWeaponMagazined>("CWeaponM134")
			.def(constructor<>())
	];
}
