#pragma once
#ifndef CPHMOVEMENT_CONTROL_H
#define CPHMOVEMENT_CONTROL_H

#include "PHCharacter.h"
namespace DetailPathManager {
	struct STravelPathPoint;
};

class CPHAICharacter;
class CPHSimpleCharacter;
class CPHCapture;
class CPHSynchronize;
class CPHMovementControl 
{
static const path_few_point=10;
public:
void				PHCaptureObject(CPhysicsShellHolder* object);
void				PHCaptureObject(CPhysicsShellHolder* object,u16 element);
CPHCapture*			PHCapture      (){return m_capture;}
void				PHReleaseObject();
Fvector				PHCaptureGetNearestElemPos(const CPhysicsShellHolder* object);
Fmatrix				PHCaptureGetNearestElemTransform(CPhysicsShellHolder* object);
void				SetMaterial(u16 material);
void				SetAirControlParam(float param){fAirControlParam=param;}
enum				JumpType 
{
					jtStrait, //end point before uppermost point
					jtCurved, //end point after uppermost point
					jtHigh	  //end point is uppermost point
};
void				JumpV(const Fvector &jump_velocity);
void				Jump(const Fvector &start_point, const Fvector &end_point, float time);
void				Jump(const Fvector &end_point, float time);
float				Jump(const Fvector &end_point);
bool				JumpState(){return (m_character&&m_character->b_exist&&m_character->IsEnabled()&&m_character->JumpState());};
///
void				GetJumpMinVelParam(Fvector &min_vel,float &time,JumpType &type,const Fvector &end_point);	//returns vector of velocity of jump with minimal start speed
																												//in min_vel and correspondent jump time in time
float				JumpMinVelTime(const Fvector &end_point); // return time of jump with min start speed
// input: end_point and time; return velocity and type of jump
void				GetJumpParam(Fvector &velocity, JumpType &type,const Fvector &end_point, float time);
bool				b_exect_position;
public:

	enum EEnvironment
	{
		peOnGround,
		peAtWall,
		peInAir
	};
	enum CharacterType
	{
		actor,
		ai
	};
private:
	void				CheckEnvironment	(const Fvector& V);


	CPHCharacter*		m_character;
	CPHCapture  *		m_capture;

	float				m_fGroundDelayFactor;
	BOOL				bIsAffectedByGravity;
	//------------------------------
	CObject*			pObject;
	EEnvironment		eOldEnvironment;
	EEnvironment		eEnvironment;
	Fbox				aabb;
	Fbox				boxes	[4];
	DWORD				m_dwCurBox;
	//Fvector				vFootCenter;			// задаются относительно Position()
	//Fvector				vFootExt;				//

	//float				fAirFriction;
	//float				fWallFriction;
	//float				fGroundFriction;
	//float				fFriction;				// Current friction

	float				fMass;
	float				fMinCrashSpeed;
	float				fMaxCrashSpeed;
	
	float				fAirControlParam;

	Fvector				vVelocity;
	Fvector				vPosition;

	Fvector				vPathPoint;
	Fvector				vPathDir;
	int					m_path_size;
	int					m_start_index;
	float				m_path_distance;
	u16					m_material;

	float				fLastMotionMag;

	float				fActualVelocity;
	float				fContactSpeed;
	float				fLastUpdateTime;
	Fvector				vLastUpdatePosition;
	
public:
	Fvector				vExternalImpulse;
	bool				bExernalImpulse;
	BOOL				bSleep;

	BOOL				gcontact_Was;			// Приземление
	float				gcontact_Power;			// Насколько сильно ударились
	float				gcontact_HealthLost;	// Скоко здоровья потеряли

	void				AllocateCharacterObject(CharacterType type);
	void				DeleteCharacterObject();

	void				CreateCharacter();		
	void				DestroyCharacter();
	void				Load					(LPCSTR section);
#ifdef DEBUG
	void				dbg_Draw(){
		if(m_character)
			m_character->OnRender();
	};
#endif

	//void				SetFriction(float air, float wall, float ground)
	//{
	//	fAirFriction	= air;
	//	fWallFriction	= wall;
	//	fGroundFriction	= ground;
	//}
	void SetPLastMaterial(u16* p){m_character->SetPLastMaterial(p);}
	//float				GetCurrentFriction()		{ return fFriction; }

	const Fvector&		GetVelocity			( )		{ return vVelocity;	}
	void				GetCharacterVelocity(Fvector& velocity )		{if(m_character)m_character->GetVelocity(velocity); else velocity.set(0.f,0.f,0.f);}
	float				GetVelocityMagnitude()		{ return vVelocity.magnitude();	}
	float				GetVelocityActual	()		{ return fActualVelocity;	}
	float				GetContactSpeed		()		{ return fContactSpeed; }
	void				GroundNormal		(Fvector &norm)					;
	CPHSynchronize*		GetSyncItem			()								;
	void				Freeze				()								;
	void				UnFreeze			()								;
	void				SetVelocity		(float x, float y, float z)	{vVelocity.set(x,y,z);m_character->SetVelocity(vVelocity);}
	void				SetVelocity		(const Fvector& v)	{vVelocity.set(v);m_character->SetVelocity(vVelocity);}
	void				SetPhysicsRefObject(CPhysicsShellHolder* ref_object){m_character->SetPhysicsRefObject(ref_object);};

