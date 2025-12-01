#!/bin/bash
Title=1021_30080126_trigger40_-1030
# Title=mu-_30050126_40
Project=SimCalModule
MyDir=/cefs/higgs/diaohb/SIM/AHCAL-simulation/
rundir=${MyDir}/run/${Title}/
# rm -rf ${rundir}
mkdir -p ${rundir}
cp ${MyDir}/run/e-run.mac ${rundir}/run.mac
NFile=200
for i in $(seq 1 ${NFile})
do
    RunDir=${rundir}/run${i}
    if [ ! -d ${RunDir} ]
    then
        mkdir -p ${RunDir}
    fi
    echo "cd ${RunDir}" > ${RunDir}/job.sh
    chmod +x ${RunDir}/job.sh
    j=$(expr ${i} \* 2 + 50489)
	echo "source ${MyDir}/setup_LCG.sh" >> ${RunDir}/job.sh
    echo "${MyDir}/build/${Project} ${rundir}/run.mac ${j}" >> ${RunDir}/job.sh
    hep_sub -os CentOS7  ${RunDir}/job.sh
done
