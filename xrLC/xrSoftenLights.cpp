#include "stdafx.h"
#include "build.h"

#define HEMI1_LIGHTS	26
#define HEMI1_LIGHTS_F	42
#define HEMI2_LIGHTS	91

const double hemi_1[HEMI1_LIGHTS][3] = 
{
	{0.00000,	1.00000,	0.00000	},
	{0.52573,	0.85065,	0.00000	},
	{0.16246,	0.85065,	0.50000	},
	{-0.42533,	0.85065,	0.30902	},
	{-0.42533,	0.85065,	-0.30902},
	{0.16246,	0.85065,	-0.50000},
	{0.89443,	0.44721,	0.00000	},
	{0.27639,	0.44721,	0.85065	},
	{-0.72361,	0.44721,	0.52573	},
	{-0.72361,	0.44721,	-0.52573},
	{0.27639,	0.44721,	-0.85065},
	{0.68819,	0.52573,	0.50000	},
	{-0.26287,	0.52573,	0.80902	},
	{-0.85065,	0.52573,	-0.00000},
	{-0.26287,	0.52573,	-0.80902},
	{0.68819,	0.52573,	-0.50000},
	{0.95106,	0.00000,	0.30902	},
	{0.58779,	0.00000,	0.80902	},
	{-0.00000,	0.00000,	1.00000	},
	{-0.58779,	0.00000,	0.80902	},
	{-0.95106,	0.00000,	0.30902	},
	{-0.95106,	0.00000,	-0.30902},
	{-0.58779,	0.00000,	-0.80902},
	{0.00000,	0.00000,	-1.00000},
	{0.58779,	0.00000,	-0.80902},
	{0.95106,	0.00000,	-0.30902}
};

const double hemi_1F[HEMI1_LIGHTS_F][3] = 
{
	{0.00000,	0.00000,	1.00000	},
	{0.89443,	0.00000,	0.44721	},
	{0.27639,	0.85065,	0.44721	},
	{-0.72361,	0.52573,	0.44721 },
	{-0.72361,	-0.52573,	0.44721 },
	{0.27639,	-0.85065,	0.44721	},
	{0.72361,	0.52573,	-0.44721},
	{-0.27639,	0.85065,	-0.44721},
	{-0.89443,	-0.00000,	-0.44721},
	{-0.27639,	-0.85065,	-0.44721},
	{0.72361,	-0.52573,	-0.44721},
	{0.00000,	0.00000,	-1.00000},
	{0.52573,	0.00000,	0.85065	},
	{0.16246,	0.50000,	0.85065	},
	{-0.42533,	0.30902,	0.85065	},
	{-0.42533,	-0.30902,	0.85065	},
	{0.16246,	-0.50000,	0.85065	},
	{0.68819,	0.50000,	0.52573	},
	{-0.26287,	0.80902,	0.52573	},
	{-0.85065,	-0.00000,	0.52573	},
	{-0.26287,	-0.80902,	0.52573	},
	{0.68819,	-0.50000,	0.52573	},
	{0.95106,	0.30902,	0.00000	},
	{-0.00000,	1.00000,	0.00000	},
	{-0.95106,	0.30902,	0.00000	},
	{-0.58779,	-0.80902,	0.00000	},
	{0.58779,	-0.80902,	0.00000	},
	{0.95106,	-0.30902,	0.00000	},
	{0.58779,	0.80902,	0.00000	},
	{-0.58779,	0.80902,	0.00000	},
	{-0.95106,	-0.30902,	0.00000	},
	{0.00000,	-1.00000,	0.00000	},
	{0.26287,	0.80902,	-0.52573},
	{-0.68819,	0.50000,	-0.52573},
	{-0.68819,	-0.50000,	-0.52573},
	{0.26287,	-0.80902,	-0.52573},
	{0.85065,	0.00000,	-0.52573},
	{0.42533,	0.30902,	-0.85065},
	{-0.16246,	0.50000,	-0.85065},
	{-0.52573,	-0.00000,	-0.85065},
	{-0.16246,	-0.50000,	-0.85065},
	{0.42533,	-0.30902,	-0.85065}
};

