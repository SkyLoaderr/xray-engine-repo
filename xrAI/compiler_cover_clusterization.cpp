////////////////////////////////////////////////////////////////////////////
//	Module 		: compielr_cover_clusterization.h
//	Created 	: 21.03.2002
//  Modified 	: 22.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Cover clusterization algorithm
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

struct SPair {
	u32				pair[2];
};

struct SCover {
	u8				cover[4];
};

struct SCoverCluster : public SCover {
	u32				acc_cover[4];
	u32				sort_cover;
	u32				m_magnitude;
	u32				item_count;
	SCoverCluster	*cluster;

	IC u32	distance(const SCoverCluster &node) const
	{
		return		(
			_sqr(int(cover[0]) - int(node.cover[0])) +
			_sqr(int(cover[1]) - int(node.cover[1])) +
			_sqr(int(cover[2]) - int(node.cover[2])) +
			_sqr(int(cover[3]) - int(node.cover[3]))
			);
//		return		_sqr(
//			_abs(int(cover[0]) - int(node.cover[0])) +
//			_abs(int(cover[1]) - int(node.cover[1])) +
//			_abs(int(cover[2]) - int(node.cover[2])) +
//			_abs(int(cover[3]) - int(node.cover[3]))
//			);
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
//		m_magnitude			= _sqr(((u32)cover[0]) + ((u32)cover[1]) + ((u32)cover[2]) + ((u32)cover[3]));
	}

	IC void	compress(const u32 &node)
	{
		cluster				= this;
		item_count			= 1;
		Memory.mem_copy		(cover,&node,sizeof(cover));

		for (int i=0; i<4; ++i) {
			acc_cover[i]	= cover[i];
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
		Msg				("\tSorting vectors...");
		sort				(clusters.begin(),clusters.end(),cover_predicate());
		Msg				("completed!\n\tRemoving duplicates...");
		for (int i=1, j=0, n=clusters.size(); i<n; ++i)
			if (clusters[j]->sort_cover == clusters[i]->sort_cover) {
				clusters[j]->merge(*clusters[i]);
				clusters[i]	= 0;
			}
			else
				j			= i;

		clusters.erase		(remove_if(clusters.begin(),clusters.end(),cover_predicate_equal()),clusters.end());
		Msg				("completed\n\tRemoved duplicates : %d\n\tElements left      : %d\n",n - clusters.size(),clusters.size());
	}

	Msg					("\tSorting magnitudes...");
	sort					(clusters.begin(),clusters.end(),cover_predicate_magnitude());
	Msg					("completed!\n");
	// clasterizing covers
	xr_vector<SPair>		best_pair;
	best_pair.reserve		(clusters.size());
	u32						best_distance;
//	double					iterations = compute_formula(clusters.size(),256);
//	double					portion = iterations/10000;
//	double					accumulator = 0;
//	Msg					("\t\tIteration : %6.2f%%",0.f);
	for (int ii=0, nn = clusters.size(); clusters.size() > 256; ) {
//		if (accumulator >= portion) {
//			for (int j=0; j<32; j++)
//				Msg		("\b \b");
//			Msg			("\t\tIteration : %6.2f%%",100.f*compute_percents(nn,nn - ii,256));
//			accumulator		= 0;
//		}
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

//		accumulator			+= compute_formula(nn - ii,256) - compute_formula(nn - ii - temp.size(),256);
//		ii					+= temp.size();
	}
	
//	for (int j=0; j<32; j++)
//		Msg				("\b \b");
//	Msg					("\t\tIteration : %6.2f%%\n\tTotal clusters : %d\n",100.f,clusters.size());

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

