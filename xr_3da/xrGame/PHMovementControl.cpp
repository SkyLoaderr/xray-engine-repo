#include "stdafx.h"
#include "../cl_intersect.h"
#include "phmovementcontrol.h"
#include "entity.h"
#include "PHDynamicData.h"
#include "Physics.h"
#include "PHAICharacter.h"
#include "PHActorCharacter.h"
#include "PHCapture.h"
#include "ai_space.h"
#include "detail_path_manager.h"

#define GROUND_FRICTION	10.0f
#define AIR_FRICTION	0.01f
#define WALL_FRICTION	3.0f
//#define AIR_RESIST		0.001f

#define def_X_SIZE_2	0.35f
#define def_Y_SIZE_2	0.8f
#define def_Z_SIZE_2	0.35f

CPHMovementControl::CPHMovementControl(void)
{
	//m_character->Create();
	m_material			=0;
	m_capture			=NULL;
	b_exect_position	=true;
	m_start_index		=0;
	pObject	=			NULL;
	eOldEnvironment =	peInAir;
	eEnvironment =		peInAir;
	aabb.set			(-def_X_SIZE_2,0,-def_Z_SIZE_2, def_X_SIZE_2, def_Y_SIZE_2*2, def_Z_SIZE_2);
	///vFootCenter.set		(0,0,0);
	//vFootExt.set		(0,0,0);
	fMass				= 100;
	fMinCrashSpeed		= 12.0f;
	fMaxCrashSpeed		= 25.0f;
	vVelocity.set		(0,0,0);
	vPosition.set		(0,0,0);
	vExternalImpulse.set(0,0,0);
	bExernalImpulse		=false;
	fLastMotionMag		= 1.f;
	//fAirFriction		= AIR_FRICTION;
	//fWallFriction		= WALL_FRICTION;
	//fGroundFriction		= GROUND_FRICTION;
	//fFriction			= fAirFriction;
	bIsAffectedByGravity= TRUE;
	fActualVelocity		= 0;
	m_fGroundDelayFactor= 1.f;
	gcontact_HealthLost = 0;
	fContactSpeed		= 0.f;
	fAirControlParam	= 0.f;
	m_character=NULL;

	m_dwCurBox = 0xffffffff;
}

CPHMovementControl::~CPHMovementControl(void)
{
	if(m_character)
		m_character->Destroy();
	DeleteCharacterObject();
	xr_delete(m_capture);
}

//static Fvector old_pos={0,0,0};
//static bool bFirst=true;
void CPHMovementControl::AddControlVel	(const Fvector& vel)
{
	vExternalImpulse.add(vel);
	bExernalImpulse=true;
}
void CPHMovementControl::ApplyImpulse(const Fvector& dir,const dReal P)
{
	Fvector force;
	force.set(dir);
	force.mul(P/fixed_step);
	
	AddControlVel(force);
	/*m_character->ApplyImpulse(dir,P);*/
}
void CPHMovementControl::Calculate(Fvector& vAccel,float /**ang_speed/**/,float jump,float /**dt/**/,bool /**bLight/**/){

	
	
	if(m_capture) 
	{
		if(m_capture->Failed()) xr_delete(m_capture);
	}
	
	m_character->IPosition(vPosition);
	if(bExernalImpulse)
	{
		
		vAccel.add(vExternalImpulse);
		m_character->ApplyForce(vExternalImpulse);
		vExternalImpulse.set(0.f,0.f,0.f);
		
		bExernalImpulse=false;
	}
	//vAccel.y=jump;
	float mAccel=vAccel.magnitude();
	m_character->SetMaximumVelocity(mAccel/10.f);
	//if(!fis_zero(mAccel))vAccel.mul(1.f/mAccel);
	m_character->SetAcceleration(vAccel);
	if(!fis_zero(jump)) m_character->Jump(vAccel);

	m_character->GetVelocity(vVelocity); 
	fActualVelocity=vVelocity.magnitude();
	gcontact_Was=m_character->ContactWas();
	fContactSpeed=0.f;
	//if(gcontact_Was)
 	{
		fContactSpeed=m_character->ContactVelocity();
		//m_character->ContactVelocity()=0.f;
		gcontact_Power				= fContactSpeed/fMaxCrashSpeed;

		gcontact_HealthLost			= 0;
		if (fContactSpeed>fMinCrashSpeed) 
		{
			//float k=10000.f/(B-A);
			//float dh=_sqrt((dv-A)*k);
			gcontact_HealthLost = 
				(100*(fContactSpeed-fMinCrashSpeed))/(fMaxCrashSpeed-fMinCrashSpeed);
		}
	}
	CheckEnvironment(vPosition);
	bSleep=false;
}

