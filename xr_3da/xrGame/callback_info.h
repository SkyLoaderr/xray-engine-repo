#pragma once

#include "../script_callback_ex.h"
#include <boost/function.hpp>
#include <boost/bind.hpp>


struct SCallbackInfo{
	CScriptCallbackEx<void>	m_callback;
	boost::function<void()>	m_cpp_callback;
	shared_str				m_controlName;
	s16						m_event;
	SCallbackInfo():m_controlName(""),m_event(-1){};
};

