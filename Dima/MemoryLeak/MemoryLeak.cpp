#include "stdafx.h"
#include "image.h"
#include <functional>
#include <algorithm>

#pragma comment(lib,"x:\\xrCore.lib")
#pragma comment(lib,"x:\\FreeImage.lib")

extern void Surface_Init();

struct SPair {
	u32				pair[2];
};

struct SCover {
	u8				cover[4];
};

struct SCoverCluster : public SCover {
	u32				acc_cover[4];
//	u64				acc_cover2[4];
	u32				sort_cover;
	u32				m_magnitude;
	u32				item_count;
	SCoverCluster	*cluster;

	IC u32	distance(const SCoverCluster &node) const
	{
//		if (item_count*node.item_count == 1)
			return		(
				_sqr(int(cover[0]) - int(node.cover[0])) +
				_sqr(int(cover[1]) - int(node.cover[1])) +
				_sqr(int(cover[2]) - int(node.cover[2])) +
				_sqr(int(cover[3]) - int(node.cover[3]))
				);
//		double	n1 = item_count, n2 = node.item_count, result = 0, s1, s2;
//		for (int i=0; i<4; ++i) {
//			if (cover[i] == node.cover[i])
//				continue;
//			s1		= (double)acc_cover2[i];
//			s2		= (double)node.acc_cover2[i];
//			result	+= (_sqr(n2)*(2*s1 + (item_count > 1 ? n1*s1 : 0)) + _sqr(n1)*(2*s2 + (node.item_count > 1 ? n2*s2 : 0)) - 4*n1*n2*_sqrt(s1*s2))/(2*n1*n2*(n1+n2));
//		}
//		return				(_abs(iFloor(result)));
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
		cluster				= this;
		item_count			= 1;
		Memory.mem_copy		(cover,&node,sizeof(cover));

		for (int i=0; i<4; ++i) {
			acc_cover[i]	= cover[i];
//			acc_cover2[i]	= _sqr(u64(cover[i]));
		}

		set_sort			();
		set_magnitude		();
	}

	IC void merge(SCoverCluster &cluster)
	{
		cluster.cluster		= this;
		item_count			+= cluster.item_count;

		for (u32 i=0; i<4; ++i) {
			acc_cover[i]	+= cluster.acc_cover[i];
//			acc_cover2[i]	+= cluster.acc_cover2[i];
			cover[i]		= acc_cover[i]/item_count;
		}
		set_sort			();
		set_magnitude		();
	}

	IC u32 magnitude() const
	{
		return		(m_magnitude);
	}

