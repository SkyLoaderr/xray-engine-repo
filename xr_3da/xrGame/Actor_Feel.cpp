#include "stdafx.h"
#include "actor.h"
#include "weapon.h"
#include "mercuryball.h"
#include "inventory.h"
#include "hudmanager.h"
#include "character_info.h"
#include "xr_level_controller.h"
#include "UsableScriptObject.h"
#include "customzone.h"
#include "level.h"
#include "GameMtlLib.h"
#include "ui/UIMainIngameWnd.h"
#include "Grenade.h"
#include "clsid_game.h"

#define PICKUP_INFO_COLOR 0xFFDDDDDD
//AAAAAA

void CActor::feel_touch_new				(CObject* O)
{
}

void CActor::feel_touch_delete	(CObject* O)
{
}

BOOL CActor::feel_touch_contact		(CObject *O)
{
	CInventoryItem	*item = smart_cast<CInventoryItem*>(O);
	CInventoryOwner	*inventory_owner = smart_cast<CInventoryOwner*>(O);

	if (item && item->Useful() && !item->object().H_Parent()) 
		return TRUE;

	if(inventory_owner && inventory_owner != smart_cast<CInventoryOwner*>(this))
		return TRUE;

/**
	CCustomZone	*custom_zone = smart_cast<CCustomZone*>(O);
	if (!custom_zone)
		return	(TRUE);

	if (custom_zone->inside(Position()))
		return	(TRUE);
/**/

	return		(FALSE);
}

BOOL CActor::feel_touch_on_contact	(CObject *O)
{
	CCustomZone	*custom_zone = smart_cast<CCustomZone*>(O);
	if (!custom_zone)
		return	(TRUE);

	Fsphere		sphere;
	sphere.P	= Position();
	sphere.R	= EPS_L;
	if (custom_zone->inside(sphere))
		return	(TRUE);

	return		(FALSE);
}

void CActor::PickupModeOn()
{
	m_bPickupMode = true;
}

void CActor::PickupModeOff()
{
	m_bPickupMode = false;
}

ICF static BOOL info_trace_callback(collide::rq_result& result, LPVOID params)
{
	BOOL& bOverlaped	= *(BOOL*)params;
	if(result.O){
		if (Level().CurrentEntity()!=result.O){	
			bOverlaped		= TRUE;
			return			FALSE;
		}else{
			return			TRUE;
		}
	}else{
		//�������� ����������� � ������ ��� ��������
		CDB::TRI* T		= Level().ObjectSpace.GetStaticTris()+result.element;
		if (GMLib.GetMaterialByIdx(T->material)->Flags.is(SGameMtl::flPassable)) 
			return TRUE;
	}	
	bOverlaped			= TRUE;
	return				FALSE;
}

BOOL CanPickItem(const CFrustum& frustum, const Fvector& from, CObject* item)
{
	BOOL	bOverlaped		= FALSE;
	Fvector dir,to; 
	item->Center			(to);
	float range				= dir.sub(to,from).magnitude();
	if (range>0.25f){
		if (frustum.testSphere_dirty(to,item->Radius())){
			dir.div	(range);
			collide::ray_defs	RD	(from, dir, range, CDB::OPT_CULL, collide::rqtBoth);
			collide::rq_results	RQR	;
			Level().ObjectSpace.RayQuery		(RQR,RD, info_trace_callback, &bOverlaped, NULL, item);
		}
	}
	return !bOverlaped;
}

void CActor::PickupModeUpdate()
{
	if(!m_bPickupMode) return;

	//���������� �������
	if(inventory().m_pTarget && inventory().m_pTarget->Useful() &&
		m_pUsableObject && m_pUsableObject->nonscript_usable())
	{
		NET_Packet P;
		u_EventGen(P,GE_OWNERSHIP_TAKE, ID());
		P.w_u16(inventory().m_pTarget->object().ID());
		u_EventSend(P);
	}

	//. ????? GetNearest ?????
	feel_touch_update	(Position(), /*inventory().GetTakeDist()*/m_fPickupInfoRadius);
	
	CFrustum frustum;
	frustum.CreateFromMatrix(Device.mFullTransform,FRUSTUM_P_LRTB|FRUSTUM_P_FAR);
	//. slow (ray-query test)
	for(xr_vector<CObject*>::iterator it = feel_touch.begin(); it != feel_touch.end(); it++)
		if (CanPickItem(frustum,Device.vCameraPosition,*it)) PickupInfoDraw(*it);
}

