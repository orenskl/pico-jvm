#!/bin/bash
../picotool/build/picotool load build/pjvm.uf2 -f
if [ $? -ne 0 ] 
then 
  sleep 2
  ../picotool/build/picotool load build/pjvm.uf2 -f
fi
../picotool/build/picotool reboot -f

