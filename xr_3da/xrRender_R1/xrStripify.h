#pragma once

void	xrStripify	(std::xr_vector<WORD> &indices, std::xr_vector<WORD> &perturb, int iCacheSize, int iMinStripLength);
int		xrSimulate	(std::xr_vector<WORD> &indices, int iCacheSize);
