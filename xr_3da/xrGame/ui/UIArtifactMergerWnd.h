// UIArtefactMergerWnd.h:  ������� ��� ������ � ��������� 
// ������������ ����� ����������
//////////////////////////////////////////////////////////////////////

#pragma once

#include "uiframewindow.h"
#include "uidragdroplist.h"
#include "uibutton.h"
#include "../ArtifactMerger.h"

class CUIArtefactMerger: public CUIFrameWindow
{
private:
	typedef CUIFrameWindow inherited;
public:
	CUIArtefactMerger();
	virtual ~CUIArtefactMerger();
	
	
	virtual void Init(int x, int y, int width, int height);

//	typedef enum{CLOSE_BUTTON_CLICKED,
//				 PERFORM_BUTTON_CLICKED} E_MESSAGE;
	virtual void SendMessage(CUIWindow *pWnd, s16 msg, void *pData);

	void InitArtefactMerger(CArtefactMerger* pArtefactMerger);
	void PerformArtefactMerger();

	void Show();
	void Hide();

	virtual void Update();


	//����� ��� ���������� � �������
	CUIDragDropList		UIArtefactList;
protected:
	CUIButton UIPerformButton;
	CUIButton UICloseButton;

	static bool ArtefactProc(CUIDragDropItem* pItem, CUIDragDropList* pList);

	//��������� �� ���������� ��������� ����������
	CArtefactMerger* m_pArtefactMerger;
};