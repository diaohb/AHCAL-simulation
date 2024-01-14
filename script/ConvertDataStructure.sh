#!/bin/bash
Time=1208_25000126
InDir=/cefs/higgs/diaohb/SIM/cern-testbeam-simulation-for-scecal-and-ahcal/run/${Time}/calo
OutDir=/cefs/higgs/diaohb/SIM/cern-testbeam-simulation-for-scecal-and-ahcal/run/${Time}/MC

if [ ! -d ${OutDir} ]
then
    mkdir -p ${OutDir}
fi

# cd /cefs/higgs/qibh/G4Simulation/SimCalModule/script

for datafile in $(ls ${InDir} | grep '.root')
do
    root -l -b -q 'BeamDataStructure.cxx("'${InDir}/${datafile}'","'${OutDir}/MC_${datafile#*_}'",0)'
done
