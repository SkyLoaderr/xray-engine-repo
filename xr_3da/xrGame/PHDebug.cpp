#include "stdafx.h"
#ifdef DEBUG
#include "PHDebug.h"
#include "PHObject.h"
#include "ExtendedGeom.h"
Flags32		ph_dbg_draw_mask;
bool		draw_frame=0;

#ifdef DRAW_CONTACTS
CONTACT_VECTOR Contacts0;
CONTACT_VECTOR Contacts1;
#endif

PHOBJ_DBG_V	dbg_draw_objects0;
PHOBJ_DBG_V	dbg_draw_objects1;
void DBG_DrawPHObject(CPHObject* obj)
{
	if(ph_dbg_draw_mask.test(phDbgDrawEnabledAABBS))
	{
		SPHObjDBGDraw obj_draw;
		obj_draw.AABB.set(obj->AABB);
		obj_draw.AABB_center.set(obj->spatial.center);
		if(draw_frame)
		{
			dbg_draw_objects1.push_back(obj_draw);
		}else
		{
			dbg_draw_objects0.push_back(obj_draw);
		}
	}
}
void DBG_DrawContact(dContact& c)
{
#ifdef DRAW_CONTACTS

	if(ph_dbg_draw_mask.test(phDbgDrawContacts))
	{
		if(draw_frame)Contacts0.push_back(c);
		else		  Contacts1.push_back(c);
	}
#endif
}
void DBG_DrawFrameStart()
{
draw_frame=!draw_frame;
	if(draw_frame)
	{
		Contacts0.clear();
		dbg_draw_objects0.clear();
	}
	else
	{
		Contacts1.clear();
		dbg_draw_objects1.clear();
	}
}

void PH_DBG_Render()
{
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


			dContact& c=*i;
			int geom_class;
			if(dGeomGetBody(c.geom.g1))
			{
				geom_class=dGeomGetClass(retrieveGeom(c.geom.g1));
			}
			else
			{
				geom_class=dGeomGetClass(retrieveGeom(c.geom.g2));
			}
			bool is_cyl=geom_class==dCylinderClassUser;
			RCache.dbg_DrawAABB			(*((Fvector*)c.geom.pos),.01f,.01f,.01f,D3DCOLOR_XRGB(255*is_cyl,0,255*!is_cyl));
			Fvector dir;
			dir.set(c.geom.normal[0],c.geom.normal[1],c.geom.normal[2]);
			dir.mul(c.geom.depth*100.f);
			dir.add(*((Fvector*)c.geom.pos));
			RCache.dbg_DrawLINE(Fidentity,*((Fvector*)c.geom.pos),dir,D3DCOLOR_XRGB(255*is_cyl,0,255*!is_cyl));
		}
	}
#endif
}
#endif