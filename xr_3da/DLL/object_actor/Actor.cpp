// Actor.cpp: implementation of the CActor class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Actor.h"
#include "..\..\xr_creator.h"
#include "..\..\xr_area.h"
#include "..\..\networkclient.h"
#include "..\..\net_messages.h"
#include "..\..\bodyinstance.h"
#include "..\..\std_classes.h"
#include "..\..\physics\PhysicsObject.h"
#include "..\..\collide\cl_intersect.h"
#include "..\..\effectorfall.h"

// font
#include "..\..\xr_smallfont.h"
#include "..\..\x_ray.h"

// breakpoints
#include "..\..\xr_input.h"

#define	WALK_ACCEL		35.0f
#define	JUMP_SPEED		8.0f
#define RUN_COEF		2.0f

#define GROUND_FRICTION	10.0f
#define AIR_FRICTION	0.01f
#define WALL_FRICTION	10.0f

#define MIN_CRASH_SPEED	12.0f
#define MAX_CRASH_SPEED	25.0f

#define X_SIZE_2 0.35f
#define Y_SIZE_2 0.8f
#define Z_SIZE_2 0.35f

static Fbox		bbStandBox;
static Fbox		bbCrouchBox;
static Fvector	vFootCenter;
static Fvector	vFootExt;
//--------------------------------------------------------------------
#define _Walk		(mcFwd|mcBack|mcLStrafe|mcRStrafe)
#define _Fwd		(mcFwd)
#define _Back		(mcBack)
#define _LStr		(mcLStrafe)
#define _RStr		(mcRStrafe)
#define _FwdLStr	(mcFwd|mcLStrafe)
#define _FwdRStr	(mcFwd|mcRStrafe)
#define _BackLStr	(mcBack|mcLStrafe)
#define _BackRStr	(mcBack|mcRStrafe)
#define _AFwd		(mcAccel|mcFwd)
#define _ABack		(mcAccel|mcBack)
#define _ALStr		(mcAccel|mcLStrafe)
#define _ARStr		(mcAccel|mcRStrafe)
#define _AFwdLStr	(mcAccel|mcFwd|mcLStrafe)
#define _AFwdRStr	(mcAccel|mcFwd|mcRStrafe)
#define _ABackLStr	(mcAccel|mcBack|mcLStrafe)
#define _ABackRStr	(mcAccel|mcBack|mcRStrafe)
#define _Jump		(mcJump)
//--------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CActor::CActor(void *p) : CCustomActor(p)
{
	bbStandBox.set		(-X_SIZE_2,0,-Z_SIZE_2, X_SIZE_2,Y_SIZE_2*2,Z_SIZE_2);
	bbCrouchBox.set		(-X_SIZE_2,0,-Z_SIZE_2, X_SIZE_2,Y_SIZE_2,Z_SIZE_2);
	Fvector sh;
	sh.set				(X_SIZE_2*0.25f,0,Z_SIZE_2*0.25f);
	Fbox bbFootBox;
	bbFootBox.set		(-X_SIZE_2,	-0.01f, -Z_SIZE_2, X_SIZE_2, Y_SIZE_2*.4f, Z_SIZE_2);
	// уменьшим размер ступней что-бы чуть зависать в воздухе при запрыгивании на препятствие
	bbFootBox.shrink	(sh); 
	bbFootBox.getcenter	(vFootCenter);
	bbFootBox.getradius	(vFootExt);

	fTimeToExport		= A_NET_EXPORTTIME;
	
	yaw					= 0;
	vControlSpeed.set	(0,0,0);
	mMoveOrientation.identity();

	fMass				= 120;
	vVelocity.set		(0,0,0);
	fBobCycle			= 0;

	eMoveState			= msIdle;
	bEyeCrouch			= false;
	fPrevCamPos			= 0;

	pCreator->Cameras.seqCameraChange.Add(this);
}

CActor::~CActor()
{
	pCreator->Cameras.seqCameraChange.Remove(this);
}

void CActor::Load(CInifile* ini, const char* section )
{
	inherited::Load(ini,section);
	
	curBox.set(bbStandBox);
	
	Respawn	( );
	
	bVisible = true;
}

