#ifndef xrstringH
#define xrstringH
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
	void				dump			();
						~str_container	();
};
XRCORE_API	extern		str_container*	g_pStringContainer;

//////////////////////////////////////////////////////////////////////////
class		XRCORE_API	ref_str
{
private:
	str_value*			p_;
protected:
	// ref-counting
	void				_inc		()								{	if (0==p_) return;	p_->dwReference++;														}
	void				_dec		()								{	if (0==p_) return;	p_->dwReference--; 	if (0==p_->dwReference)	p_=0;						}
public:
	void				_set		(str_c rhs) 					{	str_value* v = g_pStringContainer->dock(rhs); if (0!=v) v->dwReference++; _dec(); p_ = v;	}
	void				_set		(ref_str const &rhs)			{	str_value* v = rhs.p_; if (0!=v) v->dwReference++; _dec(); p_ = v;							}
	const str_value*	_get		()	const						{	return p_;																					}
public:
	// construction
						ref_str		()								{	p_ = 0;											}
						ref_str		(str_c rhs) 					{	p_ = 0;	_set(rhs);								}
						ref_str		(ref_str const &rhs)			{	p_ = 0;	_set(rhs);								}
						~ref_str	()								{	_dec();											}

	// assignment & accessors
	ref_str&			operator=	(str_c rhs)						{	_set(rhs);	return (ref_str&)*this;				}
	ref_str&			operator=	(ref_str const &rhs)			{	_set(rhs);	return (ref_str&)*this;				}
	str_c				operator*	() const						{	return p_?p_->value:0;							}
	bool				operator!	() const						{	return p_ == 0;									}
	char				operator[]	(size_t id)						{	return p_->value[id];							}

	// fast swapping
	void				swap		(ref_str & rhs)					{	str_value* tmp = p_; p_ = rhs.p_; rhs.p_ = tmp;	}
	bool				equal		(ref_str & rhs)					
	{
		if (p_ == rhs.p_)	return true;
		return	0==strcmp	(p_->value,rhs.p_->value);
	}
};

// res_ptr == res_ptr
// res_ptr != res_ptr
// const res_ptr == ptr
// const res_ptr != ptr
// ptr == const res_ptr
// ptr != const res_ptr
// res_ptr < res_ptr
// res_ptr > res_ptr
inline bool operator	==	(ref_str const & a, ref_str const & b)		{ return a._get() == b._get();					}
inline bool operator	!=	(ref_str const & a, ref_str const & b)		{ return a._get() != b._get();					}
inline bool operator	<	(ref_str const & a, ref_str const & b)		{ return a._get() <  b._get();					}
inline bool operator	>	(ref_str const & a, ref_str const & b)		{ return a._get() >  b._get();					}

// externally visible swap
inline void swap			(ref_str & lhs, ref_str & rhs)				{ lhs.swap(rhs);	}
#pragma pack(pop)

#endif
