////////////////////////////////////////////////////////////////////////////
//	Module 		: UIHelper.h
//	Created 	: 17.01.2008
//	Author		: Evgeniy Sokolov
//	Description : UI Helper class
////////////////////////////////////////////////////////////////////////////

#ifndef	UI_HELPER_H_INCLUDED
#define UI_HELPER_H_INCLUDED

class CUIXml;
class CUIWindow;
class CUIStatic;

class UIHelper
{
public:
	UIHelper		() {};
	~UIHelper		() {};

	static	CUIStatic*			CreateStatic		( CUIXml& xml, LPCSTR ui_path, CUIWindow* parent );
};

#endif // UI_HELPER_H_INCLUDED