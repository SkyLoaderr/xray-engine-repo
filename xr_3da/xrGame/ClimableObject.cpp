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

#define  MIN_OF(x,on_x,y,on_y,z,on_z)\
	if(x<y){\
					if	(x<z)	{on_x;}\
					else		{on_z;}\
				}\
				else\
				{\
					if	(y<z)	{on_y;}\
					else		{on_z;}\
				}

#define  NON_MIN_OF(x,on_x1,on_x2,y,on_y1,on_y2,z,on_z1,on_z2)\
					if(x<y){\
						if	(x<z){if(y<z){on_z1;on_y2;}else{on_z2;on_y1;}}\
							else{on_x2;on_y1;}\
						}\
					else\
							{\
							if	(y<z)	{if(x>z){ on_x1;on_z2;}else{on_z1;on_x2;}}\
							else		{on_x1;on_y2;}\
							}

#define  SORT(x,on_x1,on_x2,on_x3,y,on_y1,on_y2,on_y3,z,on_z1,on_z2,on_z3)\
	if(x<y){\
	if	(x<z){if(y<z){on_z1;on_y2;on_x3;}else{on_z2;on_y1;on_x3;}}\
				else{on_x2;on_y1;on_z3;}\
	}\
	else\
		{\
			if	(y<z)	{if(x>z){ on_x1;on_z2;on_y3;}else{on_z1;on_x2;on_y3;}}\
			else		{on_x1;on_y2;on_z3;}\
		}


