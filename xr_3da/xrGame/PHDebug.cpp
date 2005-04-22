#include "stdafx.h"
#ifdef DEBUG
#include "physics.h"
#include "MathUtils.h"
#include "PHDebug.h"
#include "PHObject.h"
#include "ExtendedGeom.h"
#include "Level.h"
#include "Hudmanager.h"
Flags32		ph_dbg_draw_mask;
bool		draw_frame=0;

#ifdef DRAW_CONTACTS
CONTACT_VECTOR Contacts0;
CONTACT_VECTOR Contacts1;
#endif

PHOBJ_DBG_V	dbg_draw_objects0;
PHOBJ_DBG_V	dbg_draw_objects1;

PHABS_DBG_V	dbg_draw_abstruct0;
PHABS_DBG_V	dbg_draw_abstruct1;

PHABS_DBG_V	dbg_draw_cashed;
bool		b_cash_draw=false;
u32			cash_draw_remove_time=u32(-1);

struct SPHDBGDrawTri :public SPHDBGDrawAbsract
{
	Fvector v[3];
	u32		c;
	SPHDBGDrawTri(CDB::RESULT* T,u32 ac)
	{
		v[0].set(T->verts[0]);
		v[1].set(T->verts[1]);
		v[2].set(T->verts[2]);
		c=ac;
	}
	SPHDBGDrawTri(CDB::TRI* T,const Fvector*	V_array,u32 ac)
	{
		
		v[0].set(V_array[T->verts[0]]);
		v[1].set(V_array[T->verts[1]]);
		v[2].set(V_array[T->verts[2]]);
		c=ac;
	}
	virtual void render()
	{
		RCache.dbg_DrawLINE(Fidentity,v[0],v[1],c);
		RCache.dbg_DrawLINE(Fidentity,v[1],v[2],c);
		RCache.dbg_DrawLINE(Fidentity,v[2],v[0],c);
	}
};

static void clear_vector(PHABS_DBG_V& v)
{
	PHABS_DBG_I i,e;i=v.begin();e=v.end();
	for(;e!=i;++i)
	{
		xr_delete(*i);
	}
	v.clear();
}

void DBG_DrawTri(CDB::RESULT* T,u32 c)
{
	DBG_DrawPHAbstruct(xr_new<SPHDBGDrawTri>(T,c));
}
void DBG_DrawTri(CDB::TRI* T,const Fvector* V_verts,u32 c)
{
	DBG_DrawPHAbstruct(xr_new<SPHDBGDrawTri>(T,V_verts,c));
}


struct SPHDBGDrawLine : public SPHDBGDrawAbsract
{
	Fvector p[2];u32 c;
	SPHDBGDrawLine(const Fvector& p0,const Fvector& p1,u32 ca)
	{
		p[0].set(p0);p[1].set(p1);c=ca;
	}
	virtual void render()
	{
		RCache.dbg_DrawLINE(Fidentity,p[0],p[1],c);
	}
};

void DBG_DrawLine (const Fvector& p0,const Fvector& p1,u32 c)
{
	DBG_DrawPHAbstruct(xr_new<SPHDBGDrawLine>(p0,p1,c));
}
struct SPHDBGDrawAABB :public SPHDBGDrawAbsract
{
	Fvector p[2];u32 c;	
	SPHDBGDrawAABB(const Fvector& center,const Fvector& AABB,u32 ac)
	{
		p[0].set(center);p[1].set(AABB);
		c=ac;
	}
	virtual void render()
	{
		RCache.dbg_DrawAABB			(p[0],p[1].x,p[1].y,p[1].z,c);
	}
};

void DBG_DrawAABB(const Fvector& center,const Fvector& AABB,u32 c)
{
	DBG_DrawPHAbstruct(xr_new<SPHDBGDrawAABB>(center,AABB,c));
}

