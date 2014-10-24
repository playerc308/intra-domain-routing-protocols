#!/bin/bash

N=$1
echo
echo "[nodes]"
echo

for((i=1;i<=N;i++));
do
printf "%d " $i;
done 
printf "\n"

echo
echo "[links]"
echo

for((i=1;i<N;i++));
do
let rrt=$RANDOM%9+1;
let n=1+i;
printf "(%d,%d) delay 0.0%d0 prob 0.0\n" $i $n $rrt;
done 


for((i=1;i<N;i++));
do
for((j=i+1;j<N;));
do
let dn=N/5;
let j=$RANDOM%dn+1+j;
let rrt=$RANDOM%9+1;
if [ $j -le $N ];
then
printf "(%d,%d) delay 0.0%d0 prob 0.0\n" $i $j $rrt;
fi;
done;
done;

echo
echo "[events]"
echo
