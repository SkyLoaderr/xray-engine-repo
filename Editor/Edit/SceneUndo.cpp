//----------------------------------------------------
// file: SceneUndo.cpp
//
// Tanya M. : 265-74-96
//            265-86-00
//            251-46-69
//            251-46-70
//            251-46-76
//            251-46-77
//            251-46-78
//            264-89-00
//
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "UI_Main.h"
#include "Scene.h"
#include "FileSystem.h"

//----------------------------------------------------

void EScene::UndoClear(){
	while( !m_RedoStack.empty() ){
		unlink( m_RedoStack.back().m_FileName );
		m_RedoStack.pop_back(); }
	while( !m_UndoStack.empty() ){
		unlink( m_UndoStack.back().m_FileName );
		m_UndoStack.pop_back(); }
}

void EScene::UndoPrepare(){
    UI.UpdateScene();

	UndoItem item;
	GetTempFileName( FS.m_Temp.m_Path, "undo", 0, item.m_FileName );

	Save( item.m_FileName );
	m_UndoStack.push_back( item );

	while( !m_RedoStack.empty() ){
		unlink( m_RedoStack.back().m_FileName );
		m_RedoStack.pop_back(); }

	if( m_UndoStack.size() > 25 ){
		unlink( m_UndoStack.front().m_FileName );
		m_UndoStack.pop_front(); }

    Modified();
}

bool EScene::Undo(){
	if( !m_UndoStack.empty() ){

		Unload();
		Load( m_UndoStack.back().m_FileName );

		m_RedoStack.push_back( m_UndoStack.back() );
		m_UndoStack.pop_back();

		if( m_RedoStack.size() > 25 ){
			unlink( m_RedoStack.front().m_FileName );
			m_RedoStack.pop_front(); }

		return true;
	}
	return false;
}

bool EScene::Redo(){
	if( !m_RedoStack.empty() ){

		Unload();
		Load( m_RedoStack.back().m_FileName );

		m_UndoStack.push_back( m_RedoStack.back() );
		m_RedoStack.pop_back();

		if( m_UndoStack.size() > 25 ){
			unlink( m_UndoStack.front().m_FileName );
			m_UndoStack.pop_front(); }

		return true;
	}
	return false;
}

//----------------------------------------------------

