#!/bin/bash
Time=0312_30050126_trigger40
# Time=mu-_25000126_40
# Time=test
Dir=/cefs/higgs/diaohb/SIM/cern-testbeam-simulation-for-scecal-and-ahcal/run/${Time}/
pedestal=/cefs/higgs/diaohb/CEPC2023/PS/calibration/pedestal.root
# dac=/cefs/higgs/diaohb/CEPC2023/PS/calibration/dac.root
dac=/cefs/higgs/diaohb/CEPC2023/PS/ratio/dac.root
_mip=/cefs/higgs/diaohb/CEPC2023/PS/mip/script/mip.root
mip=/cefs/higgs/diaohb/CEPC2023/PS/calibration/mip.root
spe=/cefs/higgs/diaohb/CEPC2023/PS/calibration/spe.root
sipm_model=/cefs/higgs/diaohb/SIM/saturation_model/sipm_model_0.0xt.root
lowgain_adc=/cefs/higgs/diaohb/CEPC2023/SPS/calibration/lowgain_e-calib.root

caloDir=${Dir}/calo
MCDir=${Dir}/MC_nobirks
DigiDir=${Dir}/Digi_PS_lrms_spe_mpv
CalibDir=${Dir}/Calib_PS_lrms_spe_mpv
RunDir=${Dir}/Run
ListDir=${Dir}/list_PS_lrms_spe_mpv
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
	mergecmd="hadd -f ${caloDir}/${datafile%_*}.root"
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
    # echo -e "source ~/.bash_setup">${RunDir}/${datafile}.sh
	# echo -e "${mergecmd}" >>${RunDir}/${datafile}.sh
    infile=${caloDir}/${datafile}
    outfile=${MCDir}/MC_${datafile#*_}
	# echo -e "root -l -b -q '/cefs/higgs/diaohb/SIM/cern-testbeam-simulation-for-scecal-and-ahcal/script/BeamDataStructure.cxx(\"${infile}\",\"${outfile}\",0)'">>${RunDir}/${datafile}.sh
	echo -e "${outfile}"  >> ${ListDir}/truth
    infile=MC_${datafile#*_}
    outfile=Digi_${datafile#*_}
	echo -e "digi ${MCDir}/${infile} ${pedestal} ${dac} ${_mip} ${spe} ${lowgain_adc} ${sipm_model} ${DigiDir}/${outfile} ">>${RunDir}/${datafile}.sh
    echo -e "${DigiDir}/${outfile}"  >> ${ListDir}/digi
    infile=Digi_${datafile#*_}
    outfile=Calib_${datafile#*_}
	echo -e "Calib ${DigiDir}/${infile} $pedestal $dac $mip ${CalibDir}/${outfile}">>${RunDir}/${datafile}.sh
	echo -e "${CalibDir}/${outfile}"  >> ${ListDir}/calib
    chmod +x ${RunDir}/${datafile}.sh
	hep_sub ${RunDir}/${datafile}.sh
done
