#pragma once
#include "physicsshellholder.h"
class CPHLeaderGeomShell;
class CPHCharacter;
class CClimableObject: public CPhysicsShellHolder 
#ifdef DEBUG
,public pureRender
#endif
{
	typedef	CPhysicsShellHolder		inherited;
	CPHLeaderGeomShell* m_pStaticShell;
	Fobb				m_box;
	Fvector				m_axis;
	Fvector				m_side;
	Fvector				m_norm;
	float				m_radius;
public:
					CClimableObject		();
					~CClimableObject	();
	virtual void	Load				( LPCSTR section);
	virtual BOOL	net_Spawn			( LPVOID DC);
	virtual	void	net_Destroy			();
	virtual void	shedule_Update		( u32 dt);							// Called by sheduler
	virtual void	UpdateCL			( );								// Called each frame, so no need for dt
	virtual void	Center				(Fvector &C) const;
	virtual float	Radius				() const;
#ifdef DEBUG
	virtual void	OnRender			();
#endif

	const Fvector&	Axis				()const		{return m_axis;}
	float			DDAxis				(Fvector& dir)const;

	const Fvector&	Side				()const {return m_side;}
	float			DDSide				(Fvector& dir)const;

	const Fvector&	Norm				()const{return m_norm;} 
	float			DDNorm				(Fvector &dir)const;
	bool			BeforeLadder		(CPHCharacter *actor)const;
	float			DDLowerP			(CPHCharacter	*actor,Fvector &out_dir)const;//returns distance and dir to lover point
	float			DDUpperP			(CPHCharacter	*actor,Fvector &out_dir)const;//returns distance and dir to upper point

	void			DToAxis				(CPHCharacter	*actor,Fvector &dir)const;
	float			DDToAxis			(CPHCharacter	*actor,Fvector &out_dir)const;//returns distance and dir to ladder axis
	void			POnAxis				(CPHCharacter	*actor,Fvector	&P)const;
	
	float			AxDistToUpperP		(CPHCharacter	*actor)const;
	float			AxDistToLowerP		(CPHCharacter	*actor)const;

	void			DSideToAxis			(CPHCharacter	*actor,Fvector	&dir)const;
	float			DDSideToAxis		(CPHCharacter	*actor,Fvector	&dir)const;
	
	void			DToPlain			(CPHCharacter	*actor,Fvector	&dist)const;
	float			DDToPlain			(CPHCharacter	*actor,Fvector	&dir)const;
	bool			InRange				(CPHCharacter	*actor)const;
	bool			InTouch				(CPHCharacter	*actor)const;

	void			LowerPoint			(Fvector	&P)const;
	void			UpperPoint			(Fvector	&P)const;
	void			DefineClimbState	(CPHCharacter	*actor)const;
};
