#ifndef _PURE_H_AAA_
#define _PURE_H_AAA_

// messages
#define REG_PRIORITY_LOW		0x11111111ul
#define REG_PRIORITY_NORMAL		0x22222222ul
#define REG_PRIORITY_HIGH		0x33333333ul
#define REG_PRIORITY_CAPTURE	0x7ffffffful
#define MAX_REGISTERED_OBJECTS	256

typedef void __fastcall RP_FUNC	(void *obj);
#define DECLARE_MESSAGE(name)	extern ENGINE_API RP_FUNC rp_##name; class ENGINE_API pure##name { public: virtual void  On##name(void)=0; }

DECLARE_MESSAGE(Frame);
DECLARE_MESSAGE(Render);
DECLARE_MESSAGE(AppActivate);
DECLARE_MESSAGE(AppDeactivate);
DECLARE_MESSAGE(AppCycleStart);
DECLARE_MESSAGE(AppCycleEnd);

//-----------------------------------------------------------------------------
struct _REG_INFO {
	void*	Object;
	int		Prio;
	u32		Flags;
};

ENGINE_API extern int	__cdecl	_REG_Compare(const void *, const void *);

template <class T> class CRegistrator		// the registrator itself
{
	friend ENGINE_API int	__cdecl	_REG_Compare(const void *, const void *);
public:
	_REG_INFO	R[MAX_REGISTERED_OBJECTS];
	int			Count;

	// constructor
	CRegistrator()
	{ Count = 0; }

	//
	void Add	(T *obj, int priority=REG_PRIORITY_NORMAL, u32 flags=0)
	{
#ifdef DEBUG
		VERIFY	(obj);
		for		(int i=0; i<Count; i++) VERIFY(R[i].Object!=(void*)obj);
#endif
		R[Count].Object	=obj;
		R[Count].Prio	=priority;
		R[Count].Flags	=flags;
		Count++;
		R_ASSERT(Count<MAX_REGISTERED_OBJECTS);
		Resort	( );
	};
	void Remove	(T *obj)
	{
		for (int i=0; i<Count; i++) {
			if (R[i].Object==obj) R[i].Prio = -1;
		}
		Resort	();
	};
	void Process(RP_FUNC *f)
	{
		if (R[0].Prio==REG_PRIORITY_CAPTURE)	f(R[0].Object);
		else {
			for (int i=0; i<Count; i++)	{
				f(R[i].Object);
			}
		}
	};
	void Resort	(void)
	{
		qsort(R,Count,sizeof(_REG_INFO),_REG_Compare);
		while ((Count>0) && (R[Count-1].Prio<0)) Count--;
	};
};

#endif
