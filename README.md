# Shared-Memory-TeraSort -- CS553 Cloud Computing
External Sorting application implemented in a single node shared memory multi-threaded approach. 
Objective: PA3-Objective.pdf

## check and test 

build mysort
```
make mysort
```

clean data files
```
make clean FILE="filename_to_be_deleted"
```

generate data files using gensort for number of records = numRecords and filename 'data.in'
```
./gensort -a -b0 -t8 $numRecords data.in
```

run the sorting program
```
./mysort <input file> <output file> <number of threads>
```

bash run the run-mysort.sh to run, test the Shared-Memory-TeraSort
```
$ bash run-mysort.sh
```

test script  
```
$ bash test-submission.sh
```

