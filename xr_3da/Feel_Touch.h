#pragma once

#include "pure_relcase.h"

class ENGINE_API CObject;
namespace Feel
{
	class ENGINE_API Touch: private pure_relcase
	{
		struct DenyTouch
		{
			CObject*	O;
			DWORD		Expire;
		};
		xr_vector<DenyTouch>	feel_touch_disable;
		void __stdcall			feel_touch_relcase			(CObject* O);
	public:
								Touch						();
		virtual					~Touch						();
		xr_vector<CObject*>		feel_touch;

		virtual BOOL			feel_touch_contact			(CObject* O);
		virtual void			feel_touch_update			(Fvector& P, float	R);
		virtual void			feel_touch_deny				(CObject* O, DWORD	T);
		virtual void			feel_touch_new				(CObject* O)			{	};
		virtual void			feel_touch_delete			(CObject* O)			{	};
	};
};
