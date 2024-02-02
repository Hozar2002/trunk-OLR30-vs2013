////////////////////////////////////////////////////////////////////////////
//	Module 		: UIHelper.cpp
//	Created 	: 17.01.2008
//	Author		: Evgeniy Sokolov
//	Description : UI Helper class implementation
////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "UIHelper.h"
#include "UIXmlInit.h"
#include "UIStatic.h"

CUIStatic* UIHelper::CreateStatic( CUIXml& xml, LPCSTR ui_path, CUIWindow* parent )
{
	CUIStatic* ui			= new CUIStatic();
	if(parent)
	{
		parent->AttachChild	( ui );
		ui->SetAutoDelete	( true );
	}
	CUIXmlInit::InitStatic	( xml, ui_path, 0, ui );
	return ui;
} 