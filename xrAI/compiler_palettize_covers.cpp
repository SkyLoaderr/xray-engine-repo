////////////////////////////////////////////////////////////////////////////
//	Module 		: compiler_palettize_covers.cpp
//	Created 	: 06.11.2003
//  Modified 	: 06.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Palettizing cover values to save space
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "compiler.h"

struct SCoverCluster : public SCover {
	u32				acc_cover[4];
	xr_vector<u32>	indexes;

	IC u32	distance(const SCoverCluster &node) const
	{
		return		(
			_sqr(int(cover[0]) - int(node.cover[0])) +
			_sqr(int(cover[1]) - int(node.cover[1])) +
			_sqr(int(cover[2]) - int(node.cover[2])) +
			_sqr(int(cover[3]) - int(node.cover[3]))
		);
	}

	IC u8   compress(const float c) const
	{
		int			cover = iFloor(c*255.f+EPS_L);
		clamp		(cover,0,255);
		return		(u8(cover));
	}

	IC void	compress(const Node &node)
	{
		for (u32 i=0; i<4; ++i)
			acc_cover[i] = cover[i] = compress(node.cover[i]);
	}

	IC void merge(const SCoverCluster &cluster)
	{
		indexes.insert				(indexes.end(),cluster.indexes.begin(),cluster.indexes.end());
		for (u32 i=0; i<4; ++i)
			acc_cover[i] += cluster.acc_cover[i];
		for (u32 i=0, n = indexes.size(); i<4; ++i)
			cover[i]	= acc_cover[i]/n;
	}
};

void xrPalettizeCovers()
{
	xr_vector<SCoverCluster>	g_cover_clusters;
	
	g_cover_clusters.resize		(g_nodes.size());

	// convert cover values and init clusters
	{
		Nodes::const_iterator				I = g_nodes.begin(), B = I;
		Nodes::const_iterator				E = g_nodes.end();
		xr_vector<SCoverCluster>::iterator	J = g_cover_clusters.begin();
		for ( ; I != E; ++I, ++J) {
			(*J).compress					(*I);
			(*J).indexes.push_back			(u32(I - B));
		}
	}

	{
		// clasterizing covers
		u32						best_pair[2];
		u32						best_distance;
		for (;g_cover_clusters.size() > 256;) {
			// choosing the nearest pair
			best_pair[0]		= 0;
			best_pair[1]		= 1;
			best_distance		= g_cover_clusters[0].distance(g_cover_clusters[1]);
			
			xr_vector<SCoverCluster>::const_iterator		b = g_cover_clusters.begin(), i = b;
			xr_vector<SCoverCluster>::const_iterator		e = g_cover_clusters.end();
			for ( ; i != e; ++i) {
				xr_vector<SCoverCluster>::const_iterator	j = b;
				for ( ; j != e; ++j) {
					if (i == j)
						continue;
					u32	distance = (*i).distance(*j);
					if (distance < best_distance) {
						best_distance = distance;
						best_pair[0] = u32(i - b);
						best_pair[1] = u32(j - b);
					}
				}
			}
			// merging the pair
			g_cover_clusters[best_pair[0]].merge(g_cover_clusters[best_pair[1]]);
			g_cover_clusters.erase	(g_cover_clusters.begin() + best_pair[1]);
		}
		
		g_covers_palette.resize(g_cover_clusters.size());
		xr_vector<SCoverCluster>::const_iterator	I = g_cover_clusters.begin(), B = I;
		xr_vector<SCoverCluster>::const_iterator	E = g_cover_clusters.end();
		xr_vector<SCover>::iterator					J = g_covers_palette.begin();
		for ( ; I != E; ++I, ++J) {
			Memory.mem_copy							((*J).cover,(*I).cover,sizeof((*I).cover));
			xr_vector<u32>::const_iterator			i = (*I).indexes.begin();
			xr_vector<u32>::const_iterator			e = (*I).indexes.end();
			for ( ; i != e; ++i)
				g_nodes[*i].cover_index				= u8(I - B);
		}
	}
}
