#include "StdAfx.h"
#include "PHDynamicData.h"
#include "Physics.h"
#include "tri-colliderknoopc/dTriList.h"
///////////////////////////////////////////////////////////////
///#pragma warning(disable:4995)
////#include "..\ode\src\collision_kernel.h"
//#include <..\ode\src\joint.h>
//#include <..\ode\src\objects.h>

//#pragma warning(default:4995)
///////////////////////////////////////////////////////////////////

#include "ExtendedGeom.h"

#include "PHElement.h"
#include "PHJoint.h"
#include "PHShell.h"
void CPHJoint::CreateBall()
{

	m_joint=dJointCreateBall(phWorld,0);
	Fvector pos;
	Fmatrix first_matrix,second_matrix;
	CPHElement* first=dynamic_cast<CPHElement*>(pFirst_element);
	CPHElement* second=dynamic_cast<CPHElement*>(pSecond_element);



	dBodyID body1=0;
	dBodyID body2=0;

	if(first)
	{
		first->InterpolateGlobalTransform(&first_matrix);
		body1=first->get_body();
	}
	else
	{
		first_matrix.identity();
	}

	if(second)
	{
		second->InterpolateGlobalTransform(&second_matrix);
		body2=second->get_body();
	}
	else
	{
		second_matrix.identity();

	}
pos.set(0,0,0);
	switch(vs_anchor){
case vs_first :first_matrix.transform_tiny(pos,anchor); break;
case vs_second:second_matrix.transform_tiny(pos,anchor); break;
case vs_global:pShell->mXFORM.transform_tiny(pos,anchor);break;				
default:NODEFAULT;	
	}

	if(!(body1&&body2)) 
	{

		m_joint=dJointCreateBall(phWorld,0);
		dJointAttach(m_joint,body1,body2);


		dJointSetBallAnchor(m_joint,pos.x,pos.y,pos.z);

		return;
	}

	dJointAttach(m_joint,first->get_body(),second->get_body());
	dJointSetBallAnchor(m_joint,pos.x,pos.y,pos.z);


}


void CPHJoint::CreateCarWeel()
{
}

void CPHJoint::CreateHinge()
{

	m_joint=dJointCreateHinge(phWorld,0);

	Fvector pos;
	Fmatrix first_matrix,second_matrix;
	Fvector axis;
	

	CPHElement* first=dynamic_cast<CPHElement*>(pFirst_element);
	CPHElement* second=dynamic_cast<CPHElement*>(pSecond_element);
	first->InterpolateGlobalTransform(&first_matrix);
	second->InterpolateGlobalTransform(&second_matrix);

pos.set(0,0,0);
switch(vs_anchor)
{
case vs_first :first_matrix.transform_tiny(pos,anchor); break;
case vs_second:second_matrix.transform_tiny(pos,anchor); break;
case vs_global:pShell->mXFORM.transform_tiny(pos,anchor);break;			
default:NODEFAULT;	
}


	axis.set(0,0,0);

	first_matrix.invert();

	Fmatrix rotate;
	rotate.mul(first_matrix,second_matrix);

	float hi,lo;
	CalcAxis(0,axis,lo,hi,first_matrix,second_matrix,rotate);

	dJointAttach(m_joint,first->get_body(),second->get_body());

	dJointSetHingeAnchor(m_joint,pos.x,pos.y,pos.z);
	dJointSetHingeAxis(m_joint,axis.x,axis.y,axis.z);

	dJointSetHingeParam(m_joint,dParamLoStop ,lo);
	dJointSetHingeParam(m_joint,dParamHiStop ,hi);
	if(axes[0].force>0.f){
		dJointSetHingeParam(m_joint,dParamFMax ,axes[0].force);
		dJointSetHingeParam(m_joint,dParamVel ,axes[0].velocity);
	}
	dJointSetHingeParam(m_joint,dParamStopERP ,axes[0].erp);
	dJointSetHingeParam(m_joint,dParamStopCFM ,axes[0].cfm);

	dJointSetHingeParam(m_joint,dParamCFM ,m_cfm);
}