void CActor::Die	( )
{
	// Inherited
	inherited::Die	();

	eState = psDead;
	cur_mstate=0;
	
	// Respawn
//	Respawn			();
}

void CActor::Hit	(int percentage)
{
	inherited::Hit	(percentage);
}

void CActor::HitAnother(CCustomActor* pActor, int Power )
{
	inherited::HitAnother(pActor,Power);

#ifndef NO_XR_NETWORK	
	struct {
		DWORD	dpID;
		int		Power;
	} A = { dpID, Power	};
	pNetwork->SendMsg	(M_ACTOR_HIT_ANOTHER,&A,sizeof(A),TRUE);
#endif
}

void CActor::Respawn	( )
{
	inherited::Respawn	( );
	
	// *** movement state - respawn
	cur_mstate			= 0;
	new_mstate			= 0;
	
	// *** respawn point
	vPosition.set		(pCreator->GetRespawnPoint());
	vPosition.y			+=1.f;
//	fPrevCamPos			= vPosition.y+1.f;
	
	// *** network
	// signal others that we are respawned
#ifndef NO_XR_NETWORK	
	pNetwork->SendMsg	(M_ACTOR_RESPAWN,&vPosition,sizeof(Fvector),TRUE);
#endif
	
	// *** misc
	bVisible = TRUE;
}

void CActor::FireStart	( )
{
	if (!Weapon->isWorking()) {
		inherited::FireStart( );
		
#ifndef NO_XR_NETWORK	
		// *** network
		Fvector				PD[2];
		GetFireParams		(PD[0],PD[1]);
		pNetwork->SendMsg	(M_ACTOR_FIRE_START,PD,2*sizeof(Fvector),TRUE);
#endif
	}
}

void CActor::FireEnd	( )
{
	if (Weapon->isWorking()) {
		inherited::FireEnd	( );
		
#ifndef NO_XR_NETWORK	
		// *** network
		pNetwork->SendMsg	(M_ACTOR_FIRE_STOP,0,0,TRUE);
#endif
	}
}

BOOL CActor::TakeItem		( DWORD CID )
{
	BOOL bResult = inherited::TakeItem(CID);
	if (bResult){// *** network
#ifndef NO_XR_NETWORK	
		pNetwork->SendMsg(M_ACTOR_TAKE_ITEM,&CID,sizeof(CID),TRUE);
#endif
	}
	return bResult;
}

void CActor::NetworkExport()
{
#ifndef NO_XR_NETWORK	
	// check if it is local actor
	fTimeToExport-=Device.fTimeDelta;
	if (fTimeToExport<0) {
		fTimeToExport+= A_NET_EXPORTTIME;
		A_NET_export	E;
		E.vPos.set		(vPosition	);
		E.vDir.div		(vMotion, Device.fTimeDelta	);
		E.yaw			= yaw;
		E.MState		= cur_mstate;
		
		pNetwork->SendMsg(M_ACTOR_UPDATE,&E,sizeof(E),false);
	}
#endif
}

void CActor::OnNetworkMessage( )
{
#ifndef NO_XR_NETWORK	
	DWORD dwTimeStamp;
	switch (pNetwork->GetMsg(&dwTimeStamp)) {
	case M_ACTOR_UPDATE:
	case M_ACTOR_WEAPON_CHANGE:
	case M_ACTOR_FIRE_START:
	case M_ACTOR_FIRE_STOP:
	case M_ACTOR_HIT_ANOTHER:
	case M_ACTOR_RESPAWN:
	case M_ACTOR_TAKE_ITEM:
		R_ASSERT(0=="Invalid message received");
		break;
	}
#endif
}

void CActor::DoMove(EMoveCommand move_type, BOOL bSet)
{
	if (eState!=psDead){
		if (bSet)	new_mstate |= move_type;
		else		new_mstate &=~move_type;
	}
}

