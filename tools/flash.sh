#!/bin/bash

#
# SPDX-License-Identifier: GPL-2.0-or-later
# Copyright (C) Oren Sokoler (https://github.com/orenskl)
#

../picotool/build/picotool load build/pjvm.uf2 -f
if [ $? -ne 0 ] 
then 
  sleep 2
  ../picotool/build/picotool load build/pjvm.uf2 -f
fi
../picotool/build/picotool reboot -f

