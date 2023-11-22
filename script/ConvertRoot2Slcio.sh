#!/bin/bash
source /afs/ihep.ac.cn/users/q/qibh/setup_HTCondor.sh
#------------------------------------------------------------------------------------------------------#
INPUTDATADIR=/cefs/higgs/qibh/G4Simulation/Data/SimCalModule/run20230216_AHCAL_Data/mu+
MARLIN_INIT_FILE=/cefs/higgs/qibh/G4Simulation/SimCalModule/script/Root2Slcio/run.xml
COUNT=0
#------------------------------------------------------------------------------------------------------#
for ENERGY in $(ls ${INPUTDATADIR} | grep '.root' | grep 'BeamData')
do
    INPUTDATAFILE=${INPUTDATADIR}/${ENERGY}
    OUTPUTDATAFILE=${INPUTDATADIR}/$(basename ${ENERGY} '.root').slcio
    if [ ! -f ${INPUTDATAFILE} ]
    then
        echo File missing!!!
        return
    fi
    MARLIN_FILE_NAME=Root2Slcio_${COUNT}.xml
    JOB_FILE_NAME=job_${COUNT}.sh
    cp ${MARLIN_INIT_FILE} ${INPUTDATADIR}/${MARLIN_FILE_NAME}
    sed -i 's#SimuInput.root#'${INPUTDATAFILE}'#g' ${INPUTDATADIR}/${MARLIN_FILE_NAME}
    sed -i 's#SimuOutput.slcio#'${OUTPUTDATAFILE}'#g' ${INPUTDATADIR}/${MARLIN_FILE_NAME}
    cat > ${INPUTDATADIR}/${JOB_FILE_NAME} <<EOF
source /afs/ihep.ac.cn/users/q/qibh/setup_CEPC_Software.sh
source /cefs/higgs/qibh/G4Simulation/SimCalModule/script/Root2Slcio/enable_env.sh
cd ${INPUTDATADIR}
Marlin ${MARLIN_FILE_NAME}
EOF
    chmod a+x ${INPUTDATADIR}/${JOB_FILE_NAME}
    hep_sub ${INPUTDATADIR}/${JOB_FILE_NAME}
    let COUNT++
done