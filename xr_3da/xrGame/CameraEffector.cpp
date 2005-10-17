// CameraEffector.cpp: implementation of the CCameraEffector class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CameraEffector.h"

#include "script_engine.h"
#include "script_space.h"
#include "luabind/wrapper_base.hpp"
#include "../objectAnimator.h"


class CCameraEffector_script :public CCameraEffector//, public luabind::wrap_base
{
public:
	CCameraEffector_script							(ECameraEffectorType type, float tm, bool affected):CCameraEffector(type, tm, affected) {};
	virtual BOOL Process(Fvector &p, Fvector &d, Fvector &n, float &fFov, float &fFar, float &fAspect)									
	{																				
		return Process_(&p, &d, &n/*, &fFov, &fFar, &fAspect*/)	;	
	}                                   											
	virtual bool Process_(Fvector *p, Fvector *d, Fvector *n/*, float *fFov, float *fFar, float *fAspect*/)	{return false;};

};

class CCamEffectorWrapper :public CCameraEffector_script ,public luabind::wrap_base
{
public:
	CCamEffectorWrapper		(ECameraEffectorType type, float tm, bool affected):CCameraEffector_script(type, tm, affected) {};
	virtual bool Process_(Fvector *p, Fvector *d, Fvector *n)
	{																				
		return call<bool>("Process",p,d,n);										
	}                                   											

};

using namespace luabind;
void play_object_animator(CObjectAnimator* self, bool bLooped)
{
	self->Play	(bLooped);
}

void object_animator_xform(CObjectAnimator* self, Fmatrix* dst)
{
	*dst = self->XFORM();
/*	Msg("i %f %f %f",VPUSH(dst->i));
	Msg("j %f %f %f",VPUSH(dst->j));
	Msg("k %f %f %f",VPUSH(dst->k));
	Msg("c %f %f %f",VPUSH(dst->c));
*/
}

void CCameraEffector::script_register(lua_State *L)
{
	typedef CCameraEffector_script BaseType;
	module(L)
	[
		class_<enum_exporter<ECameraEffectorType> >("cam_effector_type")
			.enum_("events")
			[
				value("eCEFall",			int(eCEFall)),
				value("eCENoise",			int(eCENoise)),
				value("eCEShot",			int(eCEShot)),
				value("eCEZoom",			int(eCEZoom)),
				value("eCERecoil",			int(eCERecoil)),
				value("eCEBobbing",			int(eCEBobbing)),
				value("eCEHit",				int(eCEHit)),
				value("eCEUser",			int(eCEUser))
			],
		class_<CCameraEffector> ("cam_effector_base")
			.def_readwrite("fLifeTime",	&CCameraEffector::fLifeTime)
			.def("GetType",					&CCameraEffector::GetType),

		class_<BaseType, CCamEffectorWrapper, CCameraEffector>("cam_effector")
			.def(	constructor<ECameraEffectorType ,float ,bool>()),

		class_<CObjectAnimator>("object_animator")
		.def(	constructor<>				())
		.def("xform",						&object_animator_xform)
		.def("Update",						&CObjectAnimator::Update)
		.def("Load",						&CObjectAnimator::Load)
		.def("Play",						&play_object_animator)
		.def("GetLengthSec",				&CObjectAnimator::GetLength)
		
	];
};
