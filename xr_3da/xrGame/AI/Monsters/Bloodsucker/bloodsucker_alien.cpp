#include "stdafx.h"
#include "bloodsucker_alien.h"
#include "bloodsucker.h"
#include "../../../level.h"
#include "../../../actor.h"
#include "../../../inventory.h"
#include "../../../HudItem.h"
#include "../../../../CustomHUD.h"
#include "../../../../effector.h"
#include "../../../../effectorPP.h"

#define EFFECTOR_ID_GEN(type) (type( u32(u64(this) & u32(-1)) ))


////////////////////////////////////////////////////////////////////////////////////
// CAlienEffectorPP
////////////////////////////////////////////////////////////////////////////////////
class CAlienEffectorPP : public CEffectorPP {
	typedef CEffectorPP inherited;

	SPPInfo		state;
	float		factor;
	float		target_factor;

public:
					CAlienEffectorPP	(const SPPInfo &ppi, EEffectorPPType type);
	virtual			~CAlienEffectorPP	();

	void	Update			(float new_factor) {factor = new_factor;}
	void	Destroy			();

private:
	virtual	BOOL	Process			(SPPInfo& pp);
};


CAlienEffectorPP::CAlienEffectorPP(const SPPInfo &ppi, EEffectorPPType type) :
CEffectorPP(type, flt_max, false)
{
	state			= ppi;
	factor			= 0.f;
	target_factor	= 1.f;
}

CAlienEffectorPP::~CAlienEffectorPP()
{
}

#define PERIOD_SPEED	0.3f

BOOL CAlienEffectorPP::Process(SPPInfo& pp)
{
	inherited::Process(pp);

	if (fsimilar(factor, target_factor)) {
		target_factor = (target_factor > 0.5f) ? .3f : .6f;
	}
	
	def_lerp(factor,target_factor, PERIOD_SPEED, Device.fTimeDelta);

	
	SPPInfo	def;

	pp.duality.h		= def.duality.h			+ (state.duality.h			- def.duality.h)		* factor; 			
	pp.duality.v		= def.duality.v			+ (state.duality.v			- def.duality.v)		* factor;
	pp.gray				= def.gray				+ (state.gray				- def.gray)				* factor;
	pp.blur				= def.blur				+ (state.blur				- def.blur)				* factor;
	pp.noise.intensity	= def.noise.intensity	+ (state.noise.intensity	- def.noise.intensity)	* factor;
	pp.noise.grain		= def.noise.grain		+ (state.noise.grain		- def.noise.grain)		* factor;
	pp.noise.fps		= def.noise.fps			+ (state.noise.fps			- def.noise.fps)		* factor;	
	VERIFY(!fis_zero(pp.noise.fps));

	pp.color_base.set(
		def.color_base.r	+ (state.color_base.r - def.color_base.r) * factor, 
		def.color_base.g	+ (state.color_base.g - def.color_base.g) * factor, 
		def.color_base.b	+ (state.color_base.b - def.color_base.b) * factor
		);

	pp.color_gray.set(
		def.color_gray.r	+ (state.color_gray.r - def.color_gray.r) * factor, 
		def.color_gray.g	+ (state.color_gray.g - def.color_gray.g) * factor, 
		def.color_gray.b	+ (state.color_gray.b - def.color_gray.b) * factor
		);

	pp.color_add.set(
		def.color_add.r	+ (state.color_add.r - def.color_add.r) * factor, 
		def.color_add.g	+ (state.color_add.g - def.color_add.g) * factor, 
		def.color_add.b	+ (state.color_add.b - def.color_add.b) * factor
		);

	return TRUE;
}

void CAlienEffectorPP::Destroy()
{
	fLifeTime			= 0.f;
	CAlienEffectorPP	*self = this;
	xr_delete			(self);
}



//////////////////////////////////////////////////////////////////////////
// Alien Camera Effector
//////////////////////////////////////////////////////////////////////////

class CAlienEffector : public CEffectorCam {
	typedef CEffectorCam inherited;	

	float	m_time_total;
	Fvector	dangle_target;
	Fvector dangle_current;

	CAI_Bloodsucker *monster;

	float		m_current_fov;
	Fmatrix		m_prev_eye_matrix;
	float		m_inertion;

public:
					CAlienEffector	(ECamEffectorType type, CAI_Bloodsucker *obj);
	virtual	BOOL	Process			(Fvector &p, Fvector &d, Fvector &n, float& fFov, float& fFar, float& fAspect);
};


#define DELTA_ANGLE_X		10 * PI / 180
#define DELTA_ANGLE_Y		10 * PI / 180
#define DELTA_ANGLE_Z		10 * PI / 180
#define ANGLE_SPEED			0.2f	

#define MIN_FOV				70.f
#define	MAX_FOV				175.f
#define FOV_SPEED			80.f
#define	MAX_CAMERA_DIST		3.5f


CAlienEffector::CAlienEffector(ECamEffectorType type, CAI_Bloodsucker *obj) :
	inherited(type, flt_max)
{
	dangle_target.set		(angle_normalize(Random.randFs(DELTA_ANGLE_X)),angle_normalize(Random.randFs(DELTA_ANGLE_Y)),angle_normalize(Random.randFs(DELTA_ANGLE_Z)));
	dangle_current.set		(0.f, 0.f, 0.f);

	monster					= obj;
	
	m_prev_eye_matrix.c		= get_head_position(monster);
	m_prev_eye_matrix.k		= monster->Direction();
	Fvector::generate_orthonormal_basis(m_prev_eye_matrix.k,m_prev_eye_matrix.j,m_prev_eye_matrix.i);
	m_inertion				= 1.f;
	m_current_fov			= MIN_FOV;
}

