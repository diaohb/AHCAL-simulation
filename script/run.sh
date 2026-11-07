# !/bin/bash
Title=ML_0126_trigger60_model_25_material_0_pi_gradient
# Title=1222_0126_trigger60_model_25_material_0_pi
# Title=0630_0126_trigger60_model_25_material_0_pi_SPS
Dir=/cefs/higgs/diaohb/SIM/AHCAL-simulation/run/${Title}/
pedestal=/cefs/higgs/diaohb/CEPC2023/SPS/calibration/pedestal.root
dac=/cefs/higgs/diaohb/CEPC2023/SPS/calibration/ratio/calib/pi-/calib.root
# dac=/cefs/higgs/diaohb/CEPC2023/SPS/calibration/analyse/dac.root
mip=/cefs/higgs/diaohb/CEPC2023/SPS/calibration/mip.root
# _mip=/cefs/higgs/diaohb/CEPC2023/SPS/mip/script/mip.root
_mip=$mip
spe=/cefs/higgs/diaohb/CEPC2023/SPS/calibration/spe.root
sipm_model=/cefs/higgs/diaohb/SIM/saturation_model/sipm_model_0.0xt.root
lowgain_adc=/cefs/higgs/diaohb/CEPC2023/SPS/calibration/lowgain_e-calib.root

caloDir=${Dir}/calo
MCDir=${Dir}/MC
DigiDir=${Dir}/Digi_SPS_nolow
CalibDir=${Dir}/Calib_SPS_nolow
CorDir=${Dir}/Correct_SPS_nolow
RunDir=${Dir}/Run
ListDir=${Dir}/list_SPS_nolow_correct
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
# echo "no sipm non linearity">$log
echo "birks constant 0.126">$log
echo "original digitization">>$log
# echo "no birks">>$log
echo "consider intercept in highgain lowgian ratio">>$log
echo "add 0.015 temperature uniformity">>$log
echo "increase highgain pedestal sigma to 2 times">>$log
echo "increase lowgain pedestal sigma to 2 times">>$log
echo "increase spe sigma to 8">>$log
echo "add 0.02 non linearity (gaussian) to highgain lowgian adc">>$log
# echo "0.00002 noise exp photon energy / 4.6">>$log
echo "no noise">>$log
echo "channel level noise: histogram sample">>$log
echo "increase light yield uncertenty to 0.06">>$log
# echo "add lowgain non-linearity, layer 2 to 13 use fit parameters, other layers use (400 1)">>$log
# echo "signal 100% overwrite noise">>$log
RunNumber=200
for datafile in $(ls ${Dir}/run2 |grep 'root')
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
	# if ((energy>=10 && energy!=500));then
		# continue
	# fi
	infile=${caloDir}/${datafile}
	outfile=${MCDir}/MC_${datafile#*_}

	# echo -e "source ~/.bash_profile" >>${RunDir}/${shellfile}
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
