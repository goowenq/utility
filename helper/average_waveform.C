#include "PeakFinder.h"

void average_waveform(int loch, int hich, int lotk, int hitk, int color=2){ // range of tick and channel
  auto pfinder = new PeakFinder();

  auto file = TFile::Open("/data2/protodune/beam_7GeV/magnify/magnify_run5815_record0_mc_driftv_tuned_DTDL_red20per_elife1000ms-v2.root");
  TH1F* hu = (TH1F*)file->Get("hu_raw");
  TH1F* hv = (TH1F*)file->Get("hv_raw");
  TH1F* hw = (TH1F*)file->Get("hw_raw");
  hu->Add(hv);
  hu->Add(hw);

  auto file2 = TFile::Open("/data2/protodune/beam_7GeV/magnify/magnify_run5815_record0-v2.root");
  // auto file2 = TFile::Open("/data2/protodune/beam_7GeV/magnify/magnify_run5815_record0_mc_driftv_tuned-v2.root");
  TH1F* hu2 = (TH1F*)file2->Get("hu_raw");
  TH1F* hv2 = (TH1F*)file2->Get("hv_raw");
  TH1F* hw2 = (TH1F*)file2->Get("hw_raw");
  hu2->Add(hv2);
  hu2->Add(hw2);

  TH1F* hmc_u = new TH1F("hmc_u","",100,0,100);
  TH1F* hdat_u = new TH1F("hdat_u","",100,0,100);
  TH1F* h1 = new TH1F("h1","",6000,0,6000);
  TH1F* h2 = new TH1F("h2","",6000,0,6000);
  hmc_u->SetLineColor(1);
  hdat_u->SetLineColor(color); // U4V8W2
  h1->SetLineColor(1);
  h2->SetLineColor(4);

  int n_avg_ch = 0;
  for(int ich=loch; ich<hich; ich++){
    std::cout << "channel: " << ich << endl;
    for(int i=0; i<6000; i++){ // fill channel waveform
      h1->SetBinContent(i+1, hu->GetBinContent(ich+1, i+1));
      h2->SetBinContent(i+1, hu2->GetBinContent(ich+1, i+1));
    }
    std::vector<int> xps = pfinder->Analyze(h1, 0.5, 20, lotk, hitk);
    //h1->Draw();
     
    // find the largest peak in MC
    int max1 = -1;
    int max1_bin = -1; 
    for(int x: xps){
      if (h1->GetBinContent(x) > max1){
        max1 = h1->GetBinContent(x);
        max1_bin = x;
      }
    }

    if(max1_bin>0){ // find peak in data that is close to the peak in MC
      std::vector<int> xps2 = pfinder->Analyze(h2, 0.25, 20, lotk, hitk);

      //find largest peak in data
      int max2 = -1;
      int max2_bin = -1;
      for(int x: xps2){
        if (h2->GetBinContent(x)>max2){
          max2 = h2->GetBinContent(x);
          max2_bin = x;
        }
      }

      if(max2_bin>0 and fabs(max2_bin - max1_bin)<150){
        n_avg_ch ++;
        for(int i=0; i<100; i++){
          hmc_u->AddBinContent(i+1, h1->GetBinContent(max1_bin-40 + i));
          hdat_u->AddBinContent(i+1, h2->GetBinContent(max2_bin-40 + i));
        }
      }
      

    }

  } // end of ch loop

  
  //h2->Draw();
  //h1->Draw("same");

  hdat_u->Scale(1./n_avg_ch);
  hmc_u->Scale(1./n_avg_ch);


  double max_dat = hdat_u->GetMaximum();
  double max_mc = hmc_u->GetMaximum();
  std::cout << "scaling factor for MC: " << max_dat/max_mc << std::endl;
  hmc_u->Scale(max_dat/max_mc);

  hdat_u->Draw();
  hmc_u->Draw("same");


  pfinder->Close();
}
