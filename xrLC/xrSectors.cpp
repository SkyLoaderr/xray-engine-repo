#include "stdafx.h"
#include "build.h"
#include "sector.h"

vector<CSector*>	g_sectors;

void CBuild::BuildSectors()
{
	Status("Determining sectors...");
	Progress(0);
	DWORD SectorMax=0;
	for (DWORD I=0; I<g_tree.size(); I++)
		if (g_tree[I]->Sector>SectorMax) SectorMax=g_tree[I]->Sector;
	R_ASSERT(SectorMax<0xffff);

	DWORD SectorCount = SectorMax+1; 
	g_sectors.resize(SectorCount);
	ZeroMemory(g_sectors.begin(),g_sectors.size()*sizeof(void*));
	clMsg("%d sectors accepted.",SectorCount);

	Status("Spatializing geometry...");
	for (I=0; I<g_tree.size(); I++)
	{
		DWORD Sector = g_tree[I]->Sector;
		if (0==g_sectors[Sector]) g_sectors[Sector] = xr_new<CSector> (Sector);
	}

	Status("Building hierrarhy...");
	for (I=0; I<g_sectors.size(); I++)
	{
		R_ASSERT(g_sectors[I]);
		g_sectors[I]->BuildHierrarhy();
		Progress(float(I)/float(g_sectors.size()));
	}

	Status("Assigning portals, occluders, glows, lights...");
	// portals
	for (I=0; I<portals.size(); I++)
	{
		b_portal &P = portals[I];
		R_ASSERT(DWORD(P.sector_front)<g_sectors.size());
		R_ASSERT(DWORD(P.sector_back) <g_sectors.size());
		g_sectors[DWORD(P.sector_front)]->add_portal	(WORD(I));
		g_sectors[DWORD(P.sector_back)]->add_portal		(WORD(I));
	}
	// glows
	for (I=0; I<glows.size(); I++)
	{
		b_glow		&G = glows[I];
		b_material	&M = materials[G.dwMaterial];
		R_ASSERT(M.sector<g_sectors.size());
		g_sectors[M.sector]->add_glow			(WORD(I));
	}
	// lights
	for (I=0; I<L_dynamic.size(); I++)
	{
		b_light_dynamic	&L = L_dynamic[I];
		if (L.data.type == D3DLIGHT_DIRECTIONAL)
		{
			for (DWORD j=0; j<g_sectors.size(); j++)
			{
				R_ASSERT(g_sectors[j]);
				g_sectors[j]->add_light(WORD(I));
			}
		} else {
			if	(L.sectors.size()) {
				for (DWORD j=0; j<L.sectors.size(); j++)
				{
					R_ASSERT	(L.sectors[j]<g_sectors.size());
					g_sectors	[L.sectors[j]]->add_light(WORD(I));
				}
			} else {
				clMsg("Fuck!!! Light at position %f,%f,%f non associated!!!",
					L.data.position.x,L.data.position.y,L.data.position.z
					);
			}
		}
	}
}

void CBuild::SaveSectors(IWriter& fs)
{
	CMemoryWriter MFS;
	Status("Processing...");

	// validate & save
	for (DWORD I=0; I<g_sectors.size(); I++)
	{
		MFS.open_chunk(I);
		g_sectors[I]->Validate();
		g_sectors[I]->Save(MFS);
		MFS.close_chunk();
		Progress(float(I)/float(g_sectors.size()));
	}

	fs.w_chunk(fsL_SECTORS|CFS_CompressMark,MFS.pointer(),MFS.size());
}
