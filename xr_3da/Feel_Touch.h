#pragma once

class ENGINE_API CObject;

namespace Feel
{
	class CTouch
	{
	public:
		vector<CObject*>		feel_touch;

		virtual void			feel_touch_process			(Fvector& P, float R);
		virtual void			feel_touch_new				(CObject* O)			{};
		virtual void			feel_touch_delete			(CObject* O)			{};
	};
};