void CPHJoint::CreateHinge2()
{

	m_joint=dJointCreateHinge2(phWorld,0);

	Fvector pos;
	Fmatrix first_matrix,second_matrix;
	Fvector axis;
	CPHElement* first=dynamic_cast<CPHElement*>(pFirst_element);
	CPHElement* second=dynamic_cast<CPHElement*>(pSecond_element);
	first->InterpolateGlobalTransform(&first_matrix);
	second->InterpolateGlobalTransform(&second_matrix);
	pos.set(0,0,0);
	switch(vs_anchor)
	{
	case vs_first :first_matrix.transform_tiny(pos,anchor); break;
	case vs_second:second_matrix.transform_tiny(pos,anchor); break;
	case vs_global:pShell->mXFORM.transform_tiny(pos,anchor);break;					
	default:NODEFAULT;	
	}
	//////////////////////////////////////

	
	dJointAttach(m_joint,first->get_body(),second->get_body());
	dJointSetHinge2Anchor(m_joint,pos.x,pos.y,pos.z);

	/////////////////////////////////////////////

	Fmatrix first_matrix_inv;
	first_matrix_inv.set(first_matrix);
	first_matrix_inv.invert();
	Fmatrix rotate;

	rotate.mul(first_matrix_inv,second_matrix);
	/////////////////////////////////////////////

	float lo;
	float hi;
	//////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////
	axis.set(0,0,0);
	CalcAxis(0,axis,lo,hi,first_matrix,second_matrix,rotate);
	dJointSetHinge2Axis1 (m_joint, axis.x, axis.y, axis.z);


	dJointSetHinge2Param(m_joint,dParamLoStop ,lo);
	dJointSetHinge2Param(m_joint,dParamHiStop ,hi);

	if(!(axes[0].force<0.f)){
		dJointSetHinge2Param(m_joint,dParamFMax,axes[0].force);
		dJointSetHinge2Param(m_joint,dParamVel ,axes[0].velocity);
	}

	CalcAxis(1,axis,lo,hi,first_matrix,second_matrix,rotate);

	dJointSetHinge2Axis2 (m_joint, axis.x, axis.y, axis.z);

	dJointSetHinge2Param(m_joint,dParamLoStop2 ,lo);
	dJointSetHinge2Param(m_joint,dParamHiStop2 ,hi);
	if(!(axes[1].force<0.f)){
		dJointSetHinge2Param(m_joint,dParamFMax2 ,axes[1].force);
		dJointSetHinge2Param(m_joint,dParamVel2 ,axes[1].velocity);
	}
	//////////////////////////////////////////////////////////////////

}

void CPHJoint::CreateShoulder1()
{
}

void CPHJoint::CreateShoulder2()
{
}

void CPHJoint::CreateUniversalHinge()
{


}

void CPHJoint::CreateWelding()
{
	dynamic_cast<CPHElement*>(pFirst_element)
		->DynamicAttach(dynamic_cast<CPHElement*>(pSecond_element));

}

