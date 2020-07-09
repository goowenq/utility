{
  auto file = TFile::Open("celltree_th2f.root");
  auto hcalib = (TH2F*)file->Get("h1");
  auto hsimch = (TH2F*)file->Get("h2");

  auto hc1 = new TH1F("hc1","",6000,0,6000);
  auto hc2 = new TH1F("hc2","",6000,0,6000);


  int lastch=0;

  for(int ich=0; ich<15360; ich++) {
    for(int itk=0; itk<6000; itk++) {
      hc1->SetBinContent(itk+1, hcalib->GetBinContent(ich+1,itk+1));
      hc2->SetBinContent(itk+1, hsimch->GetBinContent(ich+1,itk+1));
    }

    if(hc1->Integral()>1 and hc2->Integral()<100) {
      cout << "channel: " << ich << " no simchannel" << ", ch increment to last: " << ich - lastch<< endl;
      lastch = ich;
    } 
    if(ich==15359){
      cout << hc1->Integral() << " " << hc2->Integral() << endl;
    }
  }
}