const double hemi_2[HEMI2_LIGHTS][3] = 
{
	{0.00000,	0.00000,	1.00000},
	{0.52573,	0.00000,	0.85065},
	{0.16246,	0.50000,	0.85065},
	{-0.42533,	0.30902,	0.85065},
	{-0.42533,	-0.30902,	0.85065},
	{0.16246,	-0.50000,	0.85065},
	{0.89443,	0.00000,	0.44721},
	{0.27639,	0.85065,	0.44721},
	{-0.72361,	0.52573,	0.44721},
	{-0.72361,	-0.52573,	0.44721},
	{0.27639,	-0.85065,	0.44721},
	{0.68819,	0.50000	,	0.52573},
	{-0.26287,	0.80902	,	0.52573},
	{-0.85065,	-0.00000,	0.52573},
	{-0.26287,	-0.80902,	0.52573},
	{0.68819,	-0.50000,	0.52573},
	{0.95106,	0.30902	,	0.00000},
	{0.58779,	0.80902	,	0.00000},
	{-0.00000,	1.00000	,	0.00000},
	{-0.58779,	0.80902	,	0.00000},
	{-0.95106,	0.30902	,	0.00000},
	{-0.95106,	-0.30902,	0.00000},
	{-0.58779,	-0.80902,	0.00000},
	{0.00000,	-1.00000,	0.00000},
	{0.58779,	-0.80902,	0.00000},
	{0.95106,	-0.30902,	0.00000},
	{0.27327,	0.00000	,	0.96194},
	{0.08444,	0.25989	,	0.96194},
	{-0.22108,	0.16062	,	0.96194},
	{-0.22108,	-0.16062,	0.96194},
	{0.08444,	-0.25989,	0.96194},
	{0.36180,	0.26287	,	0.89443},
	{-0.13820,	0.42533	,	0.89443},
	{-0.44721,	-0.00000,	0.89443},
	{-0.13820,	-0.42533,	0.89443},
	{0.36180,	-0.26287,	0.89443},
	{0.73818,	0.00000	,	0.67461},
	{0.22811,	0.70205	,	0.67461},
	{-0.59720,	0.43389	,	0.67461},
	{-0.59720,	-0.43389,	0.67461},
	{0.22811,	-0.70205,	0.67461},
	{0.63820,	0.26287	,	0.72361},
	{-0.05279,	0.68819	,	0.72361},
	{-0.67082,	0.16246	,	0.72361},
	{-0.36180,	-0.58779,	0.72361},
	{0.44721,	-0.52573,	0.72361},
	{0.63820,	-0.26287,	0.72361},
	{0.44721,	0.52573	,	0.72361},
	{-0.36180,	0.58779	,	0.72361},
	{-0.67082,	-0.16246,	0.72361},
	{-0.05279,	-0.68819,	0.72361},
	{0.82262,	0.25989	,	0.50572},
	{0.00703,	0.86267	,	0.50572},
	{-0.81827,	0.27327	,	0.50572},
	{-0.51275,	-0.69378,	0.50572},
	{0.50138,	-0.70205,	0.50572},
	{0.82262,	-0.25989,	0.50572},
	{0.50138,	0.70205	,	0.50572},
	{-0.51275,	0.69378	,	0.50572},
	{-0.81827,	-0.27327,	0.50572},
	{0.00703,	-0.86267,	0.50572},
	{0.95925,	0.16062	,	0.23245},
	{0.86180,	0.42533	,	0.27639},
	{0.67082,	0.68819	,	0.27639},
	{0.44919,	0.86267	,	0.23245},
	{0.14366,	0.96194	,	0.23245},
	{-0.13820,	0.95106	,	0.27639},
	{-0.44721,	0.85065	,	0.27639},
	{-0.68164,	0.69378	,	0.23245},
	{-0.87046,	0.43389	,	0.23245},
	{-0.94721,	0.16246	,	0.27639},
	{-0.94721,	-0.16246,	0.27639},
	{-0.87046,	-0.43389,	0.23245},
	{-0.68164,	-0.69378,	0.23245},
	{-0.44721,	-0.85065,	0.27639},
	{-0.13820,	-0.95106,	0.27639},
	{0.14366,	-0.96194,	0.23245},
	{0.44919,	-0.86267,	0.23245},
	{0.67082,	-0.68819,	0.27639},
	{0.86180,	-0.42533,	0.27639},
	{0.95925,	-0.16062,	0.23245},
	{0.80902,	0.58779	,	0.00000},
	{0.30902,	0.95106	,	0.00000},
	{-0.30902,	0.95106	,	0.00000},
	{-0.80902,	0.58779	,	0.00000},
	{-1.00000,	-0.00000,	0.00000},
	{-0.80902,	-0.58779,	0.00000},
	{-0.30902,	-0.95106,	0.00000},
	{0.30902,	-0.95106,	0.00000},
	{0.80902,	-0.58779,	0.00000},
	{1.00000,	0.00000	,	0.00000}
};

