#pragma once

template <typename T>
struct CWrapperBase : public T, public luabind::wrap_base {
	typedef T inherited;
	typedef CWrapperBase<T>	self_type;

	virtual bool OnKeyboard(int dik, EUIMessages keyboard_action)
	{ return call_member<bool>(this,"OnKeyboard", dik, keyboard_action);}
	static bool OnKeyboard_static(inherited* ptr, int dik, EUIMessages keyboard_action)
	{ return ptr->self_type::inherited::OnKeyboard(dik,keyboard_action );}

};

typedef CWrapperBase<UIDialogWndEx> WrapType;
typedef UIDialogWndEx BaseType;

template<typename T>
IC T*	UIDialogWndEx::GetControl(LPCSTR name){
	shared_str n = name;
	CUIWindow* pWnd = FindChild(n);
	if(pWnd == NULL)
		return NULL;

	return smart_cast<T*>(pWnd);
}

typedef luabind::class_<UIDialogWndEx, WrapType, luabind::bases<CUIDialogWnd,DLL_Pure> > export_class;
