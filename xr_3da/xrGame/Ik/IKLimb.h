#pragma once


#include	"limb.h"

class	CKinematics	;
class	CDB::TRI	;

#ifdef DEBUG
struct SCalculateData;
#endif
class CIKLimb
{
public:
		enum	Etype{
			tpRight,
			tpLeft
		};
					CIKLimb				()															;
		void		Create				(CKinematics* K,u16 bones[3],const Fvector& toe_pos,Etype tp);	
		void		Destroy				()															;
		void		Calculate			(CKinematics* K,const Fmatrix &obj)							;

private:
#ifdef DEBUG
		SCalculateData *dbg_calculate_data;
#endif
		Limb		m_limb																		;
		CDB::TRI	*m_tri																		;
		float		m_tri_hight																	;
		Fvector		m_toe_position																;
		u16			m_bones				[4]														;	
		void		Invalidate			()														;
		void		GoalMatrix			(Matrix		&M,CKinematics *K,const Fmatrix &obj)		;
		void		Collide				(CKinematics* K,const Fmatrix &obj)						;
		void		CalculateBones		(CKinematics* K,const float x[])						;
static	void 		BonesCallback0		(CBoneInstance* B)										;
static	void 		BonesCallback1		(CBoneInstance* B)										;
static	void 		BonesCallback2		(CBoneInstance* B)										;
protected:
private:
};