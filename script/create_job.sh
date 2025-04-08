#!/bin/bash
Title=0405_35080126_trigger40
# Title=mu-_30050126_40
Project=SimCalModule
MyDir=/home/diaohb/CEPC/AHCAL-simulation/
rundir=${MyDir}/run/${Title}/
mkdir -p ${rundir}
cp ${MyDir}/run/e-run.mac ${rundir}/run.mac
NFile=50
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
    echo "${MyDir}/build/${Project} ${rundir}/run.mac ${j}" >> ${RunDir}/job.sh
    bash ${RunDir}/job.sh
done
