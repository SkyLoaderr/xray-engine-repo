#pragma once

class ENGINE_API CObject;

namespace Feel
{
	class ENGINE_API Touch
	{
	public:
		vector<CObject*>		feel_touch;

		virtual void			feel_touch_update			(Fvector& P, float R);
		virtual void			feel_touch_new				(CObject* O)			{};
		virtual void			feel_touch_delete			(CObject* O)			{};
	};
};
