#ifndef _PURE_H_AAA_
#define _PURE_H_AAA_

// messages
#define REG_PRIORITY_LOW		0x11111111ul
#define REG_PRIORITY_NORMAL		0x22222222ul
#define REG_PRIORITY_HIGH		0x33333333ul
#define REG_PRIORITY_CAPTURE	0x7ffffffful

typedef void __fastcall RP_FUNC		(void *obj);
#define DECLARE_MESSAGE(name)		extern ENGINE_API RP_FUNC rp_##name; class ENGINE_API pure##name { public: virtual void  On##name(void)=0;	}
#define DECLARE_RP(name) void __fastcall rp_##name(void *p) { ((pure##name *)p)->On##name(); }

DECLARE_MESSAGE(Frame);
DECLARE_MESSAGE(Render);
DECLARE_MESSAGE(AppActivate);
DECLARE_MESSAGE(AppDeactivate);
DECLARE_MESSAGE(AppStart);
DECLARE_MESSAGE(AppEnd);


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
	xr_vector<_REG_INFO>	R;

	// constructor
	CRegistrator()			{ }

	//
	void Add	(T *obj, int priority=REG_PRIORITY_NORMAL, u32 flags=0)
	{
#ifdef DEBUG
		VERIFY	(obj);
		for		(u32 i=0; i<R.size(); i++) VERIFY(R[i].Object!=(void*)obj);
#endif
		_REG_INFO			I;
		I.Object			=obj;
		I.Prio				=priority;
		I.Flags				=flags;
		R.push_back			(I);
		Resort				( );
	};
	void Remove	(T *obj)
	{
		for (u32 i=0; i<R.size(); i++) {
			if (R[i].Object==obj) R[i].Prio = -1;
		}
		Resort	();
	};
	void Process(RP_FUNC *f)
	{
    	if (R.empty()) return;
		if (R[0].Prio==REG_PRIORITY_CAPTURE)	f(R[0].Object);
		else {
			for (u32 i=0; i<R.size(); i++)	{
				f(R[i].Object);
			}
		}
	};
	void Resort	(void)
	{
		qsort	(&*R.begin(),R.size(),sizeof(_REG_INFO),_REG_Compare);
		while	((R.size()) && (R[R.size()-1].Prio<0)) R.pop_back();
	};
};

#endif
