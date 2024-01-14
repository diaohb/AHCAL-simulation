#!/bin/bash
# source /afs/ihep.ac.cn/users/q/qibh/setup_HTCondor.sh
Time=0112_25000126_1010_trigger60
Project=SimCalModule
MyDir=/cefs/higgs/diaohb/SIM/cern-testbeam-simulation-for-scecal-and-ahcal
rundir=${MyDir}/run/${Time}/
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
    # cp /cefs/higgs/qibh/G4Simulation/${Project}/build/${Project} ${RunDir}
    # cp /cefs/higgs/qibh/G4Simulation/${Project}/build/run.mac ${RunDir}
    # touch ${RunDir}/job.sh
    echo "source /afs/ihep.ac.cn/users/d/diaohb/.bash_qibh_setup" > ${RunDir}/job.sh
    chmod +x ${RunDir}/job.sh
    echo "cd ${RunDir}" >> ${RunDir}/job.sh
    j=$(expr ${i} \* 2 + 50489)
    echo "${MyDir}/build/${Project} ${rundir}/run.mac ${j}" >> ${RunDir}/job.sh
    hep_sub ${RunDir}/job.sh
done
