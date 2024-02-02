// Blender_Vertex_aref.cpp: implementation of the CBlender_Detail_Still class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#include "Blender_Detail_still.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBlender_Detail_Still::CBlender_Detail_Still()
{
	description.CLS		= B_DETAIL;
	description.version	= 0;
}

CBlender_Detail_Still::~CBlender_Detail_Still()
{

}

void	CBlender_Detail_Still::Save		(IWriter& fs )
{
	IBlender::Save		(fs);
	xrPWRITE_PROP		(fs,"Alpha-blend",	xrPID_BOOL,		oBlend);
}

void	CBlender_Detail_Still::Load		(IReader& fs, u16 version )
{
	IBlender::Load		(fs,version);
	xrPREAD_PROP		(fs,xrPID_BOOL,		oBlend);
}

#if RENDER==R_R1
void	CBlender_Detail_Still::Compile	(CBlender_Compile& C)
{
	IBlender::Compile	(C);
	
	if (C.bEditor)
	{
		C.PassBegin		();
		{
			C.PassSET_ZB		(TRUE,TRUE);
			if (oBlend.value)	C.PassSET_Blend_BLEND	(TRUE, 200);
			else				C.PassSET_Blend_SET		(TRUE, 200);
			C.PassSET_LightFog	(TRUE,TRUE);
			
			// Stage1 - Base texture
			C.StageBegin		();
			C.StageSET_Color	(D3DTA_TEXTURE,	  D3DTOP_MODULATE,	D3DTA_DIFFUSE);
			C.StageSET_Alpha	(D3DTA_TEXTURE,	  D3DTOP_MODULATE,	D3DTA_DIFFUSE);
			C.StageSET_TMC		(oT_Name,"$null","$null",0);
			C.StageEnd			();
		}
		C.PassEnd			();
	} else {
		switch (C.iElement)
		{
		case SE_R1_NORMAL_HQ:
			//Msg("detail SE_R1_NORMAL_HQ");
			//			LPCSTR _vs,       LPCSTR _ps, bool bFog, BOOL bZtest, BOOL bZwrite,	BOOL bABlend, D3DBLEND abSRC, D3DBLEND abDST, BOOL aTest, u32 aRef
			C.r_Pass	("detail_wave",	  "detail",   TRUE,      TRUE,        TRUE,         FALSE,        D3DBLEND_ONE,   D3DBLEND_ZERO,  oBlend.value?TRUE:FALSE,oBlend.value?200:0);
			C.r_Sampler	("s_base",	C.L_textures[0]);
			C.r_End		();
			break;
		case SE_R1_NORMAL_LQ:
			//Msg("detail SE_R1_NORMAL_LQ");
			// это малые детейлы типа листвы и палок
			C.r_Pass	("detail_still",	"detail",TRUE,TRUE,TRUE,FALSE, D3DBLEND_ONE,D3DBLEND_ZERO,oBlend.value?TRUE:FALSE,oBlend.value?200:0);
			C.r_Sampler	("s_base",	C.L_textures[0]);
			C.r_End		();
			break;
		case SE_R1_LPOINT:
			//Msg("detail SE_R1_LPOINT");
			//////////////////////////
			/*C.r_Pass		((oNotAnTree.value)?"tree_s_point":"tree_w_point",	"add_point",FALSE,TRUE,FALSE,TRUE,D3DBLEND_ONE,D3DBLEND_ONE,TRUE,0);
			C.r_Sampler		("s_base",	C.L_textures[0]);
			C.r_Sampler_clf	("s_lmap",	TEX_POINT_ATT	);
			C.r_Sampler_clf	("s_att",	TEX_POINT_ATT	);
			C.r_End			();*/
			//////////////////////////
			break;
		case SE_R1_LSPOT:
			//Msg("detail SE_R1_LSPOT");
			// вот тут освещение от фонарика ГГ
			//////////////////////////
			C.r_Pass		("detail_w_spot",	"add_spot",	FALSE,TRUE,FALSE,TRUE,D3DBLEND_ONE,D3DBLEND_ONE,TRUE,0);
			C.r_Sampler		("s_base",	C.L_textures[0]);
			C.r_Sampler_clf	("s_lmap",	"internal\\internal_light_att",		true);  // текстура читается
			C.r_Sampler_clf	("s_att",	"internal\\internal_light_attclip"	);		// текстура читается
			C.r_End			();
			break;
			//////////////////////////
			break;
		case SE_R1_LMODELS:
			//Msg("detail SE_R1_LMODELS");
			break;
		}
	}
}
#else
//////////////////////////////////////////////////////////////////////////
// R2
//////////////////////////////////////////////////////////////////////////
#include "uber_deffer.h"
void	CBlender_Detail_Still::Compile	(CBlender_Compile& C)
{
	IBlender::Compile	(C);

	switch(C.iElement) 
	{
	case SE_R2_NORMAL_HQ: 		// deffer wave
		uber_deffer				(C,false,"detail_w","base",true);
		break;
	case SE_R2_NORMAL_LQ: 		// deffer still
		uber_deffer				(C,false,"detail_s","base",true);
		break;
	}
}
#endif
