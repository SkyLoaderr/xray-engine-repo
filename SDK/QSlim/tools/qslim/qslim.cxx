/************************************************************************

  QSlim command line program.  This provides a very simple interface to
  the underlying functionality.  Basically, it just reads in the input,
  simplifies it, and writes out the results.  It couldn't be simpler.

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.
  
  $Id: qslim.cxx,v 1.10 2000/11/20 20:52:41 garland Exp $

 ************************************************************************/

#include <stdmix.h>
#include <MxTimer.h>
#include "qslim.h"

static ostream& vfcount(ostream& out, uint v, uint f)
{
    return out << "(" << v << "v/" << f << "f)";
}

void startup_and_input(int argc, char **argv)
{
    smf = new MxSMFReader;

    process_cmdline(argc, argv);
    if( m->face_count() == 0 )
    {
	smf->read(cin, m);
    }

    output_preamble();
}

main(int argc, char **argv)
{
    double input_time, init_time, slim_time, output_time;

    // Process command line and read input model(s)
    //
    TIMING(input_time, startup_and_input(argc, argv));

    if(!be_quiet) cerr << "+ Initial model    ";
    if(!be_quiet) vfcount(cerr, m->vert_count(), m->face_count()) << endl;

    // Initial simplification process.  Collect contractions and build heap.
    //
    TIMING(init_time, slim_init());

    // Decimate model until target is reached
    //
    TIMING(slim_time, slim->decimate(face_target));

    if(!be_quiet) cerr << "+ Simplified model ";
    if(!be_quiet) vfcount(cerr, slim->valid_verts, slim->valid_faces) << endl;

    // Output the result
    //
    TIMING(output_time, output_final_model());

    if( !be_quiet )
    {
	cerr << endl << endl;
	cerr << "+ Running time" << endl;
	cerr << "    Setup      : " << input_time << " sec" << endl;
	cerr << "    QSlim init : " << init_time << " sec" << endl;
	cerr << "    QSlim run  : " << slim_time << " sec" << endl;
	cerr << "    Output     : " << output_time << " sec" << endl;
	cerr << endl;
	cerr << "    Total      : "
	     << input_time+init_time+slim_time+output_time <<endl;
    }
    else
    {
	cerr << slim->valid_faces << " " << init_time+slim_time << endl;
    }

    slim_cleanup();

    return 0;
}
