#!/bin/bash
PROJNAME=$(basename $PWD)
SHORTNAME=$PROJNAME
LONGNAME=$PROJNAME
PRODUCT_CODE=$PROJNAME
PUBNAME='flarn2006'
UNIQUE_ID='0x12345'

cp template.rsf cia.rsf
sed -i "s/@@TITLE@@/$SHORTNAME/g" cia.rsf
sed -i "s/@@PRODUCT_CODE@@/$PRODUCT_CODE/g" cia.rsf
sed -i "s/@@UNIQUE_ID@@/$UNIQUE_ID/g" cia.rsf

echo "> Running make..."
if make; then
	arm-none-eabi-strip $PROJNAME.elf -o stripped.elf
	bannertool makebanner -i banner.png -a audio.wav -o banner.bin
	bannertool makesmdh -s "$SHORTNAME" -l "$LONGNAME" -p "$PUBNAME" -i icon.png -o cia.smdh -r regionfree -f visible
	echo "> Creating CXI..."
	makerom -elf stripped.elf -icon cia.smdh -banner banner.bin -rsf cia.rsf -o $PROJNAME.cxi
	echo "> Creating CIA..."
	makerom -f cia -content $PROJNAME.cxi:0:0 -rsf cia.rsf
	echo "> Done!"
else
	echo "> Make returned error, exiting."
	exit 1
fi
