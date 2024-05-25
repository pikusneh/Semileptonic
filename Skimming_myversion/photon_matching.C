#include <ROOT/RDataFrame.hxx>
#include <Math/VectorUtil.h>
#include <iostream>
#include <Math/VectorUtil.h>
#include <ROOT/RVec.hxx>

using namespace ROOT::VecOps;
using namespace ROOT::Math;

//delta R
double deltaR(double eta1, double phi1, double eta2, double phi2) {
    ROOT::Math::XYZTVector vec1(0, 0, eta1, phi1);
    ROOT::Math::XYZTVector vec2(0, 0, eta2, phi2);
    return ROOT::Math::VectorUtil::DeltaR(vec1, vec2);
}

// match photons
ROOT::VecOps::RVec<int> matchPhotons(const ROOT::VecOps::RVec<float> &reco_pt, const ROOT::VecOps::RVec<float> &reco_eta, const ROOT::VecOps::RVec<float> &reco_phi,
                                     const ROOT::VecOps::RVec<float> &gen_pt, const ROOT::VecOps::RVec<float> &gen_eta, const ROOT::VecOps::RVec<float> &gen_phi,
                                     const ROOT::VecOps::RVec<int> &gen_pdgid, const ROOT::VecOps::RVec<int> &gen_status) {
    ROOT::VecOps::RVec<int> matchedIndices(reco_pt.size(), -1);

    for (size_t i = 0; i < reco_pt.size(); ++i) {
        int bestMatch = -1;
        double bestDr = 0.3;

        for (size_t j = 0; j < gen_pt.size(); ++j) {
            if (gen_status[j] != 1) continue; // Only stable particles

            double dr = deltaR(reco_eta[i], reco_phi[i], gen_eta[j], gen_phi[j]);
            if (dr < bestDr && 0.5 * reco_pt[i] < gen_pt[j] && gen_pt[j] < 1.5 * reco_pt[i]) {
                bestDr = dr;
                bestMatch = j;
            }
        }

        matchedIndices[i] = bestMatch;
    }

    return matchedIndices;
}

// Function to categorize photons
std::vector<int> categorizePhotons(const RVec<int> &matchedIndices, const RVec<int> &gen_pdgid, 
                                   const RVec<float> &gen_eta, const RVec<float> &gen_phi, const RVec<int> &gen_status) {
    std::vector<int> categories(matchedIndices.size(), -1);

    for (size_t i = 0; i < matchedIndices.size(); ++i) {
        int idx = matchedIndices[i];
        if (idx == -1) {
            bool foundPion = false;
            bool foundPhoton = false;

            for (size_t j = 0; j < gen_pdgid.size(); ++j) {
                if (gen_status[j] != 1) continue;
                double dr = deltaR(gen_eta[j], gen_phi[j], gen_eta[idx], gen_phi[idx]);
                if (dr < 0.3) {
                    if (abs(gen_pdgid[j]) == 111) foundPion = true; // Neutral pion
                    if (abs(gen_pdgid[j]) == 22) foundPhoton = true; // Photon
                }
            }

            if (foundPion && foundPhoton) {
                categories[i] = 2; // Hadronic photon
            } else {
                categories[i] = 3; // Pileup photon
            }
        } else if (gen_pdgid[idx] == 22) {
            if (gen_status[idx] == 1) { // Origin from lepton, boson, or quark
                categories[i] = 0; // Genuine photon
            } else if (gen_status[idx] == 2) { // Origin from hadronic source
                categories[i] = 2; // Hadronic photon
            }
        } else if (abs(gen_pdgid[idx]) == 11) {
            categories[i] = 1; // Misidentified electron
        } else {
            categories[i] = 2; // Hadronic photon
        }
    }

    return categories;
}

void photon_matching() {
    // Create a data frame
     auto df = ROOT::RDataFrame("Events", "root://eosuser.cern.ch//eos/user/s/ssnehshu/skimmed.root");   


    // Apply the matching and categorization functions
   auto df2 = df.Define("matchedIndices", matchPhotons,
                     {"Photon_pt", "Photon_eta", "Photon_phi",
                      "GenPart_pt", "GenPart_eta", "GenPart_phi",
                      "GenPart_pdgId", "GenPart_status"})
            .Define("photonCategories", categorizePhotons, 
                     {"matchedIndices", "GenPart_pdgId", "GenPart_eta", 
                      "GenPart_phi", "GenPart_status"});

    // Display the results
    df2.Display({"photonCategories"})->Print();

auto hphoton_categories = df2.Histo1D({"Photon_categories", "Photon categories", 4, 0, 4}, "photonCategories");
auto c = new TCanvas();

// Set bin labels
hphoton_categories->GetXaxis()->SetBinLabel(1, "Genuine photon");
hphoton_categories->GetXaxis()->SetBinLabel(2, "Misidentified electron");
hphoton_categories->GetXaxis()->SetBinLabel(3, "Hadronic photon");
hphoton_categories->GetXaxis()->SetBinLabel(4, "Pileup photon");

hphoton_categories->Draw();
c->SaveAs("photon_categories.png");
}

int main() {
    photon_matching();
    return 0;
}
