#include "stdafx.h"
#pragma hdrstop

#include "ui_tools.h"
#include "ui_main.h"
#include "EditorPref.h"

void CActorTools::UndoClear()
{
	while( !m_RedoStack.empty() ){
		unlink( m_RedoStack.back().m_FileName );
		m_RedoStack.pop_back(); }
	while( !m_UndoStack.empty() ){
		unlink( m_UndoStack.back().m_FileName );
		m_UndoStack.pop_back(); }
}

void CActorTools::UndoSave()
{
    UI.RedrawScene();

	UndoItem item;
	GetTempFileName( FS.get_path(_temp_)->m_Path, "undo", 0, item.m_FileName );

	Save( 0, item.m_FileName, true );
	m_UndoStack.push_back( item );

	while( !m_RedoStack.empty() ){
		unlink( m_RedoStack.back().m_FileName );
		m_RedoStack.pop_back(); }

	if( frmEditPrefs&&(m_UndoStack.size() > frmEditPrefs->seUndoLevels->Value) ){
		unlink( m_UndoStack.front().m_FileName );
		m_UndoStack.pop_front(); }
}

bool CActorTools::Undo()
{
//	if( !m_UndoStack.empty() ){
	if( m_UndoStack.size()>1 ){
		m_RedoStack.push_back( m_UndoStack.back() );
		m_UndoStack.pop_back();

		if( m_RedoStack.size() > frmEditPrefs->seUndoLevels->Value ){
			unlink( m_RedoStack.front().m_FileName );
			m_RedoStack.pop_front();
        }

		if( !m_UndoStack.empty() ){
			Clear();
         	Load( 0, m_UndoStack.back().m_FileName );
        }

        Modified();
        
		return true;
	}
	return false;
}

bool CActorTools::Redo()
{
	if( !m_RedoStack.empty() ){
        Clear();
		Load( 0, m_RedoStack.back().m_FileName );

		m_UndoStack.push_back( m_RedoStack.back() );
		m_RedoStack.pop_back();

		if( m_UndoStack.size() > frmEditPrefs->seUndoLevels->Value ){
			unlink( m_UndoStack.front().m_FileName );
			m_UndoStack.pop_front(); 
        }

        Modified();

		return true;
	}
	return false;
}

//----------------------------------------------------

 