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
#include "TF1.h"       
#include "TLegend.h"  
#include "TLine.h"

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

#include "TMath.h"

Double_t TruncatedGaussian(Double_t *x, Double_t *par) {
    // par[0] = amplitude (normalization)
    // par[1] = mean (mu)
    // par[2] = sigma
    // par[3] = x_cut (truncation threshold)

    Double_t xx = x[0];
    Double_t A  = par[0];
    Double_t mu = par[1];
    Double_t sigma = par[2];
    Double_t x_cut = par[3];

    if (xx < x_cut) return 0;

    //Double_t norm = 1.0 / (1.0 - TMath::Prob((x_cut - mu) / sigma, 1)); // use TMath::Erfc or Erf for more precise control
    Double_t gaus = TMath::Gaus(xx, mu, sigma, kFALSE); 
    //return A * norm * gaus;
    return A * gaus;
}

Double_t crystalBall(Double_t *x, Double_t *par) {
    Double_t t = (x[0] - par[1]) / par[2];
    if (par[0] < 0) t = -t;
    Double_t absAlpha = fabs((Double_t)par[0]);
    if (t >= -absAlpha) {
        return par[3] * exp(-0.5 * t * t);
    } else {
        Double_t a = TMath::Power(par[4] / absAlpha, par[4]) * exp(-0.5 * absAlpha * absAlpha);
        Double_t b = par[4] / absAlpha - absAlpha;
        return par[3] * (a / TMath::Power(b - t, par[4]));
    }
}

Double_t GaussExpRight(Double_t *x, Double_t *par) {
    // par[0] = A: amplitude
    // par[1] = mu: mean
    // par[2] = sigma: gaussian width
    // par[3] = x_exp: switch point to exponential
    // par[4] = lambda: exponential slope
    // par[5] = x_cut: (optional) truncation threshold

    Double_t xx = x[0];
    Double_t A      = par[0];
    Double_t mu     = par[1];
    Double_t sigma  = par[2];
    Double_t x_exp  = par[3];
    Double_t lambda = par[4];
    Double_t x_cut  = par[5];

    if (xx < x_cut) return 0;

    if (xx <= x_exp) {
        return A * TMath::Gaus(xx, mu, sigma, kFALSE);
    } else {
        // Continuity at x_exp
        Double_t match = A * TMath::Gaus(x_exp, mu, sigma, kFALSE);
        return match * TMath::Exp(-lambda * (xx - x_exp));
    }
}




