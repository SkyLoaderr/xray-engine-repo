//----------------------------------------------------
#ifndef MotionH
#define MotionH

#ifdef _LW_EXPORT
#include <lwrender.h>
#include <lwhost.h>
#endif

#include "bone.h"

// refs
class CEnvelope;
class IWriter;
class IReader;

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

struct st_BoneMotion
{
	enum {
		flWorldOrient = 1<<0,
	};
	LPSTR		name;
	CEnvelope*	envs[ctMaxChannel];
	Flags8		m_Flags;
    			st_BoneMotion(){name=0; m_Flags.zero(); ZeroMemory(envs,sizeof(CEnvelope*)*ctMaxChannel);}
    void        SetName(LPCSTR nm){xr_free(name); name=xr_strdup(nm);}
};
// vector по костям
DEFINE_VECTOR(st_BoneMotion,BoneMotionVec,BoneMotionIt);

//--------------------------------------------------------------------------
class ENGINE_API CCustomMotion
{
protected:
	enum EMotionType
	{
		mtObject	= 0,
		mtSkeleton,
		ForceDWORD	= u32(-1)
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

	virtual void	Save			(IWriter& F);
	virtual bool	Load			(IReader& F);

	virtual void	SaveMotion		(const char* buf)=0;
	virtual bool	LoadMotion		(const char* buf)=0;

#ifdef _LW_EXPORT
	CEnvelope*		CreateEnvelope	(LWChannelID chan, LWChannelID* chan_parent=0);
#endif
};

//--------------------------------------------------------------------------
class ENGINE_API COMotion: public CCustomMotion
{
	CEnvelope*		envs			[ctMaxChannel];
public:
					COMotion		();
					COMotion		(COMotion* src);
	virtual			~COMotion		();

	void			Clear			();

	void			_Evaluate		(float t, Fvector& T, Fvector& R);
	virtual void	Save			(IWriter& F);
	virtual bool	Load			(IReader& F);

	virtual void	SaveMotion		(const char* buf);
	virtual bool	LoadMotion		(const char* buf);

#ifdef _LW_EXPORT
	void			ParseObjectMotion(LWItemID object);
#endif
#ifdef _EDITOR
	void 			FindNearestKey	(float t, float& min_k, float& max_k, float eps=EPS_L);
	void			CreateKey		(float t, const Fvector& P, const Fvector& R);
	void			DeleteKey		(float t);
    void			NormalizeKeys	();
    int				KeyCount		();
	CEnvelope*		Envelope		(EChannelType et=ctPositionX){return envs[et];}
    BOOL			ScaleKeys		(float from_time, float to_time, float scale_factor);
    BOOL			NormalizeKeys	(float from_time, float to_time, float speed);
    float			GetLength		(float* mn=0, float* mx=0);
#endif
};

//--------------------------------------------------------------------------
enum ESMFlags{
    esmFX		= 1<<0,
    esmStopAtEnd= 1<<1,
    esmNoMix	= 1<<2,
    esmSyncPart	= 1<<3
};

class ENGINE_API CSMotion: public CCustomMotion{
	BoneMotionVec	bone_mots;
public:
    s32				iBoneOrPart;
    float			fSpeed;
    float			fAccrue;
    float			fFalloff;
    float			fPower;
	Flags8			m_Flags;

    void			Clear			();
public:
					CSMotion		();
					CSMotion		(CSMotion* src);
	virtual			~CSMotion		();

	void			_Evaluate		(int bone_idx, float t, Fvector& T, Fvector& R);

    void			CopyMotion		(CSMotion* src);

    st_BoneMotion*	FindBoneMotion	(LPCSTR name);
    BoneMotionVec&	BoneMotions		()				{return bone_mots;}
	void			SetBoneOrPart	(s16 idx)		{iBoneOrPart=idx;}
	Flags8			GetMotionFlags	(int bone_idx)	{return bone_mots[bone_idx].m_Flags;}

	virtual void	Save			(IWriter& F);
	virtual bool	Load			(IReader& F);

	virtual void	SaveMotion		(const char* buf);
	virtual bool	LoadMotion		(const char* buf);

    void			SortBonesBySkeleton(BoneVec& bones);
    void			WorldRotate		(int boneId, float h, float p, float b);
#ifdef _LW_EXPORT
	void			ParseBoneMotion	(LWItemID bone);
#endif
};
#endif
