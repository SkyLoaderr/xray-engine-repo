//----------------------------------------------------
// file: SceneClassList.h
//----------------------------------------------------

#ifndef _INCDEF_SceneClassList_H_
#define _INCDEF_SceneClassList_H_
//----------------------------------------------------


class SceneObject;

class SLandscape;
class SLandscapeEditor;

class SObject2;
class SObject2Editor;

class Light;
class LightEditor;

class SndPlane;
class SndPlaneEditor;

class CSound;
class CSoundEditor;

//----------------------------------------------------


#define OBJCLASS_DUMMY			0
#define OBJCLASS_LANDSCAPE		1
#define OBJCLASS_SOBJECT2		2
#define OBJCLASS_LIGHT			3
#define OBJCLASS_SNDPLANE		4
#define OBJCLASS_SOUND			5

//----------------------------------------------------


SceneObject *NewObjectFromClassID( int _ClassID );


//----------------------------------------------------


extern SLandscapeEditor E_Landscape;
extern SObject2Editor E_Object2;
extern LightEditor E_Light;
extern SndPlaneEditor E_SndPlane;
extern CSoundEditor E_Sound;


//----------------------------------------------------
#endif /*_INCDEF_SceneClassList_H_*/