void CPHMovementControl::Calculate(const Fvector& desired_pos,float velocity,float dt){

	
	if(m_capture) 
	{
		if(m_capture->Failed()) xr_delete(m_capture);
	}
	
	
	m_character->IPosition(vPosition);


	m_character->SetDesiredPosition(desired_pos);
	m_character->BringToDesired(dt,velocity);


	m_character->GetVelocity(vVelocity); 
	fActualVelocity=vVelocity.magnitude();
	gcontact_Was=m_character->ContactWas();
	fContactSpeed=0.f;
	//if(gcontact_Was)
	{
		fContactSpeed=m_character->ContactVelocity();
		//m_character->ContactVelocity()=0.f;
		gcontact_Power				= fContactSpeed/fMaxCrashSpeed;

		gcontact_HealthLost			= 0;
		if (fContactSpeed>fMinCrashSpeed) 
		{
			//float k=10000.f/(B-A);
			//float dh=_sqrt((dv-A)*k);
			gcontact_HealthLost = 
				(100*(fContactSpeed-fMinCrashSpeed))/(fMaxCrashSpeed-fMinCrashSpeed);
		}
	}
	CheckEnvironment(vPosition);
	bSleep=false;


}

void CPHMovementControl::Calculate(const xr_vector<DetailPathManager::STravelPathPoint>& path,float speed,  u32& travel_point,  float& precision  )
{
	
	if(m_capture) 
	{
		if(m_capture->Failed()) xr_delete(m_capture);
	}
	
	
	Fvector new_position;
	m_character->IPosition(new_position);

	int index=0;//nearest point
	//float distance;//distance

	bool  near_line;
	m_path_size=path.size();
	Fvector dir;
	dir.set(0,0,0);
	if(m_path_size==0)
	{
		//m_character->SetMaximumVelocity(0.f);
		speed=0;
		vPosition.set(new_position);
	}
	else if(b_exect_position)
	{
		m_start_index=travel_point;

		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		if((m_path_size-1)>(int)travel_point)
			dir.sub(path[travel_point+1].position,path[travel_point].position);
		else
			dir.sub(path[travel_point].position,new_position);
		m_start_index=travel_point;
		dir.y=0.f;
		dir.normalize_safe();
		vPosition.set(new_position);
		m_path_distance=0;
		vPathDir.set(dir);
		vPathPoint.set(vPosition);
	

	}
	else {
		Fvector dif;
	
		dif.sub(new_position,vPathPoint);
		float radius = dif.magnitude()*2.f;
		if(m_path_size==1)
		{
			//m_character->SetMaximumVelocity(0.f);
			speed=0.f;
			vPosition.set(new_position);	//todo - insert it in PathNearestPoint
			index=0;
			vPathPoint.set(path[0].position);
			vPathDir.sub(path[0].position,new_position);
			m_path_distance=vPathDir.magnitude();
			if(m_path_distance>EPS)
			{
				vPathDir.mul(1.f/m_path_distance);
			}
			near_line=false;
		}
		else
		{
			m_path_distance=dInfinity;
			near_line=true;
			if(m_start_index<m_path_size)
			{
				PathNearestPointFindUp(path,new_position,index,radius,near_line);
				PathNearestPointFindDown(path,new_position,index,radius,near_line);
			}
			if(m_path_distance>radius)
			{
				m_start_index=0;
				PathNearestPoint(path,new_position,index,near_line);
			}
			vPosition.set(new_position);//for PathDirLine && PathDirPoint
			if(near_line) PathDIrLine(path,index,m_path_distance,precision,dir);
			else		  PathDIrPoint(path,index,m_path_distance,precision,dir);


			travel_point=(u32)index;
			m_start_index=index;
			dir.y=0.f;

#pragma TODO ("this must be done on the funnction start!!")
			if(fis_zero(speed)) dir.set(0,0,0);


		}
		
	}
/////////////////////////////////////////////////////////////////
	if(bExernalImpulse)
	{

		//vAccel.add(vExternalImpulse);
		Fvector V;
		V.set(dir);
		//V.mul(speed*fMass/fixed_step);
		V.mul(speed*10.f);
		V.add(vExternalImpulse);
		m_character->ApplyForce(vExternalImpulse);
		speed=V.magnitude();
		
		if(!fis_zero(speed))
		{
			dir.set(V);
			dir.mul(1.f/speed);
		}
		speed/=10.f;
		vExternalImpulse.set(0.f,0.f,0.f);
		bExernalImpulse=false;
	}

	m_character->SetMaximumVelocity(speed);
	m_character->SetAcceleration(dir);
//////////////////////////////////////////////////////
	m_character->GetVelocity(vVelocity); 
	fActualVelocity=vVelocity.magnitude();
	gcontact_Was=m_character->ContactWas();
	fContactSpeed=0.f;
	//if(gcontact_Was)
	{
		fContactSpeed=m_character->ContactVelocity();
		//m_character->ContactVelocity()=0.f;
		gcontact_Power				= fContactSpeed/fMaxCrashSpeed;

		gcontact_HealthLost			= 0;
		if (fContactSpeed>fMinCrashSpeed) 
		{
			//float k=10000.f/(B-A);
			//float dh=_sqrt((dv-A)*k);
			gcontact_HealthLost = 
				(100*(fContactSpeed-fMinCrashSpeed))/(fMaxCrashSpeed-fMinCrashSpeed);
		}
	}
	CheckEnvironment(vPosition);
	bSleep=false;
	b_exect_position=false;
	

}


