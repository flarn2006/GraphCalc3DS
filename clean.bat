@echo off
for %%d in (.) do set PROJNAME=%%~nd
rmdir /s /q build
del banner.bin
del cia.smdh
del stripped.elf
del %PROJNAME%.3dsx
del %PROJNAME%.cia
del %PROJNAME%.cxi
del %PROJNAME%.elf
del %PROJNAME%.smdh
del cia.rsf