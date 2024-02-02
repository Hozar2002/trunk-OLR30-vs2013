#include "pch_script.h"
#include "UIEventsWnd.h"
#include "UIFrameWindow.h"
#include "UIFrameLineWnd.h"
#include "UIAnimatedStatic.h"
//#include "UIMapWnd.h"
#include "UIScrollView.h"
#include "UITabControl.h"
#include "UI3tButton.h"
#include "../HUDManager.h"
#include "../level.h"
#include "../actor.h"
#include "../gametaskmanager.h"
#include "../gametask.h"
#include "../map_manager.h"
#include "../map_location.h"
#include "../string_table.h"
#include "UITaskItem.h"
#include "../alife_registry_wrappers.h"
#include "../encyclopedia_article.h"
#include "UIInventoryUtilities.h"
#include "UINewsWnd.h"

#include "stdafx.h"
#include "UIDiaryWnd.h"
#include "UIFrameWindow.h"
#include "UIFrameLineWnd.h"
#include "UINewsWnd.h"
#include "UIAnimatedStatic.h"
#include "UIXmlInit.h"
#include "../object_broker.h"
#include "UITabControl.h"
#include "UIScrollView.h"
#include "UIListWnd.h"
#include "UITreeViewItem.h"
#include "UIEncyclopediaArticleWnd.h"
#include "../level.h"
#include "../actor.h"
#include "../alife_registry_wrappers.h"
#include "../encyclopedia_article.h"
#include "UIPdaAux.h"
#include "UIInventoryUtilities.h"

CUIEventsWnd::CUIEventsWnd			()
{
	m_flags.zero			();
}

CUIEventsWnd::~CUIEventsWnd			()
{
	delete_data(m_UINewsWnd);
	delete_data(m_UINewsTalkWnd);
	delete_data			(m_UITaskInfoWnd);
	delete_data(m_SrcListWnd);
	delete_data(m_DescrView);
}