void CPHMovementControl::PathNearestPoint(const xr_vector<DetailPathManager::STravelPathPoint>  &path,			//in path
										  const Fvector					&new_position,  //in position
										  int							&index,			//in start from; out nearest
										  bool							&near_line       //out type
										  )
{

	Fvector from_first,from_second,dir;
	bool after_line=true;//to check first point

	Fvector path_point,vtemp;
	float temp;

	for(int i=0;i<m_path_size-1;++i)
	{
		const Fvector &first=path[i].position, &second=path[i+1].position;
		from_first.sub(new_position,first);
		from_second.sub(new_position,second);
		dir.sub(second,first);
		dir.normalize_safe();


		if(from_first.dotproduct(dir)<0.f)//befor this line
		{
			if(after_line)//after previous line && befor this line = near first point
			{
				vtemp.sub(new_position,first);
				temp=vtemp.magnitude();
				if(temp<m_path_distance)
				{
					m_path_distance=temp;
					index=i;
					vPathPoint.set(first);
					vPathDir.set(dir);
					near_line=false;
				}
			}
			after_line=false;

		}
		else //after first 
		{
			if(from_second.dotproduct(dir)<0.f) //befor second && after first = near line
			{
				//temp=dir.dotproduct(new_position); seems to be wrong
				temp=dir.dotproduct(from_first);
				vtemp.set(dir);
				vtemp.mul(temp);
				path_point.add(vtemp,first);
				vtemp.sub(path_point,new_position);
				temp=vtemp.magnitude();
				if(temp<m_path_distance)
				{
					m_path_distance=temp;
					index=i;
					vPathPoint.set(path_point);
					vPathDir.set(dir);
					near_line=true;
				}
			}
			else							//after second = after this line
			{
				after_line=true;
			}
		}
	}

	if(m_path_distance==dInfinity)	//after whall path
	{

		R_ASSERT2(after_line,"Must be after line");
		vtemp.sub(new_position,path[i].position);
		m_path_distance=vtemp.magnitude();
		vPathDir.set(dir);
		vPathPoint.set(path[i].position);
		index=i;
		near_line=false;
	}

	return;
}



