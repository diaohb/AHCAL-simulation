#!/bin/bash
Time=0630_0126_trigger60_model_25_material_0_pi
# Time=1222_0126_trigger60_model_25_material_0
# Time=ML_0126_trigger60_model_25_material_0_e_8GeV

Dir=/cefs/higgs/diaohb/SIM/AHCAL-simulation/run/${Time}/
pedestal=/cefs/higgs/diaohb/CEPC2023/PS/calibration/pedestal.root
# dac=/cefs/higgs/diaohb/CEPC2023/PS/calibration/dac.root
dac=/cefs/higgs/diaohb/CEPC2023/PS/ratio/dac.root
# mip=/cefs/higgs/diaohb/CEPC2023/PS/calibration/mip.root
mip=/cefs/higgs/diaohb/CEPC2023/PS/mip/script//mip.root
_mip=/cefs/higgs/diaohb/CEPC2023/PS/mip/script/mip.root
_mip=$mip
spe=/cefs/higgs/diaohb/CEPC2023/PS/calibration/spe.root
sipm_model=/cefs/higgs/diaohb/SIM/saturation_model/sipm_model_0.0xt.root
lowgain_adc=/cefs/higgs/diaohb/CEPC2023/SPS/calibration/lowgain_e-calib.root

caloDir=${Dir}/calo
MCDir=${Dir}/MC
DigiDir=${Dir}/Digi_PS_nolow
CalibDir=${Dir}/Calib_PS_nolow
CorDir=${Dir}/Correct_PS_nolow
RunDir=${Dir}/Run_PS
ListDir=${Dir}/list_PS_nolow_correct

rm -rf ${RunDir}
rm -rf ${ListDir}/*
mkdir -p ${caloDir}
mkdir -p ${MCDir}
mkdir -p ${DigiDir}
mkdir -p ${CalibDir}
mkdir -p ${CorDir}
mkdir -p ${RunDir}
mkdir -p ${ListDir}
log=$ListDir/log
echo "birks constant 0.126">$log
echo "original digitization">>$log
echo "no birks">>$log
echo "consider intercept in highgain lowgian ratio">>$log
echo "add 0.015 temperature uniformity">>$log
echo "increase highgain pedestal sigma to 2 times">>$log
echo "increase lowgain pedestal sigma to 2 times">>$log
echo "increase spe sigma to 8">>$log
echo "add 0.02 non linearity (gaussian) to highgain lowgian adc">>$log
# echo "0.00002 noise exp photon energy / 4.6">>$log
echo "channel level noise">>$log
echo "increase light yield uncertenty to 0.06">>$log
# echo "">>$log
RunNumber=200
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
	# if awk "BEGIN{exit !($energy >= 20 && $energy != 500)}"; then
		# continue
	# fi
	infile=${caloDir}/${datafile}
	outfile=${MCDir}/MC_${datafile#*_}

	echo -e "${mergecmd}" >>${RunDir}/${shellfile}
	echo -e "root -l -b -q '/cefs/higgs/diaohb/SIM/AHCAL-simulation/script/BeamDataStructure.cxx(\"${infile}\",\"${outfile}\",0)'">>${RunDir}/${shellfile}
	echo -e "${outfile}"  >> ${ListDir}/truth
	infile=MC_${datafile#*_}
	outfile=Digi_${datafile#*_}
	echo -e "digi ${MCDir}/${infile} ${pedestal} ${dac} ${_mip} ${spe} ${lowgain_adc} ${sipm_model} ${DigiDir}/${outfile} ">>${RunDir}/${shellfile}
	echo -e "${DigiDir}/${outfile}"  >> ${ListDir}/digi
	infile=Digi_${datafile#*_}
	outfile=Calib_${datafile#*_}
	echo -e "Calib ${DigiDir}/${infile} $pedestal $dac $mip ${CalibDir}/${outfile}">>${RunDir}/${shellfile}
	echo -e "${CalibDir}/${outfile}"  >> ${ListDir}/calib
	infile=${outfile}
	outfile=Correct_${datafile#*_}
	echo -e "satu_correct ${CalibDir}/${infile} $pedestal $dac $mip $spe ${CorDir}/${outfile}">>${RunDir}/${shellfile}
	echo -e "${CorDir}/${outfile}"  >> ${ListDir}/correct
	chmod +x ${RunDir}/${shellfile}
	hep_sub  ${RunDir}/${shellfile}
done