void CUIEventsWnd::Init				()
{
	CUIXml uiXml;
	bool xml_result					= uiXml.Init(CONFIG_PATH, UI_PATH, "pda_events.xml");
	R_ASSERT3						(xml_result, "xml file not found", "pda_events.xml");

	CUIXmlInit xml_init;
	xml_init.InitWindow				(uiXml, "main_wnd", 0, this);


	m_UILeftFrame					= xr_new<CUIFrameWindow>(); m_UILeftFrame->SetAutoDelete(true);
	AttachChild						(m_UILeftFrame);
	xml_init.InitFrameWindow		(uiXml, "main_wnd:left_frame", 0, m_UILeftFrame);


	m_UILeftHeader					= xr_new<CUIFrameLineWnd>(); m_UILeftHeader->SetAutoDelete(true);
	m_UILeftFrame->AttachChild		(m_UILeftHeader);
	xml_init.InitFrameLine			(uiXml, "main_wnd:left_frame:left_frame_header", 0, m_UILeftHeader);

	m_UIRightWnd					= xr_new<CUIWindow>(); m_UIRightWnd->SetAutoDelete(true);
	AttachChild						(m_UIRightWnd);
	xml_init.InitWindow				(uiXml, "main_wnd:right_frame", 0, m_UIRightWnd);

	m_UITaskInfoWnd					= xr_new<CUITaskDescrWnd>(); m_UITaskInfoWnd->SetAutoDelete(false);
	m_UITaskInfoWnd->Init			(&uiXml,"main_wnd:right_frame:task_descr_view");
	
	m_UINewsWnd = xr_new<CUINewsWnd>(); m_UINewsWnd->SetAutoDelete(false);
	m_UINewsWnd->Init();

	m_UINewsTalkWnd = xr_new<CUINewsTalkWnd>(); m_UINewsTalkWnd->SetAutoDelete(false);
	m_UINewsTalkWnd->Init();


	xml_init.InitFont(uiXml, "main_wnd:left_frame:work_area:src_list:tree_item_font", 0, m_uTreeItemColor, m_pTreeItemFont);
	R_ASSERT(m_pTreeItemFont);
	xml_init.InitFont(uiXml, "main_wnd:left_frame:work_area:src_list:tree_root_font", 0, m_uTreeRootColor, m_pTreeRootFont);
	R_ASSERT(m_pTreeRootFont);

	m_UILeftWnd = xr_new<CUIWindow>(); m_UILeftWnd->SetAutoDelete(true);
	xml_init.InitWindow(uiXml, "main_wnd:left_frame:work_area", 0, m_UILeftWnd);
	m_UILeftFrame->AttachChild(m_UILeftWnd);

	m_SrcListWnd = xr_new<CUIListWnd>(); m_SrcListWnd->SetAutoDelete(false);
	xml_init.InitListWnd(uiXml, "main_wnd:left_frame:work_area:src_list", 0, m_SrcListWnd);
	m_SrcListWnd->SetWindowName("src_list");
	Register(m_SrcListWnd);
	AddCallback("src_list", LIST_ITEM_CLICKED, CUIWndCallback::void_function(this, &CUIEventsWnd::OnSrcListItemClicked));

	m_DescrView = xr_new<CUIScrollView>(); m_DescrView->SetAutoDelete(false);
	xml_init.InitScrollView(uiXml, "main_wnd:right_frame:work_area:scroll_view", 0, m_DescrView);





	// active jobs btn
	m_ActiveJobBtn = xr_new<CUI3tButton>();	m_ActiveJobBtn->SetAutoDelete(true); AttachChild(m_ActiveJobBtn);
	m_ActiveJobBtn->SetWindowName("active_news_btn");
	Register(m_ActiveJobBtn);
	AddCallback("active_news_btn", BUTTON_CLICKED, CUIWndCallback::void_function(this, &CUIEventsWnd::ActiveJobBtn));
	xml_init.Init3tButton(uiXml, "main_wnd:right_frame:work_area:active_news_btn", 0, m_ActiveJobBtn);



	// complete jobs btn  (журнал)
	m_CompleteJobBtn = xr_new<CUI3tButton>();	m_CompleteJobBtn->SetAutoDelete(true); AttachChild(m_CompleteJobBtn);
	m_CompleteJobBtn->SetWindowName("complete_news_btn");
	Register(m_CompleteJobBtn);
	AddCallback("complete_news_btn", BUTTON_CLICKED, CUIWndCallback::void_function(this, &CUIEventsWnd::CompleteJobBtn));
	xml_init.Init3tButton(uiXml, "main_wnd:right_frame:work_area:complete_news_btn", 0, m_CompleteJobBtn);

	// назад с журнала
	m_JornalBackBtn = xr_new<CUI3tButton>();	m_JornalBackBtn->SetAutoDelete(true); AttachChild(m_JornalBackBtn);
	m_JornalBackBtn->SetWindowName("jornal_back_btn");
	Register(m_JornalBackBtn);
	AddCallback("jornal_back_btn", BUTTON_CLICKED, CUIWndCallback::void_function(this, &CUIEventsWnd::JornalBackBtn));
	xml_init.Init3tButton(uiXml, "main_wnd:right_frame:work_area:jornal_back_btn", 0, m_JornalBackBtn);
	m_JornalBackBtn->Show(false);


	// news btn
	m_ActiveNews = xr_new<CUI3tButton>();	m_ActiveNews->SetAutoDelete(true); AttachChild(m_ActiveNews);
	m_ActiveNews->SetWindowName("news_list_btn");
	Register(m_ActiveNews);
	AddCallback("news_list_btn", BUTTON_CLICKED, CUIWndCallback::void_function(this, &CUIEventsWnd::NewsNBtn));
	xml_init.Init3tButton(uiXml, "main_wnd:right_frame:work_area:news_list_btn", 0, m_ActiveNews);


	// talk history btn
	m_ActiveTalk = xr_new<CUI3tButton>();	m_ActiveTalk->SetAutoDelete(true); AttachChild(m_ActiveTalk);
	m_ActiveTalk->SetWindowName("talk_list_btn");
	Register(m_ActiveTalk);
	AddCallback("talk_list_btn", BUTTON_CLICKED, CUIWndCallback::void_function(this, &CUIEventsWnd::TalkNBtn));
	xml_init.Init3tButton(uiXml, "main_wnd:right_frame:work_area:talk_list_btn", 0, m_ActiveTalk);



	// сам список заданий HF
	m_ListWnd						= xr_new<CUIScrollView>(); m_ListWnd->SetAutoDelete(true);
	AttachChild(m_ListWnd);
	xml_init.InitScrollView(uiXml, "list", 0, m_ListWnd);

	m_TaskFilter					= xr_new<CUITabControl>(); m_TaskFilter->SetAutoDelete(true);
	m_UILeftFrame->AttachChild		(m_TaskFilter);
	xml_init.InitTabControl			(uiXml, "main_wnd:left_frame:filter_tab", 0, m_TaskFilter);
	m_TaskFilter->SetWindowName		("filter_tab");
	Register						(m_TaskFilter);
    AddCallback						("filter_tab",TAB_CHANGED,CUIWndCallback::void_function(this,&CUIEventsWnd::OnFilterChanged));

	// Timer background
	UITimerBackground		= xr_new<CUIFrameLineWnd>(); UITimerBackground->SetAutoDelete(true);
	AttachChild(UITimerBackground);
	xml_init.InitFrameLine	(uiXml, "timer_frame_line", 0, UITimerBackground);

   m_currFilter						= eActiveTask;
   SetDescriptionMode				(false);

   m_ui_task_item_xml.Init			(CONFIG_PATH, UI_PATH, "job_item.xml");
}

