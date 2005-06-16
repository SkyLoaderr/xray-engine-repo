// file:		UISpinNum.h
// description:	Spin Button with numerical data (unlike text data)
// created:		15.06.2005
// author:		Serge Vynnychenko
//

#include "UICustomSpin.h"

class CUISpinNum : public CUICustomSpin {
public:
	CUISpinNum();
	~CUISpinNum();

	virtual void	Init(float x, float y, float width, float height);

    virtual void	OnBtnUpClick();
	virtual void	OnBtnDownClick();

			void	SetMax(int max);
			void	SetMin(int min);
			void	SetLtxEntry(const char* entry);
			void	SetDefaultLtxValue();

protected:
			void	SetValue();


	int		m_iMax;
	int		m_iMin;
	int		m_iStep;
	int		m_iVal;
};