void CPHMovementControl::PathNearestPointFindUp(const xr_vector<DetailPathManager::STravelPathPoint>	&path,			//in path
												const Fvector					&new_position,  //in position
												int								&index,			//in start from; out nearest
												float							radius,	//out m_path_distance in exit radius
												bool							&near_line      //out type
										  )
{

	Fvector from_first,from_second,dir;
	bool after_line=true;//to check first point

	Fvector path_point,vtemp;
	float temp;

	for(int i=m_start_index;i<m_path_size-1;++i)
	{
		const Fvector &first=path[i].position, &second=path[i+1].position;
		from_first.sub(new_position,first);
		from_second.sub(new_position,second);
		dir.sub(second,first);
		dir.normalize_safe();
		float from_first_dir=from_first.dotproduct(dir);
		float from_second_dir=from_second.dotproduct(dir);

		if(from_first_dir<0.f)//befor this line
		{
			temp=from_first.magnitude();
			if(after_line)//after previous line && befor this line = near first point
			{
				if(temp<m_path_distance)
				{
					m_path_distance=temp;
					index=i;
					vPathPoint.set(first);
					vPathDir.set(dir);
					near_line=false;
				}
			}
	
			if(temp>radius) break;//exit test
			after_line=false;

		}
		else //after first 
		{
			if(from_second_dir<0.f) //befor second && after first = near line
			{
				vtemp.set(dir);
				vtemp.mul(from_first_dir);
				path_point.add(vtemp,first);
				vtemp.sub(path_point,new_position);
				temp=vtemp.magnitude();
				if(temp<m_path_distance)
				{
					m_path_distance=temp;
					index=i;
					vPathPoint.set(path_point);
					vPathDir.set(dir);
					near_line=true;
				}
				if(temp>radius) break;//exit test
			}
			else							//after second = after this line
			{
				after_line=true;
				if(from_second.magnitude()>radius) break;//exit test
			}
		}
	}

	if(m_path_distance==dInfinity && i==m_path_size-1)	
	{

		R_ASSERT2(after_line,"Must be after line");
		vtemp.sub(new_position,path[i].position);
		m_path_distance=vtemp.magnitude();
		vPathDir.set(dir);
		vPathPoint.set(path[i].position);
		index=i;
		near_line=false;
	}
	

	return;
}


void CPHMovementControl::PathNearestPointFindDown(const xr_vector<DetailPathManager::STravelPathPoint>	&path,			//in path
												const Fvector					&new_position,  //in position
												int								&index,			//in start from; out nearest
												float							radius,	//out m_path_distance in exit radius
												bool							&near_line      //out type
										  )
{

	Fvector from_first,from_second,dir;
	bool after_line=true;//to check first point

	Fvector path_point,vtemp;
	float temp;
	//(going down)
	
	for(int i=m_start_index;i>1;--i)
	{
		const Fvector &first=path[i-1].position, &second=path[i].position;
		from_first.sub(new_position,first);
		from_second.sub(new_position,second);
		dir.sub(second,first);
		dir.normalize_safe();
		float from_first_dir=from_first.dotproduct(dir);
		float from_second_dir=from_second.dotproduct(dir);

		if(from_second_dir>0.f)//befor this line
		{
			temp=from_second.magnitude();
			if(after_line)//after previous line && befor this line = near second point (going down)
			{
				if(temp<m_path_distance)
				{
					m_path_distance=temp;
					index=i;
					vPathPoint.set(second);
					vPathDir.set(dir);
					near_line=false;
				}
			}

			if(temp>radius) break;//exit test
			after_line=false;

		}
		else //after second
		{

			if(from_first_dir>0.f) //after second && before first = near line (going down)
			{
				vtemp.set(dir);
				vtemp.mul(from_second_dir);
				path_point.add(second,vtemp); //from_second_dir <0.f !!
				vtemp.sub(path_point,new_position);
				temp=vtemp.magnitude();
				if(temp<m_path_distance)
				{
					m_path_distance=temp;
					index=i-1;
					vPathPoint.set(path_point);
					vPathDir.set(dir);
					near_line=true;
				}
				if(temp>radius) break;//exit test
			}
			else							//after first = after this line(going down)
			{
				after_line=true;
				if(from_first.magnitude()>radius) break;//exit test
			}
		}
	}

	if(m_path_distance==dInfinity && i==1)	
	{

		R_ASSERT2(after_line,"Must be after line");
		vtemp.sub(new_position,path[i].position);
		m_path_distance=vtemp.magnitude();
		vPathDir.set(dir);
		vPathPoint.set(path[i].position);
		index=i;
		near_line=false;
	}


	return;
}

void CPHMovementControl::PathDIrLine(const xr_vector<DetailPathManager::STravelPathPoint> &/**path/**/,  int /**index/**/,  float distance,  float precesition, Fvector &dir  )
{

	Fvector to_path_point;
	to_path_point.sub(vPathPoint,vPosition);	//_new position
	float mag=to_path_point.magnitude();
	if(mag<EPS)
	{
	dir.set(vPathDir);
	return;
	}
	to_path_point.mul(1.f/mag);
	to_path_point.mul(distance*precesition);
	dir.add(vPathDir,to_path_point);
	dir.normalize_safe();
}

