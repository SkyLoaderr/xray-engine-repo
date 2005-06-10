#pragma once

struct SCallbackInfo;

namespace boost {
	template<typename Signature, typename Allocator>
	class function;
};
class CUIWindow;

class CUIWndCallback
{
	typedef boost::function<void(CUIWindow*,void*),std::allocator<void> > void_function;
	typedef xr_vector<SCallbackInfo*>	CALLBACKS;
	typedef CALLBACKS::iterator			CALLBACK_IT;
private:
			CALLBACKS			m_callbacks;
			SCallbackInfo*		NewCallback			();


public:
	virtual void				OnEvent				(CUIWindow* pWnd, s16 msg, void* pData = NULL);
			void				Register			(CUIWindow* pChild);
			void				AddCallback			(LPCSTR control_id, s16 event, const void_function &f);
};