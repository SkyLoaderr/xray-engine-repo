#ifndef __OBJECT_ANIMATOR_H__
#define __OBJECT_ANIMATOR_H__

// refs
class COMotion;
class ENGINE_API CInifile;

struct st_AnimParam{
    float				t;
    float				min_t;
    float				max_t;
	bool				bLoop;
    void				Set		(COMotion* M, bool _loop);
    IC float			Frame	(){return t;}
    bool				Update	(float dt);
};


class CObjectAnimator
{
private:
	struct str_pred : public binary_function<char*, char*, bool>{	
		IC bool operator()(const char* x, const char* y) const {return strcmp(x,y)<0;}
	};
	DEFINE_MAP_PRED		(LPSTR,COMotion*,MotionMap,MotionPairIt,str_pred);
protected:
	Fvector				vPosition;
	Fmatrix				mRotate;

    COMotion*			m_ActiveMotion;
    st_AnimParam		m_MParam;
	MotionMap			m_Motions;
	void				LoadMotions		(const char* fname);

    IC bool				IsMotionable	()	{return	!!m_Motions.size();}
	void				SetActiveMotion	(COMotion* mot, bool bLoop=true);
	COMotion*			FindMotionByName(LPCSTR name);
public:
						CObjectAnimator	();
	virtual				~CObjectAnimator();

	void				Load			(CInifile* ini, const char * section);

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
