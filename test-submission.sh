#!/bin/bash

echoerr()
{
    echo "$@" >&2
}

TEST1="1. Test mysort with 8 threads and 1GB of data                                   "
TEST1_ARGS="1 10000000 data.in data.out 8"
TEST2="2. Test mysort with 8 threads and 4GB of data                                   "
TEST2_ARGS="2 40000000 data.in data.out 8"
TEST3="3. Test mysort with 8 threads and 16GB of data                                  "
TEST3_ARGS="3 160000000 data.in data.out 8"
TEST4="4. Test mysort with 8 threads and 64GB of data                                  "
TEST4_ARGS="4 640000000 data.in data.out 8"

NUM_TESTS=4

STATUS=0

TEST()
{
    local testnum=$1
    local numRecords=$2
    local inputFile=$3
    local outputFile=$4
    local numThreads=$5
    local program="mysort"
    local cmd="./mysort $inputFile $outputFile $numThreads"

    if [ ! -f $program ]
    then
        local var="TEST$testnum"
        local msg1="${!var} failed!"
        local msg2= "*** $program is missing ***"
        echo -e "$msg1\n$msg2"
    else
        ./gensort -a -b0 -t8 $numRecords data.in
        eval $cmd &> mysort.log
        ./valsort -t8 -o data.sum data.out &> valsort.log
        
        local rc=$(cat valsort.log | grep "SUCCESS" | wc -l)
        if [ $rc -eq 1 ]
        then
            local var="TEST$testnum"
            local msg1="${!var} passed!"
            local msg2="*** Test $testnum run log ***"
            local msg3=">>> $cmd"
            local msg4=$(cat mysort.log)
            local msg5="*** *** ***"
            echo -e "$msg1\n$msg2\n$msg3\n$msg4\n$msg5"
        else
            local var="TEST$testnum"
            local msg1="${!var} failed!"
            local msg2="*** Test $testnum run log ***"
            local msg3=">>> ./gensort -a -b0 -t8 $numRecords data.in"
            local msg4=">>> $cmd"
            local msg5=$(cat mysort.log)
            local msg6=">>> ./valsort -t8 -o data.sum data.out"
            local msg7=$(cat valsort.log)
            local msg8="*** *** ***"
            echo -e "$msg1\n$msg2\n$msg3\n$msg4\n$msg5\n$msg6\n$msg7\n$msg8"
            STATUS=1
        fi
    fi
}

HOW_TO_USE="HOW TO USE: bash test-submission.sh [<check number> | list | all]"

if [ $# -ne 1 ]
then
    echoerr "$HOW_TO_USE"
    exit 1
fi

arg1=$1

if [ "$arg1" == "list" ]
then
    echo "List of available checks:"
    for((i=1;i<=$NUM_TESTS;i++))
    do
        var="TEST$i"
        echo "${!var}"
    done
    exit 0
fi

if [ "$arg1" == "all" ]
then
    for((i=1;i<=$NUM_TESTS;i++))
    do
        var="TEST${i}_ARGS"
        cmd="TEST ${!var}"
        eval $cmd
    done
    
    if [ $STATUS -ne 0 ]
    then
        exit 1
    fi
    
    exit 0
fi

if [ $arg1 -eq $arg1 ]
then
    var="TEST${i}_ARGS"
    cmd="TEST ${!var}"
    eval $cmd
    
    if [ $STATUS -ne 0 ]
    then
        exit 1
    fi
    
    exit 0
else
    echoerr "$HOW_TO_USE"
    exit 2
fi

exit 0