// добавить в последующем ratio для правильной передачи расстояния
void CActor::OnMove	()
{
	if (!bEnabled) return;

	float dt = Device.fTimeDelta;

	// enabled & disabled motion state on current frame
	dis_mstate = cur_mstate & (~new_mstate);
	enb_mstate = dis_mstate?new_mstate:((~cur_mstate) & new_mstate);
	cur_mstate = new_mstate;

	eOldState		= eState;
	eOldEnvironment	= eEnvironment;
	
	// update controls
	CheckControls();

	// prepare safe translate object
	Fmatrix*		T;
	Fvector			accel, V_calc, V_term, temp, center, motion, start_pos, final_pos, final_vel; // start_pos, final_pos - object center position
	switch(eEnvironment){
		case peInAir:		
		case peAtWall:		T=&mMoveOrientation;break;
		case peOnGround:	T=&mTransform;		break;
		default: VERIFY(0);
	}
	T->transform_dir(accel, vControlAccel);
	temp.mul		(accel,dt*dt*0.5f);
	motion.direct	(temp,vVelocity,dt);
	V_calc.direct	(vVelocity,accel,dt);

	curBox.getcenter(center);

	start_pos.set	(vPosition);
	start_pos.add	(center);

	// test nearest object
	pCreator->ObjectSpace.TestNearestObject(cfModel, start_pos, curBox.getradius());

	// safe translate object in world
	float sq_vel = (eEnvironment==peOnGround)?psSqueezeVelocity:EPS;
	// final position & velocity at the !last! collision movement
	pCreator->ObjectSpace.SafeTranslate( cfModel, mTransform, start_pos, motion, curBox, final_pos, final_vel, sq_vel);

	float s_calc	= motion.magnitude();	// length of motion - dS - requisted
	motion.sub		(final_pos,start_pos);	// motion - resulting
	float s_res		= motion.magnitude();	// length of motion - dS - resulting

	V_term.direct	(vVelocity,accel,(s_res/s_calc)*dt);	// ???? Расчетная конечная 

	temp.div		(motion,dt);			// средняя скорость после колиж.
	vVelocity.lerp	(temp,V_calc,.3f);		// fake correction :)

	//	Don't allow new velocity to go against original velocity unless told otherwise
	final_vel.normalize_safe();
	V_calc.normalize_safe	();
	if ((final_vel.dotproduct(V_calc)<0.f)||((s_res/s_calc)<0.01f)){ 
		vVelocity.set(0,0,0);
		final_pos.set(start_pos);
		eMoveState=msStill;
	}

	// set position
	final_pos.sub	(center);
	vPosition.set	(final_pos);

	CheckEnvironment();

	// calculate friction
	float new_speed=0, speed = sqrtf(vVelocity.x*vVelocity.x+vVelocity.z*vVelocity.z);//vVelocity.magnitude();
	if (speed>0){
		switch(eEnvironment){
			case peInAir:		new_speed	= speed-dt*speed*AIR_FRICTION;		break;
			case peAtWall:		new_speed	= speed-dt*speed*WALL_FRICTION;		break;
			case peOnGround:	new_speed	= speed-dt*speed*GROUND_FRICTION;	break;
			default: VERIFY(0);
		}
		if (new_speed<0)new_speed=0;
		new_speed		/= speed;
	}
	// Apply movement friction
	vVelocity.set(vVelocity.x*new_speed,vVelocity.y,vVelocity.z*new_speed);

	// check state
	UpdateState		();

	// если определили приземление или резко уиеньшилась скорость (порядок MIN_CRASH_SPEED)
	// определим возможное повреждение
	float term_speed = V_term.magnitude();
	if ((((eOldEnvironment==peInAir)||(eOldEnvironment==peAtWall))
		&&(eEnvironment==peOnGround))||((term_speed-vVelocity.magnitude())>MIN_CRASH_SPEED))
		CheckDamage	(term_speed);

	// update motions
	SetAnimation	();

	UpdateTransform		();
	CObject::OnMove		();
	Weapon->UpdatePosition();

	Statistic();
}

