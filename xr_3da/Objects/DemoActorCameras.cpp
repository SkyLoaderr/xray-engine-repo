#include "stdafx.h"
#include "DemoActor.h"
#include "..\CameraBase.h"

void CDemoActor::UpdateCamera()
{
	camera->Set	(vPosition,mRotate.k,mRotate.j);
	pCreator->Cameras.Update	(camera);
}