/// активные задания
void CUIEventsWnd::ActiveJobBtn(CUIWindow*, void*)
{
	//Msg("active jobs btn");
	m_ListWnd->Clear();
	UnloadNewsTab();
	UnloadNewsTalkTab();
	UnloadJournalTab();

	LoadActiveJobsTasks();
}

// выполненые  (журнал)
void CUIEventsWnd::CompleteJobBtn(CUIWindow*, void*)
{
	//Msg("CompleteJobBtn jobs btn");
	m_ListWnd->Clear();
	UnloadNewsTab();
	UnloadNewsTalkTab();
	UnloadJournalTab();

	m_JornalBackBtn->Show(true);

	m_ActiveJobBtn->Show(false);
	m_CompleteJobBtn->Show(false);
	m_ActiveNews->Show(false);
	m_ActiveTalk->Show(false);

	LoadJournalTab(ARTICLE_DATA::eJournalArticle);
	//LoadCompleteJobsTasks();
}


//   (журнал назад)
void CUIEventsWnd::JornalBackBtn(CUIWindow*, void*)
{
	//Msg("JornalBackBtn  btn");
	m_ListWnd->Clear();
	UnloadNewsTab();
	UnloadNewsTalkTab();
	UnloadJournalTab();

	m_JornalBackBtn->Show(false);

	m_ActiveJobBtn->Show(true);
	m_CompleteJobBtn->Show(true);
	m_ActiveNews->Show(true);
	m_ActiveTalk->Show(true);

	//LoadJournalTab(ARTICLE_DATA::eJournalArticle);
	//LoadCompleteJobsTasks();
}


// новости
void CUIEventsWnd::NewsNBtn(CUIWindow*, void*)
{
	//Msg("NewsNBtn btn");
	m_ListWnd->Clear();
	UnloadNewsTab();
	UnloadNewsTalkTab();
	UnloadJournalTab();

	LoadNewsTab();
}

// история сообщений
void CUIEventsWnd::TalkNBtn(CUIWindow*, void*)
{
	//Msg("NewsNBtn btn");
	m_ListWnd->Clear();
	UnloadNewsTab();
	UnloadNewsTalkTab();
	UnloadJournalTab();

	LoadNewsTalkTab();
}

void CUIEventsWnd::Update			()
{
	if(m_flags.test(flNeedReload) ){
		ReloadList(false);
		m_flags.set(flNeedReload,FALSE );
	}
	inherited::Update		();
	UpdateDateTime			();
}

void CUIEventsWnd::UpdateDateTime()
{
	static shared_str prevStrTime = " ";
	xr_string strTime = *InventoryUtilities::GetGameTimeAsString(InventoryUtilities::etpTimeToMinutes);
				strTime += " ";
				strTime += *InventoryUtilities::GetGameDateAsString(InventoryUtilities::edpDateToDay);

	if (xr_strcmp(strTime.c_str(), prevStrTime))
	{
		UITimerBackground->UITitleText.SetText(strTime.c_str());
		prevStrTime = strTime.c_str();
	}
}

