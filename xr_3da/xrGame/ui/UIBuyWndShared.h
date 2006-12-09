#pragma once

//.#define _new_buy_wnd

#ifdef _new_buy_wnd

	#include	"UIMpTradeWnd.h"
	typedef		CUIMpTradeWnd			BUY_WND;

#else

	#include	"UIBuyWnd.h"
	typedef		CUIBuyWnd				BUY_WND;

#endif