void CPHMovementControl::PathDIrPoint(const xr_vector<DetailPathManager::STravelPathPoint> &path,  int index,  float distance,  float precesition, Fvector &dir  )
{
	Fvector to_path_point,tangent;
	to_path_point.sub(vPathPoint,vPosition);	//_new position
	float mag=to_path_point.magnitude();

	if(mag<EPS) //near the point
	{  
		if(0==index||m_path_size-1==index) //on path eidge
		{
			dir.set(vPathDir);//??
			return;
		}
		dir.sub(path[index].position,path[index-1].position);
		dir.normalize_safe();
		dir.add(vPathDir);
		dir.normalize_safe();
	}
	to_path_point.mul(1.f/mag);

	if(m_path_size-1==index)//on_path_edge
	{
	dir.set(to_path_point);
	return;
	}

	tangent.crossproduct(to_path_point,vPathDir);//for basis
	Fmatrix basis,inv_basis;
	basis.i.set(tangent);
	Fvector::generate_orthonormal_basis(basis.i,basis.j,basis.k);
	basis.c.set(0,0,0);
	inv_basis.set(basis);
	inv_basis.transpose();
	Fvector dir_in_b,tpathp_in_b,tangent_in_b;
	inv_basis.transform_dir(dir_in_b,vPathDir);
	inv_basis.transform_dir(tpathp_in_b,to_path_point);


	//build mean dir
	if(0 != index) {
		dir.sub(path[index].position,path[index-1].position);
		dir.normalize_safe();
		dir.add(vPathDir);
		dir.normalize_safe();
	}
	else {
		dir.set(vPathDir);
	}

	//build perpendicular in j - k plane
	tangent_in_b.x=0.;
	tangent_in_b.y=-tpathp_in_b.z;
	tangent_in_b.z=tpathp_in_b.y;
	basis.transform_dir(tangent,tangent_in_b);
	mag=tangent.magnitude();
	if(mag<EPS)
	{
			return;//mean dir
	}
	tangent.mul(1.f/mag);

	
	if(tangent.dotproduct(dir)<0.f)tangent.invert();
	
	to_path_point.mul(distance*precesition);
	dir.add(tangent,to_path_point);
	dir.normalize_safe();
}
void CPHMovementControl::Load					(LPCSTR section){

	//capture
	
	//strcpy(m_capture_bone,pSettings->r_string(section,"capture_bone"));
	
	Fbox	bb;

	// m_PhysicMovementControl: BOX
	Fvector	vBOX1_center= pSettings->r_fvector3	(section,"ph_box1_center"	);
	Fvector	vBOX1_size	= pSettings->r_fvector3	(section,"ph_box1_size"		);
	bb.set	(vBOX1_center,vBOX1_center); bb.grow(vBOX1_size);
	SetBox		(1,bb);

	// m_PhysicMovementControl: BOX
	Fvector	vBOX0_center= pSettings->r_fvector3	(section,"ph_box0_center"	);
	Fvector	vBOX0_size	= pSettings->r_fvector3	(section,"ph_box0_size"		);
	bb.set	(vBOX0_center,vBOX0_center); bb.grow(vBOX0_size);
	SetBox		(0,bb);

	//// m_PhysicMovementControl: Foots
	//Fvector	vFOOT_center= pSettings->r_fvector3	(section,"ph_foot_center"	);
	//Fvector	vFOOT_size	= pSettings->r_fvector3	(section,"ph_foot_size"		);
	//bb.set	(vFOOT_center,vFOOT_center); bb.grow(vFOOT_size);
	//SetFoots	(vFOOT_center,vFOOT_size);

	// m_PhysicMovementControl: Crash speed and mass
	float	cs_min		= pSettings->r_float	(section,"ph_crash_speed_min"	);
	float	cs_max		= pSettings->r_float	(section,"ph_crash_speed_max"	);
	float	mass		= pSettings->r_float	(section,"ph_mass"				);
	

	SetCrashSpeeds	(cs_min,cs_max);
	SetMass		(mass);


	// m_PhysicMovementControl: Frictions
	//float af, gf, wf;
	//af					= pSettings->r_float	(section,"ph_friction_air"	);
	//gf					= pSettings->r_float	(section,"ph_friction_ground");
	//wf					= pSettings->r_float	(section,"ph_friction_wall"	);
	//SetFriction	(af,wf,gf);

	// BOX activate
	ActivateBox	(0);
}

