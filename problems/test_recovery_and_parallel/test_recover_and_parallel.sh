#!/bin/bash

# This script is for testing the robustness of PRARIEDOG in terms of recovery.
# It can do several loops over the input file for a range of steps and numbers of cpus
#   to test if the recovery works with timestepping, mesh adaptivity, etc...
# It picks random seeds for the event timing and location objects, so it is best to
#   do several iterations ("runs").

# enter these parameters
runs=1
max_cpus=1
max_steps=10

# set path to executable
exec=../../PRARIEDOG-opt

# set path to exodiff
exodiff=../../../moose/framework/contrib/exodiff/exodiff

# set name of input file
inputfile=test.i

# write information on failed tests to file
rm -f failed-tests

# loop
for ((run=1;run<=$runs;run++))
do
  for ((cpus=1;cpus<=$max_cpus;cpus++))
  do
    for ((steps=2;steps<=$max_steps;steps++))
    do
      # clean
      rm -rf *.e* *_cp run run-recover exodus-output

      # get some random numbers for seeding the RNG's
      seed1=$RANDOM
      seed2=$RANDOM
      echo "run $run using $steps steps on $cpus cpu's with these seeds: $seed1 $seed2 ..."

      # run once all the way through
      echo "running all the way through..."
      mpiexec -np $cpus $exec -i $inputfile Executioner/num_steps=$steps Outputs/file_base=gold UserObjects/random_point_uo/seed=$seed1 UserObjects/inserter/seed=$seed2 > run

      # count exodus output files
      num_gold_files=`for file in gold.e*; do echo $file; done | wc -l`

      # do half-transient and recover
      echo "running a half-transient and recover..."
      mpiexec -np $cpus $exec -i $inputfile Executioner/num_steps=$steps Outputs/file_base=recover UserObjects/random_point_uo/seed=$seed1 UserObjects/inserter/seed=$seed2 --half-transient Outputs/checkpoint=true > run-recover
      mpiexec -np $cpus $exec -i $inputfile Executioner/num_steps=$steps Outputs/file_base=recover UserObjects/random_point_uo/seed=$seed1 UserObjects/inserter/seed=$seed2 --recover  >> run-recover

      # count exodus output files again
      num_recover_files=`for file in recover.e*; do echo $file; done | wc -l`

      # number of output files should be the same
      if [ $num_gold_files != $num_recover_files ]
      then
          echo "error: number of output files does not match, cpus: $cpus, steps: $steps, seeds: $seed1 $seed2" | tee -a failed-tests
          continue
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
          echo "error: outputs are different, cpus: $cpus, steps: $steps, seeds: $seed1 $seed2" | tee -a failed-tests
          continue
      else
          echo "OK!"
      fi

    done
  done
done

# print results
echo "-----------------"
if [ -e "failed-tests" ]
then
  echo "summary:"
  cat failed-tests
else
  echo "no errors!"
fi
