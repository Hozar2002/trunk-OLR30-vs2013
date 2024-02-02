#include "stdafx.h"
#include "uizonemap.h"

#include "hudmanager.h"



#include "InfoPortion.h"
#include "Pda.h"

#include "Grenade.h"
#include "level.h"
#include "game_cl_base.h"

#include "actor.h"
#include "ai_space.h"
#include "game_graph.h"

#include "ui/UIMap.h"
#include "ui/UIXmlInit.h"
//////////////////////////////////////////////////////////////////////////

CUIZoneMap::CUIZoneMap() {
	m_fZoomFactor = 1.f;
}

CUIZoneMap::~CUIZoneMap() {
	
}

void CUIZoneMap::Init() {

	CUIXml uiXml;
	bool xml_result = uiXml.Init(CONFIG_PATH, UI_PATH, "zone_map.xml");
	R_ASSERT3(xml_result, "xml file not found", "zone_map.xml");

	// load map background
	CUIXmlInit xml_init;
	xml_init.InitStatic(uiXml, "minimap:background", 0, &m_background);

	if (IsGameTypeSingle()){
		xml_init.InitStatic(uiXml, "minimap:background:dist_text", 0, &m_pointerDistanceText);
		m_background.AttachChild(&m_pointerDistanceText);
	}

	xml_init.InitStatic(uiXml, "minimap:level_frame", 0, &m_clipFrame);

	xml_init.InitStatic(uiXml, "minimap:center", 0, &m_center);
	
	
	m_activeMap = xr_new<CUIMiniMap>();
	m_clipFrame.AttachChild(m_activeMap);
	m_activeMap->SetAutoDelete(true);

	m_activeMap->EnableHeading(true);  
	xml_init.InitStatic(uiXml, "minimap:compass", 0, &m_compass);
	m_activeMap->SetWindowName("minimap");

//	m_background.AttachChild(&m_compass);

	xml_init.InitStatic					(uiXml, "static_map_zoom", 0, &UIPdaZoom);
	m_background.AttachChild	(&UIPdaZoom);

	string64 str;
	sprintf_s(str,"%.1fx",m_fZoomFactor);
	UIPdaZoom.SetText(str);

	m_clipFrame.AttachChild(&m_center);
	m_center.SetWndPos(m_clipFrame.GetWidth()/2,m_clipFrame.GetHeight()/2);
}

void CUIZoneMap::Render() {
	m_clipFrame.Draw();
	m_background.Draw();
	m_compass.Draw();
}

void CUIZoneMap::SetHeading(float angle) {
	m_activeMap->SetHeading(angle);
	m_compass.SetHeading(angle);
};

void CUIZoneMap::UpdateRadar(Fvector pos) {
	m_clipFrame.Update();
	m_background.Update();
	m_activeMap->SetActivePoint( pos );

	if(IsGameTypeSingle()) {
		if(m_activeMap->GetPointerDistance()>0.5f) {
			string64 str;
			sprintf_s(str,"%.1f m.",m_activeMap->GetPointerDistance());
			m_pointerDistanceText.SetText(str);
		}
		else{
			m_pointerDistanceText.SetText("");
		}
	}
}

bool CUIZoneMap::ZoomIn() {
	return true;
}

bool CUIZoneMap::ZoomOut() {
	return true;
}

void CUIZoneMap::SetupCurrentMap() {
	CInifile* pLtx = pGameIni;

	if(!pLtx->section_exist(Level().name())) {
		pLtx = Level().pLevel;
	}

	m_clipFrame.DetachChild			(m_activeMap);
	m_activeMap						= xr_new<CUIMiniMap>();
	m_clipFrame.AttachChild			(m_activeMap);
	m_activeMap->SetAutoDelete		(true);
	m_activeMap->EnableHeading		(true);
	m_activeMap->SetWindowName		("minimap");

	m_activeMap->Init(Level().name(),*pLtx,"hud\\hud_map");

	Frect r;
	m_clipFrame.GetAbsoluteRect(r);
	m_activeMap->SetClipRect(r);
	
	SetZoomFactor(5.f);
}

void CUIZoneMap::SetZoomFactor(float value) {
	if (value > pMaxZoomFactor) value = pMaxZoomFactor;
	if (value < pMinZoomFactor) value = pMinZoomFactor;
	if (m_fZoomFactor == value) return ;
	m_fZoomFactor = value;
	Fvector2 wnd_size;
	const float zoom_factor = (static_cast<float>(m_clipFrame.GetWndRect().width())/800.0f)*value;
	wnd_size.x = m_activeMap->BoundRect().width()*zoom_factor;
	wnd_size.y = m_activeMap->BoundRect().height()*zoom_factor;
	m_activeMap->SetWndSize(wnd_size);

	string64 str;
	sprintf_s(str,"%.1fx",m_fZoomFactor);
	
	UIPdaZoom.SetText(str);
}

void CUIZoneMap::load(IReader &input_packet) {
	SetZoomFactor(input_packet.r_float());
}

void CUIZoneMap::save(NET_Packet &output_packet) {
	output_packet.w_float(GetZoomFactor());
}

