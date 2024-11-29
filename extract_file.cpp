#include <iostream>
#include <filesystem>
#include <stdio.h>  
#include <stdlib.h>  
#include "Tool.h"
#include "Store.h"
#include "WCTERawData.h"

using namespace std;
using std::cout;  
using std::endl;  
using std::vector; 
using namespace std::filesystem; 

int main(){
        
        
    
    std::string directoryPath = "/storage/wcte-data/run045/";
    
    
    int processedFiles = 0;

    int maxFiles = 1;
    
    
    for (const auto& entry : directory_iterator(directoryPath)) {
        

        if (entry.is_regular_file()) { // Assicurati che sia un file regolare
            
            string filePath = entry.path().string();
            
            cout << "Processing file: " << filePath << std::endl;
            
        
            WCTERawData p;
            BinaryStream input;

            // Apri un singolo file di 20 s
            input.Bopen(filePath, READ, UNCOMPRESSED);

            input >> p;

            //Leggi tutto il file
            //p.Print()

            // Leggi un singolo time slice di 1 s 
            //p.readout_windows[16].Print()

            //Apri un singolo time slice (cioè la 16) e da questo prendi l'evento hkmpmt_hits[0]
            //p.readout_windows[16].hkmpmt_hits[0].Print();
            //Leggi i dati raw di questo evento 
            //p.readout_windows[16].hkmpmt_hits[0].Dump()
            //int charge[sizeof(p.readout_windows[16])];
            

            //int n = sizeof(p.readout_windows[16]) / sizeof(p.readout_windows[16].hkmpmt_hits[0]);   
            //cout << n << endl;              
            //int* charge = new int[n];
            std::vector<int> charge;
            //cout<<"Size of charge:"<< sizeof(charge)<<endl<<endl;
            for (int i=0; i<sizeof(p.readout_windows[16]); i++){
                cout<<"Valore carica dell'elemento"<<i<<" : "<<p.readout_windows[16].hkmpmt_hits[i].footer.GetCharge()<<endl;
                //charge[i] = p.readout_windows[16].hkmpmt_hits[i].footer.GetCharge();
                //cout<< p.readout_windows[16].hkmpmt_hits[i].footer.GetCharge()<<endl;
                
                /*if (p.readout_windows[16].hkmpmt_hits[i].footer.GetCharge() == 4095){

                    cout<< p.readout_windows[16].hkmpmt_hits[i].subhits<<endl;
                }*/
               charge.push_back(p.readout_windows[16].hkmpmt_hits[i].footer.GetCharge());
            }

            cout << "Charge vector filled"<<endl;
            for (int val : charge) {
                std::cout << val << endl;
            }

            //Leggi quale pmt ha visto l'evento
            p.readout_windows[16].hkmpmt_hits[4].header.GetCardID();
            // Leggi la carica dell'evento
            p.readout_windows[16].hkmpmt_hits[0].footer.GetCharge();
                      
            processedFiles++;
            
            if (processedFiles >= maxFiles) {
                break;
            }
        }
    }



return 0;
}
