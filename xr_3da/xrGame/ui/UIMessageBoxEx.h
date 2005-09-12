#include "UIDialogWnd.h"
//#include "UIMessageBox.h"

class CUIMessageBox;

class CUIMessageBoxEx : public CUIDialogWnd{
public:
	CUIMessageBoxEx();
	virtual ~CUIMessageBoxEx();
	virtual void Init(LPCSTR xml_template);
	virtual void SendMessage(CUIWindow* pWnd, s16 msg, void* pData = NULL);
protected:
    CUIMessageBox*	m_pMessageBox;
};