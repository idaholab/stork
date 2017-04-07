#!/bin/bash

# This script is to be used to run one set of simulations to test recovery.
# It requires 3 parameters from the command line (in this order):
#   1. total number of steps to perform
#   2. seed 1
#   3. seed 2

if [ $# -ne 3 ]
then
  echo "usage: $0 num_steps seed1 seed2"
  exit
fi

steps=$1
seed1=$2
seed2=$3
echo "run using $steps steps with these seeds: $seed1 $seed2 ..."

# set path to executable
exec=../../PRARIEDOG-opt

# set path to exodiff
exodiff=../../../moose/framework/contrib/exodiff/exodiff

# set name of input file
inputfile=test.i

# clean
rm -rf *.e* *_cp run run-recover exodus-output

# run once all the way through
echo "running all the way through..."
$exec -i $inputfile Executioner/num_steps=$steps Outputs/file_base=gold UserObjects/random_point_uo/seed=$seed1 UserObjects/inserter/seed=$seed2 > run

# do half-transient and recover
echo "running a half-transient and recover..."
$exec -i $inputfile Executioner/num_steps=$steps Outputs/file_base=recover UserObjects/random_point_uo/seed=$seed1 UserObjects/inserter/seed=$seed2 --half-transient Outputs/checkpoint=true > run-recover
$exec -i $inputfile Executioner/num_steps=$steps Outputs/file_base=recover UserObjects/random_point_uo/seed=$seed1 UserObjects/inserter/seed=$seed2 --recover  >> run-recover

# count exodus output files
num_gold_files=`for file in gold.e*; do echo $file; done | wc -l`
num_recover_files=`for file in recover.e*; do echo $file; done | wc -l`

# number of output files should be the same
if [ $num_gold_files != $num_recover_files ]
then
  echo "error: number of output files does not match, cpus: $cpus, steps: $steps, seeds: $seed1 $seed2"
  exit 1
fi

# get name of last exodus file
if [ $num_gold_files -gt 1 ]
then
  gold_file=gold.e-s`printf "%03d" $num_gold_files`
  recover_file=recover.e-s`printf "%03d" $num_gold_files`
else
  gold_file=gold.e
  recover_file=recover.e
fi

# now compare the last files the same way the moose tester does it
# this assumes you have this app installed next to moose
$exodiff -m  -F 1e-10 -t 5.5e-06 $gold_file $recover_file > exodiff-output

# check for message in exodiff output
if ! grep -q 'Files are the same' exodiff-output
then
  echo "error: outputs are different, cpus: $cpus, steps: $steps, seeds: $seed1 $seed2"
  exit 1
else
  echo "OK!"
fi