struct SPHDBGDrawOBB: public SPHDBGDrawAbsract
{
	Fmatrix m;Fvector h;u32 c;
	SPHDBGDrawOBB(const Fmatrix am,const Fvector ah, u32 ac)
	{
		m.set(am);h.set(ah);c=ac;
	}
	virtual void render()
	{
		RCache.dbg_DrawOBB(m,h,c);
	}
};
void DBG_DrawOBB(const Fmatrix& m,const Fvector h,u32 c)
{
	DBG_DrawPHAbstruct(xr_new<SPHDBGDrawOBB>(m,h,c));
};
struct SPHDBGDrawPoint :public SPHDBGDrawAbsract
{
	Fvector p;float size;u32 c;
	SPHDBGDrawPoint(const Fvector ap,float s,u32 ac)
	{
		p.set(ap),size=s;c=ac;
	}
	virtual void render()
	{
		//RCache.dbg_DrawAABB(p,size,size,size,c);
		Fmatrix m;m.identity();m.scale(size,size,size);m.c.set(p);
		RCache.dbg_DrawEllipse(m,c);
	}
};
void DBG_DrawPoint(const Fvector& p,float size,u32 c)
{
	DBG_DrawPHAbstruct(xr_new<SPHDBGDrawPoint>(p,size,c));
}

struct SPHDBGOutText : public SPHDBGDrawAbsract
{
string64 s;
	SPHDBGOutText(LPCSTR t)
	{
		strcpy(s,t);
	}
	virtual void render()
	{
		HUD().Font().pFontSmall->OutNext(s);
	}
};
void _cdecl DBG_OutText(LPCSTR s,...)
{
	string64 t;
	va_list   marker;
	va_start  (marker,s);
	vsprintf(t,s,marker);
	va_end    (marker);
	DBG_DrawPHAbstruct(xr_new<SPHDBGOutText>(t));
}

void DBG_OpenCashedDraw()
{
	b_cash_draw=true;
}
void DBG_ClosedCashedDraw(u32 remove_time)
{
	b_cash_draw				=false			;
	cash_draw_remove_time	=remove_time+Device.dwTimeGlobal;
}
void DBG_DrawPHAbstruct(SPHDBGDrawAbsract* a)
{

	if(b_cash_draw)
	{
		dbg_draw_cashed.push_back(a);
	}
	else if(draw_frame)
	{
		dbg_draw_abstruct0.push_back(a);
	}else
	{
		dbg_draw_abstruct1.push_back(a);
	}
}

void DBG_PHAbstruactStartFrame(bool dr_frame)
{
	PHABS_DBG_I i,e;
	if(dr_frame)
	{
		i=dbg_draw_abstruct0.begin();
		e=dbg_draw_abstruct0.end();
	}else
	{
		i=dbg_draw_abstruct1.begin();
		e=dbg_draw_abstruct1.end();
	}
	for(;e!=i;++i)
	{
		xr_delete(*i);
	}
	if(dr_frame)
	{
		dbg_draw_abstruct0.clear();
	}
	else
	{
		dbg_draw_abstruct1.clear();
	}
}
void DBG_PHAbstructRender()
{
	PHABS_DBG_I i,e;
	if(!draw_frame)
	{
		i=dbg_draw_abstruct0.begin();
		e=dbg_draw_abstruct0.end();
	}else
	{
		i=dbg_draw_abstruct1.begin();
		e=dbg_draw_abstruct1.end();
	}
	for(;e!=i;++i)
	{
		(*i)->render();
	}
	if(!b_cash_draw)
	{
		PHABS_DBG_I i,e;
		i=dbg_draw_cashed.begin();e=dbg_draw_cashed.end();
		for(;e!=i;++i)
		{
				(*i)->render();
		}
		if(cash_draw_remove_time<Device.dwTimeGlobal)
		{
			clear_vector(dbg_draw_cashed);
		}
	}
}

void DBG_PHAbstructClear()
{
	DBG_PHAbstruactStartFrame(true);
	DBG_PHAbstruactStartFrame(false);
	clear_vector(dbg_draw_cashed);
}

