#ifndef __OBJECT_ANIMATOR_H__
#define __OBJECT_ANIMATOR_H__

// refs
class COMotion;
class ENGINE_API CInifile;

struct ENGINE_API st_AnimParam{
    float				t;
    float				min_t;
    float				max_t;
	bool				bLoop;
	bool				bWrapped;
    void				Set		(COMotion* M, bool _loop);
    IC float			Frame	(){return t;}
    bool				Update	(float dt);
};


class ENGINE_API CObjectAnimator
{
private:
	struct str_pred : public std::binary_function<char*, char*, bool>{	
		IC bool operator()(LPCSTR x, LPCSTR y) const {return strcmp(x,y)<0;}
	};
	DEFINE_MAP_PRED		(LPSTR,COMotion*,MotionMap,MotionPairIt,str_pred);
protected:
	Fvector				vPosition;
	Fmatrix				mRotate;

    COMotion*			m_ActiveMotion;
    st_AnimParam		m_MParam;
	MotionMap			m_Motions;
	void				LoadMotions		(LPCSTR fname);

    IC bool				IsMotionable	()	{return	!!m_Motions.size();}
	void				SetActiveMotion	(COMotion* mot, bool bLoop=true);
	COMotion*			FindMotionByName(LPCSTR name);
public:
						CObjectAnimator	();
	virtual				~CObjectAnimator();

	// void				Load			(CInifile* ini, LPCSTR section);
	void				Load			(LPCSTR name);

	COMotion*			PlayMotion		(LPCSTR name, bool bLoop);
	void				StopMotion		();

    IC bool				IsMotionActive	()	{return IsMotionable()&&m_ActiveMotion; }

	IC const Fmatrix&	GetRotate		(){return mRotate;}
	IC const Fvector&	GetPosition		(){return vPosition;}
	
	// Update
	void				OnMove			();

	// manual calculation
	COMotion*			GetMotion		(LPCSTR name);
	void				ManualUpdate	(COMotion* M, float frame);
};

#endif //__OBJECT_ANIMATOR_H__
