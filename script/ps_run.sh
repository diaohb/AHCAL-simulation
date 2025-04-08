#!/bin/bash
Time=0405_30080126_trigger40
# Time=mu-_30050126_40
# Time=test
Dir=/home/diaohb/CEPC/AHCAL-simulation/run/${Time}/
pedestal=/home/diaohb/CEPC/ihep/PS/calibration/pedestal.root
# dac=/home/diaohb/CEPC2023/PS/calibration/dac.root
dac=/home/diaohb/CEPC/ihep/PS/ratio/dac.root
mip=/home/diaohb/CEPC/ihep/PS/calibration/mip.root
_mip=/home/diaohb/CEPC/ihep/PS/mip/script/mip.root
_mip=$mip
spe=/home/diaohb/CEPC/ihep/PS/calibration/spe.root
sipm_model=/home/diaohb/CEPC/AHCAL-analyse/calibration/sipm_model/sipm_model_0.0xt.root
lowgain_adc=/home/diaohb/CEPC/ihep/SPS/calibration/lowgain_e-calib.root

caloDir=${Dir}/calo
MCDir=${Dir}/MC_nobirks
DigiDir=${Dir}/Digi_PS8
CalibDir=${Dir}/Calib_PS8
RunDir=${Dir}/Run
ListDir=${Dir}/list_PS8
rm -rf ${RunDir}
rm -rf ${ListDir}
mkdir -p ${caloDir}
mkdir -p ${MCDir}
mkdir -p ${DigiDir}
mkdir -p ${CalibDir}
mkdir -p ${RunDir}
mkdir -p ${ListDir}
log=$ListDir/log
echo "original digitization">$log
echo "no birks">>$log
echo "consider intercept in highgain lowgian ratio">>$log
echo "add 0.015 temperature uniformity">>$log
echo "increase highgain pedestal sigma to 2 times">>$log
echo "increase lowgain pedestal sigma to 5 times">>$log
echo "increase spe sigma to 8">>$log
echo "add 0.02 non linearity (gaussian) to highgain lowgian adc">>$log
RunNumber=50
for datafile in $(ls ${Dir}/run1 |grep 'root')
do
	shellfile=${datafile%_*}.sh

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
	infile=${caloDir}/${datafile}
	outfile=${MCDir}/MC_${datafile#*_}

	# echo -e "${mergecmd}" >>${RunDir}/${shellfile}
	echo "# nobirks" >> ${RunDir}/${shellfile}
	# echo -e "root -l -b -q '/home/diaohb/CEPC/AHCAL-simulation/script/BeamDataStructure.cxx(\"${infile}\",\"${outfile}\",0)'">>${RunDir}/${shellfile}
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