void CPHJoint::CreateFullControl()
{


	Fvector pos;
	Fmatrix first_matrix,second_matrix;
	Fvector axis;
	CPHElement* first=dynamic_cast<CPHElement*>(pFirst_element);
	CPHElement* second=dynamic_cast<CPHElement*>(pSecond_element);
	dBodyID body1=0;
	dBodyID body2=0;
	if(first)
	{
		first->InterpolateGlobalTransform(&first_matrix);
		body1=first->get_body();
	}
	else
	{
		first_matrix.identity();
	}

	if(second)
	{
		second->InterpolateGlobalTransform(&second_matrix);
		body2=second->get_body();
	}
	else
	{
		second_matrix.identity();

	}



pos.set(0,0,0);
switch(vs_anchor){
case vs_first :first_matrix.transform_tiny(pos,anchor); break;
case vs_second:second_matrix.transform_tiny(pos,anchor); break;
case vs_global:pShell->mXFORM.transform_tiny(pos,anchor);break;	
default:NODEFAULT;	
	}
	//////////////////////////////////////

	if(!(body1&&body2)) 
	{
		m_joint=dJointCreateHinge(phWorld,0);

		dJointAttach(m_joint,body1,body2);

#ifndef ODE_SLOW_SOLVER
		dJointSetHingeAnchor(m_joint,pos.x-1.f,pos.y,pos.z);
#else
		dJointSetHingeAnchor(m_joint,pos.x,pos.y,pos.z);
#endif

		dJointSetHingeAxis(m_joint,0.f,0.f,1.f);
		dJointSetHingeParam(m_joint,dParamLoStop ,0.00f);
		dJointSetHingeParam(m_joint,dParamHiStop ,0.00f);

		dJointSetHingeParam(m_joint,dParamCFM,world_cfm);
		dJointSetHingeParam(m_joint,dParamStopERP,world_erp);
		dJointSetHingeParam(m_joint,dParamStopCFM,world_cfm);


#ifndef ODE_SLOW_SOLVER

		m_joint1=dJointCreateHinge(phWorld,0);

		dJointAttach(m_joint1,body1,body2);

		dJointSetHingeAnchor(m_joint1,pos.x+1.f,pos.y,pos.z);
		dJointSetHingeAxis(m_joint1,0.f,0.f,1.f);

		dJointSetHingeParam(m_joint1,dParamLoStop ,0.00f);
		dJointSetHingeParam(m_joint1,dParamHiStop ,0.00f);

		dJointSetHingeParam(m_joint1,dParamCFM,world_cfm);
		dJointSetHingeParam(m_joint1,dParamStopERP,world_erp);
		dJointSetHingeParam(m_joint1,dParamStopCFM,world_cfm);
#endif

		return;
	}

	m_joint=dJointCreateBall(phWorld,0);
	dJointAttach(m_joint,body1,body2);
	dJointSetBallAnchor(m_joint,pos.x,pos.y,pos.z);



	m_joint1=dJointCreateAMotor(phWorld,0);
	dJointSetAMotorMode (m_joint1, dAMotorEuler);
	dJointSetAMotorNumAxes (m_joint1, 3);

	dJointAttach(m_joint1,body1,body2);

	/////////////////////////////////////////////

	Fmatrix first_matrix_inv;
	first_matrix_inv.set(first_matrix);
	first_matrix_inv.invert();
	Fmatrix rotate;
	rotate.mul(first_matrix_inv,second_matrix);
	/////////////////////////////////////////////

	float lo;
	float hi;
	//////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////
	axis.set(0,0,0);
	//axis 0
	CalcAxis(0,axis,lo,hi,first_matrix,second_matrix,rotate);
	dJointSetAMotorAxis (m_joint1, 0, 1, axis.x, axis.y, axis.z);
	dJointSetAMotorParam(m_joint1,dParamLoStop ,lo);
	dJointSetAMotorParam(m_joint1,dParamHiStop ,hi);

	if(!(axes[0].force<0.f)){
		dJointSetAMotorParam(m_joint1,dParamFMax ,axes[0].force);
		dJointSetAMotorParam(m_joint1,dParamVel ,axes[0].velocity);
	}

	//axis 1
	CalcAxis(1,axis,lo,hi,first_matrix,second_matrix,rotate);
	dJointSetAMotorParam(m_joint1,dParamLoStop2 ,lo);
	dJointSetAMotorParam(m_joint1,dParamHiStop2 ,hi);
	if(!(axes[1].force<0.f)){
		dJointSetAMotorParam(m_joint1,dParamFMax2 ,axes[1].force);
		dJointSetAMotorParam(m_joint1,dParamVel2 ,axes[1].velocity);
	}

	//axis 2
	CalcAxis(2,axis,lo,hi,first_matrix,second_matrix,rotate);
	dJointSetAMotorAxis (m_joint1, 2, 2, axis.x, axis.y, axis.z);
	dJointSetAMotorParam(m_joint1,dParamLoStop3 ,lo);
	dJointSetAMotorParam(m_joint1,dParamHiStop3 ,hi);	
	if(!(axes[2].force<0.f)){
		dJointSetAMotorParam(m_joint1,dParamFMax3 ,axes[2].force);
		dJointSetAMotorParam(m_joint1,dParamVel3 ,axes[2].velocity);
	}

	dJointSetAMotorParam(m_joint1,dParamStopERP ,axes[0].erp);
	dJointSetAMotorParam(m_joint1,dParamStopCFM ,axes[0].cfm);

	dJointSetAMotorParam(m_joint1,dParamStopERP2 ,axes[1].erp);
	dJointSetAMotorParam(m_joint1,dParamStopCFM2 ,axes[1].cfm);

	dJointSetAMotorParam(m_joint1,dParamStopERP3 ,axes[2].erp);
	dJointSetAMotorParam(m_joint1,dParamStopCFM3 ,axes[2].cfm);

	dJointSetAMotorParam(m_joint1,dParamCFM ,m_cfm);
	dJointSetAMotorParam(m_joint1,dParamCFM2 ,m_cfm);
	dJointSetAMotorParam(m_joint1,dParamCFM3 ,m_cfm);
}


