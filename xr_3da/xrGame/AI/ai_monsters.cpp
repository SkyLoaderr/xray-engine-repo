////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_monsters.cpp
//	Created 	: 24.04.2002
//  Modified 	: 24.04.2002
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for all the monsters
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_monsters.h"

IC void vfNormalizeSafe(Fvector& Vector)
{
	float fMagnitude = (float)sqrt(SQR(Vector.x) + SQR(Vector.y) + SQR(Vector.z));
	if (fMagnitude > MAGNITUDE_EPSILON) {
		Vector.x /= fMagnitude;
		Vector.y /= fMagnitude;
		Vector.z /= fMagnitude;
	}
	else {
		Vector.x = 1.f;
		Vector.y = 0.f;
		Vector.z = 0.f;
	}
}

