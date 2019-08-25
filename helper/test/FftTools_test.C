#include "FftTools.h"

void FftTools_test(){
  TFile* file = TFile::Open("magnify_4875_3r-v2.root");
  TH2I * hu_orig = (TH2I*)file->Get("hw_orig");
  //int nsmp = hu_orig->GetNbinsY();
  int nsmp = 5550; // ignore last 4 bogous sample
  TH1F *h1 = new TH1F("h1","h1",nsmp,0,nsmp);
  int channel=2175;
  for (int i=0;i<nsmp;i++){
    h1->SetBinContent(i+1,hu_orig->GetBinContent(channel+1,i+1));
  }
  h1->SetLineColor(1);
  h1->Draw();

  
  TH1F *h2 = new TH1F("h2","h2",nsmp,0,nsmp);
  FftShiftSampling(h1, 1, h2); 

  h2->SetLineColor(2);
  h2->Draw("same"); 
}
