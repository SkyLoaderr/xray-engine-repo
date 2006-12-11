#pragma once

//#define _new_buy_wnd

#include	"UIMpTradeWnd.h"
#include	"UIBuyWnd.h"

#ifdef _new_buy_wnd
	typedef		CUIMpTradeWnd			BUY_WND_TYPE;
#else
	typedef		CUIBuyWnd				BUY_WND_TYPE;
#endif
