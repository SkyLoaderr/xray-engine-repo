//----------------------------------------------------
// file: SceneObject.h
//----------------------------------------------------
#ifndef _INCDEF_SceneObject_H_
#define _INCDEF_SceneObject_H_

#include "SceneClassList.h"

#define SCENEOBJECT_CHUNK_PARAMS 0xF900
#define CHUNK_OBJECT_BODY   0x7777
//----------------------------------------------------
struct SPickInfo;
struct FSChunkDef;
class CFrustum;
class CStream;
class CFS_Base;

struct st_ObjVer{
	union{
    	struct{
	    	int f_age;
    	    int f_size;
        };
        __int64 ver;
    };
    int size()						{return sizeof(st_ObjVer);}
    bool operator == (st_ObjVer& v)	{return v.ver==ver;}
    void reset(){ver=0;}
};

class SceneObject {
protected:
	EObjClass m_ClassID;
	char m_Name[MAX_OBJ_NAME];

	BOOL m_Selected;
	BOOL m_Visible;
    BOOL m_Locked;
    BOOL m_Valid;
public:
	IC BOOL Visible(){return m_Visible; }
	IC BOOL Locked(){return m_Locked; }
	IC BOOL Selected(){return m_Selected; }
	IC EObjClass& ClassID(){return m_ClassID; }
	IC char *GetName(){return m_Name; }

	virtual bool __inline IsRender( Fmatrix& parent ){return true;}
	virtual void Render( Fmatrix& parent, bool bAlpha ){};
	virtual void RTL_Update( float dT ){ };

	virtual bool RTL_Pick(float *distance,Fvector& start,Fvector& direction,
		                Fmatrix& parent, SPickInfo* pinf = NULL ){ return false; };

    virtual bool FrustumPick(const CFrustum& frustum, const Fmatrix& parent){ return false; };
    virtual bool SpherePick(const Fvector& center, float radius, const Fmatrix& parent){ return false; };

//	virtual void Select( BOOL flag );
//	virtual void Show( BOOL flag );
//	virtual void Lock( BOOL flag );

	virtual void Move( Fvector& amount ){};
	virtual void Rotate( Fvector& center, Fvector& axis, float angle ){};
	virtual void Scale( Fvector& center, Fvector& amount ){};
	virtual void LocalRotate( Fvector& axis, float angle ){};
	virtual void LocalScale( Fvector& amount ){};

	virtual bool Load(CStream&){return false;}
	virtual void Save(CFS_Base&){;}

	virtual bool GetBox( Fbox& box ){	return false; }
	virtual void OnSceneUpdate( ){;}
	virtual void OnDestroy( ){;}

	virtual void OnDeviceCreate	( ){;}
	virtual void OnDeviceDestroy( ){;}

	SceneObject(){
		m_ClassID = OBJCLASS_DUMMY;
		m_Name[0] = 0;
		m_Selected = false;
		m_Visible = true;
        m_Locked = false;
        m_Valid = false;
        }

	SceneObject(SceneObject* source){
		m_ClassID = source->m_ClassID;
		strcpy(m_Name,source->m_Name);
		m_Selected = false;
		m_Visible = true;
        m_Locked = false;
        m_Valid = false;
        }

	virtual ~SceneObject(){
	}
};

//----------------------------------------------------
#endif /* _INCDEF_SceneObject_H_ */
