DirectX 8 Borland LIBs (OMF format)
-----------------------------------
Updated Jan. 28, 2001: dxerr8.lib and dxerr8bor.dll included.
Updated Feb. 10, 2001: "c_dfDI..." structures added to dinput.lib
and dinput8.lib. c_dinput.cpp added.
Updated Feb. 24, 2001: "c_dfDI..." structures removed from
dinput.lib and dinput8.lib. c_dinput.cpp updated. c_dinput.lib
added.
Updated Apr. 15, 2001: New d3dx8bor.dll wrapper DLL for the
version 8.0b release of the D3DX static libraries. (See
http://www.microsoft.com/downloads/release.asp?ReleaseID=29079
for more information on D3DX 8.0b.)
Updated May 5, 2001: Added qedit.lib and dxtrans.lib

The LIB files in this archive are conversions of or
replacements for the COFF LIBs in the DirectX 8 SDK. Most of
the LIBs were created by using the ImpLib command-line utility.
To create the GUID LIBs (dxguid.lib, amstrmid.lib, etc.), I
compiled generated code listings containing the GUID
definitions with C++Builder. I wrapped up the other static
libraries in Visual C++ DLLs and created import libraries for
the DLLs.

The following LIBs are included in this archive:
amstrmid.lib
c_dinput.lib
d3d8.lib
d3dx.lib
d3dx8.lib
d3dx8d.lib
d3dxd.lib
d3dxof.lib
ddraw.lib
dinput.lib
dinput8.lib
dmoguids.lib
dplayx.lib
dpnaddr.lib
dpnet.lib
dpnlobby.lib
dpvoice.lib
dsetup.lib
dsound.lib
dxerr8.lib
dxguid.lib
dxtrans.lib
ksproxy.lib
ksuser.lib
msdmo.lib
qedit.lib
quartz.lib
strmiids.lib

Please note that the replacements for the D3DX static libraries
(d3dx.lib, d3dxd.lib, and d3dx8.lib) are import libraries for
wrapper DLLs (d3dxbor.dll, d3dxdbor.dll, and d3dx8bor.dll,
respectively). The wrapper DLLs were created using Visual C++
and are included in this archive. If you link to d3dx.lib,
d3dxd.lib, or d3dx8.lib, you must ship the corresponding DLLs
with your application. This is also true for dxerr8.lib.
A replacement for the D3DX 8 static debug library (d3dx8dt.lib)
is not included. Use d3dx8d.lib, the import library for the
D3DX 8 debug DLL, instead. The DLL is part of the DirectX 8
SDK.

Note: dinput.lib and dinput8.lib do not contain the "c_dfDI..."
global instances of the DIDATAFORMAT structure. To use these
structures, add c_dinput.cpp to your project or link to
c_dinput.lib.

You are free to use the LIBs and DLLs in this archive to create
your DirectX applications. You may redistribute this archive
(borland_dx8_libs.zip) as long as you do not modify its contents
or provide misleading information about its origin.



URL of this archive:
http://www.geocities.com/foetsch/borland_dx8_libs.zip
Visit my homepage: http://www.geocities.com/foetsch
Contact me: foetsch@crosswinds.net

Copyright © 2001 Michael Fötsch