IC void OrientToNorm(const Fvector& normal,Fmatrix& form,Fobb& box)
{
	
	Fvector * ax_pointer= (Fvector*)&form;
	float   * s_pointer = (float*) &(box.m_halfsize);
	float max_dot=abs(ax_pointer[0].dotproduct(normal));
	float min_size=box.m_halfsize.x;
	int max_ax_i=0,min_size_i=0;
	for(int i=1;3>i;++i)
	{
		float dot_pr=abs(ax_pointer[i].dotproduct(normal));
		if(max_dot<dot_pr)
		{
			max_ax_i=i;max_dot=dot_pr;
		}
		if(min_size>s_pointer[i])
		{
			min_size_i=i;min_size=s_pointer[i];
		}
	}
	VERIFY(min_size_i==max_ax_i);
	if(ax_pointer[max_ax_i].dotproduct(normal)<0.f)
	{
		ax_pointer[max_ax_i].invert();
		ax_pointer[(max_ax_i+1)%3].invert();
	}
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
	if(m_box.m_halfsize.x<5.f*EPS_L)m_box.m_halfsize.x=5.f*EPS_L;
	if(m_box.m_halfsize.y<5.f*EPS_L)m_box.m_halfsize.y=5.f*EPS_L;
	if(m_box.m_halfsize.z<5.f*EPS_L)m_box.m_halfsize.z=5.f*EPS_L;
	m_radius=_max(_max(m_box.m_halfsize.x,m_box.m_halfsize.y),m_box.m_halfsize.z);
	BOOL ret	= inherited::net_Spawn(DC);
	m_box.xform_set(Fidentity);
	m_pStaticShell=xr_new<CPHLeaderGeomShell>(this);
	P_BuildStaticGeomShell(smart_cast<CPHStaticGeomShell*>(m_pStaticShell),smart_cast<CGameObject*>(this),0,m_box);
	m_pStaticShell->SetMaterial("materials\\fake_ladders");
	
	SORT(	m_box.m_halfsize.x,m_axis.set(XFORM().i);m_axis.mul(m_box.m_halfsize.x),m_side.set(XFORM().i);m_side.mul(m_box.m_halfsize.x),m_norm.set(XFORM().i);m_norm.mul(m_box.m_halfsize.x),
			m_box.m_halfsize.y,m_axis.set(XFORM().j);m_axis.mul(m_box.m_halfsize.y),m_side.set(XFORM().j);m_side.mul(m_box.m_halfsize.y),m_norm.set(XFORM().j);m_norm.mul(m_box.m_halfsize.y),
			m_box.m_halfsize.z,m_axis.set(XFORM().k);m_axis.mul(m_box.m_halfsize.z),m_side.set(XFORM().k);m_side.mul(m_box.m_halfsize.z),m_norm.set(XFORM().k);m_norm.mul(m_box.m_halfsize.z)
			);

	if(m_axis.y<0.f)
	{
		m_axis.invert();
		m_side.invert();

	}
	
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

float		CClimableObject::	DDLowerP		(CPHCharacter	*actor,Fvector	&out_dir)const
{
	Fvector pos;
	LowerPoint(out_dir);
	actor->GetFootCenter(pos);
	out_dir.sub(pos);
	return to_mag_and_dir(out_dir);
}
float		CClimableObject::	DDUpperP		(CPHCharacter	*actor,Fvector	&out_dir)const
{
	Fvector pos;
	UpperPoint(out_dir);
	actor->GetFootCenter(pos);
	out_dir.sub(pos);
	return to_mag_and_dir(out_dir);
}


void		CClimableObject::	DefineClimbState	(CPHCharacter	*actor)const
{


}
float		CClimableObject::DDAxis(Fvector& dir)const
{
	dir.set(m_axis);
	return to_mag_and_dir(dir);
}

float	CClimableObject::DDSide(Fvector& dir)const
{
	dir.set(m_side);
	return to_mag_and_dir(dir);
}
float	CClimableObject::DDNorm(Fvector &dir)const
{
	dir.set(m_norm);
	return to_mag_and_dir(dir);
}
float		CClimableObject::	DDToAxis			(CPHCharacter	*actor,Fvector &out_dir)const
{
	DToAxis(actor,out_dir);
	return to_mag_and_dir(out_dir);
}

void	CClimableObject::	POnAxis	(CPHCharacter	*actor,Fvector	&P)const
{
	actor->GetFootCenter(P);
	prg_pos_on_axis(Position(),m_axis,P);
}
void		CClimableObject::	LowerPoint			(Fvector &P)const
{
	P.sub(XFORM().c,m_axis);
}

void		CClimableObject::	UpperPoint			(Fvector &P)const
{
	P.add(XFORM().c,m_axis);
}

void		CClimableObject::DToAxis(CPHCharacter *actor,Fvector &dir)const
{
	POnAxis(actor,dir);
	Fvector pos;actor->GetFootCenter(pos);
	dir.sub(pos);
}
void CClimableObject::DSideToAxis		(CPHCharacter	*actor,Fvector	&dir)const
{
DToAxis(actor,dir);
Fvector side;side.set(m_side);
to_mag_and_dir(side);
side.mul(side.dotproduct(dir));
dir.set(side);
}

float CClimableObject::DDSideToAxis(CPHCharacter *actor,Fvector &dir)const
{
	DToAxis(actor,dir);
	Fvector side;side.set(m_side);to_mag_and_dir(side);
	float dot=side.dotproduct(dir);
	if(dot>0.f)
	{
		dir.set(side);
		return dot;
	}
	else
	{
		dir.set(side);
		dir.invert();
		return -dot;
	}
	
}

void CClimableObject::DToPlain(CPHCharacter *actor,Fvector &dist)const
{
 DToAxis(actor,dist);
 Fvector norm;norm.set(m_norm);
 to_mag_and_dir(norm);
 float dot=norm.dotproduct(dist);
 norm.mul(dot);
 dist.set(norm);
}

float CClimableObject::DDToPlain(CPHCharacter *actor,Fvector &dir)const
{
	DToPlain(actor,dir);
	return to_mag_and_dir(dir);
}

bool CClimableObject::InTouch(CPHCharacter *actor)const
{
	Fvector dir;
	const float normal_tolerance=0.005f;
	float foot_radius=actor->FootRadius();
	return (DDToPlain(actor,dir)<foot_radius+m_norm.magnitude()+normal_tolerance&&
			DDSideToAxis(actor,dir)<m_side.magnitude())&&InRange(actor);
	
}

float CClimableObject::AxDistToUpperP(CPHCharacter *actor)const
{
Fvector v1,v2;
actor->GetFootCenter(v1);
UpperPoint(v2);
v2.sub(v1);
v1.set(m_axis);to_mag_and_dir(v1);
return v1.dotproduct(v2);
}

float CClimableObject::AxDistToLowerP(CPHCharacter *actor)const
{
	Fvector v1,v2;
	actor->GetFootCenter(v1);
	LowerPoint(v2);
	v2.sub(v1);
	v1.set(m_axis);to_mag_and_dir(v1);
	return -v1.dotproduct(v2);
}
bool CClimableObject::InRange(CPHCharacter *actor)const
{
	return AxDistToLowerP(actor)>0.f && AxDistToUpperP(actor)+actor->FootRadius()>0.f;
}

bool CClimableObject::BeforeLadder(CPHCharacter *actor)const
{
	Fvector d;
	DToAxis(actor,d);
	Fvector n;n.set(Norm());
	to_mag_and_dir(n);
	return d.dotproduct(n)<-actor->FootRadius()/2.f;
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

	d.set(m_side);
	p1.add(XFORM().c,d);
	p2.sub(XFORM().c,d);
	RCache.dbg_DrawLINE(Fidentity,p1,p2,D3DCOLOR_XRGB(255,0,0));

	d.set(m_norm);
	d.mul(10.f);
	p1.add(XFORM().c,d);
	p2.set(XFORM().c);
	RCache.dbg_DrawLINE(Fidentity,p1,p2,D3DCOLOR_XRGB(0,255,0));
}
#endif