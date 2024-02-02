#pragma once
#include "UIWindow.h"
#include "UIWndCallback.h"
#include "UIXmlInit.h"
#include "UITaskDescrWnd.h"

#include "../encyclopedia_article_defs.h"
class CUIFrameWindow;
class CUIFrameLineWnd;
class CUIAnimatedStatic;
//class CUIMapWnd;
class CUI3tButton;
class CUITabControl;
class CGameTask;
class CUITaskDescrWnd;
class CUIScrollView;
class CUITaskItem;
class CUINewsWnd;
class CUINewsTalkWnd;
class CUIListWnd;

class CUIEventsWnd	:public CUIWindow, public CUIWndCallback{
	typedef CUIWindow			inherited;
	enum ETaskFilters{	eActiveTask			=	0,
						eAccomplishedTask,
						eFailedTask,
						eJornalTask,  // my
						eNewsTask,     // my
//.						eOwnTask,
						eMaxTask};
	enum EEventWndFlags{
						flNeedReload	=(1<<0),
						flMapMode		=(1<<1),
	};

protected:

	CUINewsWnd*			m_UINewsWnd;
	CUINewsTalkWnd*			m_UINewsTalkWnd;
	CUIListWnd*			m_SrcListWnd;

	CUI3tButton*		m_ActiveJobBtn;
	CUI3tButton*		m_CompleteJobBtn;
	CUI3tButton*		m_JornalBackBtn;

	CUI3tButton*		m_ActiveNews;

	CUI3tButton*		m_ActiveTalk;

	CUIWindow*			m_UILeftWnd;

//	CUIScrollView*		m_DescrView;

	CUIScrollView*		m_DescrView;
	CGameFont*			m_pTreeRootFont;
	u32					m_uTreeRootColor;
	CGameFont*			m_pTreeItemFont;
	u32					m_uTreeItemColor;

	void __stdcall	OnSrcListItemClicked(CUIWindow*, void*);
	void __stdcall	ActiveJobBtn(CUIWindow*, void*);
	void __stdcall	CompleteJobBtn(CUIWindow*, void*);
	void __stdcall	JornalBackBtn(CUIWindow*, void*);

	void __stdcall	NewsNBtn(CUIWindow*, void*);

	void __stdcall	TalkNBtn(CUIWindow*, void*);

	typedef xr_vector<CEncyclopediaArticle*>			ArticlesDB;
	typedef xr_vector<CEncyclopediaArticle*>::iterator	ArticlesDB_it;
	ArticlesDB				m_ArticlesDB;

	void		UnloadNewsTab();
	void		LoadNewsTab();

	void		UnloadNewsTalkTab();
	void		LoadNewsTalkTab();

	void		UnloadJournalTab();
	void		LoadJournalTab(ARTICLE_DATA::EArticleType _type);

	Flags16						m_flags;
	ETaskFilters				m_currFilter;
	CUIFrameWindow*				m_UILeftFrame;
	CUIWindow*					m_UIRightWnd;
	CUIFrameLineWnd*			m_UILeftHeader;
	//CUIAnimatedStatic*			m_UIAnimation;
	//CUIMapWnd*					m_UIMapWnd;
	CUITaskDescrWnd*			m_UITaskInfoWnd;
	CUIScrollView*				m_ListWnd;
	CUITabControl*				m_TaskFilter;


	CUIFrameLineWnd*		UITimerBackground;
	CUIStatic*				UIMainPdaFrame;

	bool						Filter					(CGameTask* t);
	void __stdcall				OnFilterChanged			(CUIWindow*,void*);
	void						ReloadList				(bool bClearOnly);
	void						LoadActiveJobsTasks();
	void						LoadCompleteJobsTasks();
	void						UpdateDateTime					();

public:
	void						SetDescriptionMode		(bool bMap);
	bool						GetDescriptionMode		();
	void						ShowDescription			(CGameTask* t, int idx);
	void						ClearDescription		() {
		if (m_UITaskInfoWnd) m_UITaskInfoWnd->ClearAll	();
	}
	bool						ItemHasDescription		(CUITaskItem*);
public:

								CUIEventsWnd			();
	virtual						~CUIEventsWnd			();
	virtual void				SendMessage				(CUIWindow* pWnd, s16 msg, void* pData);
			void				Init					();
	virtual void				Update					();
	virtual void				Draw					();
	virtual void				Show					(bool status);
			void				Reload					();
	virtual void				Reset					();

	CUIXml						m_ui_task_item_xml;

};
