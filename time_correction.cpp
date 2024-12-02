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

using namespace std;

int main() {


    TFile *inputFile = TFile::Open("/home/alangella/wcte_analysis/runs/run045_prova.root", "READ");

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

    newtree->Branch("Event_Time", &event_time, "Event_Time/I");
    newtree->Branch("ToT", &tot, "ToT/I");


    int N = tree->GetEntries();
    for (int ev=0; ev<N;ev++){

        tree->GetEntry(ev);
        
        event_time = UnixTime+(TimeFine+TDCStartTime/17)*5*pow(10,-9);
        tot = (TDCStartTime/17+TDCCoarseTime-TDCStopTime/17)*5*pow(10,-9);

        newtree->Fill();

    }

    outputFile->Write();
    outputFile->Close();
    inputFile->Close();












    return 0; 
}