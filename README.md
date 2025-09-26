# wcte_analysis
Repository to analyze FD-mPMTs at WCTE

git repository: https://github.com/AuroraLangella/wcte_analysis.git / SSH--> git@github.com:AuroraLangella/wcte_analysis.git

path to .root runs file: /storage/wcte-recon/runs 

path to .root runs file with time reconstructed: /storage/wcte-recon/runs_time_recon
path to charge spectrum plots:  /home/alangella/wcte_analysis/plots



extract_file.cpp : apre tutti i file di un run e li mette su un file root.
 
Esempio per girarlo da wcte_analysis:
$ ./extract_file 90  (dove 90 è il numero del run)

Output directory: /storage/wcte-recon/runs/

organize_data_by_mPMT.cpp : apre il file .root del run  e lo separa in 4 run diversi a seconda dei mPMT 

Esempio per girarlo da wcte_analysis:
$ ./organize_data_by_mPMT 90 (dove 90 è il numero del run)

Output directory: /storage/wcte-recon/runs_by_mPMT/

time_reconstruction.cpp: dato un run di un singolo mPMT calcola l'event time (correggendo l'overflow) e il tot, non portando con sé le info dei singoli tempi (unix, tdc, etc) 

Esempio per girarlo da wcte_analysis:
$ ./time_reconstruction 1 90 (dove 1 è l'ID del mPMT e 90 il numero del run)

Output directory: /storage/wcte-recon/runs_by_mPMT_time_rec/

charge_spectrum.cpp: crea una cartella con tutti gli spettri di carica di un singolo run

Esempio per girarlo da wcte_analysis:
$ ./charge_spectrum 1 90 (dove 1 è l'ID del mPMT e 90 il numero del run)

Output directory: /home/alangella/wcte_analysis/plots/mPMT1_run90/ (esempio ovviamente)