void CPHJoint::SetAnchor(const float x,const float y,const float z)
{
	vs_anchor=vs_global;
	anchor.set(x,y,z);
}

void CPHJoint::SetAnchorVsFirstElement(const float x,const float y,const float z)
{
	vs_anchor=vs_first;
	anchor.set(x,y,z);
}

void CPHJoint::SetAnchorVsSecondElement(const float x,const float y,const float z)
{
	vs_anchor=vs_second;
	anchor.set(x,y,z);	
}

void CPHJoint::SetAxisDir(const float x,const float y,const float z,const int axis_num)
{
	int ax=axis_num;
	 LimitAxisNum(ax);
	 if(-1==ax) return;
	axes[ax].vs=vs_global;
	axes[ax].direction.set(x,y,z);
}

void CPHJoint::SetAxisDirVsFirstElement(const float x,const float y,const float z,const int axis_num)
{
	int ax=axis_num;
    LimitAxisNum(ax);
	if(-1==ax) return;
	axes[ax].vs=vs_first;
	axes[ax].direction.set(x,y,z);
}

void CPHJoint::SetAxisDirVsSecondElement(const float x,const float y,const float z,const int axis_num)
{
	int ax=axis_num;
	LimitAxisNum(ax);
	if(-1==ax) return;
	axes[ax].vs=vs_second;
	axes[ax].direction.set(x,y,z);

}

void CPHJoint::SetLimits(const float low, const float high, const int axis_num)
{
	if(!(pFirst_element&&pSecond_element))return;

	int ax=axis_num;
	LimitAxisNum(ax);
	if(-1==ax)return;

		Fvector axis;
	switch(axes[ax].vs){
			case vs_first :pFirst_element->mXFORM.transform_dir(axis,axes[ax].direction);	break;
			case vs_second:pSecond_element->mXFORM.transform_dir(axis,axes[ax].direction); break;
			case vs_global:
			default:		axis.set(axes[ax].direction);							
	}

	axes[ax].low=low;
	axes[ax].high=high;
	Fmatrix m1,m2;
	m1.set(pFirst_element->mXFORM);
	m1.invert();
	m2.mul(m1,pSecond_element->mXFORM);
	//m2.mul(pSecond_element->mXFORM,m1);



	float zer;
	//axis_angleB(m2,axis,zer);
	axis_angleA(m2,axes[ax].direction,zer);

	axes[ax].zero=zer;
	//m2.invert();
	//axes[ax].zero_transform.set(m2);

}


CPHJoint::CPHJoint(CPhysicsJoint::enumType type ,CPhysicsElement* first,CPhysicsElement* second)
{

	pFirst_element=first;
	pSecond_element=second; 
	eType=type;
	bActive=false;

#ifndef ODE_SLOW_SOLVER
	m_erp=world_erp;
	m_cfm=world_cfm;
#else
	m_erp=world_erp;
	m_cfm=world_cfm;
#endif

	SPHAxis axis,axis2,axis3;
	axis2.set_direction(1,0,0);
	axis3.direction.crossproduct(axis.direction,axis3.direction);
	vs_anchor=vs_first;

	switch(eType){
	case ball:					;						break;
	case welding:				;						break;
	case hinge:					axes.push_back(axis);	
		break;
	case hinge2:


	case universal_hinge:		

	case shoulder1:	

	case shoulder2:	

	case car_wheel:	
		axes.push_back(axis);
		axes.push_back(axis2);	
		break;
	case full_control:
		axes.push_back(axis);
		axes.push_back(axis2);	
		axes.push_back(axis3);
	}

}

