#!/bin/bash

# Place this script inside the
# Workspace folder to work properly

if [ -d "$targets" ]; then 
    rm -r -f "$targets" 
fi

submissions=$1
targets=$2
tests=$3
answers=$4

verbose=false
noexecute=false

for i in "$@"
do
    if [[ $i = -v ]]; then 
        verbose=true
    fi

    if [[ $i = -noexecute ]]; then
        noexecute=true
    fi
done

#making necessary directory
arr=("targets/c" "targets/java" "targets/python")

#mkdir -p ${arr[*]}
for i in ${arr[*]}
do
    mkdir -p "$i"
done

for i in "submissions"/*
do
    foldername=${i: -11 : -4}    
    mkdir -p codes/$foldername
    unzip -qq "$i" -d codes/"$foldername"
done

#mandatory
executing(){
    # Checking if it's a directory
    if [ -d "$1" ]; then
        for i in "$1"/*
        do
            executing "$i" "$2" 
        done
    elif [ -f "$1" ]; then
        if [[ "$1" = *.c ]]; then
            if [[ $verbose = true ]]; then
                echo -n "$indent"
                echo "Organizing files of $2"
            fi

            mkdir targets/c/"$2"
            touch targets/c/"$2"/main.c
            cp "$1" targets/c/"$2"/main.c

            if [[ ! $noexecute = true ]]; then
                if [[ $verbose = true ]]; then
                    echo "Executing files of $2"
                fi

                gcc targets/c/"$2"/main.c -o targets/c/"$2"/main.o
                tst=1
                
                for test in "tests"/*
                do
                    ./targets/c/$2/main.o < "$test" > targets/c/"$2"/out"$tst".txt
                    tst=$(( $tst+1 ))
                done
            fi
        fi

        #java
        if [[ "$1" = *.java ]]; then
            if [[ $verbose = true ]]; then
                echo -n "$indent"
                echo "Organizing files of $2"
            fi
            
            mkdir targets/java/"$2"
            touch targets/java/"$2"/Main.java
            cp "$1" targets/java/"$2"/Main.java

            if [[ ! $noexecute = true ]]; then
                if [[ $verbose = true ]]; then
                    echo "Executing files of $2"
                fi

                javac targets/java/"$2"/Main.java
                tst=1
                
                for test in "tests"/*
                do
                    java -cp ./targets/java/"$2"/ Main < "$test" > targets/java/"$2"/out"$tst".txt
                    tst=$(( $tst+1 ))
                done
            fi
        fi

        #python
        if [[ "$1" = *.py ]]; then
            if [[ $verbose = true ]]; then
                echo -n "$indent"
                echo "Organizing files of $2"
            fi
            
            mkdir targets/python/"$2"
            touch targets/python/"$2"/main.py
            cp "$1" targets/python/"$2"/main.py

            if [[ ! $noexecute = true ]]; then
                if [[ $verbose = true ]]; then
                    echo "Executing files of $2"
                fi

                tst=1
                
                for test in "tests"/*
                do
                    python3 ./targets/python/"$2"/main.py < "$test" > targets/python/"$2"/out"$tst".txt
                    tst=$(( $tst+1 ))
                done
            fi
        fi
    fi
}

#cd ./Codes/
for varname in "Codes"/*
do
    required=${varname: -7}
    executing $varname $required
done
#cd ..
rm -r -f Codes

#creating CSV file
if [[ ! noexecute = true ]]; then 
    touch targets/result.csv
    echo "student_id,type,matched,not_matched" >> targets/result.csv

    for type in "targets"/*
    do
        if [ -d "$type" ];then
            for id in "$type"/*
            do
                s_id=$( basename $id )
                s_l=$( basename $type ) 
                x=1
                cor=0
                incor=0
                for out in "$id"/*
                do
                    if [[ $out = *.txt ]]; then
                        string=$(diff -w -q -s "$out" ./"$answers"/ans"$x".txt)
                        if [[ $string = *identical ]]; then
                            cor=$(( $cor+1 ))
                        else
                            incor=$(( $incor+1 ))
                        fi
                        x=$(( $x+1 )) 
                    fi
                done
                
                echo "$( basename $id ),$( basename $type ),$cor,$incor" >> targets/result.csv
            done
        fi
    done
fi
