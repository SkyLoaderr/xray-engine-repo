#include "UIDialogWnd.h"
#include "UIWndCallback.h"
//#include "UIMessageBox.h"

class CUIMessageBox;

class CUIMessageBoxEx : public CUIDialogWnd, public CUIWndCallback{
public:
	CUIMessageBoxEx();
	virtual ~CUIMessageBoxEx();
			void SetText(LPCSTR text);
	virtual void Init(LPCSTR xml_template);
	virtual void SendMessage(CUIWindow* pWnd, s16 msg, void* pData = NULL);

	LPCSTR		 GetHost			();
	LPCSTR		 GetPassword		();

protected:			
    CUIMessageBox*	m_pMessageBox;
};