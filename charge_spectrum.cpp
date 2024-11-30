#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TMath.h"

using namespace std;

int main() {
    
    TFile *inputFile = TFile::Open("/home/alangella/run045.root", "READ");
    

    
    TTree *tree = (TTree*)inputFile->Get("data");

    int charge, PMT_ID;
    tree->SetBranchAddress("charge", &charge);
    tree->SetBranchAddress("PMT_ID", &PMT_ID);

    //le mappe secondo chatgpt erano utili per fare questa cosa, aveva ragione
    map<int, vector<int>> chargeByPMT;

    
    Long64_t nEntries = tree->GetEntries();
    for (Long64_t i = 0; i < nEntries; ++i) {
        tree->GetEntry(i);
        chargeByPMT[PMT_ID].push_back(charge);
    }

    
    TFile *outputFile = new TFile("histograms_run045.root", "RECREATE");

    
    TCanvas *canvas = new TCanvas("canvas", "Charge Spectrum (logscale)", 800, 600);
    canvas->SetLogy();

    // const auto prende direttamente la dimensione della mappa e permette di ciclare sopra sia 
    //la chiave (pmtID che è un int) che la carica (che è un vector<int>)
    for (const auto& entry : chargeByPMT) {
        int pmt = entry.first;
        const vector<int>& charges = entry.second;

        
        int maxCharge = *max_element(charges.begin(), charges.end());
        int nBins = TMath::Min(100, maxCharge / 10 + 1); 

        
        string histName = "hist_PMT_" + to_string(pmt);
        string histTitle = "Charge Spectrum PMT (" + to_string(pmt) + ")";
        TH1F *hist = new TH1F(histName.c_str(), histTitle.c_str(), nBins, -1000, maxCharge + 1000);

        
        for (int c : charges) {
            hist->Fill(c);
        }

        
        hist->GetXaxis()->SetTitle("ADC");
        hist->GetYaxis()->SetTitle("Counts");
        hist->GetXaxis()->CenterTitle();
        hist->GetYaxis()->CenterTitle();

        
        hist->Draw();
        canvas->SaveAs((histName + "_log.png").c_str());
        hist->Write(); 
    }

    
    outputFile->Close();
    inputFile->Close();

    cout << "Istogrammi creati e salvati in 'histograms.root' xoxo" << endl;
    return 0;
}
