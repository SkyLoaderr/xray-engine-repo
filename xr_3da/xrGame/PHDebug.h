#ifndef PH_DEBUG_H
#define PH_DEBUG_H
#ifdef DEBUG
#define DRAW_CONTACTS

extern Flags32 ph_dbg_draw_mask;
extern	bool		  draw_frame;
#ifdef DRAW_CONTACTS
#include "Physics.h"
struct dContact;
DEFINE_VECTOR(dContact,CONTACT_VECTOR,CONTACT_I);
extern CONTACT_VECTOR Contacts0;
extern CONTACT_VECTOR Contacts1;
#endif

enum
{
	phDbgDrawContacts		=		1<<0,
	phDbgDrawEnabledAABBS	=		1<<1
};
struct SPHObjDBGDraw
{
	Fvector AABB;
	Fvector AABB_center;
};

DEFINE_VECTOR(SPHObjDBGDraw,PHOBJ_DBG_V,PHOBJ_DBG_I);
extern PHOBJ_DBG_V	dbg_draw_objects0;
extern PHOBJ_DBG_V	dbg_draw_objects1;
class CPHObject;
void DBG_DrawPHObject(CPHObject* obj);
void DBG_DrawContact (dContact& c);
void DBG_DrawFrameStart();
void PH_DBG_Render();

#endif
#endif