void CPHJoint::SetLimitsVsFirstElement(const float low, const float high,const  int axis_num)
{
}

void CPHJoint::SetLimitsVsSecondElement(const float low, const float high,const  int axis_num)
{
}

void CPHJoint::Activate()
{
	if(bActive) return;
	switch(eType){
	case ball:					CreateBall();			break;
	case hinge:					CreateHinge();			break;
	case hinge2:				CreateHinge2();			break;
	case universal_hinge:		CreateUniversalHinge(); break;
	case shoulder1:				CreateShoulder1();		break;
	case shoulder2:				CreateShoulder2();		break;
	case car_wheel:				CreateCarWeel();		break;
	case welding:				CreateWelding();		break;
	case full_control:			CreateFullControl();	break;
	}
	bActive=true;
}

void CPHJoint::Deactivate()
{
	if(!bActive) return;
	switch(eType){
	case welding:				; break;
	case ball:					;
	case hinge:					;
	case hinge2:				;
	case universal_hinge:		;
	case shoulder1:				;
	case shoulder2:				;
	case car_wheel:				dJointDestroy(m_joint); 
		break;

	case full_control:			dJointDestroy(m_joint);
		dJointDestroy(m_joint1);
		break;
	}

	bActive=false;
}

void CPHJoint::SetForceAndVelocity		(const float force,const float velocity,const int axis_num)
{
SetForce(force,axis_num);
SetVelocity(velocity,axis_num);
}




void CPHJoint::SetForce		(const float force,const int axis_num){
	int ax;
	ax=axis_num;
	LimitAxisNum(ax);

	if(ax==-1) 
		switch(eType){
					case welding:				; 
					case ball:					return;
					case hinge:					
						axes[0].force=force;
						break;
					case hinge2:				;
					case universal_hinge:		;
					case shoulder1:				;
					case shoulder2:				;
					case car_wheel:				
						axes[0].force=force;
						axes[1].force=force;
						break;

					case full_control:			
						axes[0].force=force;
						axes[1].force=force;
						axes[2].force=force;
						break;
		}

	else{
		axes[ax].force=force;
		}

	if(bActive)
	{
	SetForceActive(ax);
	}
}

void CPHJoint::SetForceActive		(const int axis_num)
{
	switch(eType){

						case hinge2:switch(axis_num)
									{
						case -1:
							dJointSetHinge2Param(m_joint,dParamFMax ,axes[0].force);
							dJointSetHinge2Param(m_joint,dParamFMax2 ,axes[1].force);
						case 0:		dJointSetHinge2Param(m_joint,dParamFMax ,axes[0].force);break;
						case 1:		dJointSetHinge2Param(m_joint,dParamFMax2 ,axes[1].force);break;
									}
									break;
						case universal_hinge:		;
						case shoulder1:				;
						case shoulder2:				;
						case car_wheel:				;
						case welding:				; 
						case ball:					break;
						case hinge:					dJointSetHingeParam(m_joint,dParamFMax ,axes[0].force);
							break;



						case full_control:
							switch(axis_num){
						case -1:
							dJointSetAMotorParam(m_joint1,dParamFMax ,axes[0].force);
							dJointSetAMotorParam(m_joint1,dParamFMax2 ,axes[1].force);
							dJointSetAMotorParam(m_joint1,dParamFMax3 ,axes[2].force);
						case 0:dJointSetAMotorParam(m_joint1,dParamFMax ,axes[0].force);break;
						case 1:dJointSetAMotorParam(m_joint1,dParamFMax2 ,axes[1].force);break;
						case 2:dJointSetAMotorParam(m_joint1,dParamFMax3 ,axes[2].force);break;
							}
							break;
	}
}

