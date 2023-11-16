# SimCalModule

*Last modified in November 13th, 2023. (release v1.0)*

## Introduction

This Geant4 simulation toolkit helps you to understand shower profiles with high granularity calorimeters. The ECAL and HCAL modules have been realized, as well as upstream and intermediate materials (optional).

## Instructions

* Change variables in ***DetectorConstruction.cc*** to modify geometry.
* Use optional UI commands in ***run.mac*** to perform customized runs.
* Change number of CPU cores (threads) you want to use in ***SimCalModule.cc***, or with UI commands.
* Usage: `./SimCalModule [MacFile] [FixedRamdonNumber(Optional)] [OutputGDMLFileName(Optional)]`

## Notes

* This toolkit is highly recommended to be compiled with Geant4 version 11.0 or later, since shell commands are used under the multithreading mode, which will be wrongly executed with old versions. If you are now using version 10.7, please uncomment the line with the declaration of **SerialOnly RunManager** in ***SimCalModule.cc***, and the followed lines related to multithreading need to be commented out. As shown:
  ```c++
  auto runManager = G4RunManagerFactory::CreateRunManager(G4RunManagerType::SerialOnly);
  // auto runManager = G4RunManagerFactory::CreateRunManager();
  // G4int nThreads = std::min(G4Threading::G4GetNumberOfCores(), 6);
  // runManager->SetNumberOfThreads(nThreads);
  ```
* If you do not manually set a random number, the random number will be based on the system time. However, when submitting jobs to HTCondor, this number may be the same, so the following changes can be made in ***SimCalModule.cc***:
  ```c++
  seed = atoi(argv[2]) * seed;
  ```

## Change History

* 2022.12.30: PCB now needs to be placed manully. A basic cell is composed of active, passive and attached materials (real PCB for crossed bars ECAL).
* 2023.06.25: General bug fixes and code improvements. Add GSHCAL setup, StepTimeLimit commands.
* 2023.11.13: General bug fixes and code improvements. Add cover to calorimeters. The default setting changed to ScW ECAL and AHCAL CERN beamteat simulation.

## Known issues

* Incorrect RunID output with worker threads when simulating very few events per run (generally less than number of threads) with Geant4 11.1.1.
