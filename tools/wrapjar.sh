#!/bin/bash

#
# SPDX-License-Identifier: GPL-2.0-or-later
# Copyright (C) Oren Sokoler (https://github.com/orenskl)
#

#
# Append a header to a JAR file, the header continas the following fields :
#
#           +--------------------------+
#           |   Signatue (0xCAFE1973)  |      4 Bytes
#           +--------------------------+
#           |     Size of JAR file     |      4 Bytes
#           +--------------------------+
#

if [ $# -ne 2 ]
  then
    echo "Wrap a JAR file with a header"
    echo ""
    echo "Usage : $0 input-jar-file output-bin-file"
    exit 1
fi

if [ ! -f $1 ]; then
    echo "ERROR : Input file $1 not found"
    exit 2
fi

SIZE=$(stat -c%s $1)
printf "CAFE1973%08X" $SIZE | xxd -r -p - >$1.tmp
cat $1.tmp $1 >$2
rm $1.tmp