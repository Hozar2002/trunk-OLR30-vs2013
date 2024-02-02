// UI3dStatic.h
// 
//////////////////////////////////////////////////////////////////////

#ifndef _UI_3D_STATIC_H_
#define _UI_3D_STATIC_H_

#pragma once

#include "uiwindow.h"

#define R1_3DSTATIC_SHADER "models\\3dstatic"
#define R2_3DSTATIC_SHADER "models\\3dstatic"

#define DIST  (VIEWPORT_NEAR + 0.1f)

struct SStaticParamsFromGameObject
{
	Fvector m_rotate;
	float m_scale;
};

class CInventoryItem;
class CGameObject;
class IRender_Visual;

class CUI3dStatic : public CUIWindow
{
public:
	CUI3dStatic();
	virtual ~CUI3dStatic();

	virtual void Update();

	virtual void Draw();
	virtual void Draw3d();

	void Draw3D() {}
	void RemoveModelRadius();
	IRender_Visual* Visual();

	void SetRotate(float x, float y, float z);
	void Destroy();

	void SetRotatedMode(const bool value);
	bool GetRotatedMode() const;

	void SetModelRadius(float value);
	float GetModelRadius() const;

	void SetModelConstPosition(bool value);
	bool GetModelConstPosition() const;

	void SetGameObject(CInventoryItem* pItem);
	void SetGameObject(CGameObject* pItem, SStaticParamsFromGameObject params);

protected:
	float m_x_angle, m_y_angle, m_z_angle;
	float m_scale;

	Fmatrix XFORM;
	IRender_Visual* m_pCurrentVisual;

	void FromScreenToItem(float x_screen, float y_screen, float& x_item, float& y_item);

private:
	bool m_model_const_position;
	float m_model_radius;
	bool m_rotated;
	u32 m_dwRotateTime;
	float pRotateSpeed;
	float pi_mul_2;

protected:
	void SetDefaultValues();
	virtual void ModelFree();
	IRender_Visual* model_instance_Load(LPCSTR name);
	IRender_Visual* create_model(LPCSTR name);
};

class CUI3DActorStatic : public CUI3dStatic
{
	using inherited = CUI3dStatic;

private:
	IRender_Visual* m_pWeaponVisual;

public:
	CUI3DActorStatic();
	virtual void ModelFree() override;

public:
	virtual void Draw3d() override;

public:
	virtual void SelectWeapon(CWeapon* pWeapon);
};

#endif