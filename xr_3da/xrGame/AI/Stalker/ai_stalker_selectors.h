#pragma once

class CStalkerSelectorFreeHunting : public CAISelectorBase
{
	public:
		CStalkerSelectorFreeHunting();
		virtual	float Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop);
};

