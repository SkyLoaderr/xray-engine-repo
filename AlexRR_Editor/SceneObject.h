//----------------------------------------------------
// file: SceneObject.h
//----------------------------------------------------

#ifndef _INCDEF_SceneObject_H_
#define _INCDEF_SceneObject_H_

#include "FileSystem.h"
//----------------------------------------------------

#define MAX_OBJ_NAME       64
#define MAX_OBJCLS_NAME    64
#define MAX_CLASS_SCRIPT   4096
#define MAX_LINK_NAME      64
#define MAX_LTX_ADD        16384
#define MAX_ADD_FILES_TEXT 1024

//----------------------------------------------------

class SceneObject {
protected:

	int m_ClassID;
	char m_Name[MAX_OBJ_NAME];

	bool m_Selected;
	bool m_Visible;

public:

	__forceinline bool Visible(){
		return m_Visible; }
	__forceinline bool Selected(){
		return m_Selected; }
	__forceinline int ClassID(){
		return m_ClassID; }
	__forceinline char *GetName(){
		return m_Name; }

	virtual void Render( IMatrix& parent ){};
	virtual void RTL_Update( float dT ){};
	
	virtual bool RTL_Pick(
		float *distance,
		IVector& start,
		IVector& direction,
		IMatrix& parent )
			{ return false; };

	virtual bool BoxPick(
		ICullPlane *planes,
		IMatrix& parent )
			{ return false; };

	virtual bool QuadPick( int x, int z ){ return false; };
	virtual void Select( bool flag ){ m_Selected = flag; };
	virtual void Show( bool flag ){ m_Visible = flag; };

	virtual void Move( IVector& amount ){};
	virtual void Rotate( IVector& center, IVector& axis, float angle ){};
	virtual void Scale( IVector& center, IVector& amount ){};
	virtual void LocalRotate( IVector& axis, float angle ){};
	virtual void LocalScale( IVector& amount ){};

	virtual void Load( FSChunkDef *chunk ){};
	virtual void Save( int handle ){};

	virtual bool GetBox( IAABox& box ){
		return false; }

	SceneObject(){
		m_ClassID = 0;
		m_Name[0] = 0;
		m_Selected = false;
		m_Visible = true; }

	virtual ~SceneObject(){
	}
};


typedef list<SceneObject*> ObjectList;
typedef list<SceneObject*>::iterator ObjectIt;

//----------------------------------------------------
#endif /* _INCDEF_SceneObject_H_ */