//----------------------------------------------------
// file: SceneObject.h
//----------------------------------------------------
#ifndef _INCDEF_SceneObject_H_
#define _INCDEF_SceneObject_H_

//----------------------------------------------------
struct SPickInfo;
struct FSChunkDef;

class SceneObject {
protected:
	int m_ClassID;
	char m_Name[MAX_OBJ_NAME];

	bool m_Selected;
	bool m_Visible;
public:
	__inline bool Visible(){return m_Visible; }
	__inline bool Selected(){return m_Selected; }
	__inline int ClassID(){return m_ClassID; }
	__inline char *GetName(){return m_Name; }

	virtual void Render( Fmatrix& parent ){};
	virtual void RTL_Update( float dT ){ };

	virtual bool RTL_Pick(float *distance,Fvector& start,Fvector& direction,
		                Fmatrix& parent, SPickInfo* pinf = NULL ){ return false; };

	virtual bool BoxPick(ICullPlane *planes,Fmatrix& parent ){ return false; };

	virtual bool QuadPick( int x, int z ){ return false; };
	virtual void Select( bool flag );
	virtual void Show( bool flag );

	virtual void Move( Fvector& amount ){};
	virtual void Rotate( Fvector& center, Fvector& axis, float angle ){};
	virtual void Scale( Fvector& center, Fvector& amount ){};
	virtual void LocalRotate( Fvector& axis, float angle ){};
	virtual void LocalScale( Fvector& amount ){};

	virtual void Load( FSChunkDef *chunk ){};
	virtual void Save( int handle ){};

	virtual bool GetBox( IAABox& box ){	return false; }

	SceneObject(){
		m_ClassID = 0;
		m_Name[0] = 0;
		m_Selected = false;
		m_Visible = true; }

	SceneObject(SceneObject* source){
		m_ClassID = source->m_ClassID;
		strcpy(m_Name,source->m_Name);
		m_Selected = false;
		m_Visible = true;}

	virtual ~SceneObject(){
	}
};

typedef list<SceneObject*> ObjectList;
typedef ObjectList::iterator ObjectIt;

//----------------------------------------------------
#endif /* _INCDEF_SceneObject_H_ */