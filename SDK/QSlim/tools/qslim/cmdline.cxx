/************************************************************************

  This file implements the command line parsing interface for QSlim.

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.
  
  $Id: cmdline.cxx,v 1.17 1999/03/17 17:00:14 garland Exp $

 ************************************************************************/

#include <stdmix.h>
#include <mixio.h>

#ifdef HAVE_UNISTD_H
#  include <unistd.h>
#else
#  include <getopt.h>
#endif

#include "qslim.h"

static char *options = "O:B:W:t:ao:Fo:Po:m:c:rjI:M:qh";

static char *usage_string =
"-O <n>         Optimal placement policy:\n"
"                       0=endpoints, 1=endormid, 2=line, 3=optimal [default]\n"
"-B <weight>    Use boundary preservation planes with given weight.\n"
"-W <n>         Quadric weighting policy:\n"
"                       0=uniform, 1=area [default], 2=angle\n"
"-t <n>         Set the target number of faces.\n"
"-F             Use face contraction instead of edge contraction.\n"
"-o <file>      Output final model to the given file.\n"
"-I <file>      Deferred file inclusion.\n"
"-m <penalty>   Set the penalty for bad meshes.\n"
"-c <ratio>     Set the desired compactness ratio.\n"
"-r             Enable history recording.\n"
"-M <format>    Select output format:\n"
"                       {smf, iv, vrml, pm, mmf, log}\n"
"-q		Be quiet.\n"
"-j             Join only; do not remove any faces.\n"
"-a				Arcball camera usage.\n"
"-h             Print help.\n"
"\n";

static char **global_argv;

static void print_usage()
{
    cerr << endl;
    slim_print_banner(cerr);
    cerr << endl << "usage: " << global_argv[0]
	 << " <options> [filename]" << endl;
    cerr << endl
         << "Available options:" << endl
         << usage_string << endl;
}

static void usage_error(char *msg=NULL)
{
    if( msg )  cerr << "Error: " << msg << endl;
    print_usage();
    exit(1);
}

void process_cmdline(int argc, char **argv)
{
    int opt, ival;

    global_argv = argv;


    while( (opt = getopt(argc, argv, options)) != EOF )
    {
	switch( opt )
	{
	case 'a':
		cam_mode = 1;
		break;

	case 'O':
	    ival = atoi(optarg);
	    if( ival<MX_PLACE_ENDPOINTS || ival>MX_PLACE_OPTIMAL )
		usage_error("Illegal optimization policy.");
	    else  placement_policy = ival;
	    break;

	case 'W':
	    ival = atoi(optarg);
	    if( ival<MX_WEIGHT_UNIFORM || ival>MX_WEIGHT_RAWNORMALS )
		usage_error("Illegal weighting policy.");
	    else weighting_policy = ival;
	    break;

	case 'M':
	    if( !select_output_format(optarg) )
		usage_error("Unknown output format selected.");
	    break;

	case 'B':  boundary_weight = atof(optarg); break;
	case 't':  face_target = atoi(optarg); break;
	case 'F':  slim_mode = 1; break;
	case 'P':  slim_mode = 2; break;
	case 'o':  output_filename = optarg; break;
	case 'I':  defer_file_inclusion(optarg); break;
	case 'm':  meshing_penalty = atof(optarg); break;
	case 'c':  compactness_ratio = atof(optarg); break;
	case 'r':  will_record_history = true; break;
	case 'j':  will_join_only = true; break;
	case 'q':  be_quiet = true; break;
	case 'h':  print_usage(); exit(0); break;

	default:   usage_error("Malformed command line."); break;
	}
    }

    smf->unparsed_hook = unparsed_hook;
    m = new MxStdModel(100, 100);

    for(;optind < argc;optind++)
	input_file(argv[optind]);
}
