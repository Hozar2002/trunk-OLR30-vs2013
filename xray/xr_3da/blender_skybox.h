#pragma once

#include "blenders\blender.h"

class CBlender_skybox : public IBlender {
public:
	virtual LPCSTR getComment()	{ return "INTERNAL: combiner";	}
	virtual BOOL canBeDetailed()	{ return FALSE;	}
	virtual BOOL canBeLMAPped() { return FALSE;	}

	virtual void Compile(CBlender_Compile& C) {
		C.r_Pass("sky2", "sky2", FALSE, TRUE, FALSE);
		C.r_Sampler_clf("s_sky0", "$null");
		C.r_Sampler_clf("s_sky1", "$null");
		C.r_Sampler_rtf("s_tonemap", "$user$tonemap");	//. hack
		C.r_End();
	}
};

class CBlender_background : public CBlender_skybox {

	virtual void Compile(CBlender_Compile& C) {
		Msg("CBlender_background::Compile(begin)");
		CBlender_skybox::Compile(C);
		C.PassSET_ablend_aref(true,0);
		Msg("CBlender_background::Compile(end)");
	}
};

