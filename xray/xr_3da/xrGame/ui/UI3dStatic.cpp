// UI3dStatic.cpp: 
// 
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ui3dstatic.h"
#include "../Inventory_Item.h"
#include "../Actor.h"
#include "../gameobject.h"
#include "../../SkeletonAnimated.h"
#include "../HUDManager.h"
#include "../../xr_3da/fbasicvisual.h"
#include "../weapon.h"
#include "../WeaponMagazined.h"
#include "../../skeletoncustom.h"
#include "../xrserver_objects_alife_items.h"
#include "../InventoryOwner.h"

CUI3dStatic::CUI3dStatic()
{
	m_x_angle = 0.f;
	m_y_angle = 0.f;
	m_z_angle = 0.f;
	m_dwRotateTime = 0;
	m_scale = 1.f;
	m_pCurrentVisual = NULL;
	m_rotated = false;
	m_model_radius = -1;
	m_model_const_position = false;
	pRotateSpeed = 0.01;
	pi_mul_2 = M_PI*2.f;
	Enable(false);
}

CUI3dStatic::~CUI3dStatic()
{
	Destroy();
}

void CUI3dStatic::FromScreenToItem(float x_screen, float y_screen, float& x_item, float& y_item)
{
	float x = x_screen;
	float y = y_screen;

	float halfwidth = float(Device.dwWidth / 2);
	float halfheight = float(Device.dwHeight / 2);

	float size_y = VIEWPORT_NEAR * tanf(deg2rad(Device.fFOV) * 0.5f);
	float size_x = size_y / (Device.fASPECT);

	float r_pt = float(x - halfwidth) * size_x / (float)halfwidth;
	float u_pt = float(halfheight - y) * size_y / (float)halfheight;

	x_item = r_pt * DIST / VIEWPORT_NEAR;
	y_item = u_pt * DIST / VIEWPORT_NEAR;
}

void CUI3dStatic::Draw()
{
	Draw3d();
}

void CUI3dStatic::Draw3d()
{
	// BASE1707: ��������� �������. �������� ��������� ������ ������������ :^)
	if (Actor() && Actor()->m_bControllerImpact)
		return;

	if (m_pCurrentVisual)
	{
		::Render->set_Object(NULL);
		::Render->rpmask(true, false, false);
		::Render->set_Transform(&XFORM);
		::Render->add_Visual(m_pCurrentVisual);
		::Render->flush();
		::Render->rpmask(true, false, false);
	}
}

void  CUI3dStatic::Update()
{
	if (m_rotated)
	{
		if (m_dwRotateTime < Device.dwTimeGlobal)
		{
			m_dwRotateTime = Device.dwTimeGlobal + 10;
			m_y_angle += pRotateSpeed;
			if (m_y_angle>pi_mul_2)
			{
				m_y_angle -= pi_mul_2;
			}
		}
	}

	if (m_pCurrentVisual)
	{
		Frect rect;
		GetAbsoluteRect(rect);

		rect.top = (UI()->ClientToScreenScaledY(rect.top));
		rect.left = (UI()->ClientToScreenScaledX(rect.left));
		rect.bottom = (UI()->ClientToScreenScaledY(rect.bottom));
		rect.right = (UI()->ClientToScreenScaledX(rect.right));

		Fmatrix translate_matrix;
		Fmatrix scale_matrix;

		Fmatrix rx_m;
		Fmatrix ry_m;
		Fmatrix rz_m;

		XFORM.identity();

		translate_matrix.identity();

		if (m_model_const_position)
		{
			translate_matrix.translate(0.f, 0.f, 0.f);
		}
		else
		{
			translate_matrix.translate(
				-m_pCurrentVisual->vis.sphere.P.x,
				-m_pCurrentVisual->vis.sphere.P.y,
				-m_pCurrentVisual->vis.sphere.P.z
				);
		}

		XFORM.mulA_44(translate_matrix);

		rx_m.identity();
		rx_m.rotateX(m_x_angle);
		ry_m.identity();
		ry_m.rotateY(m_y_angle);
		rz_m.identity();
		rz_m.rotateZ(m_z_angle);

		XFORM.mulA_44(rx_m);
		XFORM.mulA_44(ry_m);
		XFORM.mulA_44(rz_m);

		float x1, y1, x2, y2;

		FromScreenToItem(rect.left, rect.top, x1, y1);
		FromScreenToItem(rect.right, rect.bottom, x2, y2);

		float normal_size = _min(_abs(x2 - x1), _abs(y2 - y1));
		float radius = (m_model_radius >= 0) ? m_model_radius : m_pCurrentVisual->vis.sphere.R;
		float scale = (normal_size / (radius * 2)) * m_scale;

		scale_matrix.identity();
		scale_matrix.scale(scale, scale, scale);

		XFORM.mulA_44(scale_matrix);

		float right_item_offset, up_item_offset;

		FromScreenToItem(
			rect.left + iFloor(UI()->ClientToScreenScaledX(GetWidth() / 2)),
			rect.top + iFloor(UI()->ClientToScreenScaledY(GetHeight() / 2)),
			right_item_offset,
			up_item_offset
			);

		translate_matrix.identity();
		translate_matrix.translate(right_item_offset,
			up_item_offset,
			DIST);

		XFORM.mulA_44(translate_matrix);

		Fmatrix camera_matrix;
		camera_matrix.identity();
		camera_matrix = Device.mView;
		camera_matrix.invert();

		XFORM.mulA_44(camera_matrix);
	}
}

