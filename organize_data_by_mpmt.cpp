#include <iostream>
#include <map>
#include <vector>
#include <cmath>
#include <string>
#include <algorithm>
#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TMath.h"
#include <chrono>
#include "TH1D.h"
#include <iomanip> 

using namespace std;

int main(int argc, char* argv[]) {

    auto start = std::chrono::steady_clock::now();

    

    string runNumber = argv[1];
    
    string inFileName = "/storage/wcte-recon/runs/run" + std::string(3 - runNumber.length(), '0') + runNumber + ".root";    
    cout<<"Organizing data by mPMT for  "<< inFileName<<endl;
    TFile* inputFile = TFile::Open(inFileName.c_str(), "READ");
    if (!inputFile || inputFile->IsZombie()) {
        std::cerr << "Errore: impossibile aprire il file " << inFileName << std::endl;
        return 1;
    }

   
    TTree *tree = (TTree*)inputFile->Get("data");

    if (!tree) {
        std::cerr << "Errore: TTree non trovato nel file " << inFileName << std::endl;
        inputFile->Close();
        return 1;
    }

    
    int mPMT_ID;
    tree->SetBranchAddress("mPMT_ID", &mPMT_ID);

    // Crea mappe per gestire i nuovi file e TTree
    map<int, TFile*> outputFiles;
    map<int, TTree*> outputTrees;

    // Inizializza i file e i TTree per mPMT_ID = 1, 2, 3, 4
    for (int id = 1; id <= 4; ++id) {
        string outputFileName = "/storage/wcte-recon/runs_by_mPMT/mPMT" + std::to_string(id) + "_run" + std::string(3 - runNumber.length(), '0') + runNumber + ".root";
        outputFiles[id] = TFile::Open(outputFileName.c_str(), "RECREATE");
        outputTrees[id] = tree->CloneTree(0); // Clona la struttura del TTree, ma senza dati
    }

    
    Long64_t nEntries = tree->GetEntries();
    for (Long64_t i = 0; i < nEntries; ++i) {
        tree->GetEntry(i);

        // Filtra i dati in base al valore di mPMT_ID
        if (outputTrees.find(mPMT_ID) != outputTrees.end()) {
            outputTrees[mPMT_ID]->Fill();
        }
    }

    
    for (int id = 1; id <= 4; ++id) {
        outputFiles[id]->cd();
        outputTrees[id]->Write();
        outputFiles[id]->Close();
    }

   
    inputFile->Close();

    cout << "Operazione completata. File generati per mPMT_ID 1, 2, 3, 4." << std::endl;

    auto end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start);

    cout << "Il processo ha impiegato " << duration.count() << " secondi.\n";


return 0;
}