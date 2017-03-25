#!/bin/bash

# enter these parameters
runs=10
max_cpus=4
max_steps=20

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
      mpiexec -np $cpus ../../PRARIEDOG-opt -i test.i Executioner/num_steps=$steps Outputs/file_base=gold UserObjects/random_point_uo/seed=$seed1 UserObjects/inserter/seed=$seed2 > run

      # count exodus output files
      num_gold_files=`for file in gold.e*; do echo $file; done | wc -l`

      # do half-transient and recover
      echo "running a half-transient and recover..."
      mpiexec -np $cpus ../../PRARIEDOG-opt -i test.i Executioner/num_steps=$steps Outputs/file_base=recover UserObjects/random_point_uo/seed=$seed1 UserObjects/inserter/seed=$seed2 --half-transient Outputs/checkpoint=true > run-recover
      mpiexec -np $cpus ../../PRARIEDOG-opt -i test.i Executioner/num_steps=$steps Outputs/file_base=recover UserObjects/random_point_uo/seed=$seed1 UserObjects/inserter/seed=$seed2--recover  >> run-recover

      # count exodus output files again
      num_recover_files=`for file in recover.e*; do echo $file; done | wc -l`

      # number of output files should be the same
      if [ $num_gold_files != $num_recover_files ]
      then
          echo "error: number of output files does not match"
          exit
      fi

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
      ../../../moose/framework/contrib/exodiff/exodiff -m  -F 1e-10 -t 5.5e-06 $gold_file $recover_file > exodiff-output

      # check for message in exodiff output
      if ! grep -q 'Files are the same' exodiff-output
      then
          echo "error: outputs are different"
          exit
      else
          echo "OK!"
      fi

    done
  done
done