void CUI3dStatic::SetRotate(float x, float y, float z)
{
	m_x_angle = x;
	m_y_angle = y;
	m_z_angle = z;
}

void CUI3dStatic::SetRotatedMode(const bool value)
{
	m_rotated = value;
}
bool CUI3dStatic::GetRotatedMode() const
{
	return m_rotated;
}

IRender_Visual* CUI3dStatic::Visual()
{
	return m_pCurrentVisual;
}

void CUI3dStatic::SetModelRadius(float value)
{
	m_model_radius = value;
}
float CUI3dStatic::GetModelRadius() const
{
	return m_model_radius;
}
void CUI3dStatic::RemoveModelRadius()
{
	m_model_radius = -1;
}

void CUI3dStatic::SetModelConstPosition(bool value)
{
	m_model_const_position = value;
}
bool CUI3dStatic::GetModelConstPosition() const
{
	return m_model_const_position;
}

IRender_Visual* CUI3dStatic::model_instance_Load(LPCSTR name)
{
	LPCSTR buffer{ ::Render->getShaderFromModel() };
	if (::Render->get_generation() == ::Render->GENERATION_R2)
	{
		::Render->setShaderFromModel(R2_3DSTATIC_SHADER);
	}
	else
	{
		::Render->setShaderFromModel(R1_3DSTATIC_SHADER);
	}
	IRender_Visual* m = ::Render->model_Instance_Load(name);
	::Render->setShaderFromModel(buffer);
	return m;
}

IRender_Visual* CUI3dStatic::create_model(LPCSTR name)
{
	LPCSTR buffer{ ::Render->getShaderFromModel() };
	if (::Render->get_generation() == ::Render->GENERATION_R2)
	{
		::Render->setShaderFromModel(R2_3DSTATIC_SHADER);
	}
	else
	{
		::Render->setShaderFromModel(R1_3DSTATIC_SHADER);
	}
	IRender_Visual* m = ::Render->model_Create(name);
	::Render->setShaderFromModel(buffer);
	return m;
}

void CUI3dStatic::SetGameObject(CGameObject* pItem, SStaticParamsFromGameObject params)
{
	R_ASSERT(pItem);
	ModelFree();

	m_pCurrentVisual = model_instance_Load(pItem->cNameVisual().c_str());

	if (auto visual = smart_cast<CKinematics*>(m_pCurrentVisual))
	{
		visual->CalculateBones_Invalidate();
		visual->CalculateBones();
	}

	m_x_angle = params.m_rotate.x;
	m_y_angle = params.m_rotate.y;
	m_z_angle = params.m_rotate.z;
	m_scale = params.m_scale;
}

