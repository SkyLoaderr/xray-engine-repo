#ifndef PHYSICS_COMMON_H
#define PHYSICS_COMMON_H
#include "DisablingParams.h"
extern const dReal	default_l_limit;
extern const dReal	default_w_limit;
extern const dReal	default_k_l;
extern const dReal	default_k_w;
extern const dReal	default_l_scale;
extern const dReal	default_w_scale;

extern const dReal	fixed_step;
extern const u32	dis_frames;
extern const dReal	world_gravity;
extern const dReal  world_cfm;
extern const dReal  world_erp;
extern const dReal  world_spring;
extern const dReal  world_damping;
#define ERP(k_p,k_d)		((fixed_step*(k_p)) / (((fixed_step)*(k_p)) + (k_d)))
#define CFM(k_p,k_d)		(1.f / (((fixed_step)*(k_p)) + (k_d)))
#define SPRING(cfm,erp)		((erp)/(cfm)/fixed_step)
#define DAMPING(cfm,erp)	((1.f-(erp))/(cfm))
IC float Erp(float k_p,float k_d)		{return ((fixed_step*(k_p)) / (((fixed_step)*(k_p)) + (k_d)));}
IC float Cfm(float k_p,float k_d)		{return (1.f / (((fixed_step)*(k_p)) + (k_d)));}
IC float Spring(float cfm,float erp)	{return ((erp)/(cfm)/fixed_step);}
IC float Damping(float cfm,float erp)	{return ((1.f-(erp))/(cfm));}
IC void	 MulSprDmp(float &cfm,float	&erp,float mul_spring,float mul_damping)
{
	float factor=1.f/(mul_spring*erp+mul_damping*(1-erp));
	cfm*=factor;
	erp*=(factor*mul_spring);
}
typedef  void __stdcall ContactCallbackFun(CDB::TRI* T,dContactGeom* c);
typedef	void __stdcall  ObjectContactCallbackFun(bool& do_colide,dContact& c);
typedef void __stdcall  PushOutCallbackFun(bool& do_colide,dContact& c);

typedef void __stdcall  BoneCallbackFun(CBoneInstance* B);


void __stdcall PushOutCallback(bool& do_colide,dContact& c);
void __stdcall PushOutCallback1(bool& do_colide,dContact& c);
void __stdcall ContactShotMark(CDB::TRI* T,dContactGeom* c);





#endif  //PHYSICS_COMMON_H