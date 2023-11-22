#!/bin/bash
source /cvmfs/sft.cern.ch/lcg/views/LCG_97rc4python3/x86_64-centos7-gcc9-opt/setup.sh
export PATH=/workfs2/atlas/wangz1996/software/cmake/cmake-3.17.3-Linux-x86_64/bin:$PATH

Time=1119
Project=SimCalModule
InDir=/cefs/higgs/diaohb/SIM/cern-testbeam-simulation-for-scecal-and-ahcal/run/${Time}/calo
OutDir=/cefs/higgs/diaohb/SIM/cern-testbeam-simulation-for-scecal-and-ahcal/run/${Time}/MC

if [ ! -d ${OutDir} ]
then
    mkdir -p ${OutDir}
fi

# cd /cefs/higgs/qibh/G4Simulation/SimCalModule/script

for datafile in $(ls ${InDir} | grep '.root')
do
    root -l -b -q 'BeamDataStructure.cxx("'${InDir}'","'${OutDir}'","'${datafile}'",0)'
done
