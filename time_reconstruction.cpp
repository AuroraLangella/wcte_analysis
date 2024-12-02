#include <iostream>
#include <map>
#include <vector>
#include <cmath>
#include <algorithm>
#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TMath.h"
#include <chrono>
using namespace std;

int main() {
    auto start = std::chrono::steady_clock::now();

    TFile *inputFile = TFile::Open("/home/alangella/wcte_analysis/runs/run045.root", "READ");
    
    if (!inputFile || inputFile->IsZombie()) {
        std::cerr << "Errore: impossibile aprire il file di input.\n";
        return 1;
    }
    
    TTree *tree = (TTree*)inputFile->Get("data");

    int charge, PMT_ID;
    int TimeFine, TDCStartTime, TDCCoarseTime, TDCStopTime, UnixTime, SubHitNum;
    // piedistalli mpmt04 (manca per PMT 1 perché non calibrato)
    int ped[] = {260,0,257,259,257,260,256,260,258,260,259,260,260,257,259,259,261,258,258}; 
    int cont = 0;
    
    tree->SetBranchAddress("charge", &charge);
    tree->SetBranchAddress("PMT_ID", &PMT_ID);
    tree->SetBranchAddress("FineTime", &TimeFine);
    tree->SetBranchAddress("UnixTime", &UnixTime);
    tree->SetBranchAddress("TDCStartTime", &TDCStartTime);
    tree->SetBranchAddress("TDCCoarseTime", &TDCCoarseTime);
    tree->SetBranchAddress("TDCStopTime", &TDCStopTime);
    tree->SetBranchAddress("SubHitNum", &SubHitNum);

    TFile *outputFile = new TFile("/home/alangella/wcte_analysis/runs_t_corrected/run045.root", "RECREATE");    
    TTree *newtree = new TTree("data", "mPMT data");

    newtree->Branch("Charge", &charge, "Charge/I");
    newtree->Branch("PMT_ID", &PMT_ID, "PMT_ID/I");
    newtree->Branch("SubHitNum", &SubHitNum, "SubHitNum/I");
   

    double event_time, tot; 

    newtree->Branch("EventTime", &event_time, "EventTime/D");
    newtree->Branch("ToT", &tot, "ToT/D");
   

    Long64_t N = tree->GetEntries();
    cout<<N<<endl;
    for (int ev=0; ev<N;ev++){

        tree->GetEntry(ev);
        
        event_time = double(UnixTime)+(double(TimeFine)+double(TDCStartTime)/17)*5*1e-9;
        tot = (double(TDCStartTime)/17+double(TDCCoarseTime)-double(TDCStopTime)/17)*5*1e-9;

        newtree->Fill();
        
        /*
        //debug
        if (ev<10){
            std::cout << "TDCStartTime: " << TDCStartTime<< ", TDCCoarseTime: " << TDCCoarseTime<<", TDCStopTime: " << TDCStopTime << std::endl;

            double part1 = double(TDCStartTime) / 17;
            double part2 = double(TDCCoarseTime);
            double part3 = double(TDCStopTime) / 17;
            std::cout << "Part1: " << part1 << ", Part2: " << part2 << ", Part3: " << part3 << std::endl;

            tot = (part1 + part2 - part3) * 5 * 1e-9;
            std::cout << "Tot: " << tot << std::endl;
        }
        */
        
    }
    

    outputFile->Write();
    outputFile->Close();
    inputFile->Close();

    auto end = std::chrono::steady_clock::now();

    // Calcola la durata
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start);

    std::cout << "Il processo ha impiegato " << duration.count() << " secondi.\n";



    return 0; 
}