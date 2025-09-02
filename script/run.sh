#!/bin/bash
# Time=0312_30050126_trigger40
Time=mu-_30050126_40
# Time=test
Dir=/home/diaohb/CEPC/AHCAL-simulation/run/${Time}/
pedestal=/home/diaohb/CEPC/ihep/SPS/calibration/pedestal.root
# dac=/home/diaohb/CEPC2023/PS/calibration/dac.root
dac=/home/diaohb/CEPC/ihep/SPS/calibration/dac.root
_mip=/home/diaohb/CEPC/ihep/SPS/calibration/mip.root
mip=/home/diaohb/CEPC/ihep/SPS/calibration/mip.root
spe=/home/diaohb/CEPC/ihep/SPS/calibration/spe.root
sipm_model=/home/diaohb/CEPC/AHCAL-analyse/calibration/sipm_model/sipm_model_0.0xt.root
lowgain_adc=/home/diaohb/CEPC/ihep/SPS/calibration/lowgain_e-calib.root

caloDir=${Dir}/calo
MCDir=${Dir}/MC
DigiDir=${Dir}/Digi
CalibDir=${Dir}/Calib
RunDir=${Dir}/Run
ListDir=${Dir}/list
rm -rf ${RunDir}
rm -rf ${ListDir}
mkdir -p ${caloDir}
mkdir -p ${MCDir}
mkdir -p ${DigiDir}
mkdir -p ${CalibDir}
mkdir -p ${RunDir}
mkdir -p ${ListDir}
RunNumber=1
for datafile in $(ls ${Dir}/run1 |grep 'root')
do
	datafile=${datafile%_*}.root
	infile=${caloDir}/${datafile}
	outfile=${MCDir}/MC_${datafile#*_}
	shellfile=${datafile%.*}.sh

	mergecmd="hadd -f ${caloDir}/${datafile%_*}.root"
	for i in $(seq 1 ${RunNumber})
	do
		if [ -f ${Dir}/run${i}/${datafile} ];then
			mergecmd="${mergecmd} ${Dir}/run${i}/${datafile}"
		fi
	done
	# echo -e "${mergecmd}" >>${RunDir}/${shellfile}

	echo "# nobirks" >> ${RunDir}/${shellfile}
	echo -e "root -l -b -q '/home/diaohb/CEPC/AHCAL-simulation/script/BeamDataStructure.cxx(\"${infile}\",\"${outfile}\",0)'">>${RunDir}/${shellfile}
	echo -e "${outfile}"  >> ${ListDir}/truth
	infile=MC_${datafile#*_}
	outfile=Digi_${datafile#*_}
	echo -e "digi ${MCDir}/${infile} ${pedestal} ${dac} ${_mip} ${spe} ${lowgain_adc} ${sipm_model} ${DigiDir}/${outfile} ">>${RunDir}/${shellfile}
	echo -e "${DigiDir}/${outfile}"  >> ${ListDir}/digi
	infile=Digi_${datafile#*_}
	outfile=Calib_${datafile#*_}
	echo -e "Calib ${DigiDir}/${infile} $pedestal $dac $mip ${CalibDir}/${outfile}">>${RunDir}/${shellfile}
	echo -e "${CalibDir}/${outfile}"  >> ${ListDir}/calib
	chmod +x ${RunDir}/${shellfile}
	bash ${RunDir}/${shellfile}
done
