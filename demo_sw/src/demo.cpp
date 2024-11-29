#include "TCanvas.h"

#include "Tool.h"
#include "Store.h"
#include "WCTERawData.h"

int main(int argc, char **argv) {

   WCTERawData p;
   BinaryStream input;

   input.Bopen("/storage/wcte-data/run045/dataR045S00P00.dat", READ, UNCOMPRESSED);
   input >> p;

   p.readout_windows[16].hkmpmt_hits[0].Dump();

   return 0;
}

