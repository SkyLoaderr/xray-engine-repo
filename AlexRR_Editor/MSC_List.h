//----------------------------------------------------
// file: MSC_List.h
//----------------------------------------------------

#ifndef _INCDEF_MSC_List_H_
#define _INCDEF_MSC_List_H_

//----------------------------------------------------

class MouseCallback;

class MouseCallback_AddLandscape;
class MouseCallback_SelLandscape;
class MouseCallback_MoveLandscape;
class MouseCallback_RotateLandscape;
class MouseCallback_ScaleLandscape;
class MouseCallback_AddObject;
class MouseCallback_SelObject;
class MouseCallback_MoveObject;
class MouseCallback_RotateObject;
class MouseCallback_ScaleObject;
class MouseCallback_MovePivot;
class MouseCallback_AddLight;
class MouseCallback_SelLight;
class MouseCallback_MoveLight;
class MouseCallback_RotateLight;
class MouseCallback_AddSndPlane;
class MouseCallback_SelSndPlane;
class MouseCallback_MoveSndPlane;
class MouseCallback_RotateSndPlane;
class MouseCallback_ScaleSndPlane;
class MouseCallback_AddSound;
class MouseCallback_SelSound;
class MouseCallback_MoveSound;

//----------------------------------------------------

extern MouseCallback_AddLandscape MSC_AddLandscape;
extern MouseCallback_SelLandscape MSC_SelLandscape;
extern MouseCallback_MoveLandscape MSC_MoveLandscape;
extern MouseCallback_RotateLandscape MSC_RotateLandscape;
extern MouseCallback_ScaleLandscape MSC_ScaleLandscape;
extern MouseCallback_AddObject MSC_AddObject;
extern MouseCallback_SelObject MSC_SelObject;
extern MouseCallback_MoveObject MSC_MoveObject;
extern MouseCallback_RotateObject MSC_RotateObject;
extern MouseCallback_ScaleObject MSC_ScaleObject;
extern MouseCallback_MovePivot MSC_MovePivot;
extern MouseCallback_AddLight MSC_AddLight;
extern MouseCallback_SelLight MSC_SelLight;
extern MouseCallback_MoveLight MSC_MoveLight;
extern MouseCallback_RotateLight MSC_RotateLight;
extern MouseCallback_AddSound MSC_AddSound;
extern MouseCallback_SelSound MSC_SelSound;
extern MouseCallback_MoveSound MSC_MoveSound;
extern MouseCallback_AddSndPlane MSC_AddSndPlane;
extern MouseCallback_SelSndPlane MSC_SelSndPlane;
extern MouseCallback_MoveSndPlane MSC_MoveSndPlane;
extern MouseCallback_RotateSndPlane MSC_RotateSndPlane;
extern MouseCallback_ScaleSndPlane MSC_ScaleSndPlane;

//----------------------------------------------------

void MSC_Init();
void MSC_Uninit();
MouseCallback *MSC_Select( int target, int action );

//----------------------------------------------------

#endif /*_INCDEF_MSC_List_H_*/



