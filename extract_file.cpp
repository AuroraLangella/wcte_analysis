#include <iostream>
#include <filesystem>
#include <stdio.h>  
#include <stdlib.h>  
#include "Tool.h"
#include "Store.h"
#include "WCTERawData.h"
#include "TFile.h"
#include "TTree.h"

using namespace std;
using std::cout;  
using std::endl;  
using std::vector; 
using namespace std::filesystem; 

int main(){
        
        
    
    std::string directoryPath = "/storage/wcte-data/run045/";
    
    
    int processedFiles = 0;

    int maxFiles = 999999;
    //int maxFiles = 2;
    
    //std::vector<int> charge;
    //std::vector<int> PMT_ID;
    double total_size = 0;

    TFile *outputFile = new TFile("runs/run045_prova.root", "RECREATE");

    // Creazione del TTree
    TTree *tree = new TTree("data", "Tree with charge and PMT_ID");

    // Variabili per i branch
    int charge_entry;
    int PMT_ID_entry;

    // Aggiunta dei branch al TTree
    TBranch *charge = tree->Branch("charge", &charge_entry);
    TBranch *PMT_ID = tree->Branch("PMT_ID", &PMT_ID_entry);
    
    for (const auto& entry : directory_iterator(directoryPath)) {
        

        if (entry.is_regular_file()) { // Assicurati che sia un file regolare
            
            string filePath = entry.path().string();
            
            //cout << "Processing file: " << filePath << std::endl;
            
        
            WCTERawData p;
            BinaryStream input;

            // Apri un singolo file di 20 s
            input.Bopen(filePath, READ, UNCOMPRESSED);

            input >> p;

            //Leggi tutto il file
            //p.Print();

            // Leggi un singolo time slice di 1 s 
            //p.readout_windows[16].Print();

            //Apri un singolo time slice (cioè la 16) e da questo prendi l'evento hkmpmt_hits[0]
            //p.readout_windows[16].hkmpmt_hits[0].Print();
            //Leggi i dati raw di questo evento 
            //p.readout_windows[16].hkmpmt_hits[0].Dump();
            //int charge[sizeof(p.readout_windows[16])];
            
            
                    
            
            //int j = 14;
            
            for (int i=0; i<p.readout_windows.size();i++){
            total_size += p.readout_windows[i].hkmpmt_hits.size();
            }
            
            for (int j=0; j<p.readout_windows.size();j++){
            
                for (int i=0; i<p.readout_windows[j].hkmpmt_hits.size(); i++){
                
                //cout<<"Valore carica dell'elemento "<<i<<" : "<<p.readout_windows[16].hkmpmt_hits[i].footer.GetCharge()<<endl;
                
                
                /*if (p.readout_windows[16].hkmpmt_hits[i].footer.GetCharge() == 4095){

                    cout<< p.readout_windows[16].hkmpmt_hits[i].subhits<<endl;
                }*/
                //charge.push_back(p.readout_windows[j].hkmpmt_hits[i].footer.GetCharge());
                //PMT_ID.push_back(p.readout_windows[j].hkmpmt_hits[i].header.GetChannel());
                charge_entry = p.readout_windows[j].hkmpmt_hits[i].footer.GetCharge();
                PMT_ID_entry = p.readout_windows[j].hkmpmt_hits[i].header.GetChannel();

                // Riempimento del TTree con i dati correnti
                tree->Fill();
                }
            }
            

            /*
            for (int val : charge) {
                std::cout << val << endl;
            }
            */
           


           
            //p.readout_windows[16].hkmpmt_hits[4].header.GetCardID();
            // Leggi la carica dell'evento
            //p.readout_windows[16].hkmpmt_hits[0].footer.GetCharge();
                      
            processedFiles++;
            
            if (processedFiles >= maxFiles) {
                break;
            }
        }
    }
    cout<<"Total number of events in the file:"<<total_size<<endl;
    cout << "Charge vector and PMT ID filled"<<endl;

    cout<<"Charge size: "<< charge->GetEntries()<<" PMT ID size: "<< PMT_ID->GetEntries()<<endl;

    //cout<<"Charge size: "<< charge.size()<<" PMT ID size: "<< PMT_ID.size()<<endl;

    

    
    /*
    size_t numEntries = std::min(charge.size(), PMT_ID.size());
    for (size_t i = 0; i < numEntries; ++i) {
        charge_entry = charge[i];
        PMT_ID_entry = PMT_ID[i];

        // Riempimento del TTree con i dati correnti
        tree->Fill();
    }
    */

    // Scrittura del TTree nel file
    tree->Write();

    // Chiudere il file .root
    outputFile->Close();


return 0;
}