bool CActor::CanChangeSize(const Fbox& bb){
	Fvector start_pos;
	bb.getcenter	(start_pos);
	start_pos.add	(vPosition);
	return !pCreator->ObjectSpace.EllipsoidCollide(cfModel,mTransform,start_pos,bb);
}

void CActor::CheckDamage(float delta){
	if (delta>MIN_CRASH_SPEED/3.f)
		pCreator->Cameras.AddEffector(new CEffectorFall(delta/MAX_CRASH_SPEED));
	if (delta>MIN_CRASH_SPEED){
		//float k=10000.f/(B-A);
		//float dh=sqrtf((dv-A)*k);
		float dh=(100*(delta-MIN_CRASH_SPEED))/(MAX_CRASH_SPEED-MIN_CRASH_SPEED);
		Hit(dh);
	}
}

void CActor::CheckEnvironment(){
	// verify surface and update Player State
	int cp_cnt=pCreator->ObjectSpace.tris_list.size();
	eEnvironment=peInAir;
	if (cp_cnt){
		Fmatrix33	A; A.set(mRotate);
		Fvector		C; C.add(vPosition, vFootCenter);
		Fvector		dir, point;
		curBox.getcenter(point);
		point.add	(vPosition);
		for(int i=0; i<cp_cnt; i++){
			RAPID::tri& T=pCreator->ObjectSpace.tris_list[i];
			Fvector N; N.mknormal(*T.verts[0],*T.verts[1],*T.verts[2]);
			dir.sub	(*T.verts[0],point);
			if ((dir.dotproduct(N)<0)){ // все что ниже пояса
				if (RAPID::TestBBoxTri(A,C,vFootExt,T.verts,false)){
					if ((N.y>.7f)){
						eEnvironment=peOnGround;
						return;
					}else if((N.y>=0)&&(N.y<=0.7f)){
						eEnvironment=peAtWall;
					}
				}
			}
		}
	}
}

void CActor::UpdateState(){
	// могу ли я встать
	if((eState==psCrouch)&&(!(cur_mstate&mcCrouch))){
		if (CanChangeSize(bbStandBox)){
			eState=psStand;
			curBox.set		(bbStandBox);
		}
	}
}

void CActor::CheckControls(){
	// control accel
	vControlAccel.set(0,0,0);
	eMoveState=msIdle;
	if (eEnvironment==peOnGround){
		// check player move state
		if (cur_mstate&_Walk){
			if ((!!(cur_mstate&mcAccel))^psAlwaysRun) eMoveState=msRun;
			else					eMoveState=msWalk;
		}
		// update player accel
		if (cur_mstate&mcFwd)		vControlAccel.z =  1;
		if (cur_mstate&mcBack)		vControlAccel.z = -1;
		if (cur_mstate&mcLStrafe)	vControlAccel.x = -1;
		if (cur_mstate&mcRStrafe)	vControlAccel.x =  1;
		vControlAccel.normalize_safe();
		vControlAccel.mul			(WALK_ACCEL);
		if ((!!(cur_mstate&mcAccel)) ^ psAlwaysRun) vControlAccel.mul(RUN_COEF);
		if (eState==psCrouch)		vControlAccel.mul(0.4f);
	}
	// apply gravity
	vControlAccel.y = -psGravity;
	// jump
	if(enb_mstate&mcJump){
		eMoveState=msJump; 
		if (eEnvironment==peOnGround){
			vControlAccel.y = ((eState==psCrouch)?JUMP_SPEED*.8f:JUMP_SPEED)/Device.fTimeDelta;
			mMoveOrientation.set(mRotate);
		}
		new_mstate&=~mcJump;
	}
	// update actor orientation according to camera orientation
	VERIFY(pCreator);
	float new_yaw = pCreator->Cameras.GetYaw();
	if (fabsf(yaw-new_yaw)>EPS){
		yaw = 0.7f*yaw+0.3f*new_yaw;
		Fvector D,N;
		D.set(-sinf(yaw),0,-cosf(yaw));
		N.set(0,1,0);
		mRotate.rotation	(D,N);
	}
	// crouch
	if((enb_mstate&mcCrouch)&&(eState!=psCrouch)){
		eState=psCrouch;
		curBox.set(bbCrouchBox);
	}
}