void CUIEventsWnd::Draw				()
{
	inherited::Draw			();
}

void	CUIEventsWnd::SendMessage			(CUIWindow* pWnd, s16 msg, void* pData)
{
	CUIWndCallback::OnEvent(pWnd, msg, pData);
}

void CUIEventsWnd::OnFilterChanged			(CUIWindow* w, void*)
{
	m_currFilter			=(ETaskFilters)m_TaskFilter->GetActiveIndex();
	ReloadList				(false);
	//Msg("events wnd change btn [%s]", m_currFilter);

}





///////////////////////////////////////////
void CUIEventsWnd::UnloadNewsTab()
{
	m_UIRightWnd->DetachChild(m_UINewsWnd);
	m_UINewsWnd->Show(false);
	//Msg(" !!!!!  UnloadNewsTab");
}

void CUIEventsWnd::LoadNewsTab()
{
	m_UIRightWnd->AttachChild(m_UINewsWnd);
	m_UINewsWnd->Show(true);
	//g_pda_info_state &= ~pda_section::news;
	//Msg("LoadNewsTab");
}


///////////////////////


void CUIEventsWnd::UnloadNewsTalkTab()
{
	m_UIRightWnd->DetachChild(m_UINewsTalkWnd);
	m_UINewsTalkWnd->Show(false);
}

void CUIEventsWnd::LoadNewsTalkTab()
{
	m_UIRightWnd->AttachChild(m_UINewsTalkWnd);
	m_UINewsTalkWnd->Show(true);
}
/////////////////////////////////////////// 





void CUIEventsWnd::UnloadJournalTab()
{
	//Msg("UnloadJournalTab");
	m_UILeftWnd->DetachChild(m_SrcListWnd);
	m_SrcListWnd->RemoveAll();
	m_SrcListWnd->Show(false);

	m_UIRightWnd->DetachChild(m_DescrView);
	m_DescrView->Show(false);
	delete_data(m_ArticlesDB);
	m_DescrView->Clear();
}

void CUIEventsWnd::LoadJournalTab(ARTICLE_DATA::EArticleType _type)
{
	delete_data(m_ArticlesDB);
	//Msg("LoadJournalTab");
	m_UILeftWnd->AttachChild(m_SrcListWnd);
	m_SrcListWnd->Show(true);

	m_UIRightWnd->AttachChild(m_DescrView);
	m_DescrView->Show(true);

	if (Actor()->encyclopedia_registry->registry().objects_ptr())
	{
		ARTICLE_VECTOR::const_iterator it = Actor()->encyclopedia_registry->registry().objects_ptr()->begin();
		for (; it != Actor()->encyclopedia_registry->registry().objects_ptr()->end(); it++)
		{
			if (_type == it->article_type)

			{
				m_ArticlesDB.resize(m_ArticlesDB.size() + 1);
				CEncyclopediaArticle*& a = m_ArticlesDB.back();
				a = xr_new<CEncyclopediaArticle>();
				a->Load(it->article_id);

				//bool bReaded = false;
				CreateTreeBranch(a->data()->group, a->data()->name, m_SrcListWnd, m_ArticlesDB.size() - 1,
					m_pTreeRootFont, m_uTreeRootColor, m_pTreeItemFont, m_uTreeItemColor, it->readed);
			}
		}
	}
//	g_pda_info_state &= !pda_section::journal;

}


void CUIEventsWnd::OnSrcListItemClicked(CUIWindow* w, void* p)
{
	//Msg("OnSrcListItemClicked");
	CUITreeViewItem*	pSelItem = (CUITreeViewItem*)p;
	m_DescrView->Clear();
	if (!pSelItem->IsRoot())
	{
		CUIEncyclopediaArticleWnd*	article_info = xr_new<CUIEncyclopediaArticleWnd>();
		article_info->Init("encyclopedia_item.xml", "encyclopedia_wnd:objective_item");
		article_info->SetArticle(m_ArticlesDB[pSelItem->GetValue()]);
		m_DescrView->AddWindow(article_info, true);

		if (!pSelItem->IsArticleReaded())
		{
			if (Actor()->encyclopedia_registry->registry().objects_ptr())
			{
				for (ARTICLE_VECTOR::iterator it = Actor()->encyclopedia_registry->registry().objects().begin();
					it != Actor()->encyclopedia_registry->registry().objects().end(); it++)
				{
					if (ARTICLE_DATA::eJournalArticle == it->article_type &&
						m_ArticlesDB[pSelItem->GetValue()]->Id() == it->article_id)
					{
						it->readed = true;
						break;
					}
				}
			}
		}



	}
}

