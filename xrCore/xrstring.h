#pragma once
#pragma pack(push,4)

//////////////////////////////////////////////////////////////////////////
typedef const char*		str_c;

//////////////////////////////////////////////////////////////////////////
struct		XRCORE_API	str_value
{
	u32					dwReference		;
	char				value		[]	;
};
struct		XRCORE_API	str_value_cmp
{
	IC bool		operator ()	(const str_value* A, const str_value* B) const	{ return strcmp(A->value,B->value)<0;	};
};

//////////////////////////////////////////////////////////////////////////
class		XRCORE_API	str_container
{
private:
	typedef xr_set<str_value*,str_value_cmp>	cdb;
	xrCriticalSection							cs;
	cdb											container;
public:
	str_value*			dock			(str_c value);
	void				clean			();
						~str_container	();
};
XRCORE_API	extern		str_container*	g_pStringContainer;

//////////////////////////////////////////////////////////////////////////
class		XRCORE_API	str_ref
{
private:
	str_value*			p_;
protected:
	// ref-counting
	void				_inc		()								{	if (0==p_) return;	p_->dwReference++;														}
	void				_dec		()								{	if (0==p_) return;	p_->dwReference--; 														}
public:
	void				_set		(str_c rhs) 					{	str_value* v = g_pStringContainer->dock(rhs); if (0!=v) v->dwReference++; _dec(); p_ = v;	}
	void				_set		(str_ref const &rhs)			{	str_value* v = rhs.p_; if (0!=v) v->dwReference++; _dec(); p_ = v;							}
	const str_value*	_get		()	const						{	return p_;																					}
public:
	// construction
						str_ref		()								{	p_ = 0;											}
						str_ref		(str_c rhs) 					{	_set(rhs);										}
						str_ref		(str_ref const &rhs)			{	_set(rhs);										}
						~str_ref	()								{	_dec();											}

	// assignment & accessors
	str_ref&			operator=	(str_c rhs)						{	_set(rhs);	return (str_ref&)*this;				}
	str_ref&			operator=	(str_ref rhs)					{	_set(rhs);	return (str_ref&)*this;				}
	str_c				operator*	() const						{	return p_->value;								}
	bool				operator!	() const						{	return p_ == 0;									}

	// fast swapping
	void				swap		(str_ref & rhs)					{	str_value* tmp = p_; p_ = rhs.p_; rhs.p_ = tmp;	}
};

// res_ptr == res_ptr
// res_ptr != res_ptr
// const res_ptr == ptr
// const res_ptr != ptr
// ptr == const res_ptr
// ptr != const res_ptr
// res_ptr < res_ptr
// res_ptr > res_ptr
inline bool operator	==	(str_ref const & a, str_ref const & b)		{ return a._get() == b._get();					}
inline bool operator	!=	(str_ref const & a, str_ref const & b)		{ return a._get() != b._get();					}
inline bool operator	<	(str_ref const & a, str_ref const & b)		{ return a._get() <  b._get();					}
inline bool operator	>	(str_ref const & a, str_ref const & b)		{ return a._get() >  b._get();					}

// externally visible swap
inline void swap			(str_ref & lhs, str_ref & rhs)				{ lhs.swap(rhs);	}

#pragma pack(pop)
