#include "ROOT/RDataFrame.hxx"
#include "ROOT/RVec.hxx"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "TFile.h"


void test() {
TFile *file = TFile::Open("root://cmsxrootd.fnal.gov//store/mc/RunIISummer20UL16NanoAODv9/TTGamma_Dilept_TuneCP5_13TeV-madgraph-pythia8/NANOAODSIM/106X_mcRun2_asymptotic_v17-v1/270000/AD8F98BF-3E3F-1E48-9810-05DCD0D1CE5D.root");
if (file == nullptr) {
    std::cout << "Failed to open file" << std::endl;
} else if (file->Get("Events") == nullptr) {
    std::cout << "TTree 'Events' not found in file" << std::endl;
} else {
    std::cout << "TTree 'Events' found in file" << std::endl;
}};