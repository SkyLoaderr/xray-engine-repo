// UIArtifactMergerWnd.h:  ������� ��� ������ � ��������� 
// ������������ ����� ����������
//////////////////////////////////////////////////////////////////////


#include "uiframewindow.h"
#include "uidragdroplist.h"
#include "uibutton.h"
#include "..\\ArtifactMerger.h"

class CUIArtifactMerger: public CUIFrameWindow
{
private:
	typedef CUIFrameWindow inherited;
public:
	CUIArtifactMerger();
	virtual ~CUIArtifactMerger();
	
	
	virtual void Init(int x, int y, int width, int height);

	typedef enum{CLOSE_BUTTON_CLICKED,
				 PERFORM_BUTTON_CLICKED} E_MESSAGE;
	virtual void SendMessage(CUIWindow *pWnd, s16 msg, void *pData);

	void InitArtifactMerger(CArtifactMerger* pArtifactMerger);
	void PerformArtifactMerger();

	void Show();
	void Hide();

	virtual void Update();


	//����� ��� ���������� � �������
	CUIDragDropList		UIArtifactList;
protected:
	CUIButton UIPerformButton;
	CUIButton UICloseButton;

	static bool ArtifactProc(CUIDragDropItem* pItem, CUIDragDropList* pList);

	//��������� �� ���������� ��������� ����������
	CArtifactMerger* m_pArtifactMerger;
};