int main(int argc, char* argv[]) {
    
    string mPMT_ID = argv[1];
    string runNumber = argv[2];
    
    string inFileName = "/storage/wcte-recon/runs_by_mPMT_time_rec/mPMT"+mPMT_ID+"_run" + std::string(3 - runNumber.length(), '0') + runNumber + ".root"; 
    
    string filename = getBaseFilename(inFileName);
    TFile *inputFile = TFile::Open(inFileName.c_str(), "READ");

    std::vector<double> meanValues;
    std::vector<double> sigmaValues;   
    

    
    TTree *tree = (TTree*)inputFile->Get("data");

    int charge, PMT_ID;
    // piedistalli mpmt04 (manca per PMT 1 perché non calibrato)
    //int ped[] = {260,0,257,259,257,260,256,260,258,260,259,260,260,257,259,259,261,258,258}; 
    int ped[4][19] = {
        // mPMT1
        {259, 261, 248, 253, 252, 262, 263, 250, 255, 258,
        265, 259, 266, 259, 271, 265, 261, 248, 254},
        // mPMT2
        {260, 251, 255, 261, 259, 262, 249, 269, 266, 265,
        256, 249, 254, 259, 254, 251, 255, 257, 248},
        // mPMT3
        {266, 248, 276, 259, 256, 254, 257, 261, 266, 265,
        257, 266, 256, 255, 257, 259, 263, 259, 267},
        // mPMT4 
        {260, 253, 260, 265, 257, 249, 258, 261, 267, 259,
        262, 259, 260, 264, 273, 264, 259, 248, 261}
    };
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

        
        //int maxCharge = *max_element(charges.begin(), charges.end());
        int maxCharge = 1000;
        //int nBins = TMath::Min(100, maxCharge / 10 + 1); 
        int nBins = maxCharge-ped[std::stoi(mPMT_ID)-1][pmt];
        //int nBins = maxCharge;

        


        string histName = "hist_PMT_" + to_string(pmt);
        string histTitle = "Charge Spectrum PMT (" + to_string(pmt) + ")";

        /*if (pmt == 4){
            int maxCharge = *max_element(charges.begin(), charges.end());
         
            string histName_zoom = "hist_PMT_zoom" + to_string(pmt);
            //TH1F *hist_zoom = new TH1F(histName_zoom.c_str(), histTitle.c_str(), 1000-260, 260, 1000);
            TH1F *hist_zoom = new TH1F(histName_zoom.c_str(), histTitle.c_str(), 1000, 0, 1000);
            for (int c : charges){
            hist_zoom->Fill(c);
                        }
            hist_zoom->GetXaxis()->SetTitle("ADC");
            hist_zoom->GetYaxis()->SetTitle("Counts");
            hist_zoom->GetXaxis()->CenterTitle();
            hist_zoom->GetYaxis()->CenterTitle();        
            hist_zoom->Draw();
            //canvas->SaveAs((dirName+"/"+histName_zoom + "_log.png").c_str());
            //canvas->SaveAs(("./plots/"+histName_zoom + "_log.png").c_str());

            canvas->Clear();
        }*/

        TH1F *hist = new TH1F(histName.c_str(), histTitle.c_str(), nBins,ped[std::stoi(mPMT_ID)-1][pmt], maxCharge);
        //TH1F *hist = new TH1F(histName.c_str(), histTitle.c_str(), nBins, 0, maxCharge);
        
        for (int c : charges) {
            hist->Fill(c);
        }
        int firstBinOver100 = -1;  // Valore di default nel caso non venga trovato

        for (int i = 1; i <= hist->GetNbinsX(); ++i) {  // i parte da 1 (non da 0!) perché in ROOT:
                                                        // bin 0 = underflow, bin N+1 = overflow
            if (hist->GetBinContent(i) > 100) {
                firstBinOver100 = i;
                break;
            }
        }

        if (firstBinOver100 != -1) {
            double binCenter = hist->GetBinCenter(firstBinOver100);
            std::cout << "Primo bin con più di 50 conteggi: bin #" << hist->GetBinLowEdge(firstBinOver100)
                    << " (centro = " << binCenter << ")" << std::endl;
        } else {
            std::cout << "Nessun bin con più di 50 conteggi trovato." << std::endl;
        }

        hist->SetFillStyle(0);         // Nessun riempimento
        hist->SetLineStyle(1);         // Linea continua (puoi usare altri stili: 2, 3, ecc.)
        hist->SetLineColor(kBlack);    // Colore della linea (esempio: nero)
        hist->SetLineWidth(1);   
        hist->GetXaxis()->SetTitle("ADC");
        hist->GetYaxis()->SetTitle("Counts");
        hist->GetXaxis()->CenterTitle();
        hist->GetYaxis()->CenterTitle();        
        hist->Draw("HIST");

        int mu_max = 600;
        int max_gaus = 700;

        TF1 *gaussian = new TF1("gaussian", "gaus", ped[std::stoi(mPMT_ID)-1][pmt], max_gaus);
        gaussian->SetLineColor(kRed);
        gaussian->SetLineWidth(2);
        gaussian->SetParLimits(0, 1000, 80000);
        gaussian->SetParLimits(1, ped[std::stoi(mPMT_ID)-1][pmt], mu_max);
        gaussian->SetParLimits(2, 2, 50);
        cout<<"Fit gaussiano"<<endl;
        hist->Fit(gaussian, "R");
        double mean = gaussian->GetParameter(1);
        double sigma = gaussian->GetParameter(2);
        meanValues.push_back(mean);
        sigmaValues.push_back(sigma);
        hist->Fit(gaussian, "R");


        TF1 *cb = new TF1("cb", crystalBall, ped[std::stoi(mPMT_ID)-1][pmt], hist->GetXaxis()->GetXmax(), 5);
        cout<<"Fit crystal ball"<<endl;
        cb->SetLineColor(kBlue);
        cb->SetLineWidth(2);
        cb->SetParLimits(0, 0.5, 5);   // α (positivo, vicino a 1–3)
        cb->SetParLimits(1, 300, mu_max);  // μ (vicino al picco del segnale)
        cb->SetParLimits(2, 2, 50);     // σ
        cb->SetParLimits(3, 1000, 150000); // ampiezza
        cb->SetParLimits(4, 2, 5);     // n (una coda con decadimento più realistico)
        hist->Fit(cb, "R");

        TF1 *f_truncGaus = new TF1("f_truncGaus", TruncatedGaussian, ped[std::stoi(mPMT_ID)-1][pmt], max_gaus, 4);
        cout<<"Fit gaussiano troncato"<<endl;
        f_truncGaus->SetLineColor(kGreen);
        f_truncGaus->FixParameter(3,hist->GetBinLowEdge(firstBinOver100));
        f_truncGaus->SetLineWidth(2);
        f_truncGaus->SetParLimits(0, 1000, 80000);
        f_truncGaus->SetParLimits(1, ped[std::stoi(mPMT_ID)-1][pmt], mu_max);
        f_truncGaus->SetParLimits(2, 2, 50);     // σ
        //f_truncGaus->SetParLimits(3, ped[std::stoi(mPMT_ID)-1][pmt],firstBinOver50); // x_cut (truncation threshold)
        hist->Fit(f_truncGaus, "R");

        TF1 *f_cbExp = new TF1("f_cbExp", GaussExpRight, ped[std::stoi(mPMT_ID)-1][pmt], hist->GetXaxis()->GetXmax(), 5);
        cout<<"Fit gaussiano troncato + coda exp"<<endl;
        f_cbExp->SetLineColor(kMagenta);
        f_cbExp->SetLineWidth(2);
        f_cbExp->SetParLimits(0, 1000, 80000); // ampiezza
        f_cbExp->SetParLimits(1, ped[std::stoi(mPMT_ID)-1][pmt], mu_max); 
        f_cbExp->SetParLimits(2, 2, 50);     
        //f_cbExp->SetParameter(3, 1.5);
        //f_cbExp->SetParameter(4,2);  
        f_cbExp->SetParLimits(3,mu_max,max_gaus);
        f_cbExp->SetParLimits(4, 0.01, 0.1);  
        f_cbExp->FixParameter(5, hist->GetBinLowEdge(firstBinOver100));
        hist->Fit(f_cbExp, "R");

        int maxBin = hist->GetMaximumBin();                // indice del bin con valore massimo
        double x_at_max = hist->GetBinCenter(maxBin);      // posizione in x del centro del bin

        TLine *line = new TLine(x_at_max, 0, x_at_max, hist->GetMaximum() * 1.1);
        line->SetLineColor(kRed);
        line->SetLineStyle(2); // tratteggiata
        line->SetLineWidth(2);
        line->Draw("same");
        
        // Aggiungi una legenda
        TLegend *legend = new TLegend(0.7, 0.2, 0.9, 0.4); // Posizione della legenda
        
        legend->AddEntry(gaussian, ("(GAUS #mu = " + std::to_string(mean) + ")").c_str(), "l");
        legend->AddEntry(cb, ("(CB #mu = " + std::to_string(cb->GetParameter(1)) + ")").c_str(), "l");
        legend->AddEntry(f_truncGaus, ("(TRUNC GAUS #mu = " + std::to_string(f_truncGaus->GetParameter(1)) + ")").c_str(), "l");
        legend->AddEntry(f_cbExp, ("(TRUNC CB #mu = " + std::to_string(f_cbExp->GetParameter(1)) + ")").c_str(), "l");
        legend->AddEntry(line, ("(Max Hist #mu = " + std::to_string(x_at_max) + ")").c_str(), "l");
        legend->SetBorderSize(1);
        legend->SetTextSize(0.03);
        legend->Draw();

       
        gaussian->Draw("SAME");
        cb->Draw("SAME");
        f_truncGaus->Draw("SAME");
        f_cbExp->Draw("SAME");



        canvas->SaveAs((dirName+"/"+histName + ".png").c_str());
        //canvas->SaveAs((dirName+"/"+histName + "_log.png").c_str());
        hist->Write(); 
        /*
        // For pedestal determination
        int maxBin = hist->GetMaximumBin();  
        float maxValue = hist->GetBinContent(maxBin);  
        std::cout <<"PMT: "<<pmt<< " Bin massimo: " << maxBin<<  std::endl;
        */
        canvas->Clear();
        cont = cont+1;
    }

    
    outputFile->Close();
    inputFile->Close();

    std::cout << "Valori medi dei fit gaussiani:" << std::endl;
    for (size_t i = 0; i < meanValues.size(); ++i) {
        std::cout  << i + 1 << ", " << std::fixed << std::setprecision(0) << meanValues[i] << "," << std::fixed << std::setprecision(3) << sigmaValues[i] << std::endl;
        //std::cout  << i + 1 << ", " << std::fixed << std::setprecision(0) << meanValues[i] << "," << std::fixed << std::setprecision(3) << sigmaValues[i] << std::endl;

    }

    cout << "Istogrammi creati e salvati in"<< dirName << endl;
    return 0;
}
