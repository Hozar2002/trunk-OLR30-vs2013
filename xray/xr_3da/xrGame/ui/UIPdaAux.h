//=============================================================================
//  Filename:   UIPdaAux.h
//	Created by Roman E. Marchenko, vortex@gsc-game.kiev.ua
//	Copyright 2004. GSC Game World
//	---------------------------------------------------------------------------
//  Некоторые определения которые общие для всех диалогов ПДА
//=============================================================================

#pragma once

enum EPdaTabs
{												// build 
	eptContacts			= 0,  // eptQuests		// eptContacts
	eptMap,						// eptMap		// eptMap
	eptQuests,					// eptDiary		// eptQuests
	eptDiary,				// eptContacts	// eptDiary
	//eptRanking,
	//eptActorStatistic,
	eptEncyclopedia,				// eptEncyclopedia   // eptEncyclopedia
	eptNoActiveTab		= u16(-1)
};


extern const char * const ALL_PDA_HEADER_PREFIX;

namespace pda_section{
	enum part{
		contacts			=(1<<8),		// quests	// contacts
		map				=(1<<9),		// map		// map
		quests			=(1<<10),		// diary	// quests
		diary		=(1<<11),		// contacts	// diary
		//ranking			=(1<<12),
		//statistics		=(1<<13),
		encyclopedia	=(1<<12),		//encyclopedia		//encyclopedia

		news			=diary|(1<<1),
		info			=diary|(1<<2),
		journal			=diary|(1<<3),

	};
};