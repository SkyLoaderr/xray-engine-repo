#pragma once

class CStalkerSelectorFreeHunting : public CAISelectorBase
{
	public:
		CStalkerSelectorFreeHunting();
		virtual	float Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop);
};

class CStalkerSelectorReload : public CAISelectorBase
{
	public:
		CStalkerSelectorReload();
		virtual	float Estimate(NodeCompressed* tNode, float fDistance, BOOL& bStop);
};