	IC bool valid() const
	{
		return		(cluster == this);
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

struct non_valid_predicate {
	IC bool operator()(const SCoverCluster *node) const
	{
		return		(!node->valid());
	}
};

struct item_count_predicate {
	const xr_vector<SCoverCluster*>	*clusters;
	item_count_predicate(const xr_vector<SCoverCluster*> &_clusters) {clusters = &_clusters;}

	IC bool operator()(const SPair &node1, const SPair &node2) const
	{
		return		((*clusters)[node1.pair[0]]->item_count + (*clusters)[node1.pair[1]]->item_count < (*clusters)[node2.pair[0]]->item_count + (*clusters)[node2.pair[1]]->item_count);
	}
};

struct cover_predicate_magnitude {
	IC bool operator()(const SCoverCluster *node1, const SCoverCluster *node2) const
	{
		return		(node1->magnitude() < node2->magnitude());
	}

	IC bool operator()(const u32 dwValue, const SCoverCluster *node2) const
	{
		return		(dwValue < node2->magnitude());
	}

	IC bool operator()(const SCoverCluster *node2, const u32 dwValue) const
	{
		return		(node2->magnitude() < dwValue);
	}
};

IC double single(double n)
{
	return(n*(n + 1)/2);
}

IC double square(double n)
{
	return(n*(n + 1)*(2*n + 1)/6);
}

IC double cub(double n)
{
	return(_sqr(single(n)));
}

IC double compute_formula(double n, double k)
{
	return(cub(n) - (k - 1)*square(n) - k*single(n));
}

IC float compute_percents(double n, double i, double k)
{
	return(_abs(1.f - (float)compute_formula(i,k)/(float)compute_formula(n,k)));
}

void xrPalettizeCovers(u32 *data, u32 N)
{
	xr_vector<SCoverCluster*>	clusters;
	xr_vector<SCoverCluster*>	temp;
	SCoverCluster				*memory_block = (SCoverCluster*)xr_malloc(N*sizeof(SCoverCluster));

	clusters.resize				(N);
	temp.resize					(N);

	// convert cover values and init clusters
	{
		SCoverCluster		*i = memory_block;
		u32					*I = data, *B = I;
		u32					*E = data + N;
		xr_vector<SCoverCluster*>::iterator	J = clusters.begin();
		for ( ; I != E; ++I, ++J, ++i) {
			*J				= i;
			(*J)->compress	(*I);
		}
	}

	{
		printf				("\tSorting vectors...");
		sort				(clusters.begin(),clusters.end(),cover_predicate());
		printf				("completed!\n\tRemoving duplicates...");
		for (int i=1, j=0, n=clusters.size(); i<n; ++i)
			if (clusters[j]->sort_cover == clusters[i]->sort_cover) {
				clusters[j]->merge(*clusters[i]);
				clusters[i]	= 0;
			}
			else
				j			= i;

		clusters.erase		(remove_if(clusters.begin(),clusters.end(),cover_predicate_equal()),clusters.end());
		printf				("completed\n\tRemoved duplicates : %d\n\tElements left      : %d\n",n - clusters.size(),clusters.size());
	}

	printf					("\tSorting magnitudes...");
	sort					(clusters.begin(),clusters.end(),cover_predicate_magnitude());
	printf					("completed!\n");
	// clasterizing covers
	xr_vector<SPair>		best_pair;
	best_pair.reserve		(clusters.size());
	u32						best_distance;
	double					iterations = compute_formula(clusters.size(),256);
	double					portion = iterations/10000;
	double					accumulator = 0;
	printf					("\t\tIteration : %6.2f%%",0.f);
	for (int ii=0, nn = clusters.size(); clusters.size() > 256; ) {
		if (accumulator >= portion) {
			for (int j=0; j<32; j++)
				printf		("\b \b");
			printf			("\t\tIteration : %6.2f%%",100.f*compute_percents(nn,nn - ii,256));
			accumulator		= 0;
		}
		// choosing the nearest pair
		best_pair.resize	(1);
		best_pair[0].pair[0]= 0;
		best_pair[0].pair[1]= 1;
		best_distance		= clusters[0]->distance(*clusters[1]);
		xr_vector<SCoverCluster*>::const_iterator		b = clusters.begin(), i = b, j, k;
		xr_vector<SCoverCluster*>::const_iterator		e = clusters.end();
		for ( ; i != e; ++i) {
			u32			max_magnitude = iFloor(_sqr(_sqrt((float)(*i)->magnitude()) + _sqrt((float)best_distance)) + .5f);
			k			= upper_bound(i,e,max_magnitude,cover_predicate_magnitude());
			if (k == i)
				continue;
			for (j = i + 1 ; j != k; ++j) {
				u32	distance = (*i)->distance(**j);
				if (distance < best_distance) {
					best_pair.resize		(1);
					best_pair[0].pair[0]	= u32(i - b);
					best_pair[0].pair[1]	= u32(j - b);
					best_distance			= distance;
					max_magnitude	= iFloor(_sqr(_sqrt((float)(*i)->magnitude()) + _sqrt((float)best_distance)) + .5f);
					k				= upper_bound(i,k,max_magnitude,cover_predicate_magnitude());
					if ((j == k) || (k == i))
						break;
				}
				else
					if (distance == best_distance) {
						SPair	t;
						t.pair[0] = u32(i - b);
						t.pair[1] = u32(j - b);
						best_pair.push_back(t);
					}
			}
//			if (!best_distance)
//				i = k;
		}

		// merging the pair
		temp.clear			();
		temp.reserve		(best_pair.size());
		if (clusters.size() - best_pair.size() < 256) {
			u32				N = clusters.size() - 256;
//			sort			(best_pair.begin(),best_pair.end(),item_count_predicate(clusters));
			if (!best_distance) {
				xr_vector<SPair>::iterator	I = best_pair.begin(), B = I;
				xr_vector<SPair>::iterator	E = best_pair.end();
				for (u32 i=0; I != E; ++I)
					if (clusters[(*I).pair[0]]->valid() || !clusters[(*I).pair[0]]->cluster) {
						clusters[(*I).pair[0]]->merge(*clusters[(*I).pair[1]]);
						if (clusters[(*I).pair[0]]->cluster) {
							clusters[(*I).pair[0]]->cluster = 0;
							temp.push_back	(clusters[(*I).pair[0]]);
						}
						if (++i >= N)
							break;
					}
			}
			else {
				xr_vector<SPair>::iterator	I = best_pair.begin(), B = I;
				xr_vector<SPair>::iterator	E = best_pair.end();
				for (u32 i=0; I != E; ++I)
					if (clusters[(*I).pair[0]]->valid() && clusters[(*I).pair[1]]->valid()) {
						clusters[(*I).pair[0]]->merge(*clusters[(*I).pair[1]]);
						clusters[(*I).pair[0]]->cluster = 0;
						temp.push_back	(clusters[(*I).pair[0]]);
						if (++i >= N)
							break;
					}
			}
		}
		else {
			if (!best_distance) {
				xr_vector<SPair>::iterator	I = best_pair.begin(), B = I;
				xr_vector<SPair>::iterator	E = best_pair.end();
				for ( ; I != E; ++I)
					if (clusters[(*I).pair[0]]->valid() || !clusters[(*I).pair[0]]->cluster) {
						clusters[(*I).pair[0]]->merge(*clusters[(*I).pair[1]]);
						if (clusters[(*I).pair[0]]->cluster) {
							clusters[(*I).pair[0]]->cluster = 0;
							temp.push_back	(clusters[(*I).pair[0]]);
						}
					}
			}
			else {
				xr_vector<SPair>::iterator	I = best_pair.begin(), B = I;
				xr_vector<SPair>::iterator	E = best_pair.end();
				for ( ; I != E; ++I)
					if (clusters[(*I).pair[0]]->valid() && clusters[(*I).pair[1]]->valid()) {
						clusters[(*I).pair[0]]->merge(*clusters[(*I).pair[1]]);
						clusters[(*I).pair[0]]->cluster = 0;
						temp.push_back	(clusters[(*I).pair[0]]);
					}
			}
		}

		clusters.erase		(remove_if(clusters.begin(),clusters.end(),non_valid_predicate()),clusters.end());
		{
			xr_vector<SCoverCluster*>::iterator	I = temp.begin(), J;
			xr_vector<SCoverCluster*>::iterator	E = temp.end();
			for ( ; I != E; ++I) {
				(*I)->cluster	= *I;
				u32			magnitude = (*I)->magnitude();
				J			= lower_bound(clusters.begin(),clusters.end(),magnitude,cover_predicate_magnitude());
				clusters.insert	(J,*I);
			}
		}

		accumulator			+= compute_formula(nn - ii,256) - compute_formula(nn - ii - temp.size(),256);
		ii					+= temp.size();
	}
	
	for (int j=0; j<32; j++)
		printf				("\b \b");
	printf					("\t\tIteration : %6.2f%%\n\tTotal clusters : %d\n",100.f,clusters.size());

	Memory.mem_fill			(data,0,N*sizeof(u32));

	u32						*i = data;
	SCoverCluster			*I = memory_block, *J = I;
	SCoverCluster			*E = memory_block + N;
	for ( ; I != E; ++I, J = I, ++i) {
		for (; !J->valid(); J = J->cluster);
		*i					= I->sort_cover = J->sort_cover;
		I->cluster			= I;
	}

	xr_free					(memory_block);
}

extern void vfNeuQuant(u32 *data, u32 N);

int __cdecl _tmain(int argc, _TCHAR* argv[])
{
	if (argc < 2) {
		CImage			image;
		image.Load		("d:\\0.bmp");
		CImage			image0;
		image0.Load		("d:\\0_.tga");
		CImage			image1;
		image1.Load		("s:\\0.tga");

		u64				diff0,diff1;
		diff0			= diff1 = 0;
		xr_set<u32>		a0;
		xr_set<u32>		a1;
		for (int i=0; i<(int)(image.dwWidth*image.dwHeight); ++i) {
			union Temp {
				u8  a[4];
				u32 data;
			} ;
			Temp		data, data0, data1;
			data.data	= image.pData[i];
			data0.data	= image0.pData[i];
			data1.data	= image1.pData[i];
			if (a0.find(data0.data) == a0.end())
				a0.insert(data0.data);
			if (a1.find(data1.data) == a1.end())
				a1.insert(data1.data);
			for (int j=0; j<4; ++j) {
				diff0	+= _abs((int)data.a[j] - (int)data0.a[j]);
				diff1	+= _abs((int)data.a[j] - (int)data1.a[j]);
			}
		}

		printf			("%11I64u[%f][%d] : %11I64u[%f][%d]\n",diff0,double(diff0)/double(image.dwWidth*image.dwHeight),a0.size(),diff1,double(diff1)/double(image.dwWidth*image.dwHeight),a1.size());
		return			0;
	}

	printf				("Initializing...");
	Core._initialize	("memory_leak");
	Surface_Init		();
	printf				("completed!\n");

	printf				("Loading image...");
	CImage				image;
	image.Load			(argv[1]);
	image.Vflip			();
	printf				("completed!\n");
	
	printf				("Palette construction started\n");
	u64					l_qwStartTime = CPU::GetCycleCount(), l_qwFinishTime;
//	xrPalettizeCovers	(image.pData,image.dwWidth*image.dwHeight);
	vfNeuQuant			(image.pData,image.dwWidth*image.dwHeight);
	l_qwFinishTime		= CPU::GetCycleCount();
	printf				("Palette construction finished\n");
	printf				("Time : %f seconds\n",CPU::cycles2seconds*s64(l_qwFinishTime - l_qwStartTime));
	
	printf				("Saving image...");
	string256			drive,dir,name,ext;
	_splitpath			(argv[1],drive,dir,name,ext);
	strconcat			(drive,dir,name,".tga");
	image.SaveTGA		(drive);
	printf				("completed!\n");
	
	printf				("Finalizing...");
	Core._destroy		();
	printf				("completed!\n");
	return				(0);
}