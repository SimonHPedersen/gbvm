#!/bin/sh
#
# $Id: profile.sh,v 1.4 2001/06/17 21:59:59 hempe Exp $
#
# Script to perform multiple profilations merged into one
#
# $1 : The executable compiled with profiling support
# $2 : Argument passed to the executable
# $3 : Profile runs
# $4 : Output filename suffix

rm gmon.out*

for i in `seq 1 $3`; do
  echo "Run $i"
  $1 $2
  mv gmon.out gmon.out.$i
done
echo "stage 1 done"
gprof -s $1 gmon.out.*
echo "stage 2 done"
gprof $1 gmon.sum > $4

rm gmon.out.*
rm gmon.sum
