// MemoryLeak.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "image.h"
#include <functional>
#include <algorithm>

#pragma comment(lib,"x:\\xrCore.lib")
#pragma comment(lib,"x:\\FreeImage.lib")

extern void Surface_Init();

struct SCover {
	u8				cover[4];
};

struct SCoverCluster : public SCover {
	u32				acc_cover[4];
	u32				sort_cover;
	u32				m_magnitude;
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

	IC void set_sort()
	{
		sort_cover			= cover[3];
		sort_cover			<<= 8;
		sort_cover			|= cover[2];
		sort_cover			<<= 8;
		sort_cover			|= cover[1];
		sort_cover			<<= 8;
		sort_cover			|= cover[0];
	}

	IC void set_magnitude()
	{
		m_magnitude			= _sqr((u32)cover[0]) + _sqr((u32)cover[1]) + _sqr((u32)cover[2]) + _sqr((u32)cover[3]);
	}

	IC void	compress(const u32 &node)
	{
		Memory.mem_copy	(cover,&node,sizeof(cover));
		for (int i=0; i<4; ++i)
			acc_cover[i]	= cover[i];
		set_sort			();
		set_magnitude		();
	}

	IC void merge(const SCoverCluster &cluster)
	{
		indexes.insert				(indexes.end(),cluster.indexes.begin(),cluster.indexes.end());
		for (u32 i=0, n = indexes.size(); i<4; ++i) {
			acc_cover[i]	+= cluster.acc_cover[i];
			cover[i]		= acc_cover[i]/n;
		}
		set_sort			();
		set_magnitude		();
	}

	IC u32 magnitude() const
	{
		return		(m_magnitude);
	}
};

struct cover_predicate {
	IC bool operator()(const SCoverCluster *node1, const SCoverCluster *node2) const
	{
		return		(node1->sort_cover < node2->sort_cover);
	}
};

struct cover_predicate_equal {
	IC bool operator()(const SCoverCluster *node) const
	{
		return		(!node);
	}
};

struct cover_predicate_magnitude {
	IC bool operator()(const SCoverCluster *node1, const SCoverCluster *node2) const
	{
		return		(node1->magnitude() < node2->magnitude());
	}

	IC bool operator()(const u32 dwValue, const SCoverCluster *node2) const
	{
		return		(dwValue <= node2->magnitude());
	}

	IC bool operator()(const SCoverCluster *node2, const u32 dwValue) const
	{
		return		(node2->magnitude() < dwValue);
	}
};

IC u64 single(u64 n)
{
	return(n*(n + 1)/2);
}

IC u64 square(u64 n)
{
	return(n*(n + 1)*(2*n + 1)/6);
}

IC u64 cub(u64 n)
{
	return(_sqr(single(n)));
}

IC u64 compute_formula(u64 n, u64 k)
{
	return(cub(n) - (k - 1)*square(n) - k*single(n));
}

IC float compute_percents(u64 n, u64 i, u64 k)
{
	return(_abs(1.f - (float)compute_formula(i,k)/(float)compute_formula(n,k)));
}

