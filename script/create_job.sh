#!/bin/bash
Title=ML_0126_trigger60_model_25_material_0_pi_gradient3
# Title=0630_0126_trigger60_model_25_material_0_pi
# Title=mu-_30050126_40
Project=SimCalModule
MyDir=/cefs/higgs/diaohb/SIM/AHCAL-simulation/
rundir=${MyDir}/run/${Title}/
rm -rf ${rundir}/run*
mkdir -p ${rundir}
cp ${MyDir}/run/br-run.mac ${rundir}/run.mac
NFile=200
for i in $(seq 1 ${NFile})
do
    RunDir=${rundir}/run${i}
    if [ ! -d ${RunDir} ]
    then
        mkdir -p ${RunDir}
    fi
	job_sh=job${i}.sh
    echo "cd ${RunDir}" > ${RunDir}/${job_sh}
    chmod +x ${RunDir}/${job_sh}
    j=$(expr ${i} \* 2 + 53039)
	echo "source ${MyDir}/setup_LCG.sh" >> ${RunDir}/${job_sh}
	# echo "source ${MyDir}/setup_LCG.sh" >> ${RunDir}/job.sh
    echo "${MyDir}/build/${Project} ${rundir}/run.mac ${j}" >> ${RunDir}/${job_sh}
	hep_sub ${RunDir}/${job_sh}
	# hep_sub -os CentOS7  ${RunDir}/job.sh
done