void CActor::SetAnimation( )
{
	if (new_mstate != cur_mstate){
	/*
	switch(new_mstate){
	case _Fwd:		pSkeleton->PlayMotion("RUN",true);	break;
	case _Back:		pSkeleton->PlayMotion("RUN",true);	break; // ???
	case _LStr:		pSkeleton->PlayMotion("RUN",true);	break; // ???
	case _RStr:		pSkeleton->PlayMotion("RUN",true);	break; // ???
	case _FwdLStr:	pSkeleton->PlayMotion("RUN",true);	break; // ???
	case _FwdRStr:	pSkeleton->PlayMotion("RUN",true);	break; // ???
	case _BackLStr:	pSkeleton->PlayMotion("RUN",true);	break; // ???
	case _BackRStr:	pSkeleton->PlayMotion("RUN",true);	break; // ???
	case _AFwd:		pSkeleton->PlayMotion("RUN",true);	break;
	case _ABack:	pSkeleton->PlayMotion("RUN",true);	break; // ???
	case _ALStr:	pSkeleton->PlayMotion("RUN",true);	break; // ???
	case _ARStr:	pSkeleton->PlayMotion("RUN",true);	break; // ???
	case _AFwdLStr:	pSkeleton->PlayMotion("RUN",true);	break; // ???
	case _AFwdRStr:	pSkeleton->PlayMotion("RUN",true);	break; // ???
	case _ABackLStr:pSkeleton->PlayMotion("RUN",true);	break; // ???
	case _ABackRStr:pSkeleton->PlayMotion("RUN",true);	break; // ???
	default:
		pSkeleton->PlayMotion("IDLE",true);
		*/
	}
	//	CMotion* m = pSkeleton->pCurrentMotion;
	//	DWORD E;
	//	float tKey;
	//	while (m->GetNextEvent(tKey,E)){
	//		switch(E){
	//		case etLeftStep:	EXEC_SND(Sounds[asStep], pSounds->Play3DAtPos(Sounds[asStep], vPosition)); break;
	//		case etRightStep:	EXEC_SND(Sounds[asStep], pSounds->Play3DAtPos(Sounds[asStep], vPosition)); break;
	//		};
	//	}
}

void CActor::TransformByBone(int bone_idx, Fvector& pos)
{
	PKinematics(pVisual)->GetBoneTransform(bone_idx)->transform_tiny(pos);
}


void CActor::GetEyePoint(Fvector& cam_point, Fvector& cam_dangle)
{
	// get calc eye point 90% from player height
	Fvector			R;
	curBox.getsize	(R);
	cam_point.set	(0,R.y*0.9f,0.0f);
	mTransform.transform_tiny(cam_point);

	// soft crouch
	if (((eOldState==psCrouch)&&(eState==psStand))||
		((eState==psCrouch)&&(eOldState==psStand))||bEyeCrouch){
		float v_calc, v_res=5.f;
		v_calc			= (cam_point.y-fPrevCamPos)/Device.fTimeDelta;
		if (fabsf(v_calc)>EPS_L){
			if (fabsf(v_calc)<v_res) v_res=fabsf(v_calc);
			v_res		*= fabsf(v_calc)/v_calc;
			cam_point.y	= fPrevCamPos + v_res*Device.fTimeDelta;
			bEyeCrouch	= true;
		}else
			bEyeCrouch	= false;
	}
	// save previous position of camera
	fPrevCamPos=cam_point.y;
	// apply inertion
	cam_point.y = 0.5f*fPrevCamPos+0.5f*cam_point.y;

	// apply footstep bobbing effect
	Fvector offset;
	offset.set(0,0,0);
	cam_dangle.set(0,0,0);
	fBobCycle+=Device.fTimeDelta;

	switch(eMoveState){
	case msWalk: 
		offset.y		= 0.1f*fabsf(sinf(7*fBobCycle)); 
		cam_dangle.x	= 0.01f*fabsf(cosf(fBobCycle*7));
		cam_dangle.z	= 0.01f*cosf(fBobCycle*7);
		if (eState==psCrouch)
			cam_dangle.y= 0.02f*sinf(fBobCycle*7);
		break;
	case msRun: 
		offset.y		= 0.01f*fabsf(sinf(11*fBobCycle)); 
		cam_dangle.x	= 0.01f*fabsf(cosf(fBobCycle*11));
		cam_dangle.z	= 0.01f*cosf(fBobCycle*11);
		if (eState==psCrouch)
			cam_dangle.y= 0.02f*sinf(fBobCycle*11);
		break;
	default: fBobCycle=0;
	}
	cam_point.add(offset);
}