void CUI3dStatic::SetGameObject(CInventoryItem* pItem)
{
	R_ASSERT(pItem);
	LPCSTR visual_name = pItem->Get3DStaticVisualName();

	if (visual_name == NULL)
	{
		R_ASSERT(smart_cast<CGameObject*>(pItem));
		visual_name = smart_cast<CGameObject*>(pItem)->cNameVisual().c_str();

		ModelFree();
		m_pCurrentVisual = model_instance_Load(visual_name);
	}
	else
	{
		ModelFree();
		m_pCurrentVisual = create_model(visual_name);
	}

	auto visual = smart_cast<CKinematics*>(m_pCurrentVisual);
	if (visual)
	{
		CWeapon* pWpn = smart_cast<CWeaponMagazined*>(pItem);
		if (pWpn)
		{
			LPCSTR wpn_scope = "wpn_scope";
			LPCSTR wpn_silencer = "wpn_silencer";
			LPCSTR wpn_grenade_launcher = "wpn_grenade_launcher";
			LPCSTR wpn_launcher = "wpn_launcher";

			u16 bone_id;

			bone_id = visual->LL_BoneID(wpn_scope);
			if (pWpn->ScopeAttachable())
			{
				if (pWpn->IsScopeAttached())
				{
					if (!visual->LL_GetBoneVisible(bone_id))
						visual->LL_SetBoneVisible(bone_id, TRUE, TRUE);
				}
				else
				{
					if (visual->LL_GetBoneVisible(bone_id))
						visual->LL_SetBoneVisible(bone_id, FALSE, TRUE);
				}
			}

			if (pWpn->GetScopeStatus() == CSE_ALifeItemWeapon::eAddonDisabled && bone_id != BI_NONE && visual->LL_GetBoneVisible(bone_id))
				visual->LL_SetBoneVisible(bone_id, FALSE, TRUE);

			bone_id = visual->LL_BoneID(wpn_silencer);
			if (pWpn->SilencerAttachable())
			{
				if (pWpn->IsSilencerAttached())
				{
					if (!visual->LL_GetBoneVisible(bone_id))
						visual->LL_SetBoneVisible(bone_id, TRUE, TRUE);
				}
				else
				{
					if (visual->LL_GetBoneVisible(bone_id))
						visual->LL_SetBoneVisible(bone_id, FALSE, TRUE);
				}
			}
			if (pWpn->GetSilencerStatus() == CSE_ALifeItemWeapon::eAddonDisabled && bone_id != BI_NONE && visual->LL_GetBoneVisible(bone_id))
				visual->LL_SetBoneVisible(bone_id, FALSE, TRUE);

			bone_id = visual->LL_BoneID(wpn_launcher);
			if (pWpn->GrenadeLauncherAttachable())
			{
				if (pWpn->IsGrenadeLauncherAttached())
				{
					if (!visual->LL_GetBoneVisible(bone_id))
						visual->LL_SetBoneVisible(bone_id, TRUE, TRUE);
				}
				else
				{
					if (visual->LL_GetBoneVisible(bone_id))
						visual->LL_SetBoneVisible(bone_id, FALSE, TRUE);
				}
			}
			if (pWpn->GetGrenadeLauncherStatus() == CSE_ALifeItemWeapon::eAddonDisabled && bone_id != BI_NONE && visual->LL_GetBoneVisible(bone_id))
				visual->LL_SetBoneVisible(bone_id, FALSE, TRUE);
		}

		visual->CalculateBones_Invalidate();
		visual->CalculateBones(true);
	}

	const Fvector rotate = pItem->Get3DStaticRotate();

	m_x_angle = rotate.x;
	m_y_angle = rotate.y;
	m_z_angle = rotate.z;

	m_scale = pItem->Get3DStaticScale();
}

void CUI3dStatic::SetDefaultValues()
{
	m_x_angle = m_y_angle = m_z_angle = 0.f;
	m_dwRotateTime = 0;
	m_scale = 1.f;
}

void CUI3dStatic::ModelFree()
{
	if (m_pCurrentVisual)
	{
		::Render->model_Delete(m_pCurrentVisual);
		m_pCurrentVisual = nullptr;
	}
}

void CUI3dStatic::Destroy()
{
	this->ModelFree();
	this->SetDefaultValues();
}

CUI3DActorStatic::CUI3DActorStatic() : inherited(), m_pWeaponVisual{}
{
}

void CUI3DActorStatic::ModelFree()
{
	inherited::ModelFree();

	if (m_pWeaponVisual)
	{
		::Render->model_Delete(m_pWeaponVisual);
		m_pWeaponVisual = nullptr;
	}
}

