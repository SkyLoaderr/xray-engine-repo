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

	if (custom_zone->inside(Position()))
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
		bOverlaped		= TRUE;
		return			FALSE;
	}else{
		//получить треугольник и узнать его материал
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
			BOOL item_en		= item->getEnabled();
			item->setEnabled	(FALSE);
			Level().CurrentEntity()->setEnabled(FALSE);
			collide::ray_defs RD(from, dir, range, 0, collide::rqtBoth);
			Level().ObjectSpace.RayQuery(RD, info_trace_callback, &bOverlaped);
			Level().CurrentEntity()->setEnabled(TRUE);
			item->setEnabled	(item_en);
		}
	}
	return !bOverlaped;
}

void CActor::PickupModeUpdate()
{
	if(!m_bPickupMode) return;

	//подбирание объекта
	if(inventory().m_pTarget && inventory().m_pTarget->Useful() &&
		m_pUsableObject && m_pUsableObject->nonscript_usable())
	{
		NET_Packet P;
		u_EventGen(P,GE_OWNERSHIP_TAKE, ID());
		P.w_u16(inventory().m_pTarget->object().ID());
		u_EventSend(P);
	}

	// ????? GetNearest ?????
	feel_touch_update(Position(), /*inventory().GetTakeDist()*/m_fPickupInfoRadius);
	
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
	if (!g_b_COD_PickUpMode || eacFirstEye != cam_active || Level().CurrentViewEntity() != this) 
	{
		HUD().GetUI()->UIMainIngameWnd.SetPickUpItem(NULL);
		return;
	};


	CFrustum frustum;
	frustum.CreateFromMatrix(Device.mFullTransform,FRUSTUM_P_LRTB|FRUSTUM_P_FAR);

	BOOL Enabled = getEnabled();
	setEnabled(FALSE);
	//---------------------------------------------------------------------------
	xr_vector<ISpatial*>	ISpatialResult;
	g_SpatialSpace->q_frustum(ISpatialResult, 0, STYPE_COLLIDEABLE, frustum);
	//---------------------------------------------------------------------------
	setEnabled(Enabled);

	float maxlen = 1000.0f;
	CInventoryItem* pNearestItem = NULL;
	for (u32 o_it=0; o_it<ISpatialResult.size(); o_it++)
	{
		ISpatial*		spatial	= ISpatialResult[o_it];
		CInventoryItem*	pIItem	= smart_cast<CInventoryItem*> (spatial->dcast_CObject        ());
		if (0 == pIItem) continue;
		
		Fvector A, B, tmp; 
		pIItem->object().Center			(A);
		if (A.distance_to_sqr(Position())>4) continue;

		tmp.sub(A, cam_Active()->vPosition);
		B.mad(cam_Active()->vPosition, cam_Active()->vDirection, tmp.dotproduct(cam_Active()->vDirection));
		float len = B.distance_to_sqr(A);
		if (len > 1) continue;

		if (maxlen>len)
		{
			maxlen = len;
			pNearestItem = pIItem;
		};
	}
	HUD().GetUI()->UIMainIngameWnd.SetPickUpItem(pNearestItem);

	if (pNearestItem && m_bPickupMode)
	{
		//подбирание объекта
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
		draw_str = inventory_owner->CharacterInfo().Name();
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