#include "../CameraBase.h"
BOOL	g_b_COD_PickUpMode = FALSE;
void	CActor::PickupModeUpdate_COD	()
{
	if (Level().CurrentViewEntity() != this || !g_b_COD_PickUpMode) return;
		
	if (!g_Alive() || eacFirstEye != cam_active) 
	{
		HUD().GetUI()->UIMainIngameWnd->SetPickUpItem(NULL);
		return;
	};
	
	CFrustum frustum;
	frustum.CreateFromMatrix(Device.mFullTransform,FRUSTUM_P_LRTB|FRUSTUM_P_FAR);

	//---------------------------------------------------------------------------
	xr_vector<ISpatial*>	ISpatialResult;
	g_SpatialSpace->q_frustum(ISpatialResult, 0, STYPE_COLLIDEABLE, frustum);
	//---------------------------------------------------------------------------

	float maxlen = 1000.0f;
	CInventoryItem* pNearestItem = NULL;
	for (u32 o_it=0; o_it<ISpatialResult.size(); o_it++)
	{
		ISpatial*		spatial	= ISpatialResult[o_it];
		CInventoryItem*	pIItem	= smart_cast<CInventoryItem*> (spatial->dcast_CObject        ());
		if (0 == pIItem) continue;
		if (pIItem->object().H_Parent() != NULL) continue;
		if (pIItem->object().CLS_ID == CLSID_OBJECT_G_RPG7 || pIItem->object().CLS_ID == CLSID_OBJECT_G_FAKE)
			continue;
		CGrenade*	pGrenade	= smart_cast<CGrenade*> (spatial->dcast_CObject        ());
		if (pGrenade && !pGrenade->Useful()) continue;
		
		Fvector A, B, tmp; 
		pIItem->object().Center			(A);
		if (A.distance_to_sqr(Position())>4) continue;

		tmp.sub(A, cam_Active()->vPosition);
		B.mad(cam_Active()->vPosition, cam_Active()->vDirection, tmp.dotproduct(cam_Active()->vDirection));
		float len = B.distance_to_sqr(A);
		if (len > 1) continue;

		if (maxlen>len && !pIItem->object().getDestroy())
		{
			maxlen = len;
			pNearestItem = pIItem;
		};
	}
	HUD().GetUI()->UIMainIngameWnd->SetPickUpItem(pNearestItem);

	if (pNearestItem && m_bPickupMode)
	{
		//���������� �������
		NET_Packet P;
		u_EventGen(P,GE_OWNERSHIP_TAKE, ID());
		P.w_u16(pNearestItem->object().ID());
		u_EventSend(P);

		PickupModeOff();
	}
};

void CActor::PickupInfoDraw(CObject* object)
{
	LPCSTR draw_str = NULL;
	
	CInventoryItem* item = smart_cast<CInventoryItem*>(object);
	CInventoryOwner* inventory_owner = smart_cast<CInventoryOwner*>(object);
	
	VERIFY(item || inventory_owner);

	Fmatrix res;
	res.mul(Device.mFullTransform,object->XFORM());
	Fvector4 v_res;
	Fvector shift;
	if(item){
		draw_str = item->NameComplex();
		shift.set(0,0,0);
	}else if(inventory_owner){
		draw_str = inventory_owner->Name();
		shift.set(0,1.2f,0);
	}

	res.transform(v_res,shift);

	if (v_res.z < 0 || v_res.w < 0)	return;
	if (v_res.x < -1.f || v_res.x > 1.f || v_res.y<-1.f || v_res.y>1.f) return;

	float x = (1.f + v_res.x)/2.f * (Device.dwWidth);
	float y = (1.f - v_res.y)/2.f * (Device.dwHeight);

	HUD().Font().pFontMedium->SetAligment	(CGameFont::alCenter);
	HUD().Font().pFontMedium->SetColor		(PICKUP_INFO_COLOR);
	HUD().Font().pFontMedium->Out			(x,y,draw_str);
}

void CActor::feel_sound_new(CObject* who, int type, CSound_UserDataPtr user_data, const Fvector& Position, float power)
{
	if(who == this)
		m_snd_noise = _max(m_snd_noise,power);
}
