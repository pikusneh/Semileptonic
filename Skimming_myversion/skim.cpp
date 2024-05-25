#include "skim.h"

skim::skim(bool isMC, const std::string &year, const std::string &filelist)
    : isMC(isMC), year(year), filelist(filelist), df("Events", readInputFiles(filelist)) {}

bool skim::startsWith(const char *string, const char *prefix) {
     size_t stringLen = strlen(string);
     size_t prefixLen = strlen(prefix);

    if (prefixLen > stringLen)
    {
        return false;
    }

    return strncmp(string, prefix, prefixLen) == 0;
}

std::vector<std::string> skim::readInputFiles(const std::string &filename) {
    std::vector<std::string> inputFiles;
    std::ifstream infile(filename);
    std::string line;
    while (std::getline(infile, line))
    {
        if (!line.empty() && line[0] != '#')
        { // Ignore comments and empty lines
            inputFiles.push_back(line);
        }
    }
    return inputFiles;
}

void skim::myskim() {
    std::vector<std::string> inputFiles = readInputFiles(filelist);
    // if (inputFiles.empty()) {
    //     std::cout << "No input files found" << std::endl;
    //     return;
    // }

    // // Check if each file can be opened and contains a TTree named "Events"
    // for (const auto& file : inputFiles) {
    //     TFile *rootFile = TFile::Open(file.c_str());
    //     if (rootFile == nullptr || rootFile->Get("Events") == nullptr) {
    //         std::cout << "Error with file: " << file << std::endl;
    //         return;
    //     }
    // }

    ROOT::RDataFrame df("Events", inputFiles);

    std::vector<std::string> branches;
    auto branchList = df.GetColumnNames();
    for (const auto &branchName : branchList)
    {
        if (startsWith(branchName.c_str(), "Electron_") ||
                startsWith(branchName.c_str(), "Muon_") ||
                startsWith(branchName.c_str(), "Jet_") ||
                startsWith(branchName.c_str(), "Photon_") ||
                startsWith(branchName.c_str(), "Flag_") ||
                startsWith(branchName.c_str(), "L1") ||
                startsWith(branchName.c_str(), "MET_") ||
                startsWith(branchName.c_str(), "PV_") ||
                startsWith(branchName.c_str(), "SV_") ||
                startsWith(branchName.c_str(), "btag") ||
                startsWith(branchName.c_str(), "Iso") ||
                startsWith(branchName.c_str(), "run") ||
                startsWith(branchName.c_str(), "luminosityBlock")){
            branches.push_back(branchName);
        }
    }

    if (isMC)
    {
        branches.push_back("Pileup_nPU");
        branches.push_back("Pileup_nTrueInt");
        branches.push_back("Generator_weight");
        branches.push_back("GenMET_pt");
        branches.push_back("GenMET_phi");
        branches.push_back("nGenPart");
        branches.push_back("nLHEPart");
        branches.push_back("nGenJet");
        branches.push_back("nPSWeight");
        branches.push_back("PSWeight");
        branches.push_back("nLHEPdfWeight");
        branches.push_back("LHEPdfWeight");
        branches.push_back("nLHEScaleWeight");
        branches.push_back("LHEScaleWeight");
        branches.push_back("LHEWeight_originalXWGTUP");

        for (const auto &branchName : branchList)
        {
            if (startsWith(branchName.c_str(), "GenPart_") ||
                startsWith(branchName.c_str(), "LHEPart_") ||
                startsWith(branchName.c_str(), "GenJet_"))
            {
                // startsWith(branchName.c_str(), "L1PreFiringWeight_")) {
                branches.push_back(branchName);
            }
        }
    }

    if (year == "2016" || year == "2017")
    {
        // branches.push_back("L1PreFiringWeight");
    }

    if (year == "2017" || year == "2018")
    {
        branches.push_back("Flag_ecalBadCalibFilterV2");
    }

    if (year == "2016")
    {
        branches.push_back("HLT_Ele27_WPTight_Gsf");
        branches.push_back("HLT_Ele105_CaloIdVT_GsfTrkIdT");
        branches.push_back("HLT_Ele115_CaloIdVT_GsfTrkIdT");
        branches.push_back("HLT_Photon175");
        branches.push_back("HLT_IsoMu24");
        branches.push_back("HLT_IsoTkMu24");
        branches.push_back("HLT_Mu50");
        branches.push_back("HLT_TkMu50");
        branches.push_back("HLT_Mu45_eta2p1");
    }

    if (year == "2017")
    {
        branches.push_back("HLT_IsoMu24");
        branches.push_back("HLT_IsoMu24_eta2p1");
        branches.push_back("HLT_IsoMu27");
        branches.push_back("HLT_Mu50");
        branches.push_back("HLT_OldMu100");
        branches.push_back("HLT_TkMu100");
        branches.push_back("HLT_Ele32_WPTight_Gsf_L1DoubleEG");
        branches.push_back("HLT_Ele32_WPTight_Gsf");
        branches.push_back("HLT_Ele35_WPTight_Gsf");
        branches.push_back("HLT_Ele115_CaloIdVT_GsfTrkIdT");
        branches.push_back("HLT_Photon200");
    }

    if (year == "2018")
    {
        branches.push_back("HLT_IsoMu24");
        branches.push_back("HLT_IsoMu27");
        branches.push_back("HLT_Mu50");
        branches.push_back("HLT_OldMu100");
        branches.push_back("HLT_TkMu100");
        branches.push_back("HLT_Ele32_WPTight_Gsf");
        branches.push_back("HLT_Ele35_WPTight_Gsf");
        branches.push_back("HLT_Ele38_WPTight_Gsf");
        branches.push_back("HLT_Ele32_WPTight_Gsf_L1DoubleEG");
        branches.push_back("HLT_Ele115_CaloIdVT_GsfTrkIdT");
        branches.push_back("HLT_DoubleEle25_CaloIdL_MW");
        branches.push_back("HLT_Photon200");
    }


/////////Trigger selection for electron and muon channel

    auto df1 = df.Define("Pass_trigger_mu", [this](bool HLT_IsoMu24, bool HLT_IsoTkMu24, bool HLT_Mu50, bool HLT_TkMu50, bool HLT_Mu45_eta2p1)
                         {
        if (this->year == "2016") {
            return HLT_IsoMu24 || HLT_IsoTkMu24 || HLT_Mu50 || HLT_TkMu50 || HLT_Mu45_eta2p1;
        }
        return false; }, {"HLT_IsoMu24", "HLT_IsoTkMu24", "HLT_Mu50", "HLT_TkMu50", "HLT_Mu45_eta2p1"})

                   .Define("Pass_trigger_ele", [this](bool HLT_Ele27_WPTight_Gsf, bool HLT_Ele105_CaloIdVT_GsfTrkIdT, bool HLT_Ele115_CaloIdVT_GsfTrkIdT, bool HLT_Photon175)
                           {
        if (this->year == "2016") {
            return HLT_Ele27_WPTight_Gsf || HLT_Ele105_CaloIdVT_GsfTrkIdT || HLT_Ele115_CaloIdVT_GsfTrkIdT || HLT_Photon175;
        }
        return false; }, {"HLT_Ele27_WPTight_Gsf", "HLT_Ele105_CaloIdVT_GsfTrkIdT", "HLT_Ele115_CaloIdVT_GsfTrkIdT", "HLT_Photon175"})

                   .Define("filters", [this](bool Flag_goodVertices, bool Flag_globalSuperTightHalo2016Filter, bool Flag_HBHENoiseFilter, bool Flag_HBHENoiseIsoFilter, bool Flag_EcalDeadCellTriggerPrimitiveFilter, bool Flag_BadPFMuonFilter)
                           {
        bool basicFilters = Flag_goodVertices && Flag_globalSuperTightHalo2016Filter && Flag_HBHENoiseFilter && Flag_HBHENoiseIsoFilter && Flag_EcalDeadCellTriggerPrimitiveFilter && Flag_BadPFMuonFilter;
        return basicFilters; }, {"Flag_goodVertices", "Flag_globalSuperTightHalo2016Filter", "Flag_HBHENoiseFilter", "Flag_HBHENoiseIsoFilter", "Flag_EcalDeadCellTriggerPrimitiveFilter", "Flag_BadPFMuonFilter"})

                   .Filter([](bool Pass_trigger_mu, bool Pass_trigger_ele, bool filters)
                           { return (Pass_trigger_ele || Pass_trigger_mu) && filters; }, {"Pass_trigger_mu", "Pass_trigger_ele", "filters"});
    

    std::vector<std::string> allbranches = branches;
    allbranches.push_back("Pass_trigger_mu");
    allbranches.push_back("Pass_trigger_ele");
    allbranches.push_back("filters");

    std::string outputFileName = "/eos/user/s/ssnehshu/skimmed.root";
    df1.Snapshot("Events", outputFileName, allbranches);
}