/*
			*MESH_VERTEX    0	0.00000	0.00000	1.00000
			*MESH_VERTEX    1	0.89443	0.00000	0.44721
			*MESH_VERTEX    2	0.27639	0.85065	0.44721
			*MESH_VERTEX    3	-0.72361	0.52573	0.44721
			*MESH_VERTEX    4	-0.72361	-0.52573	0.44721
			*MESH_VERTEX    5	0.27639	-0.85065	0.44721
			*MESH_VERTEX    6	0.72361	0.52573	-0.44721
			*MESH_VERTEX    7	-0.27639	0.85065	-0.44721
			*MESH_VERTEX    8	-0.89443	-0.00000	-0.44721
			*MESH_VERTEX    9	-0.27639	-0.85065	-0.44721
			*MESH_VERTEX   10	0.72361	-0.52573	-0.44721
			*MESH_VERTEX   11	0.00000	0.00000	-1.00000
			*MESH_VERTEX   12	0.27327	0.00000	0.96194
			*MESH_VERTEX   13	0.52573	0.00000	0.85065
			*MESH_VERTEX   14	0.73818	0.00000	0.67461
			*MESH_VERTEX   15	0.08444	0.25989	0.96194
			*MESH_VERTEX   16	0.16246	0.50000	0.85065
			*MESH_VERTEX   17	0.22811	0.70205	0.67461
			*MESH_VERTEX   18	-0.22108	0.16062	0.96194
			*MESH_VERTEX   19	-0.42533	0.30902	0.85065
			*MESH_VERTEX   20	-0.59720	0.43389	0.67461
			*MESH_VERTEX   21	-0.22108	-0.16062	0.96194
			*MESH_VERTEX   22	-0.42533	-0.30902	0.85065
			*MESH_VERTEX   23	-0.59720	-0.43389	0.67461
			*MESH_VERTEX   24	0.08444	-0.25989	0.96194
			*MESH_VERTEX   25	0.16246	-0.50000	0.85065
			*MESH_VERTEX   26	0.22811	-0.70205	0.67461
			*MESH_VERTEX   27	0.82262	0.25989	0.50572
			*MESH_VERTEX   28	0.68819	0.50000	0.52573
			*MESH_VERTEX   29	0.50138	0.70205	0.50572
			*MESH_VERTEX   30	0.00703	0.86267	0.50572
			*MESH_VERTEX   31	-0.26287	0.80902	0.52573
			*MESH_VERTEX   32	-0.51275	0.69378	0.50572
			*MESH_VERTEX   33	-0.81827	0.27327	0.50572
			*MESH_VERTEX   34	-0.85065	-0.00000	0.52573
			*MESH_VERTEX   35	-0.81827	-0.27327	0.50572
			*MESH_VERTEX   36	-0.51275	-0.69378	0.50572
			*MESH_VERTEX   37	-0.26287	-0.80902	0.52573
			*MESH_VERTEX   38	0.00703	-0.86267	0.50572
			*MESH_VERTEX   39	0.50138	-0.70205	0.50572
			*MESH_VERTEX   40	0.68819	-0.50000	0.52573
			*MESH_VERTEX   41	0.82262	-0.25989	0.50572
			*MESH_VERTEX   42	0.95925	0.16062	0.23245
			*MESH_VERTEX   43	0.95106	0.30902	0.00000
			*MESH_VERTEX   44	0.87046	0.43389	-0.23245
			*MESH_VERTEX   45	0.14366	0.96194	0.23245
			*MESH_VERTEX   46	-0.00000	1.00000	0.00000
			*MESH_VERTEX   47	-0.14366	0.96194	-0.23245
			*MESH_VERTEX   48	-0.87046	0.43389	0.23245
			*MESH_VERTEX   49	-0.95106	0.30902	0.00000
			*MESH_VERTEX   50	-0.95925	0.16062	-0.23245
			*MESH_VERTEX   51	-0.68164	-0.69378	0.23245
			*MESH_VERTEX   52	-0.58779	-0.80902	0.00000
			*MESH_VERTEX   53	-0.44919	-0.86267	-0.23245
			*MESH_VERTEX   54	0.44919	-0.86267	0.23245
			*MESH_VERTEX   55	0.58779	-0.80902	0.00000
			*MESH_VERTEX   56	0.68164	-0.69378	-0.23245
			*MESH_VERTEX   57	0.95925	-0.16062	0.23245
			*MESH_VERTEX   58	0.95106	-0.30902	0.00000
			*MESH_VERTEX   59	0.87046	-0.43389	-0.23245
			*MESH_VERTEX   60	0.44919	0.86267	0.23245
			*MESH_VERTEX   61	0.58779	0.80902	0.00000
			*MESH_VERTEX   62	0.68164	0.69378	-0.23245
			*MESH_VERTEX   63	-0.68164	0.69378	0.23245
			*MESH_VERTEX   64	-0.58779	0.80902	0.00000
			*MESH_VERTEX   65	-0.44919	0.86267	-0.23245
			*MESH_VERTEX   66	-0.87046	-0.43389	0.23245
			*MESH_VERTEX   67	-0.95106	-0.30902	0.00000
			*MESH_VERTEX   68	-0.95925	-0.16062	-0.23245
			*MESH_VERTEX   69	0.14366	-0.96194	0.23245
			*MESH_VERTEX   70	0.00000	-1.00000	0.00000
			*MESH_VERTEX   71	-0.14366	-0.96194	-0.23245
			*MESH_VERTEX   72	0.51275	0.69378	-0.50572
			*MESH_VERTEX   73	0.26287	0.80902	-0.52573
			*MESH_VERTEX   74	-0.00703	0.86267	-0.50572
			*MESH_VERTEX   75	-0.50138	0.70205	-0.50572
			*MESH_VERTEX   76	-0.68819	0.50000	-0.52573
			*MESH_VERTEX   77	-0.82262	0.25989	-0.50572
			*MESH_VERTEX   78	-0.82262	-0.25989	-0.50572
			*MESH_VERTEX   79	-0.68819	-0.50000	-0.52573
			*MESH_VERTEX   80	-0.50138	-0.70205	-0.50572
			*MESH_VERTEX   81	-0.00703	-0.86267	-0.50572
			*MESH_VERTEX   82	0.26287	-0.80902	-0.52573
			*MESH_VERTEX   83	0.51275	-0.69378	-0.50572
			*MESH_VERTEX   84	0.81827	-0.27327	-0.50572
			*MESH_VERTEX   85	0.85065	0.00000	-0.52573
			*MESH_VERTEX   86	0.81827	0.27327	-0.50572
			*MESH_VERTEX   87	0.22108	0.16062	-0.96194
			*MESH_VERTEX   88	0.42533	0.30902	-0.85065
			*MESH_VERTEX   89	0.59720	0.43389	-0.67461
			*MESH_VERTEX   90	-0.08444	0.25989	-0.96194
			*MESH_VERTEX   91	-0.16246	0.50000	-0.85065
			*MESH_VERTEX   92	-0.22811	0.70205	-0.67461
			*MESH_VERTEX   93	-0.27327	-0.00000	-0.96194
			*MESH_VERTEX   94	-0.52573	-0.00000	-0.85065
			*MESH_VERTEX   95	-0.73818	-0.00000	-0.67461
			*MESH_VERTEX   96	-0.08444	-0.25989	-0.96194
			*MESH_VERTEX   97	-0.16246	-0.50000	-0.85065
			*MESH_VERTEX   98	-0.22811	-0.70205	-0.67461
			*MESH_VERTEX   99	0.22108	-0.16062	-0.96194
			*MESH_VERTEX  100	0.42533	-0.30902	-0.85065
			*MESH_VERTEX  101	0.59720	-0.43389	-0.67461
			*MESH_VERTEX  102	0.36180	0.26287	0.89443
			*MESH_VERTEX  103	0.61786	0.26468	0.74040
			*MESH_VERTEX  104	0.44265	0.50583	0.74040
			*MESH_VERTEX  105	-0.13820	0.42533	0.89443
			*MESH_VERTEX  106	-0.06079	0.66941	0.74040
			*MESH_VERTEX  107	-0.34429	0.57730	0.74040
			*MESH_VERTEX  108	-0.44721	-0.00000	0.89443
			*MESH_VERTEX  109	-0.65544	0.14904	0.74040
			*MESH_VERTEX  110	-0.65544	-0.14904	0.74040
			*MESH_VERTEX  111	-0.13820	-0.42533	0.89443
			*MESH_VERTEX  112	-0.34429	-0.57730	0.74040
			*MESH_VERTEX  113	-0.06079	-0.66941	0.74040
			*MESH_VERTEX  114	0.36180	-0.26287	0.89443
			*MESH_VERTEX  115	0.44265	-0.50583	0.74040
			*MESH_VERTEX  116	0.61786	-0.26468	0.74040
			*MESH_VERTEX  117	1.00000	0.00000	0.00000
			*MESH_VERTEX  118	0.95535	-0.14904	-0.25512
			*MESH_VERTEX  119	0.95535	0.14904	-0.25512
			*MESH_VERTEX  120	0.30902	0.95106	0.00000
			*MESH_VERTEX  121	0.43697	0.86254	-0.25512
			*MESH_VERTEX  122	0.15347	0.95465	-0.25512
			*MESH_VERTEX  123	-0.80902	0.58779	0.00000
			*MESH_VERTEX  124	-0.68529	0.68212	-0.25512
			*MESH_VERTEX  125	-0.86050	0.44096	-0.25512
			*MESH_VERTEX  126	-0.80902	-0.58779	0.00000
			*MESH_VERTEX  127	-0.86050	-0.44096	-0.25512
			*MESH_VERTEX  128	-0.68529	-0.68212	-0.25512
			*MESH_VERTEX  129	0.30902	-0.95106	0.00000
			*MESH_VERTEX  130	0.15347	-0.95465	-0.25512
			*MESH_VERTEX  131	0.43697	-0.86254	-0.25512
			*MESH_VERTEX  132	0.80902	0.58779	0.00000
			*MESH_VERTEX  133	0.68529	0.68212	0.25512
			*MESH_VERTEX  134	0.86050	0.44096	0.25512
			*MESH_VERTEX  135	-0.30902	0.95106	0.00000
			*MESH_VERTEX  136	-0.43697	0.86254	0.25512
			*MESH_VERTEX  137	-0.15347	0.95465	0.25512
			*MESH_VERTEX  138	-1.00000	-0.00000	0.00000
			*MESH_VERTEX  139	-0.95535	-0.14904	0.25512
			*MESH_VERTEX  140	-0.95535	0.14904	0.25512
			*MESH_VERTEX  141	-0.30902	-0.95106	0.00000
			*MESH_VERTEX  142	-0.15347	-0.95465	0.25512
			*MESH_VERTEX  143	-0.43697	-0.86254	0.25512
			*MESH_VERTEX  144	0.80902	-0.58779	0.00000
			*MESH_VERTEX  145	0.86050	-0.44096	0.25512
			*MESH_VERTEX  146	0.68529	-0.68212	0.25512
			*MESH_VERTEX  147	0.13820	0.42533	-0.89443
			*MESH_VERTEX  148	0.06079	0.66941	-0.74040
			*MESH_VERTEX  149	0.34429	0.57730	-0.74040
			*MESH_VERTEX  150	-0.36180	0.26287	-0.89443
			*MESH_VERTEX  151	-0.61786	0.26468	-0.74040
			*MESH_VERTEX  152	-0.44265	0.50583	-0.74040
			*MESH_VERTEX  153	-0.36180	-0.26287	-0.89443
			*MESH_VERTEX  154	-0.44265	-0.50583	-0.74040
			*MESH_VERTEX  155	-0.61786	-0.26468	-0.74040
			*MESH_VERTEX  156	0.13820	-0.42533	-0.89443
			*MESH_VERTEX  157	0.34429	-0.57730	-0.74040
			*MESH_VERTEX  158	0.06079	-0.66941	-0.74040
			*MESH_VERTEX  159	0.44721	0.00000	-0.89443
			*MESH_VERTEX  160	0.65544	0.14904	-0.74040
			*MESH_VERTEX  161	0.65544	-0.14904	-0.74040
 */