void CPHJoint::SetVelocity		(const float velocity,const int axis_num){
	int ax;
	ax=axis_num;
	LimitAxisNum(ax);


	if(ax==-1) 
		switch(eType){
					case welding:				; 
					case ball:					return;
					case hinge:					
						axes[0].velocity=velocity;
						break;
					case hinge2:				;
					case universal_hinge:		;
					case shoulder1:				;
					case shoulder2:				;
					case car_wheel:	
						axes[0].velocity=velocity;
						axes[1].velocity=velocity;
						break;

					case full_control:			
						axes[0].velocity=velocity;
						axes[1].velocity=velocity;
						axes[2].velocity=velocity;
						break;
		}

	else{
			axes[ax].velocity=velocity;
		}

	if(bActive)
	{
	SetVelocityActive(ax);
	}
}

void CPHJoint::SetVelocityActive(const int axis_num)
{
	switch(eType){

						case hinge2:switch(axis_num)
									{
						case -1:
							dJointSetHinge2Param(m_joint,dParamVel ,axes[0].velocity);
							dJointSetHinge2Param(m_joint,dParamVel2 ,axes[1].velocity);
						case 0:		dJointSetHinge2Param(m_joint,dParamVel ,axes[0].velocity);break;
						case 1:		dJointSetHinge2Param(m_joint,dParamVel2 ,axes[1].velocity);break;
									}
									break;
						case universal_hinge:		;
						case shoulder1:				;
						case shoulder2:				;
						case car_wheel:				;
						case welding:				; 
						case ball:					break;
						case hinge:					dJointSetHingeParam(m_joint,dParamVel ,axes[0].velocity);
							break;



						case full_control:
							switch(axis_num){
						case -1:
							dJointSetAMotorParam(m_joint1,dParamVel ,axes[0].velocity);
							dJointSetAMotorParam(m_joint1,dParamVel2 ,axes[1].velocity);
							dJointSetAMotorParam(m_joint1,dParamVel3 ,axes[2].velocity);
						case 0:dJointSetAMotorParam(m_joint1,dParamVel ,axes[0].velocity);break;
						case 1:dJointSetAMotorParam(m_joint1,dParamVel2 ,axes[1].velocity);break;
						case 2:dJointSetAMotorParam(m_joint1,dParamVel3 ,axes[2].velocity);break;
							}
							break;
	}
}

void CPHJoint::LimitAxisNum(int &axis_num)
{
	if(axis_num<-1) 
	{
		axis_num=-1;
		return;
	}

	switch(eType){

						case welding:				; 
						case ball:					axis_num=-1;
													break;
						case hinge:					axis_num=0;
													break;
						case hinge2:				;
						case universal_hinge:		;
						case shoulder1:				;
						case shoulder2:				;
						case car_wheel:				axis_num= axis_num>1 ? 1 : axis_num; 
													break;

						case full_control:			axis_num= axis_num>2 ? 2 : axis_num; 
													break;
				}

}

void CPHJoint::SetAxis(const SPHAxis& axis,const int axis_num)
{
int ax=axis_num;
LimitAxisNum(ax);
if(ax==-1) 
	switch(eType){
		case welding:				; 
		case ball:					return;
		case hinge:					
									axes[0]=axis;
									break;
		case hinge2:				;
		case universal_hinge:		;
		case shoulder1:				;
		case shoulder2:				;
		case car_wheel:	
			axes[0]=axis;
			axes[1]=axis;
			break;

		case full_control:			
			axes[0]=axis;
			axes[1]=axis;
			axes[2]=axis;
			break;
	}
else
	axes[axis_num]=axis;
}

void CPHJoint::SetAxisSDfactors(float spring_factor,float damping_factor,int axis_num)
{
	int ax=axis_num;
	LimitAxisNum(ax);
	float erp=ERP(world_spring*spring_factor,world_damping*damping_factor);
	float cfm=CFM(world_spring*spring_factor,world_damping*damping_factor);
	if(ax==-1) 
		switch(eType){
		case welding:				; 
		case ball:					return;
		case hinge:					
			axes[0].erp=erp;
			axes[0].cfm=cfm;
			break;
		case hinge2:				;
		case universal_hinge:		;
		case shoulder1:				;
		case shoulder2:				;
		case car_wheel:	
			axes[0].erp=erp;
			axes[0].cfm=cfm;
			axes[1].erp=erp;
			axes[1].cfm=cfm;
			break;

		case full_control:			
			axes[0].erp=erp;
			axes[0].cfm=cfm;
			axes[1].erp=erp;
			axes[1].cfm=cfm;
			axes[2].erp=erp;
			axes[2].cfm=cfm;
			break;
		}
	else
	{
		axes[ax].erp=erp;
		axes[ax].cfm=cfm;
	}
}

