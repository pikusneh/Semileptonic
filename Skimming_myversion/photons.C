// #include <ROOT/RVec.hxx>
// #include "ROOT/RDataFrame.hxx"
// #include <TFile.h>
// #include <vector>
// #include <string>

// void photons()
// {
//     ROOT::EnableImplicitMT();

//     // Open the file
//     auto df = ROOT::RDataFrame("Events", "root://eosuser.cern.ch//eos/user/s/ssnehshu/skimmed.root");   
//     //number of photons
//     auto nphotons1 = df.Filter("nPhoton > 0");
//     auto hphotons = nphotons1.Histo1D({"nPhoton", "Number of Photons",15,0,15}, "nPhoton");
//     hphotons->Draw();
//     auto hphoton_pt = nphotons1.Histo1D({"Photon_pt", "Photon pt",100,0,100},"Photon_pt");
//     hphoton_pt->Draw();

//     auto df_photons = df.Filter([](const ROOT::VecOps::RVec<int>& pdgId) { return ROOT::VecOps::Contains(pdgId, 22); }, {"GenPart_pdgId"});

//     // Create histogram of photon pt
//     auto hphoton_pt = df_photons.Histo1D({"Photon_pt", "Photon pt",100,0,100},"GenPart_pt");

// //save in png
//     auto c = new TCanvas();
//     c->Divide(2,1);
//     c->cd(1);
//     hphoton_pt->Draw();
//     c->cd(2);
//     hphoton_pt1->Draw();
//     c->SaveAs("photon_pt.png");

//     auto c1 = new TCanvas();
//     hphotons->Draw();
//     c1->SaveAs("nphotons.png");

//     // TFile *f = new TFile("photons.root", "RECREATE");
//     // hphoton_pt->Write();
// }


#include <ROOT/RVec.hxx>
#include <ROOT/RDF/RInterface.hxx>

ROOT::VecOps::RVec<float> select_pt(const ROOT::VecOps::RVec<int>& pdgId, const ROOT::VecOps::RVec<float>& pt) {
    ROOT::VecOps::RVec<float> selected_pt;
    for (size_t i = 0; i < pdgId.size(); ++i) {
        if (pdgId[i] == 22) {
            selected_pt.push_back(pt[i]);
        }
    }
    return selected_pt;
}

void photons()
{
    ROOT::EnableImplicitMT();

    // Open the file
    auto df = ROOT::RDataFrame("Events", "root://eosuser.cern.ch//eos/user/s/ssnehshu/skimmed.root");   

    // Select the pt of the particles with pdgId 22
    auto df_photons = df.Define("Selected_pt", select_pt, {"GenPart_pdgId", "GenPart_pt"});

    // Create histogram of photon pt
    auto hphoton_pt_gen = df_photons.Histo1D({"Photon_pt_gen", "Photon pt",100,0,100},"Selected_pt");

    // Draw the histogram
    hphoton_pt_gen->Draw();

    // Save the histogram to a file
    auto c = new TCanvas();
    hphoton_pt_gen->Draw();
    c->SaveAs("photon_pt_gen.png");
}