#ifndef xrstringH
#define xrstringH
#pragma once

#pragma pack(push,4)
//////////////////////////////////////////////////////////////////////////
typedef const char*		str_c;

//////////////////////////////////////////////////////////////////////////
#pragma warning(disable : 4200)
struct		XRCORE_API	str_value
{
	u32					dwReference		;
	u32					dwLength		;
	char				value		[]	;
};
struct		XRCORE_API	str_value_cmp	// less
{
	IC bool		operator ()	(const str_value* A, const str_value* B) const	{ return xr_strcmp(A->value,B->value)<0;	};
};
#pragma warning(default : 4200)

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
	u32					stat_economy	();
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

	// misc func
	u32					size		()								{	if (0==p_) return 0; else return p_->dwLength;	}
	void				swap		(ref_str & rhs)					{	str_value* tmp = p_; p_ = rhs.p_; rhs.p_ = tmp;	}
	bool				equal		(const ref_str & rhs)	const	{	return (p_ == rhs.p_);							}
};

// res_ptr == res_ptr
// res_ptr != res_ptr
// const res_ptr == ptr
// const res_ptr != ptr
// ptr == const res_ptr
// ptr != const res_ptr
// res_ptr < res_ptr
// res_ptr > res_ptr
IC bool operator	==	(ref_str const & a, ref_str const & b)		{ return a._get() == b._get();					}
IC bool operator	!=	(ref_str const & a, ref_str const & b)		{ return a._get() != b._get();					}
IC bool operator	<	(ref_str const & a, ref_str const & b)		{ return a._get() <  b._get();					}
IC bool operator	>	(ref_str const & a, ref_str const & b)		{ return a._get() >  b._get();					}

// externally visible standart functionality
IC void swap			(ref_str & lhs, ref_str & rhs)				{ lhs.swap(rhs);		}
IC u32	xr_strlen		(ref_str & a)								{ return a.size();		}
IC u32	xr_strcmp		(const ref_str & a, const ref_str & b)		{ 
	if (a.equal(b))		return 0;
	else				return xr_strcmp(*a,*b);
}

#pragma pack(pop)

#endif
