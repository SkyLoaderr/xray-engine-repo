#include "stdafx.h"
#include "climableobject.h "
#include "PHStaticGeomShell.h"
#include "xrServer_Objects_ALife.h"
#include "PHCharacter.h"
#include "MathUtils.h"

#define  MAX_OF(x,on_x,y,on_y,z,on_z)\
				if(x>y){\
					if	(x>z)	{on_x;}\
					else		{on_z;}\
				}\
				else\
				{\
					if	(y>z)	{on_y;}\
					else		{on_z;}\
				}



class CPHLeaderGeomShell: public CPHStaticGeomShell
{
CClimableObject		*m_pClimable;
public:
						CPHLeaderGeomShell		(CClimableObject* climable);
void					near_callback			(CPHObject* obj);
};

CPHLeaderGeomShell::CPHLeaderGeomShell(CClimableObject* climable)
{
	m_pClimable=climable;
}
void CPHLeaderGeomShell::near_callback	(CPHObject* obj)
{
	if(obj && obj->CastType()==CPHObject::tpCharacter)
	{
		CPHCharacter* ch=static_cast<CPHCharacter*>(obj);
		ch->SetElevator(m_pClimable);
	}
}


	CClimableObject::CClimableObject		()
{
	m_pStaticShell=NULL;
}
	CClimableObject::~CClimableObject	()
{

}
void CClimableObject::	Load				( LPCSTR section)
{
	inherited::Load(section);
}
BOOL CClimableObject::	net_Spawn			( LPVOID DC)
{
	CSE_Abstract				*e = (CSE_Abstract*)(DC);
	CSE_ALifeObjectClimable	*CLB=smart_cast<CSE_ALifeObjectClimable*>(e);
	Fmatrix& b=CLB->shapes[0].data.box;
	m_box.m_halfsize.set(b._11,b._22,b._33);
	//m_box.m_halfsize.set(1.f,1.f,1.f);
	m_radius=_max(_max(m_box.m_halfsize.x,m_box.m_halfsize.y),m_box.m_halfsize.z);
	BOOL ret	= inherited::net_Spawn(DC);
	m_box.xform_set(Fidentity);
	m_pStaticShell=xr_new<CPHLeaderGeomShell>(this);
	P_BuildStaticGeomShell(smart_cast<CPHStaticGeomShell*>(m_pStaticShell),smart_cast<CGameObject*>(this),0,m_box);
	m_pStaticShell->SetMaterial("materials\\fake_ladders");
	
	MAX_OF(	_abs(XFORM().i.y),m_axis.set(XFORM().i);m_axis.mul(m_box.m_halfsize.x),
			_abs(XFORM().j.y),m_axis.set(XFORM().j);m_axis.mul(m_box.m_halfsize.y),
			_abs(XFORM().k.y),m_axis.set(XFORM().k);m_axis.mul(m_box.m_halfsize.z)
			);
	if(m_axis.y<0.f) m_axis.invert();
	return ret;
}
void CClimableObject::	net_Destroy			()
{
	inherited::net_Destroy();
	m_pStaticShell->Deactivate();
	xr_delete(m_pStaticShell);
}
void CClimableObject::	shedule_Update		( u32 dt)							// Called by shedule
{
	inherited::shedule_Update(dt);
}
void CClimableObject::	UpdateCL			( )								// Called each frame, so no need for d
{
	inherited::UpdateCL();
}

void	CClimableObject::Center				(Fvector &C) const
{
		C.set(XFORM().c);
}
float	CClimableObject::Radius				() const
{
		return							m_radius;
}

float		CClimableObject::	DDLoverP		(CPHCharacter	*actor,Fvector	&out_dir)
{
	Fvector pos;
	LoverPoint(out_dir);
	actor->GetPosition(pos);
	out_dir.sub(pos);
	return to_mag_and_dir(out_dir);
}
float		CClimableObject::	DDApperP		(CPHCharacter	*actor,Fvector	&out_dir)
{
	Fvector pos;
	UpperPoint(out_dir);
	actor->GetPosition(pos);
	out_dir.sub(pos);
	return to_mag_and_dir(out_dir);
}


void		CClimableObject::	DefineClimbState	(CPHCharacter	*actor)
{


}

float		CClimableObject::	DDToAxis			(CPHCharacter	*actor,Fvector &out_dir)
{
	POnAxis(actor,out_dir);
	Fvector pos;actor->GetPosition(pos);
	out_dir.sub(pos);
	return to_mag_and_dir(out_dir);
}

void	CClimableObject::	POnAxis	(CPHCharacter	*actor,Fvector	&P)
{
	actor->GetPosition(P);
	prg_pos_on_axis(Position(),m_axis,P);
}
void		CClimableObject::	LoverPoint			(Fvector &P)
{
	P.sub(XFORM().c,m_axis);
}

void		CClimableObject::	UpperPoint			(Fvector &P)
{
	P.add(XFORM().c,m_axis);
}
#ifdef DEBUG
void CClimableObject ::OnRender()
{
	Fmatrix form;m_box.xform_get(form);
	//form.mulA(XFORM());
	RCache.dbg_DrawOBB(XFORM(),m_box.m_halfsize,D3DCOLOR_XRGB(0,0,255));
	Fvector p1,p2,d;
	d.set(m_axis);
	p1.add(XFORM().c,d);
	p2.sub(XFORM().c,d);
	RCache.dbg_DrawLINE(Fidentity,p1,p2,D3DCOLOR_XRGB(255,0,0));
}
#endif