void DBG_DrawPHObject(CPHObject* obj)
{
	if(ph_dbg_draw_mask.test(phDbgDrawEnabledAABBS))
	{
		SPHObjDBGDraw obj_draw;
		obj_draw.AABB.set(obj->AABB);
		obj_draw.AABB_center.set(obj->spatial.center);
		if(draw_frame)
		{
			dbg_draw_objects0.push_back(obj_draw);
		}else
		{
			dbg_draw_objects1.push_back(obj_draw);
		}
	}
}
void DBG_DrawContact(dContact& c)
{
#ifdef DRAW_CONTACTS

	SPHContactDBGDraw dbc;
	if(dGeomGetBody(c.geom.g1))
	{
		dbc.geomClass =dGeomGetClass(retrieveGeom(c.geom.g1));
	}
	else
	{
		dbc.geomClass=dGeomGetClass(retrieveGeom(c.geom.g2));
	}
	dbc.norm.set(cast_fv(c.geom.normal));
	dbc.pos.set(cast_fv(c.geom.pos));
	dbc.depth=c.geom.depth;
	if(ph_dbg_draw_mask.test(phDbgDrawContacts))
	{
		if(draw_frame)Contacts0.push_back(dbc);
		else		  Contacts1.push_back(dbc);
	}
#endif
}
void DBG_DrawFrameStart()
{
	
	if(draw_frame)
	{
#ifdef DRAW_CONTACTS
		Contacts0.clear();
#endif
		dbg_draw_objects0.clear();
		dbg_draw_abstruct0.clear();
	}
	else
	{
#ifdef DRAW_CONTACTS
		Contacts1.clear();
#endif
		dbg_draw_objects1.clear();
		dbg_draw_abstruct1.clear();
	}
	DBG_PHAbstruactStartFrame(draw_frame);

}


void PH_DBG_Clear()
{
	DBG_PHAbstructClear();
	dbg_draw_objects0.clear();
	dbg_draw_objects1.clear();
#ifdef DRAW_CONTACTS
	Contacts0.clear();
	Contacts1.clear();
#endif
}

void PH_DBG_Render()
{
	HUD().Font().pFontSmall->OutSet	(550,250);

	if(ph_dbg_draw_mask.test(phDbgDrawEnabledAABBS))
	{
		PHOBJ_DBG_I i,e;
		if(!draw_frame)
		{
			i=dbg_draw_objects0.begin();
			e=dbg_draw_objects0.end();
		}else
		{
			i=dbg_draw_objects1.begin();
			e=dbg_draw_objects1.end();
		}
		for(;e!=i;++i)
		{
			SPHObjDBGDraw& ds=*i;
			RCache.dbg_DrawAABB(ds.AABB_center,ds.AABB.x,ds.AABB.y,ds.AABB.z,D3DCOLOR_XRGB(255,0,0));
		}
	}

	DBG_PHAbstructRender();

#ifdef DRAW_CONTACTS

	if(ph_dbg_draw_mask.test(phDbgDrawContacts))
	{
	
		CONTACT_I i,e;
		if(!draw_frame)
		{
			i=Contacts0.begin();
			e=Contacts0.end();
		}
		else
		{
			i=Contacts1.begin();
			e=Contacts1.end();
		}

		for(;i!=e;i++)
		{
			SPHContactDBGDraw &c=*i;
			bool is_cyl=c.geomClass==dCylinderClassUser;
			RCache.dbg_DrawAABB			(c.pos,.01f,.01f,.01f,D3DCOLOR_XRGB(255*is_cyl,0,255*!is_cyl));
			Fvector dir;
			dir.set(c.norm);
			dir.mul(c.depth*100.f);
			dir.add(c.pos);
			RCache.dbg_DrawLINE(Fidentity,c.pos,dir,D3DCOLOR_XRGB(255*is_cyl,0,255*!is_cyl));
		}
	}
	// HUD().Font().pFontSmall->OutNext("---------------------");
#endif
	draw_frame=!draw_frame;
}
#endif