void CUI3DActorStatic::Draw3d()
{
	// BASE1707: ��������� �������. �������� ��������� ������ ������������ :^)
	if (Actor() && Actor()->m_bControllerImpact)
		return;

	// ������������ �������� ��
	inherited::Draw3d();

	if (m_pWeaponVisual && m_pCurrentVisual)
	{
		// �� ���� ������� m_pWeaponVisual ������ ��������������� ������� ��������� �������� �������
		if (CWeapon* pWeapon = smart_cast<CWeapon*>(Actor()->inventory().ActiveItem()))
		{
			// ����� ��������� ���������!
			if (!pWeapon->m_bInventoryDraw3D)
				return;

			CKinematics* v = smart_cast<CKinematics*>(m_pCurrentVisual);
			CKinematics* vw = smart_cast<CKinematics*>(m_pWeaponVisual);
			if (!v || !vw)
				return;

			LPCSTR wpn_scope = "wpn_scope";
			LPCSTR wpn_silencer = "wpn_silencer";
			LPCSTR wpn_grenade_launcher = "wpn_grenade_launcher";
			LPCSTR wpn_launcher = "wpn_launcher";

			// # Настраиваем прицел
			u16 bone_id = vw->LL_BoneID(wpn_scope);

			// Указанная кость присутствует в модели
			if (bone_id != BI_NONE)
			{
				// Если прицел может быть присоединён -
				if (pWeapon->ScopeAttachable())
				{
					// Если прицеплен - отрисовываем
					if (pWeapon->IsScopeAttached())
					{
						if (!vw->LL_GetBoneVisible(bone_id))
						{
							vw->LL_SetBoneVisible(bone_id, TRUE, TRUE);
						}
					}
					// Иначе - скрываем
					else
					{
						if (vw->LL_GetBoneVisible(bone_id))
						{
							vw->LL_SetBoneVisible(bone_id, FALSE, TRUE);
						}
					}
				}
				// Если же прицел отключён - скрываем кость
				else if (pWeapon->GetScopeStatus() == CSE_ALifeItemWeapon::eAddonDisabled)
				{
					if (vw->LL_GetBoneVisible(bone_id))
					{
						vw->LL_SetBoneVisible(bone_id, FALSE, TRUE);
					}
				}
			}

			// # Настраиваем глушитель
			bone_id = vw->LL_BoneID(wpn_silencer);

			// # См. пример выше
			if (bone_id != BI_NONE)
			{
				if (pWeapon->SilencerAttachable())
				{
					if (pWeapon->IsSilencerAttached())
					{
						if (!vw->LL_GetBoneVisible(bone_id))
						{
							vw->LL_SetBoneVisible(bone_id, TRUE, TRUE);
						}
					}
					else
					{
						if (vw->LL_GetBoneVisible(bone_id))
						{
							vw->LL_SetBoneVisible(bone_id, FALSE, TRUE);
						}
					}
				}
				else if (pWeapon->GetSilencerStatus() == CSE_ALifeItemWeapon::eAddonDisabled)
				{
					if (vw->LL_GetBoneVisible(bone_id))
					{
						vw->LL_SetBoneVisible(bone_id, FALSE, TRUE);
					}
				}
			}

			// # Настраиваем подствольный гранатомёт
			bone_id = vw->LL_BoneID(wpn_launcher);

			// # См. пример выше
			if (bone_id != BI_NONE)
			{
				if (pWeapon->GrenadeLauncherAttachable())
				{
					if (pWeapon->IsGrenadeLauncherAttached())
					{
						if (!vw->LL_GetBoneVisible(bone_id))
						{
							vw->LL_SetBoneVisible(bone_id, TRUE, TRUE);
						}
					}
					else
					{
						if (vw->LL_GetBoneVisible(bone_id))
						{
							vw->LL_SetBoneVisible(bone_id, FALSE, TRUE);
						}
					}
				}
				else if (pWeapon->GetGrenadeLauncherStatus() == CSE_ALifeItemWeapon::eAddonDisabled)
				{
					if (vw->LL_GetBoneVisible(bone_id))
					{
						vw->LL_SetBoneVisible(bone_id, FALSE, TRUE);
					}
				}
			}

			int boneL = v->LL_BoneID("bip01_l_finger1");
			int boneR = v->LL_BoneID("bip01_r_finger1");
			int boneR2 = v->LL_BoneID("bip01_r_finger11");

			if ((pWeapon->HandDependence() == hd1Hand) || (pWeapon->GetState() == CWeapon::eReload || pWeapon->GetState() == CWeapon::eJammed))
				boneL = boneR2;

			// # ������� ����� � �������� -1 - ������� �����(-�)
			if (boneR2 == BI_NONE || boneL == BI_NONE || boneR == BI_NONE)
				return;

			v->CalculateBones_Invalidate();
			v->CalculateBones(true);

			Fmatrix& mL = v->LL_GetTransform(u16(boneL));
			Fmatrix& mR = v->LL_GetTransform(u16(boneR));

			// Calculate
			Fmatrix mRes;
			Fvector	R, D, N;
			D.sub(mL.c, mR.c);

			if (fis_zero(D.magnitude()))
			{
				mRes.set(XFORM);
				mRes.c.set(mR.c);
			}
			else
			{
				D.normalize();
				R.crossproduct(mR.j, D);

				N.crossproduct(D, R);
				N.normalize();

				mRes.set(R, N, D, mR.c);
				mRes.mulA_43(XFORM);
			}

			// # ��������� ����� ���������� ������
			XFORM.mul(mRes, pWeapon->GetWorldOffset());

			// # ������� � ������ (���������� ���������������� XFORM �������� ������ ������)
			::Render->set_Object(nullptr);
			::Render->rpmask(true, false, false);
			::Render->set_Transform(&XFORM);
			::Render->add_Visual(m_pWeaponVisual);
			::Render->flush();
			::Render->rpmask(true, false, false);
		}
	}
}

void CUI3DActorStatic::SelectWeapon(CWeapon* pWeapon)
{
	if (m_pWeaponVisual)
	{
		::Render->model_Delete(m_pWeaponVisual);
		m_pWeaponVisual = nullptr;
	}

	if (!pWeapon)
		return;

	m_pWeaponVisual = model_instance_Load(pWeapon->cNameVisual().c_str());

	if (auto v = smart_cast<CKinematics*>(m_pWeaponVisual))
	{
		v->CalculateBones_Invalidate();
		v->CalculateBones();
	}
}