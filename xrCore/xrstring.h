#pragma once

//////////////////////////////////////////////////////////////////////////
typedef const char*		str_c;

//////////////////////////////////////////////////////////////////////////
struct	str_value
{
	u32					dwReference;
	str_c				value;
};

//////////////////////////////////////////////////////////////////////////
class	str_container
{
private:
	xr_set<str_value>	container;
public:
	str_value*			dock		(str_c value)
	{
	}
};
extern	str_container	scontainer;

//////////////////////////////////////////////////////////////////////////
class	str_ref
{
private:
	str_value*			p_;
protected:
	// ref-counting
	void				_inc	()									{	if (0==p_) return;	p_->dwReference++;												}
	void				_dec	()									{	if (0==p_) return;	p_->dwReference--; if (0==p_->dwReference) p_=0;				}
public:
	void				_set	(str_c rhs) 						{	str_value* v = scontainer.dock(rhs); if (0!=v) v->dwReference++; _dec(); p_ = v;	}
	void				_set	(str_ref const &rhs)				{	str_value* v = rhs->p_; if (0!=v) v->dwReference++; _dec(); p_ = v;					}
	const str_value*	_get	()	const							{	return p_;																			}
public:
	// construction
	str_ref				()											{	p_ = 0;											}
	str_ref				(str_c rhs) 								{	_set(rhs);										}
	str_ref				(str_ref const &rhs)						{	_set(rhs);										}
	~str_ref			()											{	_dec();											}

	// assignment & accessors
	str_ref&			operator=	(str_c rhs)						{	_set(rhs);	return (self&)*this;				}
	str_ref&			operator=	(str_ref rhs)					{	_set(rhs);	return (self&)*this;				}
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
template<class T, class U, typename D>	inline bool operator	==	(str_ref const & a, str_ref const & b)		{ return a._get() == b._get();						}
template<class T, class U, typename D>	inline bool operator	!=	(str_ref const & a, str_ref const & b)		{ return a._get() != b._get();						}
template<class T, typename D>			inline bool operator	<	(str_ref const & a, str_ref const & b)		{ return std::less<T *>()(a._get(), b._get());		}
template<class T, typename D>			inline bool operator	>	(str_ref const & a, str_ref const & b)		{ return std::greater<T *>()(a._get(), b._get());	}

// externally visible swap
template<class T, typename D> void swap	(str_ref & lhs, str_ref & rhs)									{ lhs.swap(rhs);	}

// mem_fn support
template<class T, typename D> T * get_pointer(str_ref const & p)													{ return p.get();	}

// casting
template<class T, class U, typename D> str_ref static_pointer_cast(str_ref const & p)						{ return static_cast<T *>(p.get());				}
template<class T, class U, typename D> str_ref dynamic_pointer_cast(str_ref const & p)					{ return dynamic_cast<T *>(p.get());			}
