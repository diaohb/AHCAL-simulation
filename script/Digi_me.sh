#!/bin/bash
Time=1208_25000126
InDir=/cefs/higgs/diaohb/SIM/cern-testbeam-simulation-for-scecal-and-ahcal/run/${Time}/MC
OutDir=/cefs/higgs/diaohb/SIM/cern-testbeam-simulation-for-scecal-and-ahcal/run/${Time}/Digi
RunDir=/cefs/higgs/diaohb/SIM/cern-testbeam-simulation-for-scecal-and-ahcal/run/${Time}/Run
pedestal=/cefs/higgs/diaohb/CEPC2023/SPS/calibration/pedestal.root
dac=/cefs/higgs/diaohb/CEPC2023/SPS/calibration/dac_v2.root
mip=/cefs/higgs/diaohb/CEPC2023/SPS/calibration/mip.root
spe=/cefs/higgs/diaohb/CEPC2023/SPS/calibration/spe.root


mkdir -p ${OutDir}
mkdir -p ${RunDir}
source ~/.bash_setup
for datafile in $(ls ${InDir} | grep '.root')
do
    # echo -e "source ~/.bash_setup\ndigi ${InDir}/${datafile} ${pedestal} ${dac} ${mip} ${spe} ${OutDir}/${datafile}">${RunDir}/${datafile}.sh
    # chmod +x ${RunDir}/${datafile}.sh
    # hep_sub ${RunDir}/${datafile}.sh
	digi ${InDir}/${datafile} ${pedestal} ${dac} ${mip} ${spe} ${OutDir}/Digi_${datafile#*_}
done
