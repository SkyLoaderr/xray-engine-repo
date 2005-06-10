#pragma once

#include "../script_callback_ex.h"
#include <boost/function.hpp>
#include <boost/bind.hpp>

class CUIWindow;
struct SCallbackInfo{
	CScriptCallbackEx<void>	m_callback;
	boost::function<void(CUIWindow*,void*)>	m_cpp_callback;
	shared_str				m_controlName;
	s16						m_event;
	SCallbackInfo():m_controlName(""),m_event(-1){};
};

