#pragma once
#ifndef CPHMOVEMENT_CONTROL_H
#define CPHMOVEMENT_CONTROL_H

#include "PHCharacter.h"
#include "AI_Space.h"
using namespace AI;

class CPHAICharacter;
class CPHSimpleCharacter;
class CPHCapture;
class CPHMovementControl 
{
static const path_few_point=10;
public:
void				PHCaptureObject(CGameObject* object);
void				PHCaptureObject(CGameObject* object,u16 element);
CPHCapture*			PHCapture      (){return m_capture;}
void				PHReleaseObject();
void				JumpV(const Fvector &jump_velocity);
void				Jump(const Fvector &start_point, const Fvector &end_point, float time);
void				Jump(const Fvector &end_point, float time);
float				Jump(const Fvector &end_point);
bool				JumpState(){return (m_character&&m_character->b_exist&&m_character->IsEnabled()&&m_character->JumpState());};
bool			b_exect_position;
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
	string16			m_capture_bone;

	float				m_fGroundDelayFactor;
	BOOL				bIsAffectedByGravity;
	//------------------------------
	CObject*			pObject;
	EEnvironment		eOldEnvironment;
	EEnvironment		eEnvironment;
	Fbox				aabb;
	Fbox				boxes	[4];
	Fvector				vFootCenter;			// �������� ������������ Position()
	Fvector				vFootExt;				//

	float				fAirFriction;
	float				fWallFriction;
	float				fGroundFriction;
	float				fFriction;				// Current friction

	float				fMass;
	float				fMinCrashSpeed;
	float				fMaxCrashSpeed;

	Fvector				vVelocity;
	Fvector				vPosition;

	Fvector				vPathPoint;
	Fvector				vPathDir;
	int					m_path_size;
	int					m_start_index;
	float				m_path_distance;

	float				fLastMotionMag;

	float				fActualVelocity;
	float				fContactSpeed;
	float				fLastUpdateTime;
	Fvector				vLastUpdatePosition;
public:
	Fvector				vExternalImpulse;
	BOOL				bSleep;

	BOOL				gcontact_Was;			// �����������
	float				gcontact_Power;			// ��������� ������ ���������
	float				gcontact_HealthLost;	// ����� �������� ��������

	void				AllocateCharacterObject(CharacterType type);
	void				DeleteCharacterObject();

	void				CreateCharacter()		{	
		dVector3 size={aabb.x2-aabb.x1,aabb.y2-aabb.y1,aabb.z2-aabb.z1};
		m_character->Create(size);
	}
	void				DestroyCharacter();
	void				Load					(LPCSTR section);
#ifdef DEBUG
	void				dbg_Draw(){
		if(m_character)
			m_character->OnRender();
	};
#endif

	void				SetFriction(float air, float wall, float ground)
	{
		fAirFriction	= air;
		fWallFriction	= wall;
		fGroundFriction	= ground;
	}
	void SetPLastMaterial(u32* p){m_character->SetPLastMaterial(p);}
	float				GetCurrentFriction()		{ return fFriction; }

	const Fvector&		GetVelocity		( )			{ return vVelocity;	}
	float				GetVelocityMagnitude()		{ return vVelocity.magnitude();	}
	float				GetVelocityActual	()		{ return fActualVelocity;	}
	float				GetContactSpeed	()			{ return fContactSpeed; }
	void				SetVelocity		(float x, float y, float z)	{vVelocity.set(x,y,z);m_character->SetVelocity(vVelocity);}
	void				SetVelocity		(const Fvector& v)	{vVelocity.set(v);m_character->SetVelocity(vVelocity);}
	void				SetPhysicsRefObject(CPhysicsRefObject* ref_object){m_character->SetPhysicsRefObject(ref_object);};

	void				CalcMaximumVelocity	(Fvector& dest, Fvector& accel, float friction){};
	void				CalcMaximumVelocity	(float& dest, float accel, float friction){};