/////////////////////////////////////////////////


void CUIEventsWnd::Reload					()
{
		m_flags.set(flNeedReload,TRUE );
		if (m_currFilter == eNewsTask)
		{
			UnloadNewsTab();
		};
}

// эта функция меняет типы 
void CUIEventsWnd::ReloadList(bool bClearOnly) {

	m_ListWnd->Clear			();
	UnloadNewsTab();
	UnloadNewsTalkTab();
	UnloadJournalTab();

	if(bClearOnly)				return;
	if(!g_actor)				return;
	


	if (m_currFilter == eJornalTask)
	{
		//Msg("open eJornalTask");
		LoadJournalTab(ARTICLE_DATA::eJournalArticle);
	};

	if (m_currFilter == eNewsTask)
	{
		LoadNewsTab();
	};



	GameTasks& tasks			= Actor()->GameTaskManager().GameTasks();
	GameTasks::iterator it		= tasks.begin();
	CGameTask* task				= NULL;
	
	for(;it!=tasks.end();++it) {
		task					= (*it).game_task;
		R_ASSERT				(task);
		R_ASSERT				(task->m_Objectives.size() > 0);

		if( !Filter(task) )		continue;
		CUITaskItem* pTaskItem	= NULL;

		CUITaskRootItem* pRoot;
		for (u16 i = 0; i < task->m_Objectives.size(); ++i) {
			if (i==0) {
				pRoot						= xr_new<CUITaskRootItem>(this);
				pTaskItem					= pRoot;
			}
			else {
				pTaskItem					= xr_new<CUITaskSubItem>(this, pRoot);
			}
			pTaskItem->SetGameTask			(task, i);
			m_ListWnd->AddWindow			(pTaskItem,true);
		}

	}

}


// активные задания
void CUIEventsWnd::LoadActiveJobsTasks() {

	GameTasks& tasks = Actor()->GameTaskManager().GameTasks();
	GameTasks::iterator it = tasks.begin();
	CGameTask* task = NULL;

	for (; it != tasks.end(); ++it) {
		task = (*it).game_task;
		R_ASSERT(task);
		R_ASSERT(task->m_Objectives.size() > 0);

		//if (!Filter(task))		continue;
		ETaskState task_state = task->m_Objectives[0].TaskState();
		//if (!task_state == eTaskStateInProgress)		continue;
		if (task_state == eTaskStateInProgress){

			CUITaskItem* pTaskItem = NULL;

			CUITaskRootItem* pRoot;
			for (u16 i = 0; i < task->m_Objectives.size(); ++i) {
				if (i == 0) {
					pRoot = xr_new<CUITaskRootItem>(this);
					pTaskItem = pRoot;
				}
				else {
					pTaskItem = xr_new<CUITaskSubItem>(this, pRoot);
				}
				pTaskItem->SetGameTask(task, i);
				m_ListWnd->AddWindow(pTaskItem, true);
			}
		}

	}

}

// complete задания
void CUIEventsWnd::LoadCompleteJobsTasks() {

	GameTasks& tasks = Actor()->GameTaskManager().GameTasks();
	GameTasks::iterator it = tasks.begin();
	CGameTask* task = NULL;

	for (; it != tasks.end(); ++it) {
		task = (*it).game_task;
		R_ASSERT(task);
		R_ASSERT(task->m_Objectives.size() > 0);

		//if (!Filter(task))		continue;
		//ETaskState task_statee = task->m_Objectives[0].TaskState();
		//if (!task_statee == eTaskStateCompleted)		continue;

		CUITaskItem* pTaskItem = NULL;

		CUITaskRootItem* pRoot;
		for (u16 i = 0; i < task->m_Objectives.size(); ++i) {
			if (i == 0) {
				pRoot = xr_new<CUITaskRootItem>(this);
				pTaskItem = pRoot;
			}
			else {
				pTaskItem = xr_new<CUITaskSubItem>(this, pRoot);
			}
			pTaskItem->SetGameTask(task, i);
			m_ListWnd->AddWindow(pTaskItem, true);
		}

	}

}



