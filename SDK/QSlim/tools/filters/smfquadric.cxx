/************************************************************************

  smfquadric

  This filter can be used to generate quadric surfaces.  It is targeted
  towards constructing surfaces of the form:

  		z = 1/2 (k1*x^2 + k2*y^2)

  from successive polygonal rings.

  Copyright (C) 1998 Michael Garland, All Rights Reserved.
  
  $Id: smfquadric.cxx,v 1.3 1998/10/27 01:05:45 garland Exp $

 ************************************************************************/

#include <stdmix.h>
#include "cmdline.h"

static uint ring_count = 40;
static uint N = 24;
static bool focus_out = true;

static float k1 = 1;
static float k2 = 1;
static float extent = 1.0;

static char *options = "x:y:t:r:n:fh";
static char *usage_string =
"-x <k1>	Set the curvature along the X axis.\n"
"-y <k2>	Set the curvature along the Y axis.\n"
"-t <r>		Set the radial extent of the surface.\n"
"-r <n>		Set the number of rings generated.\n"
"-n <n>		Set the number of slices generated.\n"
"-f		Flip the normal orientation.\n"
"\n";

static
void process_options(int opt, char *optarg)
{
    switch( opt )
    {
    case 'x':  k1 = atof(optarg); break;
    case 'y':  k2 = atof(optarg); break;
    case 't':  extent = atof(optarg); break;
    case 'r':  ring_count = atoi(optarg); break;
    case 'n':  N = atoi(optarg); break;
    case 'f':  focus_out = !focus_out; break;
    case 'h':
	cerr << endl << usage_string << endl; exit(0); break;
    }
}

static
void vertex(MxStdModel *m, float x, float y)
{
    float g = sqrt(1 + k1*k1*x*x + k2*k2*y*y);

    m->add_vertex(x, y, 0.5*(k1*x*x + k2*y*y));

    if( focus_out )  m->add_normal( k1*x/g,  k2*y/g, -1/g);
    else             m->add_normal(-k1*x/g, -k2*y/g,  1/g);
}

static
void face(MxStdModel *m, uint v1, uint v2, uint v3)
{
    if( focus_out )  m->add_face(v1, v3, v2);
    else             m->add_face(v1, v2, v3);
}

static
MxStdModel *generate_quadric()
{
    MxStdModel *m = new MxStdModel(64, 64);
    m->normal_binding(MX_PERVERTEX);

    uint i,j;

    // Set up the variables we need to keep track of where we are
    uint base_vertex = 1;
    float R = extent/ring_count;

    if( k1<0 && k2<0 ) focus_out = !focus_out;

    // Output the origin
    vertex(m, 0.0, 0.0);	// vertex 0

    // Output the vertices of the first loop
    for(j=0; j<N; j++)
    {
	float theta = 2*j*M_PI/N;
	vertex(m, R*cos(theta), R*sin(theta));
    }
    //
    // Output the faces of the first loop
    for(j=0; j<N; j++)
	face(m, 0, base_vertex + j, base_vertex + ((j+1)%N));


    //
    // Output all subsequent loops
    for(i=1; i<ring_count; i++)
    {
	R += extent/ring_count;
	base_vertex += N;

	// Output new vertices
	for(j=0; j<N; j++)
	{
	    float theta = 2*j*M_PI/N;
	    float x = R*cos(theta);
	    float y = R*sin(theta);
	    vertex(m, x, y);
	}

	// Output faces joining to previous layer
	for(j=0; j<N; j++)
	{
	    uint j2 = (j+1)%N;
	    uint p = base_vertex + j - N;
	    uint p2 = base_vertex + j2 - N;

	    face(m, base_vertex + j, base_vertex + j2, p);
	    face(m, base_vertex + j2, p2, p);
	}
    }

    return m;
}

main(int argc, char *argv[])
{
    process_cmdline_only(argc, argv, options, process_options);

    MxStdModel *m = generate_quadric();

    output_final_model(m);
    delete(m);
    return 0;
}
