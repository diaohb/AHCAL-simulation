#!/bin/bash
Time=0830_30050126_1010_trigger40_mu-
# Time=0116_25000126_1010_trigger40
Dir=/cefs/higgs/diaohb/SIM/cern-testbeam-simulation-for-scecal-and-ahcal/run/${Time}/
pedestal=/cefs/higgs/diaohb/CEPC2023/PS/calibration/pedestal.root
dac=/cefs/higgs/diaohb/CEPC2023/PS/calibration/dac.root
mip=/cefs/higgs/diaohb/CEPC2023/PS/calibration/mip.root
spe=/cefs/higgs/diaohb/CEPC2023/PS/calibration/spe.root
sipm_model=/cefs/higgs/diaohb/SIM/saturation_model/sipm_model_0.0xt.root

caloDir=${Dir}/calo
MCDir=${Dir}/MC
DigiDir=${Dir}/Digi_PS
CalibDir=${Dir}/Calib_PS
RunDir=${Dir}/Run
ListDir=${Dir}/list_PS
rm -rf ${RunDir}
rm -rf ${ListDir}
mkdir -p ${caloDir}
mkdir -p ${MCDir}
mkdir -p ${DigiDir}
mkdir -p ${CalibDir}
mkdir -p ${RunDir}
mkdir -p ${ListDir}
RunNumber=200
for datafile in $(ls ${Dir}/run1 | grep '.root')
do
	mergecmd="hadd ${caloDir}/${datafile%_*}.root"
	for i in $(seq 1 ${RunNumber})
	do
		if [ -f ${Dir}/run${i}/${datafile} ];then
			mergecmd="${mergecmd} ${Dir}/run${i}/${datafile}"
		fi
	done
	datafile=${datafile%_*}.root
	energy=${datafile##*_}
	energy=${energy%G*}
	energy=${energy%M*}
	if ((energy>=10 && energy!=500));then
		continue
	fi
    echo -e "source ~/.bash_setup">${RunDir}/${datafile}.sh
	echo -e "${mergecmd}" >>${RunDir}/${datafile}.sh
# done


# for datafile in $(ls ${caloDir} | grep '.root')
# do
    infile=${caloDir}/${datafile}
    outfile=${MCDir}/MC_${datafile#*_}
	echo -e "root -l -b -q '/cefs/higgs/diaohb/SIM/cern-testbeam-simulation-for-scecal-and-ahcal/script/BeamDataStructure.cxx(\"${infile}\",\"${outfile}\",0)'">>${RunDir}/${datafile}.sh
	echo -e "${outfile}"  >> ${ListDir}/truth
    infile=MC_${datafile#*_}
    outfile=Digi_${datafile#*_}
    echo -e "digi ${MCDir}/${infile} ${pedestal} ${dac} ${mip} ${spe} ${sipm_model} ${DigiDir}/${outfile} ">>${RunDir}/${datafile}.sh
    echo -e "${DigiDir}/${outfile}"  >> ${ListDir}/digi
    infile=Digi_${datafile#*_}
    outfile=Calib_${datafile#*_}
	echo -e "Calib ${DigiDir}/${infile} $pedestal $dac $mip ${CalibDir}/${outfile}">>${RunDir}/${datafile}.sh
	echo -e "${CalibDir}/${outfile}"  >> ${ListDir}/calib
    chmod +x ${RunDir}/${datafile}.sh
	hep_sub -os CentOS7 ${RunDir}/${datafile}.sh
done
