#include "stdafx.h"
#include "build.h"
#include "cl_collector.h"

using namespace CDB;

struct SectorFaces
{
	vector<TRI*>	tris;
};

void OptimizeCFORM(CDB::CollectorPacked& CL)
{
	vector<SectorFaces>	sector_faces;

	// Convert to per-sector subdivisions
	for (int it = 0; it<int(CL.getTS()); it++)
	{
		TRI* T = CL.getT()+it;
		WORD S = T->sector;
		if (S>=sector_faces.size())	sector_faces.resize(S+1);
		sector_faces[S].tris.push_back	(T);
	}

	// Perform optimize
	for (it=0; it<int(sector_faces.size()); it++)
	{
	}
}