	void				CalcMaximumVelocity	(Fvector& /**dest/**/, Fvector& /**accel/**/, float /**friction/**/){};
	void				CalcMaximumVelocity	(float& /**dest/**/, float /**accel/**/, float /**friction/**/){};
	void				ActivateBox		(DWORD id, BOOL Check = false);
	EEnvironment		Environment		( )			{ return eEnvironment; }
	EEnvironment		OldEnvironment	( )			{ return eOldEnvironment; }
	const Fbox&			Box				( )			{ return aabb; }
	DWORD				BoxID			( )const	{ return m_dwCurBox;}
	const Fbox*			Boxes			( )			{return boxes;}
	float				FootRadius		( )			{if(m_character)return m_character->FootRadius(); else return 0.f;};
	void				CollisionEnable (BOOL enable){if(m_character) if(enable)m_character->collision_enable();else m_character->collision_disable();}
	//const Fvector&		FootExtent		( )			{return vFootExt;}
	void				SetBox			(DWORD id, const Fbox &BB)	{ boxes[id].set(BB); aabb.set(BB); }

	void				SetParent		(CObject* P){ pObject = P; }

	void				SetMass			(float M)	{ fMass = M;
	if(m_character)
		m_character->SetMas(fMass);
	}
	float				GetMass			()			{ return fMass;	}

	//void				SetFoots		(Fvector& C, Fvector &E)
	//{	vFootCenter.set	(C); vFootExt.set	(E); 	}

	void				SetCrashSpeeds	(float min, float max)
	{	fMinCrashSpeed	= min; 	fMaxCrashSpeed	= max; 	}

	void				SetPosition		(const Fvector &P)
	{	vPosition.set	(P); vLastUpdatePosition.set(P); m_character->SetPosition(vPosition);}

	void				SetPosition		(float x, float y, float z)
	{	vPosition.set	(x,y,z);m_character->SetPosition(vPosition); 	}

	void				GetPosition		(Fvector &P)
	{	P.set			(vPosition); }
	void				GetCharacterPosition(Fvector &P)
	{ m_character->GetPosition(P);}
	void				InterpolatePosition(Fvector	&P)
	{
		VERIFY(m_character&&m_character->b_exist);
		m_character->IPosition(P);
	}
	bool				TryPosition				(Fvector& pos)															{return m_character->TryPosition(pos)&&!bExernalImpulse;}
	bool				IsCharacterEnabled		()																		{return m_character->IsEnabled()||bExernalImpulse;}
	void				Calculate				(Fvector& vAccel,const Fvector& camDir, float ang_speed, float jump, float dt, bool bLight);
	void				Calculate				(const Fvector& desired_pos,float velocity,float dt);

	void				Calculate				(const xr_vector<DetailPathManager::STravelPathPoint>& path, //in path
												float speed,						 //in speed
												u32& travel_point,					 //in- travel start, out - current trev point
												float& precesition					 //in- tolerance, out - precesition
												);
	void				AddControlVel			(const Fvector& vel);
	void				PathNearestPoint		(const xr_vector<DetailPathManager::STravelPathPoint>	&path,		//in path
												 const Fvector					&new_position,  //in position
												 int							&index,			//out nearest
												 bool							&type          //out type
												);	//return nearest point
	void				PathNearestPointFindUp(const xr_vector<DetailPathManager::STravelPathPoint>		&path,			//in path
											   const Fvector					&new_position,  //in position
											   int								&index,			//out nearest
											   float							radius,			//in exit radius
											   bool								&near_line      //out type
											   );
	void				PathNearestPointFindDown(const xr_vector<DetailPathManager::STravelPathPoint>	&path,			//in path
												 const Fvector					&new_position,  //in position
												 int							&index,			//out nearest
												 float							radius,			//in exit radius
												 bool							&near_line      //out type
												 );

	void				PathDIrPoint			(const xr_vector<DetailPathManager::STravelPathPoint>				&path,		//in path
															 int							index,			//in index
															 float							distance,	//in distance
															 float							precesition,//in precesition
															 Fvector						&dir        //out dir
															 );
	void				PathDIrLine				(const xr_vector<DetailPathManager::STravelPathPoint>	&path,		//in path
												int								index,		//in point
												float							distance,	//in distance
												float							precesition,//in precesition
												Fvector							&dir        //out dir
												);

	//	void				Move					(Fvector& Dest, Fvector& Motion, BOOL bDynamic=FALSE){};
	void				SetApplyGravity			(BOOL flag)																{ bIsAffectedByGravity=flag;if(m_character&&m_character->b_exist)m_character->SetApplyGravity(flag); }
	void				GetDeathPosition		(Fvector& pos)															{ m_character->DeathPosition(pos);}
	void				SetEnvironment			( int enviroment,int old_enviroment);
	void				ApplyImpulse			(const Fvector& dir,const dReal P)										;
	void				SetJumpUpVelocity		(float velocity)														{m_character->SetJupmUpVelocity(velocity);}
	void				EnableCharacter			()																		{if(m_character)m_character->Enable();}
	void				SetOjectContactCallback (ObjectContactCallbackFun* callback){if(m_character)m_character->SetObjectContactCallback(callback);}
	ObjectContactCallbackFun* ObjectContactCallback(){if(m_character)return m_character->ObjectContactCallBack();else return NULL; }
	u16					ContactBone				(){return m_character->ContactBone();}
	void				GetDesiredPos			(Fvector& dpos)
	{	
		m_character->GetDesiredPosition(dpos);
	}
	bool				CharacterExist()		
	{
		return (m_character && m_character->b_exist);
	}
	CPHMovementControl(void);
	~CPHMovementControl(void);
};
#endif