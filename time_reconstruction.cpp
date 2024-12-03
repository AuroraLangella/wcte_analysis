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


std::string getBaseFilename(const std::string& inputFilename) {
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

template <typename T>
void plotAndCheckTrend(TTree* tree, const char* branchName, T& value, string originalFileName) {
    // Set the branch address for the input variable
    tree->SetBranchAddress(branchName, &value);

    // Create a canvas for the plot
    TCanvas *c1 = new TCanvas("c1", "Trend Check", 800, 600);
    c1->SetGrid();

    // Create a histogram to plot the trend
    Long64_t nEntries = tree->GetEntries();
    TH1D *hTrend = new TH1D("EventUnixTimeTrend", "EventUnixTimeTrend", nEntries, 0, nEntries);
    hTrend->SetYTitle("Unix Time (s)");
    // Fill the histogram and check the trend
    bool isIncreasing = true;
    T previousValue = 0;  // Assuming your data type supports comparison and is 0 initialized
    for (Long64_t i = 0; i < nEntries; ++i) {
        tree->GetEntry(i);
        hTrend->SetBinContent(i + 1, value); // Bin content starts at 1 in ROOT

        // Check if the trend is increasing
        if (i > 0 && value < previousValue) {
            //if (i < 1000){
            //    cout<<std::setprecision(25)<<value-previousValue<<endl;
            //}
            isIncreasing = false; // Found a decrease
        }

        previousValue = value;
    }

    // Generate the PNG filename appending the original run file name
    std::string outputFileName = originalFileName + "_EventUnixTimeTrend.png";
    
    // Draw the histogram and save it as PNG
    hTrend->Draw("HIST");
    c1->SaveAs(outputFileName.c_str());

    // Print trend check result
    if (isIncreasing) {
        std::cout << "The trend is always increasing." << std::endl;
    } else {
        std::cout << "The trend has one or more decreases." << std::endl;
    }

    // Clean up
    delete hTrend;
    delete c1;
}

int main() {
    auto start = std::chrono::steady_clock::now();

    //Testing Input File    
    //const char* inputfilename = "/home/alangella/wcte_analysis/runs/run045_prova.root";
    //TFile *inputFile = TFile::Open(inputfilename, "READ");
    

    const char* inputfilename = "/home/storage/wcte-data/runs/run045.root";
    TFile *inputFile = TFile::Open(inputfilename, "READ");
    
    
    if (!inputFile || inputFile->IsZombie()) {
        std::cerr << "Errore: impossibile aprire il file di input.\n";
        return 1;
    }
    std::string filename = getBaseFilename(inputfilename);
    cout<<filename<<endl;
    TTree *tree = (TTree*)inputFile->Get("data");

    int charge, PMT_ID;
    int TimeFine, TDCStartTime, TDCCoarseTime, TDCStopTime, SubHitNum;
    Long64_t UnixTime;    
    
    tree->SetBranchAddress("charge", &charge);
    tree->SetBranchAddress("PMT_ID", &PMT_ID);
    tree->SetBranchAddress("FineTime", &TimeFine);
    tree->SetBranchAddress("UnixTime", &UnixTime);
    tree->SetBranchAddress("TDCStartTime", &TDCStartTime);
    tree->SetBranchAddress("TDCCoarseTime", &TDCCoarseTime);
    tree->SetBranchAddress("TDCStopTime", &TDCStopTime);
    tree->SetBranchAddress("SubHitNum", &SubHitNum);

    // Testing Output File
    //TFile *outputFile = new TFile("/home/alangella/wcte_analysis/runs_time_recon/run045_t_recon_prova.root", "RECREATE");     
    
    TFile *outputFile = new TFile("/home/storage/wcte-data/runs_time_recon/"+filename+"_recon.root", "RECREATE");     
    TTree *newtree = new TTree("data", "mPMT data");

    newtree->Branch("Charge", &charge, "Charge/I");
    newtree->Branch("PMT_ID", &PMT_ID, "PMT_ID/I");
    newtree->Branch("SubHitNum", &SubHitNum, "SubHitNum/I");
   

    double EventUnixTime, tot,EventSubSecondTime; 

    newtree->Branch("EventUnixTime", &EventUnixTime, "EventUnixTime(s)/D");
    newtree->Branch("EventSubSecondTime", &EventSubSecondTime, "EventSubSecondTime(s)/D");
    newtree->Branch("ToT", &tot, "ToT/D");


    const double UnixTimeOverflow = 2e14-1; // Overflow ogni 2x10^14 secondi
    

    // Variabili per monitorare overflow
    double correctionUnix = 0,prevUnixTime = 0;
   

    Long64_t N = tree->GetEntries();
    cout<<N<<endl;
    for (int ev=0; ev<N;ev++){

        tree->GetEntry(ev);

        //double correctedUnixTime = correctClock(UnixTime, prevUnixTime, overflowCountUnix, UnixTimeOverflow);
        //double correctedTimeFine = correctClock(TimeFine, prevTimeFine, overflowCountFine, TimeFineOverflow);

        if (UnixTime < prevUnixTime) {
            correctionUnix += UnixTimeOverflow;
        }
        double correctedUnixTime = UnixTime + correctionUnix;

        
        
        EventUnixTime = correctedUnixTime;
        EventSubSecondTime = (double(TimeFine)+double(TDCStartTime)/17)*5*1e-9;
        tot = (double(TDCStartTime)/17+double(TDCCoarseTime)-double(TDCStopTime)/17)*5*1e-9;

        newtree->Fill();

        prevUnixTime = UnixTime;
       
        
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
    plotAndCheckTrend(newtree, "EventUnixTime", EventUnixTime, filename);
    
    outputFile->Close();
    inputFile->Close();

    auto end = std::chrono::steady_clock::now();

    // Calcola la durata
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start);

    std::cout << "Il processo ha impiegato " << duration.count() << " secondi.\n";



    return 0; 
}