void CBuild::SoftenLights()
{
	Status	("Jittering lights...");
	b_light*	L = lights_lmaps.begin();
	R_Light		RL;
	for (;L!=lights_lmaps.end(); L++)
	{
		Progress				(float(L-lights_lmaps.begin())/float(lights_lmaps.size()));

		// generic properties
		RL.diffuse.normalize_rgb	(L->diffuse);
		RL.position.set				(L->position);
		RL.direction.normalize_safe	(L->direction);
		RL.range				=	L->range*1.1f;
		RL.range2				=	RL.range*RL.range;
		RL.attenuation0			=	L->attenuation0;
		RL.attenuation1			=	L->attenuation1;
		RL.attenuation2			=	L->attenuation2;
		RL.energy				=	L->diffuse.magnitude_rgb();
		
		// select destination container
		vector<R_Light>* dest	=	0;
		if (L->flags.bProcedural)	{
			// one of the procedural lights
			lights.push_back		( b_LightLayer() );
			lights.back().original	= L;
			dest					= &(lights.back().lights);		
			RL.diffuse.set			(1,1,1,1);
			RL.energy				= RL.diffuse.magnitude_rgb();
			RL.diffuse.normalize_rgb();
		} else {
			// ambient (fully-static)
			dest					= &(lights.front().lights);
		}

		if (L->type==D3DLIGHT_DIRECTIONAL) 
		{
			RL.type				= LT_DIRECT;
			R_Light	T			= RL;
			Fmatrix				rot_y;

			Fvector				v_top,v_right,v_dir;
			v_top.set			(0,1,0);
			v_dir.set			(RL.direction);
			v_right.crossproduct(v_top,v_dir);
			v_right.normalize	();

			// Build jittered light
			T.energy			= RL.energy/14.f;
			float angle			= deg2rad(g_params.area_dispersion);
			{
				// *** center
				dest->push_back	(T);

				// *** left
				rot_y.rotateY			(3*angle/4);
				rot_y.transform_dir		(T.direction,RL.direction);
				dest->push_back	(T);

				rot_y.rotateY			(2*angle/4);
				rot_y.transform_dir		(T.direction,RL.direction);
				dest->push_back	(T);

				rot_y.rotateY			(1*angle/4);
				rot_y.transform_dir		(T.direction,RL.direction);
				dest->push_back	(T);

				// *** right
				rot_y.rotateY			(-1*angle/4);
				rot_y.transform_dir		(T.direction,RL.direction);
				dest->push_back	(T);

				rot_y.rotateY			(-2*angle/4);
				rot_y.transform_dir		(T.direction,RL.direction);
				dest->push_back	(T);

				rot_y.rotateY			(-3*angle/4);
				rot_y.transform_dir		(T.direction,RL.direction);
				dest->push_back	(T);
				
				// *** top 
				rot_y.rotation			(v_right, 3*angle/4);
				rot_y.transform_dir		(T.direction,RL.direction);
				dest->push_back	(T);

				rot_y.rotation			(v_right, 2*angle/4);
				rot_y.transform_dir		(T.direction,RL.direction);
				dest->push_back	(T);

				rot_y.rotation			(v_right, 1*angle/4);
				rot_y.transform_dir		(T.direction,RL.direction);
				dest->push_back	(T);

				// *** bottom
				rot_y.rotation			(v_right,-1*angle/4);
				rot_y.transform_dir		(T.direction,RL.direction);
				dest->push_back	(T);

				rot_y.rotation			(v_right,-2*angle/4);
				rot_y.transform_dir		(T.direction,RL.direction);
				dest->push_back	(T);

				rot_y.rotation			(v_right,-3*angle/4);
				rot_y.transform_dir		(T.direction,RL.direction);
				dest->push_back	(T);
			}

			// Build area-lights
			if (g_params.area_quality)	
			{
				int		h_count, h_table[3];
				const double (*hemi)[3] = 0;
				if	(g_params.area_quality==1)
				{
					h_count = HEMI1_LIGHTS;
					h_table[0]	= 0;
					h_table[1]	= 1;
					h_table[2]	= 2;
					hemi	= hemi_1;
				} else {
					h_count = HEMI2_LIGHTS;
					h_table[0]	= 0;
					h_table[1]	= 2;
					h_table[2]	= 1;
					hemi	= hemi_2;
				}

				T.energy				= (g_params.area_energy_summary)/float(h_count);
				Fcolor	cLight;
				cLight.normalize_rgb	(L->diffuse);
				
				Fvector		base		= RL.direction;
				base.normalize			();
				for (int i=0; i<h_count; i++)
				{
					T.direction.set			(float(hemi[i][h_table[0]]),float(hemi[i][h_table[1]]),float(hemi[i][h_table[2]]));
					T.direction.invert		();
					T.direction.normalize	();
					
					T.diffuse.set			(g_params.area_color);
					
					dest->push_back	(T);
				}
			}
		} else {
			RL.type			= LT_POINT;
			if (g_params.fuzzy_enable)	{
				// Perform jittering
				R_Light	T		= RL;
				T.energy		= RL.energy/float(g_params.fuzzy_samples);
				
				XRC.RayMode		(RAY_ONLYNEAREST);
				for (int i=0; i<g_params.fuzzy_samples; i++)
				{
					// dir & dist
					Fvector			R;
					R.random_dir	();
					float dist		= ::Random.randF(g_params.fuzzy_min,g_params.fuzzy_max);
					
					// check collision
					XRC.RayPick		(0,&RCAST_Model,RL.position,R,dist);
					if (XRC.GetRayContactCount()) {
						const RAPID::raypick_info* I = XRC.GetMinRayPickInfo();
						dist = I->range;
					}
					
					// calculate point
					T.position.direct		(RL.position,R,dist*.5f);
					dest->push_back	(T);
				}
			} else {
				dest->push_back(RL);
			}
		}
	}
	Msg("* Total light-layers: %d",lights.size());

	Status("Caching faces opacity...");
	for (vecFaceIt I=g_faces.begin(); I!=g_faces.end(); I++) (*I)->CacheOpacity();
}