BOOL CAlienEffector::Process(Fvector &p, Fvector &d, Fvector &n, float& fFov, float& fFar, float& fAspect)
{
	// Инициализация
	Fmatrix	Mdef;
	Mdef.identity		();
	Mdef.j.set			(n);
	Mdef.k.set			(d);
	Mdef.i.crossproduct	(n,d);
	Mdef.c.set			(p);


	// set angle 
	if (angle_lerp(dangle_current.x, dangle_target.x, ANGLE_SPEED, Device.fTimeDelta)) {
		dangle_target.x = angle_normalize(Random.randFs(DELTA_ANGLE_X));
	}

	if (angle_lerp(dangle_current.y, dangle_target.y, ANGLE_SPEED, Device.fTimeDelta)) {
		dangle_target.y = angle_normalize(Random.randFs(DELTA_ANGLE_Y));
	}

	if (angle_lerp(dangle_current.z, dangle_target.z, ANGLE_SPEED, Device.fTimeDelta)) {
		dangle_target.z = angle_normalize(Random.randFs(DELTA_ANGLE_Z));
	}

	// update inertion
	Fmatrix cur_matrix;
	cur_matrix.k = monster->Direction();
	cur_matrix.c = get_head_position(monster);

	float	rel_dist = m_prev_eye_matrix.c.distance_to(cur_matrix.c) / MAX_CAMERA_DIST;
	clamp	(rel_dist, 0.f, 1.f);

	def_lerp(m_inertion, 1 - rel_dist, rel_dist, Device.fTimeDelta);

	// set pos and dir with inertion
	m_prev_eye_matrix.c.inertion(cur_matrix.c, m_inertion);
	m_prev_eye_matrix.k.inertion(cur_matrix.k, m_inertion);
	Fvector::generate_orthonormal_basis_normalized(m_prev_eye_matrix.k,m_prev_eye_matrix.j,m_prev_eye_matrix.i);	

	// apply position and direction
	Mdef = m_prev_eye_matrix;

	//set fov
	float	rel_speed = monster->m_fCurSpeed / 15.f;
	clamp	(rel_speed,0.f,1.f);

	float	m_target_fov = MIN_FOV + (MAX_FOV-MIN_FOV) * rel_speed;
	def_lerp(m_current_fov, m_target_fov, FOV_SPEED, Device.fTimeDelta);
	
	fFov = m_current_fov;
	//////////////////////////////////////////////////////////////////////////

	// Установить углы смещения
	Fmatrix		R;
	R.setHPB	(dangle_current.x,dangle_current.y,dangle_current.z);

	Fmatrix		mR;
	mR.mul		(Mdef,R);

	d.set		(mR.k);
	n.set		(mR.j);
	p.set		(mR.c);

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////


CBloodsuckerAlien::CBloodsuckerAlien()
{
	m_object	= 0;
}

CBloodsuckerAlien::~CBloodsuckerAlien()
{
}

void CBloodsuckerAlien::init_external(CAI_Bloodsucker *obj)
{
	m_object	= obj;
}

void CBloodsuckerAlien::reinit()
{
	m_active				= false;	
	
	m_crosshair_show		= false;
}

void CBloodsuckerAlien::activate()
{
	if (m_active) return;

	VERIFY	(Actor());
	m_object->CControlledActor::install		(Actor());
	if (!m_object->EnemyMan.get_enemy())	m_object->EnemyMan.add_enemy(Actor());

	Actor()->inventory().setSlotsBlocked			(true);

	// hide crosshair
	m_crosshair_show			= !!psHUD_Flags.is(HUD_CROSSHAIR_RT);
	if (m_crosshair_show)		psHUD_Flags.set(HUD_CROSSHAIR_RT,FALSE);

	// Start effector
	m_effector_pp				= xr_new<CAlienEffectorPP>	(m_object->pp_vampire_effector, EFFECTOR_ID_GEN(EEffectorPPType));
	Actor()->Cameras().AddPPEffector	(m_effector_pp);
	
	m_effector					= xr_new<CAlienEffector>	(EFFECTOR_ID_GEN(ECamEffectorType),m_object);
	Actor()->Cameras().AddCamEffector	(m_effector);

	// fix it
	m_object->CInvisibility::set_manual_switch	();
	if (m_object->CInvisibility::is_active()) {
		m_object->CInvisibility::manual_deactivate();
		m_object->CInvisibility::manual_activate	();
	} else m_object->CInvisibility::manual_activate	();

	m_active					= true;

}

void CBloodsuckerAlien::deactivate()
{
	if (!m_active) return;

	m_object->CControlledActor::release			();
	m_object->CInvisibility::set_manual_switch	(false);

	//Actor()->inventory().Items_SetCurrentEntityHud	(true);
	Actor()->inventory().setSlotsBlocked			(false);
	if (m_crosshair_show)							psHUD_Flags.set(HUD_CROSSHAIR_RT,TRUE);

	// Stop camera effector
	Actor()->Cameras().RemoveCamEffector	(EFFECTOR_ID_GEN(ECamEffectorType));
	m_effector						= 0;
	
	// Stop postprocess effector
	Actor()->Cameras().RemovePPEffector	(EFFECTOR_ID_GEN(EEffectorPPType));
	m_effector_pp->Destroy			();
	m_effector_pp					= 0;

	m_active						= false;

}
