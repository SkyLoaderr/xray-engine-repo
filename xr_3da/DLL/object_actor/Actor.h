// Actor.h: interface for the CActor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ACTOR_H__C66583EA_EEA6_45F0_AC9F_918B5997F194__INCLUDED_)
#define AFX_ACTOR_H__C66583EA_EEA6_45F0_AC9F_918B5997F194__INCLUDED_
#pragma once

#include "..\..\customactor.h"

enum EMoveState{
	msIdle,
	msStill,
	msWalk,
	msRun,
	msJump
};

class CActor : public CCustomActor, public pureCameraChange
{
private:
	typedef CCustomActor inherited;
		
	// Networking
	float				fTimeToExport;

	// Motions
	Fvector				vMotion;
	DWORD				cur_mstate, new_mstate;
	DWORD				dis_mstate, enb_mstate;
	float				yaw;

//------------------------------
	Fvector				vControlSpeed;
	Fmatrix				mMoveOrientation;
//------------------------------
	// Physic
	float				fMass;
	Fvector				vVelocity;
	Fvector				vControlAccel;
//------------------------------
	float				fBobCycle;

	EMoveState			eMoveState;
	EPlayerEnvironment	eOldEnvironment;
	EPlayerState		eOldState;
	float				fPrevCamPos;
	bool				bEyeCrouch;

	void				CheckControls	( );
	void				SetAnimation	( );
	void				UpdateState		( );
	bool				CanChangeSize	(const Fbox& bb);

	void				TransformByBone	(int bone_idx, Fvector& pos);

	void				CheckEnvironment();
	void				CheckDamage		(float delta);
public:
						CActor			(void *params);
	virtual				~CActor			( );

	virtual void		Load			( CInifile* ini, const char *section );

	virtual void		GetFireParams	(Fvector &fire_pos, Fvector &fire_dir);
	virtual void		Die				( );
	virtual void		Hit				(int percentage);
	virtual void		HitAnother		(CCustomActor* pActor, int Power );
	virtual	void		Respawn			( );

	virtual void		FireStart		( );
	virtual void		FireEnd			( );
	virtual BOOL		TakeItem		( DWORD CID );

	virtual void		NetworkExport	( );
	virtual void		OnNetworkMessage( );

	virtual void		DoMove			(EMoveCommand move_type, BOOL bSet = false);
	virtual NR_RESULT	OnNear			(CCFModel* target);
	virtual BOOL		OnCollide		(CCFModel* target);
	virtual void		GetEyePoint		(Fvector& cam_point, Fvector& cam_dangle);

	virtual void		OnCameraChange	( );
	virtual void		OnMove			( ); 

	void				Statistic		();
};

#endif // !defined(AFX_ACTOR_H__C66583EA_EEA6_45F0_AC9F_918B5997F194__INCLUDED_)
