//----------------------------------------------------
#ifndef _Motion_H_
#define _Motion_H_

#ifdef _LW_EXPORT
#include <lwrender.h>
#include <lwhost.h>
#endif

// refs
class CEnvelope;
class CFS_Base;
class CStream;

enum EChannelType{
	ctUnsupported = -1,
	ctPositionX=0,
	ctPositionY,
	ctPositionZ,
	ctRotationH,
	ctRotationP,
	ctRotationB,
	ctMaxChannel
};

#define MAX_OBJ_NAME 64
#define WORLD_ORIENTATION (1<<0)

struct st_BoneMotion{
//	char		name[MAX_OBJ_NAME];
	CEnvelope*	envs[ctMaxChannel];
	DWORD		flag;
    st_BoneMotion()	{flag=0; ZeroMemory(envs,sizeof(CEnvelope*)*ctMaxChannel);}
};
// list по костям
DEFINE_VECTOR(st_BoneMotion,BoneMotionVec,BoneMotionIt);

//--------------------------------------------------------------------------
class ENGINE_API CCustomMotion{
protected:
	enum EMotionType{
		mtObject	= 0,
		mtSkeleton,
		ForceDWORD	= DWORD(-1)
	};
	EMotionType		mtype;
	string256		name;
	int				iFrameStart, iFrameEnd;
	float			fFPS;
public:
					CCustomMotion	();
					CCustomMotion	(CCustomMotion* src);
	virtual			~CCustomMotion	();

	void			SetName			(const char* n)	{if(n) strcpy(name,n); strlwr(name);}
	string256&		Name			()				{return name;}
    int				FrameStart		()				{return iFrameStart;}
    int				FrameEnd		()				{return iFrameEnd;}
    float			FPS				()				{return fFPS;}
    int				Length			()				{return iFrameEnd-iFrameStart;}

	void			SetParam		(int s, int e, float fps){iFrameStart=s; iFrameEnd=e; fFPS=fps;}

	virtual void	Save			(CFS_Base& F);
	virtual bool	Load			(CStream& F);

	virtual void	SaveMotion		(const char* buf)=0;
	virtual bool	LoadMotion		(const char* buf)=0;

#ifdef _LW_EXPORT
	CEnvelope*		CreateEnvelope	(LWChannelID chan, LWChannelID* chan_parent=0);
#endif
};

//--------------------------------------------------------------------------
class ENGINE_API COMotion: public CCustomMotion{
	CEnvelope*		envs[ctMaxChannel];
public:
					COMotion		();
					COMotion		(COMotion* src);
	virtual			~COMotion		();

	void			Evaluate		(float t, Fvector& T, Fvector& R);
	virtual void	Save			(CFS_Base& F);
	virtual bool	Load			(CStream& F);

	virtual void	SaveMotion		(const char* buf);
	virtual bool	LoadMotion		(const char* buf);

#ifdef _LW_EXPORT
	void			ParseObjectMotion(LWItemID object);
#endif
};

//--------------------------------------------------------------------------
class ENGINE_API CSMotion: public CCustomMotion{
	BoneMotionVec	bone_mots;
public:
    int				iBoneOrPart;
    BOOL			bFX;
	BOOL			bStopAtEnd;
    float			fSpeed;                      
    float			fAccrue;
    float			fFalloff;
    float			fPower;

    void			Clear			();
public:
					CSMotion		();
					CSMotion		(CSMotion* src);
	virtual			~CSMotion		();

	void			Evaluate		(int bone_idx, float t, Fvector& T, Fvector& R);

    void			CopyMotion		(CSMotion* src);

    BoneMotionVec&	BoneMotions		()				{return bone_mots;}
	void			SetBoneOrPart	(int idx)		{iBoneOrPart=idx;}
	DWORD			GetMotionFlag	(int bone_idx)	{return bone_mots[bone_idx].flag;}

	virtual void	Save			(CFS_Base& F);
	virtual bool	Load			(CStream& F);

	virtual void	SaveMotion		(const char* buf);
	virtual bool	LoadMotion		(const char* buf);
#ifdef _LW_EXPORT
	void			ParseBoneMotion	(LWItemID bone);
#endif
};
#endif
