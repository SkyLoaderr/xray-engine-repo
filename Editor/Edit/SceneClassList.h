//----------------------------------------------------
// file: SceneClassList.h
//----------------------------------------------------

#ifndef _INCDEF_SceneClassList_H_
#define _INCDEF_SceneClassList_H_
//----------------------------------------------------

class SceneObject;

#define OBJCLASS_NONE			-1
#define OBJCLASS_DUMMY			0
#define OBJCLASS_SOBJECT2		2
#define OBJCLASS_LIGHT			3
#define OBJCLASS_PCLIPPER		4
#define OBJCLASS_SOUND			5

//----------------------------------------------------
SceneObject *NewObjectFromClassID( int _ClassID );
//----------------------------------------------------
#endif /*_INCDEF_SceneClassList_H_*/


