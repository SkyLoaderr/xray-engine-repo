#pragma once

// resource itself, the base class for all derived resources
class	xr_resorce	{
public:
	u32				dwReference;

	xr_resorce()	: dwReference(0) { }
};

// templated pointer to resource - the hard part
// usually used as base class for specialized resource pointers
// *** note ***:	type must implement "T::_release_(T*)"
template <class T>
class xr_resorce_ptr
{
private:
	typedef xr_resorce_ptr	this_type;
	T *						p_;
private:
						// ref-counting
	void				_inc			()		{ if (0==p_) return;	p_->dwReference++;														}
	void				_dec			()		{ if (0==p_) return;	p_->dwReference--; if (0==p_->dwReference) { p_->_release_(p_); p_=0; }	}
public:
						// construction
						xr_resorce_ptr	(): p_(0)										{}
						xr_resorce_ptr	(T * p, bool add_ref = true): p_(p)				{	if(add_ref)	_inc(); }
	template<class U>	xr_resorce_ptr	(xr_resorce_ptr<U> const & rhs) : p_(rhs.get())	{	_inc();				}
						xr_resorce_ptr	(xr_resorce_ptr const & rhs): p_(rhs.p_)		{	_inc();				}
						~xr_resorce_ptr	()												{	_dec();				}

						// assignment
	template<class U>	xr_resorce_ptr & operator=(xr_resorce_ptr<U> const & rhs)		{	this_type(rhs).swap(*this);	return *this;	}
						xr_resorce_ptr & operator=(xr_resorce_ptr const & rhs)			{	this_type(rhs).swap(*this);	return *this;	}
						xr_resorce_ptr & operator=(T * rhs)								{	this_type(rhs).swap(*this);	return *this;	}

						// accessors
	T *					get() const														{	return p_;	}
	T &					operator*() const												{	return *p_;	}
	T *					operator->() const												{	return p_;	}

						// unspecified bool type
						typedef T * (xr_resorce_ptr::*unspecified_bool_type) () const;
						operator unspecified_bool_type () const							{	return p_ == 0? 0: &xr_resorce_ptr::get;	}
						bool operator!	() const										{	return p_ == 0;	}

						// fast swapping
	void				swap			(xr_resorce_ptr & rhs)							{	T * tmp = p_; p_ = rhs.p_; rhs.p_ = tmp;	}
};

// res_ptr == res_ptr
// res_ptr != res_ptr
// const res_ptr == ptr
// const res_ptr != ptr
// ptr == const res_ptr
// ptr != const res_ptr
// res_ptr < res_ptr
template<class T, class U> inline bool operator==(xr_resorce_ptr<T> const & a, xr_resorce_ptr<U> const & b)		{ return a.get() == b.get();					}
template<class T, class U> inline bool operator!=(xr_resorce_ptr<T> const & a, xr_resorce_ptr<U> const & b)		{ return a.get() != b.get();					}
template<class T> inline bool operator==(xr_resorce_ptr<T> const & a, T * b)									{ return a.get() == b;							}
template<class T> inline bool operator!=(xr_resorce_ptr<T> const & a, T * b)									{ return a.get() != b;							}
template<class T> inline bool operator==(T * a, xr_resorce_ptr<T> const & b)									{ return a == b.get();							}
template<class T> inline bool operator!=(T * a, xr_resorce_ptr<T> const & b)									{ return a != b.get();							}
template<class T> inline bool operator<(xr_resorce_ptr<T> const & a, xr_resorce_ptr<T> const & b)				{ return std::less<T *>()(a.get(), b.get());	}

// externally visible swap
template<class T> void swap	(xr_resorce_ptr<T> & lhs, xr_resorce_ptr<T> & rhs)									{ lhs.swap(rhs);	}

// mem_fn support
template<class T> T * get_pointer(xr_resorce_ptr<T> const & p)													{ return p.get();	}

// casting
template<class T, class U> xr_resorce_ptr<T> static_pointer_cast(xr_resorce_ptr<U> const & p)					{ return static_cast<T *>(p.get());				}
template<class T, class U> xr_resorce_ptr<T> dynamic_pointer_cast(xr_resorce_ptr<U> const & p)					{ return dynamic_cast<T *>(p.get());			}
