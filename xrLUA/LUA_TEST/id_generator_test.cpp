#include "stdafx.h"

#pragma warning(push)
#pragma warning(disable:4995)
#include "xrCore.h"
#pragma warning(pop)

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
private:
	struct SID_Block {
		CHUNK_ID	m_tCount;
		TIME_ID		m_tTimeID;
		TYPE_ID		m_tpIDs[tBlockSize];

		IC				SID_Block	() : m_tCount(0) {}
		
		IC	bool		operator<	(const SID_Block &b) const
		{
			return	(m_tCount && ((m_tTimeID < b.m_tTimeID) || !b.m_tCount));
		}
	};

private:
	enum {
		m_tBlockCount			= u32(tMaxValue - tMinValue)/tBlockSize + 1,
	};

private:
	u32							m_available_count;
	SID_Block					m_tppBlocks	[m_tBlockCount];

private:
	IC		BLOCK_ID			tfGetBlockByValue(VALUE_ID tValueID)
	{
		BLOCK_ID				l_tBlockID = BLOCK_ID((tValueID - tMinValue)/tBlockSize);
		R_ASSERT2				(l_tBlockID < m_tBlockCount,"Requesting ID is invalid!");
		return					(l_tBlockID);
	}

	IC		VALUE_ID			tfGetFromBlock	(SID_Block &l_tID_Block, VALUE_ID tValueID)
	{
		VERIFY					(l_tID_Block.m_tCount);
		BLOCK_ID				l_tBlockID = BLOCK_ID(&l_tID_Block - m_tppBlocks);

		if (l_tID_Block.m_tCount == 1) {
			--m_available_count;
			VERIFY				(m_available_count >= 0);
		}

		if (tInvalidValueID == tValueID)
			return				(VALUE_ID(l_tID_Block.m_tpIDs[--l_tID_Block.m_tCount]) + l_tBlockID*tBlockSize + tMinValue);

		TYPE_ID					*l_tpBlockID = std::find(l_tID_Block.m_tpIDs, l_tID_Block.m_tpIDs + l_tID_Block.m_tCount, TYPE_ID((tValueID - tMinValue)%tBlockSize));	
		R_ASSERT2				(l_tID_Block.m_tpIDs + l_tID_Block.m_tCount != l_tpBlockID,"Requesting ID has already been used!");
		*l_tpBlockID			= *(l_tID_Block.m_tpIDs + --l_tID_Block.m_tCount);
		return					(tValueID);
	}

public:
	VALUE_ID					m_tInvalidValueID;

	IC							CID_Generator	()
	{
		m_tInvalidValueID		= tInvalidValueID;
		m_available_count		= 0;
		for (VALUE_ID i=tMinValue; ; ++i) {
			vfFreeID			(i,tStartTime);
			if (i >= tMaxValue)
				break;
		}
		VERIFY					(m_available_count == m_tBlockCount);
		for (u32 j=0; j<m_tBlockCount; ++j)
			std::reverse		(m_tppBlocks[j].m_tpIDs,m_tppBlocks[j].m_tpIDs + m_tppBlocks[j].m_tCount);
	}

	virtual						~CID_Generator	()
	{
	}

	IC		VALUE_ID			tfGetID			(VALUE_ID tValueID = tInvalidValueID)
	{
		if (tInvalidValueID != tValueID)
			return				(tfGetFromBlock(m_tppBlocks[tfGetBlockByValue(tValueID)],tValueID));

		R_ASSERT2				(m_available_count,"Not enough IDs");
		SID_Block*				I = std::min_element(m_tppBlocks,m_tppBlocks + m_tBlockCount);
		VERIFY					(I != m_tppBlocks + m_tBlockCount);
		return					(tfGetFromBlock(*I,tValueID));
	}

	IC		void				vfFreeID		(VALUE_ID tValueID, TIME_ID tTimeID)
	{
		BLOCK_ID				l_tBlockID = tfGetBlockByValue(tValueID);
		SID_Block				&l_tID_Block = m_tppBlocks[l_tBlockID];

		VERIFY					(l_tID_Block.m_tCount < tBlockSize);

		if (!l_tID_Block.m_tCount) {
			++m_available_count;
			VERIFY				(m_available_count <= m_tBlockCount);
		}

#ifdef DEBUG
		TYPE_ID					*l_tpBlockID = std::find(l_tID_Block.m_tpIDs, l_tID_Block.m_tpIDs + l_tID_Block.m_tCount, TYPE_ID((tValueID - tMinValue)%tBlockSize));	
		VERIFY					(l_tpBlockID == l_tID_Block.m_tpIDs + l_tID_Block.m_tCount);
#endif
		l_tID_Block.m_tpIDs		[l_tID_Block.m_tCount++] = TYPE_ID((tValueID - tMinValue)%tBlockSize);
		l_tID_Block.m_tTimeID	= tTimeID;
	}
};

void test_id_generator()
{
	CID_Generator<
		u32,		// time identifier type
		u8,			// compressed id type 
		u16,		// id type
		u8,			// block id type
		u16,		// chunk id type
		0,			// min value
		u16(-2),	// max value
		256,		// block size
		u16(-1)		// invalid id
	>	idgen;

	xr_vector<u16>	m_used;

	for (u32 i=0; i<400000; ++i) {
		if (::Random.randF() < .6f) {
			m_used.push_back(idgen.tfGetID());
			printf	("new id %d\n",m_used.back());
		}
		else {
			if (m_used.empty())
				continue;
			
			xr_vector<u16>::iterator	I = m_used.begin() + ::Random.randI(m_used.size());
			idgen.vfFreeID	(*I,i);
			printf	("free id %d\n",*I);
			if (::Random.randF() < .1f) {
				printf	("new id %d\n",*I);
				idgen.tfGetID	(*I);
				printf	("free id %d\n",*I);
				idgen.vfFreeID	(*I,i);
			}
			m_used.erase	(I);
		}
	}

	printf("Total %d ids",m_used.size());
}