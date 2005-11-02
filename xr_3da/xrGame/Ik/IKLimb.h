#pragma once


#include	"limb.h"

class	CKinematics	;
class	CDB::TRI	;
class CIKLimb
{
public:
	enum	Etype{
		tpRight,
		tpLeft
	};
			CIKLimb				()															;
	void	Create				(CKinematics* K,u16 bones[3],const Fvector& toe_pos,Etype tp);	
	void	Destroy				()															;
	void	Calculate			(CKinematics* K)											;

private:
	Limb		m_limb																		;
	CDB::TRI	*m_tri																		;
	float		m_tri_hight;
	Fvector		m_toe_position																;
	u16			m_bones				[4]														;	
	void		Invalidate			()														;
	void		GoalMatrix			(Matrix		&M,CKinematics *K)							;
	void		Collide				(CKinematics* K)										;
	void		CalculateBones		(CKinematics* K,const float x[])						;
protected:
private:
};