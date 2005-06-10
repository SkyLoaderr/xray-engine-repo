#include "stdafx.h"
#include "WeaponBinocularsVision.h"
#include "WeaponBinoculars.h"
#include "ui\UIFrameWindow.h"
#include "ui.h"
#include "entity_alive.h"

#define RECT_SIZE	16

struct FindVisObjByObject{
	CObject*			O;
	FindVisObjByObject(CObject* o):O(o){}
	bool operator () (const SBinocVisibleObj& vis){
		return (O==vis.m_object);
	}
};

void SBinocVisibleObj::create_default(u32 color)
{
	m_lt.Init			("ui\\ui_enemy_frame",0,0,RECT_SIZE,RECT_SIZE);
	m_lb.Init			("ui\\ui_enemy_frame",0,0,RECT_SIZE,RECT_SIZE);
	m_rt.Init			("ui\\ui_enemy_frame",0,0,RECT_SIZE,RECT_SIZE);
	m_rb.Init			("ui\\ui_enemy_frame",0,0,RECT_SIZE,RECT_SIZE);

	m_lt.SetOriginalRect(0,			0,			RECT_SIZE,	RECT_SIZE);
	m_lb.SetOriginalRect(0,			RECT_SIZE,	RECT_SIZE,	RECT_SIZE);
	m_rt.SetOriginalRect(RECT_SIZE,	0,			RECT_SIZE,	RECT_SIZE);
	m_rb.SetOriginalRect(RECT_SIZE,	RECT_SIZE,	RECT_SIZE,	RECT_SIZE);

/*	m_lt.SetAlignment	(waCenter);
	m_lb.SetAlignment	(waCenter);
	m_rt.SetAlignment	(waCenter);
	m_rb.SetAlignment	(waCenter);*/

	u32 clr			= subst_alpha(color,128);
	m_lt.SetColor	(clr);
	m_lb.SetColor	(clr);
	m_rt.SetColor	(clr);
	m_rb.SetColor	(clr);

	cur_rect.set	(0,0, UI_BASE_WIDTH,UI_BASE_HEIGHT);

	m_flags.zero	();
}

void SBinocVisibleObj::Draw()
{
	if(m_flags.test(flVisObjNotValid)) return;

	m_lt.Draw			();
	m_lb.Draw			();
	m_rt.Draw			();
	m_rb.Draw			();
/*
		Fbox		b = m_object->Visual()->vis.box;
		b.xform		(m_object->XFORM());
		Fvector		cd,hd;
		b.get_CD	(cd,hd);
		Fmatrix M;
		M			= Fidentity;
		M.translate_add(cd);
		RCache.dbg_DrawOBB(M,hd,0xFFFFFFFF);
*/
}

