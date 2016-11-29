#!/bin/bash
PROJNAME=$(basename $PWD)
rm -rfv build
rm -v banner.bin cia.smdh stripped.elf $PROJNAME.{3dsx,cia,cxi,elf,smdh} cia.rsf