void CUIEventsWnd::Show(bool status)
{
	inherited::Show			(status);
	m_UITaskInfoWnd->Show	(status);

	ReloadList				(status == false);

	this->ClearDescription();
}

bool CUIEventsWnd::Filter(CGameTask* t)
{
	ETaskState task_state		= t->m_Objectives[0].TaskState();
//	bool bprimary_only			= m_primary_or_all_filter_btn->GetCheck();

	return (false/*m_currFilter==eOwnTask && task_state==eTaskUserDefined*/ )		||
			( 
			  ( true/*!bprimary_only || (bprimary_only && t->m_is_task_general)*/ )	&&
				(
					(m_currFilter==eAccomplishedTask	&& task_state==eTaskStateCompleted )||
					(m_currFilter==eFailedTask			&& task_state==eTaskStateFail )||
					(m_currFilter==eActiveTask			&& task_state==eTaskStateInProgress )
				)
			);
}


void CUIEventsWnd::SetDescriptionMode		(bool bMap)
{
	if(bMap){
		m_UIRightWnd->DetachChild		(m_UITaskInfoWnd);
		//m_UIRightWnd->AttachChild		(m_UIMapWnd);
	}else{
		//m_UIRightWnd->DetachChild		(m_UIMapWnd);
		m_UIRightWnd->AttachChild		(m_UITaskInfoWnd);
	}
	m_flags.set(flMapMode, bMap);
}

bool CUIEventsWnd::GetDescriptionMode		()
{
	return !!m_flags.test(flMapMode);
}

void CUIEventsWnd::ShowDescription(CGameTask* t, int idx) {
	SGameTaskObjective& o		= t->Objective(0);
	idx							= 0;

	m_UITaskInfoWnd->ClearAll	();
	
	if(Actor()->encyclopedia_registry->registry().objects_ptr()) {
		string512	need_group;
		if(0==idx){
			strcpy(need_group,*t->m_ID);
		}
		else if(o.article_key.size()) {
			sprintf_s(need_group, "%s/%s", *t->m_ID, *o.article_key);
		}
		else {
			sprintf_s(need_group, "%s/%d", *t->m_ID, idx);
		}

		ARTICLE_VECTOR::const_iterator it = Actor()->encyclopedia_registry->registry().objects_ptr()->begin();

		for(; it != Actor()->encyclopedia_registry->registry().objects_ptr()->end(); ++it) {
			if (ARTICLE_DATA::eTaskArticle == it->article_type) {
				CEncyclopediaArticle	A;
				A.Load(it->article_id);
				
				const shared_str& group = A.data()->group;

				if (strstr(group.c_str(), need_group)== group.c_str() ) {
					u32 sz = xr_strlen(need_group);
					if ( group.size()== sz || group.c_str()[sz]=='/' ) {
						m_UITaskInfoWnd->AddArticle(&A);
					}
				}
				else if(o.article_id.size() && it->article_id ==o.article_id) {
					CEncyclopediaArticle A;
					A.Load(it->article_id);
					m_UITaskInfoWnd->AddArticle(&A);
				}
			}
		}
	}

	const int sz = m_ListWnd->GetSize		();

	for(int i=0; i<sz;++i) {
		auto itm = (CUITaskItem*)m_ListWnd->GetItem(i);

		if ((itm->GameTask()==t) && (itm->ObjectiveIdx()==idx) ) {
			itm->MarkSelected(true);
		}
		else {
			itm->MarkSelected(false);
		}
	}
}

bool CUIEventsWnd::ItemHasDescription(CUITaskItem* itm)
{
	if(itm->ObjectiveIdx()==0) {
		return itm->GameTask()->HasLinkedMapLocations();
	}
	else {
		SGameTaskObjective* obj = itm->Objective();
		CMapLocation* ml = obj->LinkedMapLocation();
		const bool bHasLocation = (NULL != ml);
		const bool bIsMapMode = GetDescriptionMode(); 
		return (bIsMapMode&&bHasLocation&&ml->SpotEnabled());
	}
}
void CUIEventsWnd::Reset() {
	inherited::Reset	();
	Reload				();
}

