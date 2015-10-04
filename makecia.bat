@echo off
for %%d in (.) do set PROJNAME=%%~nd
set SHORTNAME=Graphing Calculator
set LONGNAME=Graphing Calculator
set PRODUCT_CODE=%PROJNAME%
set PUBNAME=flarn2006
set UNIQUE_ID=0xf7183

copy /y template.rsf cia.rsf >NUL
sed -i "s/@@TITLE@@/%SHORTNAME%/g" cia.rsf
sed -i "s/@@PRODUCT_CODE@@/%PRODUCT_CODE%/g" cia.rsf
sed -i "s/@@UNIQUE_ID@@/%UNIQUE_ID%/g" cia.rsf

echo on

make
arm-none-eabi-strip %PROJNAME%.elf -o stripped.elf
bannertool makebanner -i banner.png -a audio.wav -o banner.bin
bannertool makesmdh -s "%SHORTNAME%" -l "%LONGNAME%" -p "%PUBNAME%" -i icon.png -o cia.smdh -r regionfree -f visible
makerom -elf stripped.elf -icon cia.smdh -banner banner.bin -rsf cia.rsf -o %PROJNAME%.cxi
makerom -f cia -content %PROJNAME%.cxi:0:0 -rsf cia.rsf