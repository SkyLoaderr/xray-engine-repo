////////////////////////////////////////////////////////////////////////////
//	Module 		: time_smart_ptr.h
//	Created 	: 30.07.2004
//  Modified 	: 30.07.2004
//	Author		: Dmitriy Iassenev
//	Description : Smart pointer with time template
////////////////////////////////////////////////////////////////////////////

#pragma once

#pragma pack(push,4)

template <typename object_type>
class CTimeSmartPtr {
private:
	typedef CTimeSmartPtr<object_type>	self_type;
	typedef object_type					object_type;

private:
	object_type					*m_object;
	u32							*m_ref_count;
	u32							*m_last_time_dec;

protected:
	IC		void				dec				();

public:
	IC							CTimeSmartPtr	();
	IC							CTimeSmartPtr	(object_type *rhs);
	IC							CTimeSmartPtr	(self_type const &rhs);
	IC							~CTimeSmartPtr	();
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

template <typename object_type>
IC			bool				operator==		(CTimeSmartPtr<object_type> const & a, CTimeSmartPtr<object_type> const & b);

template <typename object_type>
IC			bool				operator!=		(CTimeSmartPtr<object_type> const & a, CTimeSmartPtr<object_type> const & b);

template <typename object_type>
IC			bool				operator<		(CTimeSmartPtr<object_type> const & a, CTimeSmartPtr<object_type> const & b);

template <typename object_type>
IC			bool				operator>		(CTimeSmartPtr<object_type> const & a, CTimeSmartPtr<object_type> const & b);

template <typename object_type>
IC			void				swap			(CTimeSmartPtr<object_type> & lhs, CTimeSmartPtr<object_type> & rhs);

#include "time_smart_ptr_inline.h"

#pragma pack(pop)