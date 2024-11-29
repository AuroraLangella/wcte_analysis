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
        
    /*if (gSystem->Load("/opt/ToolFrameworkCore/lib/libStore.so") < 0) {
        printf("Failed to load library\n");
    } else {
        printf("Library loaded successfully\n");
    }*/      
    
    std::string directoryPath = "/storage/wcte-data/run045/";
    
    
    int processedFiles = 0;

    int maxFiles = 2;
    
    
    for (const auto& entry : std::filesystem::directory_iterator(directoryPath)) {
        cout<<"loop iniziato"<<endl;

        if (entry.is_regular_file()) { // Assicurati che sia un file
            cout<<"file regolare"<<endl;
            std::string filePath = entry.path().string();
            
            std::cout << "Processing file: " << filePath << std::endl;
            
        
            WCTERawData p;
            BinaryStream input;

            // Apri un singolo file 
            input.Bopen(filePath, READ, UNCOMPRESSED);

            input >> p;

            //Leggi tutto il file
            //p.Print()

            // Leggi un singolo time slice 
            //p.readout_windows[16].Print()

            //Apri un singolo time slice [16] e da questo prendi l'evento hkmpmt_hits[0]
            p.readout_windows[16].hkmpmt_hits[0].Print();
            //Leggi i dati raw di questo evento 
            //p.readout_windows[16].hkmpmt_hits[0].Dump()
            //Leggi qual
            //p.readout_windows[16].hkmpmt_hits[4].header.GetCardID()

            //p.readout_windows[16].hkmpmt_hits[0].footer.GetCharge()
                      
            processedFiles++;
            
            if (processedFiles >= maxFiles) {
                break;
            }
        }
    }



return 0;
}