void xrPalettizeCovers(u32 *data, u32 N)
{
	xr_vector<SCoverCluster*>	g_cover_clusters;

	g_cover_clusters.resize		(N);

	// convert cover values and init clusters
	{
		u32									*I = data, *B = I;
		u32									*E = data + N;
		xr_vector<SCoverCluster*>::iterator	J = g_cover_clusters.begin();
		for ( ; I != E; ++I, ++J) {
			*J								= xr_new<SCoverCluster>();
			(*J)->compress					(*I);
			(*J)->indexes.push_back			(u32(I - B));
		}
	}

	{
		{
			printf	("\tSorting vectors...");
			sort	(g_cover_clusters.begin(),g_cover_clusters.end(),cover_predicate());
			printf	("completed!\n\tRemoving duplicates...");
			for (int i=1, j=0, n=g_cover_clusters.size(); i<n; ++i)
				if (!g_cover_clusters[j]->distance(*g_cover_clusters[i])) {
					g_cover_clusters[j]->merge(*g_cover_clusters[i]);
					xr_delete		(g_cover_clusters[i]);
				}
				else
					j = i;

			g_cover_clusters.erase	(remove_if(g_cover_clusters.begin(),g_cover_clusters.end(),cover_predicate_equal()),g_cover_clusters.end());
			printf	("completed\n\tRemoved duplicates : %d\n\tElements left      : %d\n",n - g_cover_clusters.size(),g_cover_clusters.size());
		}
		printf	("\tSorting magnitudes...");
		sort						(g_cover_clusters.begin(),g_cover_clusters.end(),cover_predicate_magnitude());
		printf	("completed!\n");
		// clasterizing covers
		u32						best_pair[2];
		u32						best_distance;
		u64						iterations = compute_formula(g_cover_clusters.size(),256);
		u64						portion = iterations/10000;
		u64						accumulator = 0;
		printf					("\t\tIteration : %5.2f%",0.f);
		for (int ii=0, nn = g_cover_clusters.size(); g_cover_clusters.size() > 256; ++ii) {
			if (accumulator >= portion) {
				for (int j=0; j<32; j++)
					printf			("\b \b");
				printf				("\t\tIteration : %5.2f%",100.f*compute_percents(nn,nn - ii,256));
				accumulator			= 0;
			}
			accumulator			+= square(nn - ii);
			// choosing the nearest pair
			best_pair[0]		= 0;
			best_pair[1]		= 1;
			best_distance		= g_cover_clusters[0]->distance(*g_cover_clusters[1]);
			if (best_distance) {
				xr_vector<SCoverCluster*>::const_iterator		b = g_cover_clusters.begin(), i = b, j, k;
				xr_vector<SCoverCluster*>::const_iterator		e = g_cover_clusters.end();
				for ( ; i != e; ++i) {
					u32				max_magnitude = iFloor(_sqr(_sqrt((float)(*i)->magnitude()) + _sqrt((float)best_distance)));
					k				= upper_bound(i,e,max_magnitude,cover_predicate_magnitude());
					for (j = i + 1 ; j != k; ++j) {
						u32	distance = (*i)->distance(**j);
						if (distance < best_distance) {
							best_pair[0]	= u32(i - b);
							best_pair[1]	= u32(j - b);
							best_distance	= distance;
							if (!best_distance) {
								i = b + g_cover_clusters.size() - 1;
								break;
							}
							max_magnitude	= iFloor(_sqr(_sqrt((float)(*i)->magnitude()) + _sqrt((float)best_distance)));
							k				= upper_bound(i,k,max_magnitude,cover_predicate_magnitude());
							if (j == k)
								break;
						}
					}
				}
			}
			else {
				xr_vector<SCoverCluster*>::iterator		kk,ee = g_cover_clusters.begin() + best_pair[0];
				(*ee)->merge			(*g_cover_clusters[best_pair[1]]);
				xr_delete				(g_cover_clusters[best_pair[1]]);
				g_cover_clusters.erase	(g_cover_clusters.begin() + best_pair[1]);
				continue;
			}

			// merging the pair
			xr_vector<SCoverCluster*>::iterator		kk,ee = g_cover_clusters.begin() + best_pair[0];
			
			(*ee)->merge			(*g_cover_clusters[best_pair[1]]);
			SCoverCluster			*t = *ee;
			u32						magnitude = t->magnitude();
			
			xr_delete				(g_cover_clusters[best_pair[1]]);
			g_cover_clusters.erase	(g_cover_clusters.begin() + best_pair[1]);
			
			g_cover_clusters.erase	(g_cover_clusters.begin() + best_pair[0]);
			kk						= lower_bound(g_cover_clusters.begin(),g_cover_clusters.end(),magnitude,cover_predicate_magnitude());
			g_cover_clusters.insert	(kk,t);
		}
		
		for (int j=0; j<32; j++)
			printf					("\b \b");
		printf						("\t\tIteration : %5.2f%\n",100.f);

		Memory.mem_fill				(data,0,N*sizeof(u32));

		xr_vector<SCoverCluster*>::iterator			I = g_cover_clusters.begin(), B = I;
		xr_vector<SCoverCluster*>::iterator			E = g_cover_clusters.end();
		printf										("\tTotal colors : %d\n",g_cover_clusters.size());
		for ( ; I != E; ++I) {
			xr_vector<u32>::const_iterator			i = (*I)->indexes.begin();
			xr_vector<u32>::const_iterator			e = (*I)->indexes.end();
			for ( ; i != e; ++i)
				data[*i]							= (*I)->sort_cover;
			xr_delete								(*I);
		}
	}
}

int __cdecl _tmain(int argc, _TCHAR* argv[])
{
	if (argc < 2)
		return			(-1);

	printf				("Initializing...");
	Core._initialize	("memory_leak");
	Surface_Init		();
	printf				("completed!\n");

	printf				("Loading image...");
	CImage				image;
	image.Load			(argv[1]);
	image.Vflip			();
	printf				("completed!\n");
	
	printf				("Reindexing colors started\n");
	u64					l_qwStartTime = CPU::GetCycleCount(), l_qwFinishTime;
	xrPalettizeCovers	(image.pData,image.dwWidth*image.dwHeight);
	l_qwFinishTime		= CPU::GetCycleCount();
	printf				("Reindexing colors finished\n");
	printf				("Time : %f seconds\n",CPU::cycles2seconds*s64(l_qwFinishTime - l_qwStartTime));
	
	printf				("Saving image...");
	string256			S;
	image.SaveTGA		(strconcat(S,argv[1],".tga"));
	printf				("completed!\n");
	
	printf				("Finalizing...");
	Core._destroy		();
	printf				("completed!\n");
	return				(0);
}

