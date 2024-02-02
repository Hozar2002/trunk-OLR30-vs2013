
#pragma once

#include "customzone.h"
#include "script_export_space.h"

class CAcidFog : public CCustomZone
{
private:
	typedef	CCustomZone	inherited;
public:
	CAcidFog(void);
	virtual ~CAcidFog(void);

	virtual void Load(LPCSTR section);
	virtual void Postprocess(f32 val);
	virtual bool EnableEffector() {return true;}

	virtual void Affect(SZoneObjectInfo* O);

protected:
	virtual bool BlowoutState();
	bool m_bLastBlowoutUpdate;
private:

public:
	DECLARE_SCRIPT_REGISTER_FUNCTION
};
add_to_type_list(CAcidFog)
#undef script_type_list
#define script_type_list save_type_list(CAcidFog)