void CActor::GetFireParams	(Fvector &fire_pos, Fvector &fire_dir)
{
	fire_pos.add		(Device.vCameraPosition,Weapon->Position());
	fire_pos.mul		(.5f);
	pCreator->Cameras.GetDirection(fire_dir);
}

BOOL CActor::OnCollide		( CCFModel* target )
{
	return true;
}

NR_RESULT CActor::OnNear( CCFModel* target )
{
	if (!target->GetOwner())	return NR_BUSY;
	switch (target->GetOwner()->SUB_CLS_ID){
	case CLSID_OBJECT_ITEM_STD:	target->GetOwner()->ExecuteAction(this); return NR_FREE;
	}
	return NR_FREE;
}

VOID CActor::OnCameraChange	()
{
	switch(pCreator->Cameras.GetActiveStyle()) {
	case csFirstEye:		if (bEnabled) bVisible = false;	break;
	default:				if (bEnabled) bVisible = true;	break;
	}
}

void CActor::Statistic(){
	//-------------------------------------------------------------------
//	pApp->pFont->Out(0.f,-.35f,"Speed [%.2f, %.2f, %.2f]",VPUSH(vControlSpeed));
//	pApp->pFont->Out(0.f,-.30f,"Velocity [%.2f, %.2f, %.2f]",VPUSH(vVelocity));
	
	switch(eEnvironment){
	case peOnGround: pApp->pFont->Out(0.f,-.20f,"Environment: Ground"); break;
	case peInAir:	 pApp->pFont->Out(0.f,-.20f,"Environment: Air");	break;
	case peAtWall:	 pApp->pFont->Out(0.f,-.20f,"Environment: At Wall");break;
	default: pApp->pFont->Out(0.f,-.20f,"Environment: ???");	
	}
	
	switch(eState){
	case psStand:	pApp->pFont->Out(0.f,-.15f,"State: Stand");	break;
	case psCrouch:	pApp->pFont->Out(0.f,-.15f,"State: Crouch");break;
	case psDead:	pApp->pFont->Out(0.f,-.15f,"State: Dead");	break;
	default: pApp->pFont->Out(0.f,-.15f,"State: ???");
	}
	
//	pApp->pFont->Out(0.f,-.1f,"Speed: %3.3f",	pPhysicsObject->GetLinearVelocity(pPhysicsObject->GetSourceConfig()).magnitude());
	//-------------------------------------------------------------------
}

/*
	if (0){
		float newY, Y, Yo, k, Ay, dt, z, f;
		
		Y	= cam_point.y;
		Yo	= prev_y;
		
		z	= 1.5f;
		f	= 2.5f;
		
		dt	= Device.fTimeDelta;
		k	= 2*M_PI*f;
		
		Ay	= (Y-Yo)*0.1f;//k*k - Vy*k*z;
		newY= Yo + Vy*dt + Ay*(dt*dt)/2;
		Vy	= Vy + Ay*dt;
		
//		if (bCrouch||(eState==psCrouch)){ 
//			if (bCrouch&&(fabsf(prev_y-cam_point.y)<EPS))bCrouch=false;
//			else									bCrouch=true; 
//			if (cur_mstate&mcJump) bCrouch=false;
			cam_point.y=newY;
//		}
		//	else 
		//		if (eEnvironment==peOnGround)
		//			cam_point.y=0.3f*cam_point.y+0.7f*newY;
		
		prev_y=newY;
	}
*/
