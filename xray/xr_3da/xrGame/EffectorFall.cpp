#include "stdafx.h"
#include "EffectorFall.h"
#include "CameraEffector.h"

#include "Actor.h"
#include "pch_script.h"
#include "script_callback_ex.h"
#include "script_game_object.h"
#include "game_object_space.h"
#include "alife_object_registry.h"


#define FALL_SPEED 3.5f
#define FALL_MAXDIST 0.75f

CEffectorFall::CEffectorFall(float power, float life_time ) : CEffectorCam(eCEFall, life_time, TRUE)
{
	g_actor->callback(GameObject::eActorLanded)(power);
	fPower	= (power>1)?1:((power<0)?0:power*power);
	fPhase	= 0;
}

CEffectorFall::~CEffectorFall()
{
}

BOOL CEffectorFall::Process(Fvector &p, Fvector &d, Fvector &n, float& fFov, float& fFar, float& fAspect)
{
	fPhase+=FALL_SPEED*Device.fTimeDelta;
	if (fPhase<1)	p.y-=FALL_MAXDIST*fPower*_sin(M_PI*fPhase+M_PI);
	else			fLifeTime=-1;
	return TRUE;
}