void CPHMovementControl::CheckEnvironment(const Fvector &/**V/**/){
	eOldEnvironment=eEnvironment;
	switch (m_character->CheckInvironment()){
case peOnGround : eEnvironment=peOnGround;break;
case peInAir :		eEnvironment=peInAir		;break;
case peAtWall : eEnvironment=peAtWall		;break;
	}
}

void CPHMovementControl::GroundNormal(Fvector & norm)		
{
if(m_character&&m_character->b_exist)m_character->GroundNormal(norm);
else norm.set(0.f,1.f,0.f);

}

void	CPHMovementControl::SetEnvironment( int enviroment,int old_enviroment){
	switch(enviroment){
	case 0: eEnvironment=peOnGround;
		break;
	case 1: eEnvironment=peAtWall;
		break;
	case 2: eEnvironment=peInAir;
	}
	switch(old_enviroment){
	case 0: eOldEnvironment=peOnGround;
		break;
	case 1: eOldEnvironment=peAtWall;
		break;
	case 2: eOldEnvironment=peInAir;
	}
}

void	CPHMovementControl::AllocateCharacterObject(CharacterType type)
{
	switch(type)
	{
	case actor:			m_character = xr_new<CPHActorCharacter>	();break;
	case ai:	m_character = xr_new<CPHAICharacter>	();break;
	}
	m_character->SetMas(fMass);
	m_character->SetPosition(vPosition);
}

void	CPHMovementControl::PHCaptureObject(CPhysicsShellHolder* object)
{
if(m_capture) return;

if(!object||!object->PPhysicsShell()||!object->m_pPhysicsShell->bActive) return;
m_capture=xr_new<CPHCapture>(m_character,
							 object
							 );
}

void	CPHMovementControl::PHCaptureObject(CPhysicsShellHolder* object,u16 element)
{
	if(m_capture) return;

	if(!object||!object->PPhysicsShell()||!object->PPhysicsShell()->bActive) return;
	m_capture=xr_new<CPHCapture>(m_character,
		object,
		element
		);
}

Fvector CPHMovementControl::PHCaptureGetNearestElemPos(const CPhysicsShellHolder* object)
{
	R_ASSERT3((object->m_pPhysicsShell != NULL), "NO Phisics Shell for object ", *object->cName());

	CPhysicsElement *ph_elem =  object->m_pPhysicsShell->NearestToPoint(vPosition);

	Fvector v;
	ph_elem->GetGlobalPositionDynamic(&v);	
	
	return v;
}

Fmatrix CPHMovementControl::PHCaptureGetNearestElemTransform(CPhysicsShellHolder* object)
{
	CPhysicsElement *ph_elem =  object->m_pPhysicsShell->NearestToPoint(vPosition);

	Fmatrix m;
	ph_elem->GetGlobalTransformDynamic(&m);

	return m;
}


void CPHMovementControl::PHReleaseObject()
{
	if(m_capture) m_capture->Release();
}

void	CPHMovementControl::DestroyCharacter()
{
	m_character->Destroy();
	xr_delete(m_capture);
	//xr_delete<CPHSimpleCharacter>(m_character);
}

void	CPHMovementControl::DeleteCharacterObject()
{
	xr_delete(m_character);
	xr_delete(m_capture);
}

void CPHMovementControl::JumpV(const Fvector &jump_velocity)
{
	m_character->Enable();
	m_character->Jump(jump_velocity);
}

