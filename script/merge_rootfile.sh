#!/bin/bash
Time=1119
Project=SimCalModule
RunNumber=100
MyDir=/cefs/higgs/diaohb/SIM/cern-testbeam-simulation-for-scecal-and-ahcal/run/${Time}
OutDir=${MyDir}/calo

# cd ${OutDir}
mkdir -p ${OutDir}

for datafile in $(ls ${MyDir}/run1 | grep '.root')
do
    mergecmd="hadd ${OutDir}/${datafile%_*}.root"
    for i in $(seq 1 ${RunNumber})
    do
		if [ -f ${MyDir}/run${i}/${datafile} ];then
        	mergecmd="${mergecmd} ${MyDir}/run${i}/${datafile}"
		fi
    done
    # echo ${mergecmd}
    ${mergecmd}
done

# cd -
