#include "stdafx.h"
#include "DemoActor.h"
#include "..\CameraBase.h"

void CDemoActor::UpdateCamera()
{
	camera->Set	(Position(),mRotate.k,mRotate.j);
	g_pGameLevel->Cameras.Update	(camera);
}
