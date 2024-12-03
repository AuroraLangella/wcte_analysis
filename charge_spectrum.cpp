#include <iostream>
#include <map>
#include <vector>
#include <cmath>
#include <string>
#include <algorithm>
#include <filesystem>
#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TMath.h"
#include <chrono>
#include "TH1D.h"
#include <iomanip> 

using namespace std::filesystem; 
using namespace std;

string getBaseFilename(const std::string& inputFilename) {
    // Find the position of the last '/' in the string (directory separator)
    size_t lastSlashPos = inputFilename.find_last_of("/");

    // Extract the file name (everything after the last '/')
    std::string filenameWithExtension = inputFilename.substr(lastSlashPos + 1);

    // Find the position of the last '.' in the file name (file extension separator)
    size_t lastDotPos = filenameWithExtension.find_last_of(".");

    // Extract the base filename (everything before the last '.')
    std::string baseFilename = filenameWithExtension.substr(0, lastDotPos);

    return baseFilename;
}

int main(int argc, char* argv[]) {
    
    string mPMT_ID = argv[1];
    string runNumber = argv[2];
    
    string inFileName = "/storage/wcte-recon/runs_by_mPMT_time_rec/mPMT"+mPMT_ID+"_run" + std::string(3 - runNumber.length(), '0') + runNumber + ".root"; 
    
    string filename = getBaseFilename(inFileName);
    TFile *inputFile = TFile::Open(inFileName.c_str(), "READ");
    

    
    TTree *tree = (TTree*)inputFile->Get("data");

    int charge, PMT_ID;
    // piedistalli mpmt04 (manca per PMT 1 perché non calibrato)
    int ped[] = {260,0,257,259,257,260,256,260,258,260,259,260,260,257,259,259,261,258,258}; 
    int cont = 0;
    
    tree->SetBranchAddress("Charge", &charge);
    tree->SetBranchAddress("PMT_ID", &PMT_ID);

    //le mappe secondo chatgpt erano utili per fare questa cosa, aveva ragione
    map<int, vector<int>> chargeByPMT;

    
    Long64_t nEntries = tree->GetEntries();
    for (Long64_t i = 0; i < nEntries; ++i) {
        tree->GetEntry(i);
        chargeByPMT[PMT_ID].push_back(charge);
    }
    string dirName = "/home/alangella/wcte_analysis/plots/"+filename;
    create_directory(dirName);

    string outFileName = dirName+"/ChargeSpectrum_"+filename+".root";
    TFile *outputFile = new TFile(outFileName.c_str(), "RECREATE");    
    
    TCanvas *canvas = new TCanvas("canvas", "Charge Spectrum (logscale)", 800, 600);
    //canvas->SetLogy();
    // const auto prende direttamente la dimensione della mappa e permette di ciclare sopra sia 
    //la chiave (pmtID che è un int) che la carica (che è un vector<int>)
    for (const auto& entry : chargeByPMT) {

        int pmt = entry.first;
        const vector<int>& charges = entry.second;

        
        int maxCharge = *max_element(charges.begin(), charges.end());
        //int nBins = TMath::Min(100, maxCharge / 10 + 1); 
        int nBins = maxCharge-ped[cont];

        


        string histName = "hist_PMT_" + to_string(pmt);
        string histTitle = "Charge Spectrum PMT (" + to_string(pmt) + ")";

        if (pmt == 4){
            int maxCharge = *max_element(charges.begin(), charges.end());
         
            string histName_zoom = "hist_PMT_zoom" + to_string(pmt);
            TH1F *hist_zoom = new TH1F(histName_zoom.c_str(), histTitle.c_str(), 1000-260, 260, 1000);
            for (int c : charges){
            hist_zoom->Fill(c);
                        }
            hist_zoom->GetXaxis()->SetTitle("ADC");
            hist_zoom->GetYaxis()->SetTitle("Counts");
            hist_zoom->GetXaxis()->CenterTitle();
            hist_zoom->GetYaxis()->CenterTitle();        
            hist_zoom->Draw();
            canvas->SaveAs((dirName+"/"+histName_zoom + ".png").c_str());
            //canvas->SaveAs(("./plots/"+histName_zoom + "_log.png").c_str());

            canvas->Clear();
        }

        TH1F *hist = new TH1F(histName.c_str(), histTitle.c_str(), nBins, ped[cont], maxCharge);
        
        
        for (int c : charges) {
            hist->Fill(c);
        }

        
        hist->GetXaxis()->SetTitle("ADC");
        hist->GetYaxis()->SetTitle("Counts");
        hist->GetXaxis()->CenterTitle();
        hist->GetYaxis()->CenterTitle();

        
        hist->Draw();

        //canvas->SaveAs((dirName+"/"+histName + ".png").c_str());
        //canvas->SaveAs((dirName+"/"+histName + "_log.png").c_str());
        hist->Write(); 
        canvas->Clear();
        cont = cont+1;
    }

    
    outputFile->Close();
    inputFile->Close();

    cout << "Istogrammi creati e salvati in"<< dirName << endl;
    return 0;
}