void CPHMovementControl::Jump(const Fvector &end_point, float time)
{
//vPosition
Jump(smart_cast<CGameObject*>(m_character->PhysicsRefObject())->Position(),end_point,time);
}
void CPHMovementControl::Jump(const Fvector &start_point,const Fvector &end_point, float time)
{

Fvector velosity;
velosity.x=end_point.x-start_point.x;
velosity.z=end_point.z-start_point.z;

velosity.x/=time;
velosity.z/=time;
velosity.y=time*world_gravity/2.f+(end_point.y-start_point.y)/time;
JumpV(velosity);
}
float CPHMovementControl::Jump(const Fvector &end_point)
{
Fvector start_point;
start_point.set(smart_cast<CGameObject*>(m_character->PhysicsRefObject())->Position());
Fvector velosity;
velosity.x=end_point.x-start_point.x;
velosity.y=end_point.y-start_point.y;
velosity.z=end_point.z-start_point.z;
float time=_sqrt(2.f*velosity.magnitude()/world_gravity);
velosity.x/=time;
velosity.z/=time;
velosity.y=time*world_gravity/2.f+(velosity.y)/time;
JumpV(velosity);
return time;
}
void CPHMovementControl::GetJumpMinVelParam(Fvector &min_vel,float &time,JumpType &type,const Fvector &end_point)
{
	Fvector start_point;
	start_point.set(smart_cast<CGameObject*>(m_character->PhysicsRefObject())->Position());
	min_vel.x=end_point.x-start_point.x;
	min_vel.y=end_point.y-start_point.y;
	min_vel.z=end_point.z-start_point.z;
	time=_sqrt(2.f*min_vel.magnitude()/world_gravity);
	min_vel.x/=time;
	min_vel.z/=time;
	min_vel.y=time*world_gravity/2.f+(min_vel.y)/time;
	if(min_vel.y<0.f)
	{
		type=jtStrait;
		return;
	}
	float rise_time=min_vel.y/world_gravity;
	if(_abs(rise_time-time)<EPS_L)
	{
		type=jtHigh;
	}
	else if(rise_time>time)
	{
		type=jtStrait;
	}
	else
	{
		type=jtCurved;
	}
}

float CPHMovementControl::JumpMinVelTime(const Fvector &end_point)
{
	Fvector start_point,transference;
	start_point.set(smart_cast<CGameObject*>(m_character->PhysicsRefObject())->Position());
	transference.x=end_point.x-start_point.x;
	transference.y=end_point.y-start_point.y;
	transference.z=end_point.z-start_point.z;
	return _sqrt(2.f*transference.magnitude()/world_gravity);
}

void CPHMovementControl::GetJumpParam(Fvector &velocity, JumpType &type,const Fvector &end_point, float time)
{
	Fvector start_point;
	start_point.set(smart_cast<CGameObject*>(m_character->PhysicsRefObject())->Position());
	Fvector velosity;
	velosity.x=end_point.x-start_point.x;
	velosity.z=end_point.z-start_point.z;
	
	velosity.x/=time;
	velosity.z/=time;
	velosity.y=time*world_gravity/2.f+(end_point.y-start_point.y)/time;
	if(velocity.y<0.f)
	{
		type=jtStrait;
		return;
	}
	float rise_time=velosity.y/world_gravity;
	if(_abs(rise_time-time)<EPS_L)
	{
		type=jtHigh;
	}
	else if(rise_time>time)
	{
		type=jtStrait;
	}
	else
	{
		type=jtCurved;
	}

}

void CPHMovementControl::SetMaterial(u16 material)
{
	m_material=material;
	if(m_character)
	{
		m_character->SetMaterial(material);
	}
}
void CPHMovementControl::CreateCharacter()
{	
	dVector3 size={aabb.x2-aabb.x1,aabb.y2-aabb.y1,aabb.z2-aabb.z1};
	m_character->Create(size);
	m_character->SetMaterial(m_material);
	m_character->SetAirControlFactor(fAirControlParam);
	m_character->SetPosition(vPosition);
}
CPHSynchronize*	CPHMovementControl::GetSyncItem()
{
	if(m_character)	return smart_cast<CPHSynchronize*>(m_character);
	else			return 0;
}
void CPHMovementControl::Freeze()
{
	if(m_character)m_character->Freeze();
}
void CPHMovementControl::UnFreeze()
{
	if(m_character) m_character->UnFreeze();
}

void CPHMovementControl::ActivateBox		(DWORD id, BOOL Check/*false*/)	
{ 
	if (Check && (m_dwCurBox == id)) return;
	m_dwCurBox = id;
	aabb.set(boxes[id]);
	if(!m_character) return;
	if(!m_character->b_exist) return;
	Fvector v;
	m_character->GetVelocity(v);
	m_character->Destroy();
	CreateCharacter();	
	m_character->SetVelocity(v);
	m_character->SetPosition(vPosition);	
}