void SBinocVisibleObj::Update()
{
	m_flags.set		(flVisObjNotValid,TRUE);

	Fbox		b = m_object->Visual()->vis.box;

	Fmatrix		xform;
	xform.mul	(Device.mFullTransform,m_object->XFORM());
	Fvector2	mn={flt_max,flt_max},mx={flt_min,flt_min};
	for (u32 k=0; k<8; ++k){
		Fvector p;
		b.getpoint		(k,p);
		xform.transform	(p);
		mn.x	= _min(mn.x,p.x);
		mn.y	= _min(mn.y,p.y);
		mx.x	= _max(mx.x,p.x);
		mx.y	= _max(mx.y,p.y);
	}
	static Frect screen_rect={-1.0f, -1.0f, 1.0f, 1.0f};

	Frect new_rect;
	new_rect.lt = mn;
	new_rect.rb = mx;

	if( FALSE == screen_rect.intersected(new_rect) ) return;
	if( new_rect.in(screen_rect.lt) && new_rect.in(screen_rect.rb) ) return;
	
	std::swap	(mn.y,mx.y);
	mn.x		= (1.f + mn.x)/2.f * (Device.dwWidth);
	mx.x		= (1.f + mx.x)/2.f * (Device.dwWidth);
	mn.y		= (1.f - mn.y)/2.f * (Device.dwHeight);
	mx.y		= (1.f - mx.y)/2.f * (Device.dwHeight);

	if (m_flags.is(flTargetLocked)){
		cur_rect.lt.set	(mn);
		cur_rect.rb.set	(mx);
	}else{
		cur_rect.lt.x	+= (mn.x-cur_rect.lt.x)*m_upd_speed*Device.fTimeDelta;
		cur_rect.lt.y	+= (mn.y-cur_rect.lt.y)*m_upd_speed*Device.fTimeDelta;
		cur_rect.rb.x	+= (mx.x-cur_rect.rb.x)*m_upd_speed*Device.fTimeDelta;
		cur_rect.rb.y	+= (mx.y-cur_rect.rb.y)*m_upd_speed*Device.fTimeDelta;
		if (mn.similar(cur_rect.lt,2.f)&&mx.similar(cur_rect.rb,2.f)){ 
			// target locked
			m_flags.set(flTargetLocked,TRUE);
			u32 clr		= subst_alpha(m_lt.GetColor(),255);
			m_lt.SetColor	(clr);
			m_lb.SetColor	(clr);
			m_rt.SetColor	(clr);
			m_rb.SetColor	(clr);
		}
	}

	m_lt.SetWndPos		( (cur_rect.lt.x)+2,	(cur_rect.lt.y)+2 );
	m_lb.SetWndPos		( (cur_rect.lt.x)+2,	(cur_rect.rb.y)-14 );
	m_rt.SetWndPos		( (cur_rect.rb.x)-14,	(cur_rect.lt.y)+2 );
	m_rb.SetWndPos		( (cur_rect.rb.x)-14,	(cur_rect.rb.y)-14 );

	m_flags.set		(flVisObjNotValid, FALSE);
}


CBinocularsVision::CBinocularsVision(CWeaponBinoculars* parent)
{
	m_parent = parent;
	Load									(m_parent->cNameSect());
}
CBinocularsVision::~CBinocularsVision()
{
}

void CBinocularsVision::Update()
{
	CFrustum F;
	F.CreateFromMatrix						(Device.mFullTransform,FRUSTUM_P_LRTB|FRUSTUM_P_FAR);
	xr_vector<ISpatial*>					R;
	g_SpatialSpace->q_frustum				(R,0,STYPE_VISIBLEFORAI,F); //

	VIS_OBJECTS_IT	it = m_active_objects.begin();
	for(;it!=m_active_objects.end();++it)
		(*it).m_flags.set					(flVisObjNotValid, TRUE) ;

	for (u32 o_it=0; o_it<R.size(); o_it++)
	{
		CObject*	object_					= smart_cast<CObject*>(R[o_it]);
		
		if(!object_ || !object_->Visual())				continue;
		if(!(object_->spatial.type&STYPE_RENDERABLE))	continue;

		CEntityAlive*	EA = smart_cast<CEntityAlive*>(object_);
		if(!EA || !EA->g_Alive())						continue;
		

		FindVisObjByObject	f				(object_);
		VIS_OBJECTS_IT found;
		found = std::find_if				(m_active_objects.begin(),m_active_objects.end(),f);

		if( found != m_active_objects.end() ){
			(*found).m_flags.set			(flVisObjNotValid,FALSE);
		}else{
			m_active_objects.push_back		(SBinocVisibleObj() );
			SBinocVisibleObj& new_vis_obj	= m_active_objects.back();
			new_vis_obj.m_flags.set			(flVisObjNotValid,FALSE);
			new_vis_obj.m_object			= object_;
			new_vis_obj.create_default		(m_frame_color.get());
			new_vis_obj.m_upd_speed			= m_rotating_speed;
		}
	}
	std::sort								(m_active_objects.begin(), m_active_objects.end());

	while(m_active_objects.size() && m_active_objects.back().m_flags.test(flVisObjNotValid)){
		m_active_objects.pop_back			();
	}

	it = m_active_objects.begin();
	for(;it!=m_active_objects.end();++it)
		(*it).Update						();

}

void CBinocularsVision::Draw()
{
	VIS_OBJECTS_IT	it = m_active_objects.begin();
	for(;it!=m_active_objects.end();++it)
		(*it).Draw							();
}

void CBinocularsVision::Load(const shared_str& section)
{
	m_rotating_speed	= pSettings->r_float(section,"vis_frame_speed");
	m_frame_color		= pSettings->r_fcolor(section,"vis_frame_color");
}