#!/bin/bash
# Title=0312_30050126_trigger40
Title=mu-_30050126_40
Project=SimCalModule
MyDir=/cefs/higgs/diaohb/SIM/cern-testbeam-simulation-for-scecal-and-ahcal
rundir=${MyDir}/run/${Title}/
mkdir -p ${rundir}
cp ${MyDir}/run/run.mac ${rundir}
NFile=200
for i in $(seq 1 ${NFile})
do
    RunDir=${rundir}/run${i}
    if [ ! -d ${RunDir} ]
    then
        mkdir -p ${RunDir}
    fi
    echo "source /cefs/higgs/diaohb/SIM/cern-testbeam-simulation-for-scecal-and-ahcal/setup_LCG.sh" > ${RunDir}/job.sh
    chmod +x ${RunDir}/job.sh
    echo "cd ${RunDir}" >> ${RunDir}/job.sh
    j=$(expr ${i} \* 2 + 50489)
    echo "${MyDir}/build/${Project} ${rundir}/run.mac ${j}" >> ${RunDir}/job.sh
    hep_sub -os CentOS7 ${RunDir}/job.sh
done
