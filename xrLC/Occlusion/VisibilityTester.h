#ifndef _VIS_TEST_
#define _VIS_TEST_
#pragma once

#define MAX_VIS_OBJECTS		4096
class ENGINE_API CVisibilityTest {
public:
	void*			Object	[MAX_VIS_OBJECTS];
	Fvector			Center 	[MAX_VIS_OBJECTS];
	float			Radius	[MAX_VIS_OBJECTS];
	DWORD			Result	[MAX_VIS_OBJECTS];
	DWORD			Count;

	__forceinline void Init(void) {
		Count=0;
	}
	__forceinline void Add(void *obj, Fvector &c, float r) {
		Object [Count] = obj;
		Center [Count].set(c);
		Radius [Count] = r;
		Count++;
		VERIFY(Count<MAX_VIS_OBJECTS);
	}
	__forceinline bool Test(DWORD num) {
	    if ((Result[num]&D3DSTATUS_CLIPINTERSECTIONALL) == 0)	return true;
		else													return false;
}

	void Calculate(void);
};

enum EFC_Planes {
	fcpLeft	= 0,
	fcpRight,
	fcpBottom,
	fcpTop,
//	fcpNear,
	fcpFar,
	fcp_last,
	fcp_forcedword = DWORD(-1)
};
enum EFC_Visible {
	fcvNone = 0,
	fcvPartial,
	fcvFully,
	fcv_forcedword = DWORD(-1)
};

// planes are directed outside
class ENGINE_API CFrustumClipper {
	Fplane			Planes[fcp_last];
public:
	void			BuildFrustum	(void);

	EFC_Visible		isSphereVisible(Fvector &c, float r)
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
};

#endif
