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
	float			DDLoverP			(CPHCharacter	*actor,Fvector &out_dir);//returns distance and dir to lover point
	float			DDApperP			(CPHCharacter	*actor,Fvector &out_dir);//returns distance and dir to upper point
	float			DDToAxis			(CPHCharacter	*actor,Fvector &out_dir);//returns distance and dir to ladder axis
	void			POnAxis				(CPHCharacter	*actor,Fvector	&P);

	void			LoverPoint			(Fvector	&P);
	void			UpperPoint			(Fvector	&P);
	void			DefineClimbState	(CPHCharacter	*actor);
};
