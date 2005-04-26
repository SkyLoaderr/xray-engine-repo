#ifndef PH_DEBUG_H
#define PH_DEBUG_H
#ifdef DEBUG
#define DRAW_CONTACTS

extern Flags32 ph_dbg_draw_mask;
extern	bool		  draw_frame;
#ifdef DRAW_CONTACTS

struct SPHContactDBGDraw
{
	int geomClass;
	Fvector norm;
	Fvector pos;
	float depth;
};
DEFINE_VECTOR(SPHContactDBGDraw,CONTACT_VECTOR,CONTACT_I);
extern CONTACT_VECTOR Contacts0;
extern CONTACT_VECTOR Contacts1;
#endif

enum
{
	phDbgDrawContacts			=		1<<0,
	phDbgDrawEnabledAABBS		=		1<<1,
	phDBgDrawIntersectedTries	=		1<<2,
	phDbgDrawSavedTries			=		1<<3,
	phDbgDrawTriTrace			=		1<<4,
	phDBgDrawNegativeTries		=		1<<5,
	phDBgDrawPositiveTries		=		1<<6,
	phDbgDrawTriTestAABB		=		1<<7,
	phDBgDrawTriesChangesSign	=		1<<8,
	phDbgDrawTriPoint			=		1<<9,
	phDbgDrawExplosionPos		=		1<<10,
	phDbgDrawObjectStatistics	=		1<<11,
	phDbgDrawMassCenters		=		1<<12,
	phDbgDrawDeathActivationBox =		1<<14,
	phHitApplicationPoints		=		1<<15
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


struct SPHDBGDrawAbsract
{
	virtual void				render				()						=0;
	virtual						~SPHDBGDrawAbsract	()						{};
};
DEFINE_VECTOR(SPHDBGDrawAbsract*,PHABS_DBG_V,PHABS_DBG_I)					;
extern PHABS_DBG_V	dbg_draw_abstruct0;
extern PHABS_DBG_V	dbg_draw_abstruct1;

void DBG_OpenCashedDraw();
void DBG_ClosedCashedDraw(u32 remove_time);
void DBG_DrawPHAbstruct(SPHDBGDrawAbsract* a);
void DBG_DrawPHObject(CPHObject* obj);
void DBG_DrawContact (dContact& c);
void DBG_DrawTri(CDB::RESULT* T,u32 c);
void DBG_DrawTri(CDB::TRI* T,const Fvector* V_verts,u32 c);
void DBG_DrawLine(const Fvector& p0,const Fvector& p1,u32 c);
void DBG_DrawAABB(const Fvector& center,const Fvector& AABB,u32 c);
void DBG_DrawOBB(const Fmatrix& m,const Fvector h,u32 c);
void DBG_DrawPoint(const Fvector& p,float size,u32 c);
void _cdecl DBG_OutText(LPCSTR s,...);
void DBG_DrawFrameStart();
void PH_DBG_Render();
void PH_DBG_Clear();
#endif
#endif