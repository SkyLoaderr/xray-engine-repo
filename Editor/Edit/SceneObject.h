//----------------------------------------------------
// file: SceneObject.h
//----------------------------------------------------
#ifndef _INCDEF_SceneObject_H_
#define _INCDEF_SceneObject_H_

#include "SceneClassList.h"

#define CHUNK_OBJECT_BODY   0x7777
//----------------------------------------------------
struct SRayPickInfo;
struct FSChunkDef;
class CFrustum;
class CStream;
class CFS_Base;

enum ERenderPriority{
    rpNormal,
    rpAlphaNormal,
    rpAlphaLast
};

class SceneObject {
protected:
	EObjClass 		m_ClassID;
	char 			m_Name[MAX_OBJ_NAME];

    int 			m_GroupIndex;
    
	BOOL 			m_Selected;
	BOOL 			m_Visible;
    BOOL 			m_Locked;
    BOOL 			m_Valid;
public:
	IC BOOL 		Visible			(){return m_Visible; }
	IC BOOL 		Locked			(){return m_Locked; }
	IC BOOL 		Selected		(){return m_Selected; }
	IC EObjClass& 	ClassID			(){return m_ClassID; }
	IC char*		GetName			(){return m_Name; }
	IC void			SetName			(LPCSTR N){strcpy(m_Name,N); }

    IC BOOL 		Group			(int g_idx)	{if (!IsInGroup()){m_GroupIndex=g_idx;return true;}return false;}
    IC void 		Ungroup			()			{if (IsInGroup()){m_GroupIndex=-1;}}
    IC BOOL 		IsInGroup		()			{return m_GroupIndex>=0;}
    IC int 			GetGroupIndex	()			{return m_GroupIndex;}

	IC virtual bool IsRender		(Fmatrix& parent){return true;}
	virtual void 	Render			(Fmatrix& parent, ERenderPriority flag){};
	virtual void 	RTL_Update		(float dT)	{};

	virtual bool 	RayPick			(float& dist, Fvector& start,Fvector& dir, Fmatrix& parent, SRayPickInfo* pinf=NULL){ return false; };
    virtual bool 	FrustumPick		(const CFrustum& frustum, const Fmatrix& parent){ return false; };
    virtual bool 	SpherePick		(const Fvector& center, float radius, const Fmatrix& parent){ return false; };

	virtual void 	Select			(BOOL flag);
	virtual void 	Show			(BOOL flag);
	virtual void 	Lock			(BOOL flag);

    virtual bool 	GetPosition		(Fvector& pos){ return false; }
    virtual bool 	GetRotate		(Fvector& rot){ return false; }
    virtual bool 	GetScale		(Fvector& scale){ return false; }

    virtual void 	SetPosition		(Fvector& pos){;}
    virtual void 	SetRotate		(Fvector& rot){;}
    virtual void 	SetScale		(Fvector& scale){;}
    virtual void 	UpdateTransform	(){;}
    virtual void 	ResetAnimation	(bool upd_t=true){;}

	virtual void 	Move			(Fvector& amount){};
	virtual void 	Rotate			(Fvector& center, Fvector& axis, float angle ){};
	virtual void 	Scale			(Fvector& center, Fvector& amount ){};
	virtual void 	LocalRotate		(Fvector& axis, float angle ){};
	virtual void 	LocalScale		(Fvector& amount){};

	virtual bool 	Load			(CStream&);
	virtual void 	Save			(CFS_Base&);

	virtual bool 	GetBox			(Fbox& box){return false; }
	virtual void 	OnSceneUpdate	(){;}
	virtual void 	OnDestroy		(){;}

	virtual void 	OnDeviceCreate	(){;}
	virtual void 	OnDeviceDestroy	(){;}

	virtual void 	OnSynchronize	(){;}
    virtual void    OnShowHint      (AStringVec& dest);

	virtual bool 	Valid			(){return m_Valid;}

					SceneObject		(){
                        m_ClassID 	= OBJCLASS_DUMMY;
                        m_Name[0] 	= 0;
                        m_Selected 	= false;
                        m_Visible 	= true;
                        m_Locked 	= false;
                        m_Valid		= false;
                        m_GroupIndex= -1;
                    }

					SceneObject		(SceneObject* source){
                        m_ClassID 	= source->m_ClassID;
                        strcpy		(m_Name,source->m_Name);
                        m_Selected 	= false;
                        m_Visible 	= true;
                        m_Locked 	= false;
                        m_Valid		= false;
                        m_GroupIndex= -1;
					}

	virtual 		~SceneObject();
};

typedef list<SceneObject*> ObjectList;
typedef ObjectList::iterator ObjectIt;
typedef map<EObjClass,ObjectList> ObjectMap;
typedef ObjectMap::iterator ObjectPairIt;

//----------------------------------------------------
#endif /* _INCDEF_SceneObject_H_ */
