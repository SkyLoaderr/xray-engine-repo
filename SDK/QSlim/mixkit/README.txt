------------------------------------------------------------------------
		       MixKit Software Library
			     Version 1.1
			     July 4, 2004

		 Michael Garland <garland@uiuc.edu>


 Copyright (C) 1998-2002 Michael Garland.  See "COPYING.txt" for details.


 For the latest version and up-to-date information, please visit
 http://www.uiuc.edu/~garland/software/qslim.html


 Michael Garland
 Department of Computer Science
 University of Illinois at Urbana-Champaign
 201 North Goodwin Avenue
 Urbana, IL 61801

------------------------------------------------------------------------



OVERVIEW
--------

This is the second, and last, public release of the MixKit
software library.  It contains the bulk of the code I developed in the course
of my doctoral research at Carnegie Mellon University.  Since I developed it
for my own research purposes, the code may be a little ugly in places, and the
selection of features has been largely driven by what I needed to get my work
done.  It's also been a place to do a little experimenting.  For instance, the
MxAsp module is an experimental super-minimal scripting interface.

I am releasing this library in the hope that you'll find it useful.  If you
use my software for anything more substantial than toying around, I'd be
interested to hear about it.  Any other comments you'd like to make are also
welcome.  I'd also like to encourage you to contribute bug reports, patches,
and whatever improvements you might come up with to this library.



BUILDING & INSTALLATION
-----------------------

To build on a Unix machine:

  - In order to use the GUI, you will need to have the FLTK library
    <http://www.fltk.org> installed.

  - Support for TIFF image files requires the libtiff library.

  - Running ./configure --help will tell you how to customize the
    compilation of the library.

  - To build the library, just perform the following steps:

	(1) Build the libgfx library in ../libgfx
	(2) ./configure
	(3) cd ../src
	(4) make


To build using Visual C++ under Windows 9x/NT:

        (1a) Use the SlimKit workspace
     or
        (1b) Use the MixKit.dsp project in your own workspace

No further installation is required.  The SlimKit build will
automatically grab the MixKit library from this directory.



DOCUMENTATION
-------------

I'm sorry to say that there is currently only minimal documentation available
for this library (see the 'doc' subdirectory).  You can also glean some
information from the comments in the source files.

For more details, I recommend reading my Ph.D. dissertation, which is also
distributed from my Web page.  This is the most authoritative description of
the simplification algorithms implemented by the MxQSlim classes.  It also
contains an appendix which describes the use of some of the underlying code.



FUTURE PLANS
------------

None.  This software package has changed relatively little since its initial
public release in March 1999.  The only substantial changes were switching to
an FLTK-based GUI and repackaging some of the common code in the new libgfx
library.  I have pretty much ceased active development on the
simplification-related MixKit code.  At this point, I do not have plans for
any further releases of this software.
