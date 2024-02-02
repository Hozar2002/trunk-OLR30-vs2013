
#include "stdafx.h"
#include "AcidFog.h"
#include "hudmanager.h"
#include "ParticlesObject.h"
#include "level.h"
#include "actor.h"
#include "inventory.h"
#include "physicsshellholder.h"

CAcidFog::CAcidFog(void) 
{
	Msg("CAcidFog::CAcidFog");
}

CAcidFog::~CAcidFog(void) 
{
}

void CAcidFog::Load(LPCSTR section) 
{
	inherited::Load(section);
	Msg("CAcidFog::Load");
}


void CAcidFog::Postprocess(f32 /**val/**/) 
{
		Msg("CAcidFog::Postprocess");
}

bool CAcidFog::BlowoutState()
{
	bool result = inherited::BlowoutState();
	if(!result)
	{
		m_bLastBlowoutUpdate = false;
		UpdateBlowout();
	}
	else if(!m_bLastBlowoutUpdate)
	{
		m_bLastBlowoutUpdate = true;
		UpdateBlowout();
	}

	return result;
}

void CAcidFog::Affect(SZoneObjectInfo* O) 
{
	Msg("CAcidFog::Affect");
}