void CPHJoint::SetJointSDfactors(float spring_factor,float damping_factor)
{
	m_erp=ERP(world_spring*spring_factor,world_damping*damping_factor);
	m_cfm=CFM(world_spring*spring_factor,world_damping*damping_factor);
}

void CPHJoint::CalcAxis(int ax_num,Fvector& axis, float& lo,float& hi,const Fmatrix& first_matrix,const Fmatrix& second_matrix,const Fmatrix& rotate)
{
	switch(axes[ax_num].vs)
	{
	case vs_first :first_matrix.transform_dir(axis,axes[ax_num].direction)	;	break;
	case vs_second:second_matrix.transform_dir(axis,axes[ax_num].direction)	; break;
	case vs_global:pShell->mXFORM.transform_dir(axis,axes[ax_num].direction);break;
	default:		NODEFAULT;							
	}


	float shift_angle;
	axis_angleA(rotate,axes[ax_num].direction,shift_angle);

	shift_angle-=axes[ax_num].zero;

	if(shift_angle>M_PI) shift_angle-=2.f*M_PI;
	if(shift_angle<-M_PI) shift_angle+=2.f*M_PI;

	lo=axes[ax_num].low+shift_angle;
	hi=axes[ax_num].high+shift_angle;


	if(lo<-M_PI){ 
		hi-=(lo+M_PI);
		lo=-M_PI;
	}
	if(lo>0.f) {
		hi-=lo;
		lo=0.f;
	}
	if(hi>M_PI) {
		lo-=(hi-M_PI);
		hi=M_PI;
	}
	if(hi<0.f) {
		lo-=hi;
		hi=0.f;
	}
}

void CPHJoint::CalcAxis(int ax_num,Fvector& axis,float& lo,float& hi,const Fmatrix& first_matrix,const Fmatrix& second_matrix)
{
	switch(axes[ax_num].vs)
	{

	case vs_first :first_matrix.transform_dir(axis,axes[ax_num].direction);	break;
	case vs_second:second_matrix.transform_dir(axis,axes[ax_num].direction); break;
	case vs_global:pShell->mXFORM.transform_dir(axis,axes[ax_num].direction);break;
	default:		NODEFAULT;							
	}


	Fmatrix inv_first_matrix;
	inv_first_matrix.set(first_matrix);
	inv_first_matrix.invert();

	Fmatrix rotate;
	rotate.mul(inv_first_matrix,second_matrix);

	float shift_angle;
	axis_angleA(rotate,axes[ax_num].direction,shift_angle);

	shift_angle-=axes[ax_num].zero;

	if(shift_angle>M_PI) shift_angle-=2.f*M_PI;
	if(shift_angle<-M_PI) shift_angle+=2.f*M_PI;


	lo=axes[ax_num].low;//+shift_angle;
	hi=axes[ax_num].high;//+shift_angle;
	if(lo<-M_PI){ 
		hi-=(lo+M_PI);
		lo=-M_PI;
	}
	if(lo>0.f) {
		hi-=lo;
		lo=0.f;
	}
	if(hi>M_PI) {
		lo-=(hi-M_PI);
		hi=M_PI;
	}
	if(hi<0.f) {
		lo-=hi;
		hi=0.f;
	}


}

CPHJoint::SPHAxis::SPHAxis(){
	high=M_PI/15.f;
	low=-M_PI/15.f;;
	zero=0.f;
	//erp=ERP(world_spring/5.f,world_damping*5.f);
	//cfm=CFM(world_spring/5.f,world_damping*5.f);
#ifndef ODE_SLOW_SOLVER
	erp=world_erp;
	cfm=world_cfm;
#else
	erp=0.3f;
	cfm=0.000001f;
#endif
	direction.set(0,0,1);
	vs=vs_first;
	force=0.f;
	velocity=0.f;
}