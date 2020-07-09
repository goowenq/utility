int anodeId(int channelId){
  return (int)(channelId / 2560);
}

void celltree_cmp(int fChannel=270, std::string filename="celltree.root"){
  auto in = TFile::Open(filename.c_str());
  auto t = dynamic_cast<TTree*>(in->Get("Event/Sim"));

  int calib_nChannel;
  std::vector<int>* calib_channelId = 0;
  TClonesArray* calib_wf = 0;
  t->SetBranchAddress("calib_nChannel", &calib_nChannel);
  t->SetBranchAddress("calib_channelId", &calib_channelId);
  t->SetBranchAddress("calib_wf", &calib_wf);
  // t->SetBranchAddress("raw_nChannel", &calib_nChannel);
  // t->SetBranchAddress("raw_channelId", &calib_channelId);
  // t->SetBranchAddress("raw_wf", &calib_wf);

  int simide_size;
  std::vector<int>* simide_channelIdY = 0;
  std::vector<unsigned short>* simide_tdc = 0;
  std::vector<float>* simide_numElectrons = 0;
  t->SetBranchAddress("simide_size", &simide_size);
  t->SetBranchAddress("simide_channelIdY", &simide_channelIdY);
  t->SetBranchAddress("simide_tdc", &simide_tdc);
  t->SetBranchAddress("simide_numElectrons", &simide_numElectrons);


  t->GetEntry(0);
  std::cout << calib_nChannel << std::endl;
  std::cout << simide_size << std::endl; 
  std::cout << simide_channelIdY->size() << std::endl;


  auto h1 = new TH1F("h1", "", 6000,0,6000);
  h1->SetLineColor(4);
  auto h2 = new TH1F("h2", "", 6000,0,6000);
  h2->SetLineColor(2);

  // calib_wf
  for (int ich=0; ich<calib_nChannel; ich++){
        int channelId = calib_channelId->at(ich);
        // std::cout << "channelId: " << channelId << " anode: " << anodeId(channelId) << std::endl;
  	if (channelId==fChannel){
  		auto hcalib  = (TH1F*)calib_wf->At(ich);
  		for (int it=0; it<6000; it++){
  			h1->SetBinContent(it+1, hcalib->GetBinContent(it+1)); 
  		}
  	}
  }

  // simide

  for (int idx=0; idx<simide_size; idx++){
  	int channel = simide_channelIdY->at(idx);
  	unsigned int tdc = simide_tdc->at(idx);
  	float nele = simide_numElectrons->at(idx);
  	if(channel==fChannel) {
  		// std::cout << "tdc: " << tdc - (4050-250)/0.5 << std::endl;
  		// int tbin = (int)(tdc - (4050-250)/0.5);
  		int tbin = (int)(tdc);
  		h2->AddBinContent(tbin, nele * 0.005);
  	}
  }


  //h1->GetXaxis()->SetRangeUser(0,10);
  h2->Draw();
  h1->Draw("same");

  auto lg = new TLegend(.6,0.6,0.8,0.8);
  lg->AddEntry(h1,"recob::Wire", "l")->SetTextColor(4);
  lg->AddEntry(h2,"SimChannel", "l")->SetTextColor(2);
  lg->Draw();
}
