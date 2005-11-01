#pragma once

#include "UIScrollView.h"
#include "UIStatsPlayerList.h"

class CUIXml;

class CUIStats : public CUIScrollView {
public:
	using CUIScrollView::Init;

	CUIStats();
	virtual ~CUIStats();

			void Init(CUIXml& xml_doc, int team = 0);
};