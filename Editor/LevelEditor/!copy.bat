@echo off
del !archiv\ed.rar
start /WAIT winrar a -s -m5 -md512 !archiv\ed.rar *.cpp *.h *.dfm *.bpg *.bpr *.res *.txt edit\*.cpp edit\*.h edit\*.txt engine\*.cpp engine\*.h engine\blenders\*.cpp engine\blenders\*.h *.bat !run\game\*.xr
del Z:\Projects\XRE_2\!archiv\ed.rar
copy !archiv\ed.rar Z:\Projects\XRE_2\!archiv\
pause
del a:\ed.rar
copy !archiv\ed.rar a:
echo insert new diskette
pause
del a:\ed.rar
copy !archiv\ed.rar a:
