#!/bin/bash

Time=20230515
Project=SimCalModule
InDir=/cefs/higgs/qibh/G4Simulation/Data/${Project}/run${Time}_AHCAL/pi-
OutDir=/cefs/higgs/qibh/G4Simulation/Data/${Project}/run${Time}_AHCAL/pi-

cd /cefs/higgs/qibh/G4Simulation/SimCalModule/script

for datafile in $(ls ${InDir} | grep '.root' | grep 'MC' | grep -v 'Digi')
do
    root -l -b -q 'EnergySumDigi.cxx("'${InDir}'","'${OutDir}'","'${datafile}'")'
done