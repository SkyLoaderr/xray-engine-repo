#include "pch.h"
#pragma hdrstop
#include "visibilitytester.h"

EFC_Visible	CFrustumClipper::visibleSphere(Fvector &c, float r)
{
	float		cls1,cls2,cls3,cls4,cls5;
	// left
	cls1 = Planes[fcpLeft].classify(c);
	if (cls1>r) return fcvNone;

	// right
	cls2 = Planes[fcpRight].classify(c);
	if (cls2>r) return fcvNone;

	// bottom
	cls3 = Planes[fcpBottom].classify(c);
	if (cls3>r) return fcvNone;

	// top
	cls4 = Planes[fcpTop].classify(c);
	if (cls4>r) return fcvNone;

	// far
	cls5 = Planes[fcpFar].classify(c);
	if (cls5>r) return fcvNone;

	// now we have detected that sphere is visible
	// test for 'partial' visibility...
	if (fabsf(cls1)<r || fabsf(cls2)<r || fabsf(cls3)<r || fabsf(cls4)<r || fabsf(cls5)<r)
		return fcvPartial;
	else
		return fcvFully;
}
