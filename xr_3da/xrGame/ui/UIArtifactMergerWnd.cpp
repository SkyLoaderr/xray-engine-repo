// UIArtifactMergerWnd.cpp:  подменю для работы с аппаратом 
// производства новых артефактов
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "UIArtifactMergerWnd.h"

#include "..\\Artifact.h"

CUIArtifactMerger::CUIArtifactMerger()
{
}
CUIArtifactMerger::~CUIArtifactMerger()
{
}
void CUIArtifactMerger::Init(int x, int y, int width, int height)
{

	inherited::Init("ui\\ui_frame", x, y, width, height);

	AttachChild(&UIArtifactList);
	UIArtifactList.Init(10,10,100,100);
	UIArtifactList.SetCellHeight(50);
	UIArtifactList.SetCellWidth(50);
	UIArtifactList.InitGrid(4, 4, true, 4);
	
	UIArtifactList.SetCheckProc(ArtifactProc);


	AttachChild(&UIPerformButton);
	UIPerformButton.Init("ui\\ui_button_01", 10, 250,150,40);
	UIPerformButton.SetText("perform");

	AttachChild(&UICloseButton);
	UICloseButton.Init("ui\\ui_button_01", 10, 300,150,40);
	UICloseButton.SetText("close");
}
void CUIArtifactMerger::InitArtifactMerger(CArtifactMerger* pArtifactMerger)
{
	m_pArtifactMerger = pArtifactMerger;
	R_ASSERT(m_pArtifactMerger);

	m_pArtifactMerger->RemoveAllArtifacts();

	UIArtifactList.DropAll();
	ResetAll();
}

void CUIArtifactMerger::SendMessage(CUIWindow *pWnd, s16 msg, void *pData)
{
	if(pWnd == &UIPerformButton && msg == CUIButton::BUTTON_CLICKED)
	{
		PerformArtifactMerger();
		GetParent()->SendMessage(this, PERFORM_BUTTON_CLICKED);
	}
	else if(pWnd == &UICloseButton && msg == CUIButton::BUTTON_CLICKED)
	{
		//выкинуть все артефакты
		m_pArtifactMerger->RemoveAllArtifacts();

		GetParent()->SendMessage(this, CLOSE_BUTTON_CLICKED);
	}

	inherited::SendMessage(pWnd, msg, pData);
}



void CUIArtifactMerger::Show()
{
	inherited::Show(true);
	inherited::Enable(true);
}
void CUIArtifactMerger::Hide()
{
	inherited::Show(false);
	inherited::Enable(false);
}

void CUIArtifactMerger::Update()
{
	inherited::Update();
}

bool CUIArtifactMerger::ArtifactProc(CUIDragDropItem* pItem, CUIDragDropList* pList)
{
	PIItem pInvItem = (PIItem)pItem->GetData();

	if(dynamic_cast<CArtifact*>(pInvItem))
		return true;
	else
		return false;
}

void CUIArtifactMerger::PerformArtifactMerger()
{
	for(DRAG_DROP_LIST_it it = UIArtifactList.GetDragDropItemsList().begin(); 
 						  it!= UIArtifactList.GetDragDropItemsList().end();
						  it++)
	{
		CUIDragDropItem* pDragDropItem = *it;
		PIItem pItem = (PIItem)pDragDropItem->GetData();

		m_pArtifactMerger->AddArtifact(dynamic_cast<CArtifact*>(pItem));
	}

	m_pArtifactMerger->PerformMerge();
	m_pArtifactMerger->RemoveAllArtifacts();



	//удалить из списка иконки тех артефактов, которые
	//изчезли при сочетании
	if(!m_pArtifactMerger->m_ArtifactDeletedList.empty())
	{
		for(ARTIFACT_LIST_it it = m_pArtifactMerger->m_ArtifactDeletedList.begin();
							 it!= m_pArtifactMerger->m_ArtifactDeletedList.end(); 
							 it++)
		{
		
			for(DRAG_DROP_LIST_it it1 = UIArtifactList.GetDragDropItemsList().begin(); 
 								  it1!= UIArtifactList.GetDragDropItemsList().end();
								  it1++)
			{
				if((*it1)->GetData() == *it)
				{
					((CUIDragDropList*)(*it1)->GetParent())->DetachChild(*it1);
					it1 = UIArtifactList.GetDragDropItemsList().begin();	
				}
			}
		}

		m_pArtifactMerger->m_ArtifactDeletedList.clear();
	}
}

