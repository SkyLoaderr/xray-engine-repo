#include "stdafx.h"
#include "build.h"
#include "cl_collector.h"

using namespace RAPID;

struct SectorFaces
{
	vector<tri*>	tris;
};

void OptimizeCFORM(RAPID::CollectorPacked& CL)
{
	vector<SectorFaces>	sector_faces;

	for (int it = 0; it<CL.getTS(); it++)
	{
		tri* T = CL.getT()+it;
	}
}
