#include <TH1F.h>
#include <TFile.h>

#include <fstream>
#include <string>
#include <iostream>


TH1F* readBetaSpectrum(const char* filename, const char* histname) {
    std::ifstream betaFile(filename);
    if (!betaFile.is_open()) {
        cerr << "Error: could not open file " << filename << endl;
        //return TH1F();
    }

    std::string line;
    double tmpE = 0., tmpW = 0.;
    std::vector<double> energies, weights;
    int nBins = 0;
    while (std::getline(betaFile, line)) {
        if (line[0] == '#') continue;
        std::stringstream iss(line);
        iss >> tmpE;
        iss >> tmpW;
        energies.push_back(tmpE);
        weights.push_back(tmpW);
        std::cout << "Energy: " << tmpE << " Weight: " << tmpW << std::endl;
        nBins++;
    }

    std::vector<double> binEdges;
    for (int i = 0; i < nBins; i++) {
        binEdges.push_back(energies[i]);
    }
    binEdges.push_back(energies[nBins-1] + (energies[nBins-1] - energies[nBins-2]));

    TH1F* hist = new TH1F(histname, "Beta spectrum", nBins, &binEdges[0]);
    for (int ibin = 0; ibin < nBins; ibin++) {
        hist->SetBinContent(ibin+1, weights[ibin]);
    }

    return hist;
}

void sampleElectrons() {
    TH1F* betaHist = readBetaSpectrum("betaSpectrum.dat", "betaHist");
    if (betaHist == nullptr) return;

    // betaHist->Draw("hist");
    
    // sample from the beta spectrum

    // create a new histogram to store the sampled electrons
    std::vector<double> binEdges;
    for (int i = 0; i < betaHist->GetNbinsX()+1; i++) {
        binEdges.push_back(betaHist->GetBinLowEdge(i+1));
    }
    TH1F* sampledHist = new TH1F("sampledHist", "Sampled electrons", betaHist->GetNbinsX(), &binEdges[0]);
    // sampledHist->Reset();

    auto canvas = new TCanvas("c1", "c1", 800, 600);

    for (int i = 0; i < 10000; i++) {
        double randE = betaHist->GetRandom();
        sampledHist->Fill(randE);
    }
    auto clonedHist1 = (TH1F*)sampledHist->Clone();
    clonedHist1->SetLineColor(kRed);
    clonedHist1->SetName("N = 10000");
    for (int i = 0; i < 100000; i++) {
        double randE = betaHist->GetRandom();
        sampledHist->Fill(randE);
    }
    auto clonedHist2 = (TH1F*)sampledHist->Clone();
    clonedHist2->SetLineColor(kBlue);
    clonedHist2->SetName("N = 100000");
    for (int i = 0; i < 1000000; i++) {
        double randE = betaHist->GetRandom();
        sampledHist->Fill(randE);
    }
    auto clonedHist3 = (TH1F*)sampledHist->Clone();
    clonedHist3->SetLineColor(kGreen);
    clonedHist3->SetName("N = 1000000");

    auto hframe = canvas->DrawFrame(0, 0, clonedHist3->GetXaxis()->GetXmax(), clonedHist3->GetMaximum()*1.1);
    clonedHist1->Draw("same");
    clonedHist2->Draw("same");
    clonedHist3->Draw("same");

    canvas->BuildLegend();
    canvas->Draw();
    

}