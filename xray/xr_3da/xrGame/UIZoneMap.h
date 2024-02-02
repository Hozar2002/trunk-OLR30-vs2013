#pragma once


#include "ui/UIStatic.h"

class CActor;
class CUICustomMap;
//////////////////////////////////////////////////////////////////////////
const float pUpdateZoomFactorByClick = 0.2;
const float pMaxZoomFactor = 8.0;
const float pMinZoomFactor = 1.0;

class CUIZoneMap {
	CUICustomMap*				m_activeMap;
	float						m_fScale;

	CUIStatic					m_background;
	CUIStatic					m_center;
	CUIStatic					m_compass;
	CUIStatic					m_clipFrame;
	CUIStatic					m_pointerDistanceText;

	CUIStatic			       UIPdaZoom;
	
	float						m_fZoomFactor;

	CUIStatic*			GetPDAZoom					() { return &UIPdaZoom; };

public:
								CUIZoneMap		();
	virtual						~CUIZoneMap		();

	void						SetHeading		(float angle);
	void						Init			();

	void						Render			();
	void						UpdateRadar		(Fvector pos);

	void						SetScale		(float s)							{m_fScale = s;}
	float						GetScale		() const {
		return m_fScale;
	}

	bool						ZoomIn			();
	bool						ZoomOut			();


	CUIStatic&					Background		()  {
		return m_background;
	};
	CUIStatic&					ClipFrame		()  {
		return m_clipFrame;
	}; // alpet: ��� �������� � �������
	CUIStatic&					Compass			()  {
		return m_compass;
	}; // alpet: ��� �������� � �������

	void						SetupCurrentMap	();
	
	void						SetZoomFactor(float value);
	float						GetZoomFactor() const {
		return m_fZoomFactor;
	}
	
	void						IncZoom() {
		SetZoomFactor(GetZoomFactor()+pUpdateZoomFactorByClick);
	}
	void						DecZoom() {
		SetZoomFactor(GetZoomFactor()-pUpdateZoomFactorByClick);
	}
	
	void						load(IReader &input_packet);
	void						save(NET_Packet &output_packet);
};

