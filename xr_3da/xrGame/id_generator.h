////////////////////////////////////////////////////////////////////////////
//	Module 		: id_generator.h
//	Created 	: 28.08.2003
//  Modified 	: 28.08.2003
//	Author		: Dmitriy Iassenev and Oles' Shishkovtsov
//	Description : ID generation class template
////////////////////////////////////////////////////////////////////////////

#pragma once

template<
	typename TIME_ID, 
	typename TYPE_ID, 
	typename VALUE_ID, 
	typename BLOCK_ID, 
	typename CHUNK_ID,
	VALUE_ID tMinValue, 
	VALUE_ID tMaxValue, 
	CHUNK_ID tBlockSize,
	VALUE_ID tInvalidValueID = tMaxValue,
	TIME_ID	 tStartTime = 0> 
class CID_Generator {
	typedef std::multimap< TIME_ID , BLOCK_ID >	TIME_BLOCK_MAP;
	typedef typename TIME_BLOCK_MAP::iterator	TIME_BLOCK_PAIR_IT;

	struct SID_Block {
		CHUNK_ID				m_tCount;
		TIME_ID					m_tTimeID;
		TYPE_ID					m_tpIDs[tBlockSize];
		SID_Block				() : m_tCount(0) {}
	};

	enum {
		m_tBlockCount = (u32(tMaxValue - tMinValue) + 1)/tBlockSize,
	};

	TIME_BLOCK_MAP				m_tpTimeMap;
	SID_Block					m_tppBlocks			[m_tBlockCount];

	IC		BLOCK_ID			tfGetBlockByValue(VALUE_ID tValueID)
	{
		BLOCK_ID				l_tBlockID = BLOCK_ID((tValueID - tMinValue)/tBlockSize);
		R_ASSERT2				(l_tBlockID < m_tBlockCount,"Requesting ID is invalid!");
		return					(l_tBlockID);
	}

			VALUE_ID			tfGetFromBlock	(TIME_BLOCK_PAIR_IT &tIterator, VALUE_ID tValueID)
	{
		BLOCK_ID				l_tBlockID = (*tIterator).second;
		SID_Block				&l_tID_Block = m_tppBlocks[l_tBlockID];

		VERIFY					(l_tID_Block.m_tCount);

		if (l_tID_Block.m_tCount == 1)
			m_tpTimeMap.erase	(tIterator);

		if (tInvalidValueID == tValueID)
			return				(VALUE_ID(l_tID_Block.m_tpIDs[--l_tID_Block.m_tCount]) + l_tBlockID*tBlockSize + tMinValue);

		TYPE_ID					*l_tpBlockID = std::find(l_tID_Block.m_tpIDs, l_tID_Block.m_tpIDs + l_tID_Block.m_tCount, TYPE_ID((tValueID - tMinValue)%tBlockSize));	
		R_ASSERT2				(l_tpBlockID != l_tID_Block.m_tpIDs + l_tID_Block.m_tCount,"Requesting ID has already been used!");
		*l_tpBlockID			= *(l_tID_Block.m_tpIDs + --l_tID_Block.m_tCount);
		return					(tValueID);
	}

public:
	VALUE_ID					m_tInvalidValueID;

								CID_Generator	()
	{
		m_tInvalidValueID		= tInvalidValueID;
		for (VALUE_ID i=0; ; i++) {
			vfFreeID			(i,tStartTime);
			if (i >= tMaxValue)
				break;
		}
		for (u32 j=0; j<m_tBlockCount; j++)
			std::reverse		(m_tppBlocks[j].m_tpIDs,m_tppBlocks[j].m_tpIDs + m_tppBlocks[j].m_tCount);
	}

	virtual						~CID_Generator	()
	{
	}

			VALUE_ID			tfGetID			(VALUE_ID tValueID = tInvalidValueID)
	{
		if (tInvalidValueID == tValueID) {
			TIME_BLOCK_PAIR_IT	I = m_tpTimeMap.begin();
			R_ASSERT2			(I != m_tpTimeMap.end(),"Not enough IDs");
			return				(tfGetFromBlock(I,tValueID));
		}
		else {
			BLOCK_ID			l_tBlockID = tfGetBlockByValue(tValueID);
			TIME_ID				l_tTimeID = m_tppBlocks[l_tBlockID].m_tTimeID;
			TIME_BLOCK_PAIR_IT	I = m_tpTimeMap.find(l_tTimeID);
			TIME_BLOCK_PAIR_IT	E = m_tpTimeMap.end();
			for ( ; I != E; I++)
				if ((*I).second == l_tBlockID)
					return		(tfGetFromBlock(I,tValueID));
			R_ASSERT2			(false,"Requesting ID has already been used!");
			return				(tInvalidValueID);
		}
	}

			void				vfFreeID		(VALUE_ID tValueID, TIME_ID tTimeID)
	{
		BLOCK_ID				l_tBlockID = tfGetBlockByValue(tValueID);
		SID_Block				&l_tID_Block = m_tppBlocks[l_tBlockID];

		VERIFY					(l_tID_Block.m_tCount < tBlockSize);

		if (!l_tID_Block.m_tCount)
			m_tpTimeMap.insert	(std::make_pair(tTimeID,l_tBlockID));

		l_tID_Block.m_tpIDs		[l_tID_Block.m_tCount++] = TYPE_ID((tValueID - tMinValue)%tBlockSize);
		l_tID_Block.m_tTimeID	= tTimeID;
	}
};