	void				ActivateBox		(DWORD id)	{ 
														aabb.set(boxes[id]);
														if(!m_character) return;
													    if(!m_character->b_exist) return;
														m_character->Destroy();
														CreateCharacter();	
														m_character->SetPosition(vPosition);	
													}

	EEnvironment		Environment		( )			{ return eEnvironment; }
	EEnvironment		OldEnvironment	( )			{ return eOldEnvironment; }
	const Fbox&			Box				( )			{ return aabb; }
	const Fbox*			Boxes			( )			{return boxes;}
	const Fvector&		FootExtent		( )			{return vFootExt;}
	void				SetBox			(DWORD id, const Fbox &BB)	{ boxes[id].set(BB); aabb.set(BB); }

	void				SetParent		(CObject* P){ pObject = P; }

	void				SetMass			(float M)	{ fMass = M;
	if(m_character)
		m_character->SetMas(fMass);
	}
	float				GetMass			()			{ return fMass;	}

	void				SetFoots		(Fvector& C, Fvector &E)
	{	vFootCenter.set	(C); vFootExt.set	(E); 	}

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

	bool				TryPosition				(Fvector& pos)															{return m_character->TryPosition(pos);}
	bool				IsCharacterEnabled		()																		{return m_character->IsEnabled();}
	void				Calculate				(Fvector& vAccel, float ang_speed, float jump, float dt, bool bLight);
	void				Calculate				(const Fvector& desired_pos,float velocity,float dt);

	void				Calculate				(const xr_vector<CTravelNode>& path, //in path
												float speed,						 //in speed
												u32& travel_point,					 //in- travel start, out - current trev point
												float& precesition					 //in- tolerance, out - precesition
												);
	void				PathNearestPoint		(const xr_vector<CTravelNode>	&path,		//in path
												 const Fvector					&new_position,  //in position
												 int							&index,			//out nearest
												 bool							&type          //out type
												);	//return nearest point
	void				PathNearestPointFindUp(const xr_vector<CTravelNode>		&path,			//in path
											   const Fvector					&new_position,  //in position
											   int								&index,			//out nearest
											   float							radius,			//in exit radius
											   bool								&near_line      //out type
											   );
	void				PathNearestPointFindDown(const xr_vector<CTravelNode>	&path,			//in path
												 const Fvector					&new_position,  //in position
												 int							&index,			//out nearest
												 float							radius,			//in exit radius
												 bool							&near_line      //out type
												 );

	void				PathDIrPoint			(const xr_vector<CTravelNode>				&path,		//in path
															 int							index,			//in index
															 float							distance,	//in distance
															 float							precesition,//in precesition
															 Fvector						&dir        //out dir
															 );
	void				PathDIrLine				(const xr_vector<CTravelNode>	&path,		//in path
												int								index,		//in point
												float							distance,	//in distance
												float							precesition,//in precesition
												Fvector							&dir        //out dir
												);

	//	void				Move					(Fvector& Dest, Fvector& Motion, BOOL bDynamic=FALSE){};
	void				SetApplyGravity			(BOOL flag)																{ bIsAffectedByGravity=flag; }
	void				GetDeathPosition		(Fvector& pos)															{ m_character->DeathPosition(pos);}
	void				SetEnvironment			( int enviroment,int old_enviroment);
	void				ApplyImpulse			(const Fvector& dir,const dReal P)										{m_character->ApplyImpulse(dir,P);};
	void				SetJumpUpVelocity		(float velocity)														{m_character->SetJupmUpVelocity(velocity);}
	void				EnableCharacter			()																		{m_character->Enable();}
	u16					ContactBone				(){return m_character->ContactBone();}
	void				GetDesiredPos			(Fvector& dpos)
	{	
		m_character->GetDesiredPosition(dpos);
	}
	bool				CharacterExist()		
	{
		return m_character->b_exist;
	}
	CPHMovementControl(void);
	~CPHMovementControl(void);
};
#endif