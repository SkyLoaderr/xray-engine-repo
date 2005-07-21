//=============================================================================
//  Filename:   UIPdaAux.h
//	Created by Roman E. Marchenko, vortex@gsc-game.kiev.ua
//	Copyright 2004. GSC Game World
//	---------------------------------------------------------------------------
//  Некоторые определения которые общие для всех диалогов ПДА
//=============================================================================

#ifndef UI_PDA_AUX_H_
#define UI_PDA_AUX_H_

#pragma once

//////////////////////////////////////////////////////////////////////////

/*
// Разделы ПДА на которые можно сылаться извне для принудительного открытия
enum EPdaSections
{
	epsActiveJobs	= 0,
	epsMap,
	epsContacts,
	epsEncyclopedia,
	epsDiaryArticle
};*/
enum EPdaTabs
{
	eptQuests			= 0,
	eptMap,
	eptDiary,
	eptContacts,
	eptRanking,
	eptActorStatistic,
	eptEncyclopedia,
	eptNoActiveTab		= u16(-1)
};


extern const char * const ALL_PDA_HEADER_PREFIX;

#endif	//UI_PDA_AUX_H_