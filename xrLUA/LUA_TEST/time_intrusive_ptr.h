////////////////////////////////////////////////////////////////////////////
//	Module 		: intrusive_ptr.h
//	Created 	: 30.07.2004
//  Modified 	: 30.07.2004
//	Author		: Dmitriy Iassenev
//	Description : Intrusive pointer template
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "object_type_traits.h"

#pragma pack(push,4)

struct CIntrusiveBase {
	u32		m_ref_count;

	IC			CIntrusiveBase	() : m_ref_count(0)
	{
	}

	template <typename T>
	IC	void	_release		(T*object)
	{
		xr_delete	(object);
	}
};

struct CTimeIntrusiveBase : public CIntrusiveBase {
	u32		m_last_time_dec;

	IC			CTimeIntrusiveBase	() : m_last_time_dec(0)
	{
	}

	template <typename T>
	IC	void	_release		(T*object)
	{
		m_last_time_dec = timeGetTime();
		xr_delete	(object);
	}
};

template <typename object_type, typename base_type = CIntrusiveBase>
class CIntrusivePtr {
private:
	typedef base_type					base_type;
	typedef CIntrusivePtr<object_type,base_type>	self_type;
	typedef object_type					object_type;

	enum {
		result = object_type_traits::is_base_and_derived<base_type,object_type>::value,
	};

private:
	object_type					*m_object;

protected:
	IC		void				dec				();

public:
	IC							CIntrusivePtr	();
	IC							CIntrusivePtr	(object_type *rhs);
	IC							CIntrusivePtr	(self_type const &rhs);
	IC							~CIntrusivePtr	();
	IC		self_type			&operator=		(object_type* rhs);
	IC		self_type			&operator=		(self_type const &rhs);
	IC		object_type			&operator*		() const;
	IC		bool				operator!		() const;
	IC		u32					size			();
	IC		void				swap			(self_type &rhs);
	IC		bool				equal			(const self_type & rhs)	const;
	IC		void				set				(object_type* rhs);
	IC		void				set				(self_type const &rhs);
	IC		const object_type	*get			()	const;
};

template <typename object_type, typename base_type>
IC			bool				operator==		(CIntrusivePtr<object_type,base_type> const & a, CIntrusivePtr<object_type,base_type> const & b);

template <typename object_type, typename base_type>
IC			bool				operator!=		(CIntrusivePtr<object_type,base_type> const & a, CIntrusivePtr<object_type,base_type> const & b);

template <typename object_type, typename base_type>
IC			bool				operator<		(CIntrusivePtr<object_type,base_type> const & a, CIntrusivePtr<object_type,base_type> const & b);

template <typename object_type, typename base_type>
IC			bool				operator>		(CIntrusivePtr<object_type,base_type> const & a, CIntrusivePtr<object_type,base_type> const & b);

template <typename object_type, typename base_type>
IC			void				swap			(CIntrusivePtr<object_type,base_type> & lhs, CIntrusivePtr<object_type,base_type> & rhs);


#include "time_intrusive_ptr_inline.h"

#pragma pack(pop)