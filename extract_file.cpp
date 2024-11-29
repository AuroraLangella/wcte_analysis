#pragma cling standard C++17
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
using std::filesystem; 

void extract_file(){
        
    /*if (gSystem->Load("/opt/ToolFrameworkCore/lib/libStore.so") < 0) {
        printf("Failed to load library\n");
    } else {
        printf("Library loaded successfully\n");
    }*/      
    
    std::string directoryPath = "/storage/wcte-data/";
    
    // Contatore per il numero di file elaborati
    int processedFiles = 0;

    int maxFiles = 2;
    
    // Iterazione sui file nella directory
    for (const auto& entry : directory_iterator(directoryPath)) {
        cout<<"loop iniziato"<<endl;
        if (entry.is_regular_file()) { // Assicurati che sia un file
            cout<<"file regolare"<<endl;
            std::string filePath = entry.path().string();
            
            std::cout << "Processing file: " << filePath << std::endl;
            
            // Oggetti richiesti
            WCTERawData p;
            BinaryStream input;

            // Apri il file
            input.Bopen(filePath, READ, UNCOMPRESSED);

            input >> p;

            //p.Print()
            //p.readout_windows[16].Print()
            p.readout_windows[16].hkmpmt_hits[0].Print();

            //p.readout_windows[16].hkmpmt_hits[0].Dump()

            //p.readout_windows[16].hkmpmt_hits[4].header.GetCardID()

            //p.readout_windows[16].hkmpmt_hits[0].footer.GetCharge()
                      
            processedFiles++;
            
            if (processedFiles >= maxFiles) {
                break;
            }
        }
    }


}
