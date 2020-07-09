int anodeId(int channelId){
  return (int)(channelId / 2560);
}

void celltree_th2f(std::string filename="celltree.root", int chmin=0, int chmax=2559, int nticks=6000){ // [chmin, chmax]
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

  auto h1 = new TH2F("h1","calib", chmax-chmin+1, chmin-0.5, chmax+0.5, nticks, 0,nticks);
  auto h2 = new TH2F("h2","simChan", chmax-chmin+1, chmin-0.5, chmax+0.5, nticks, 0,nticks);

  // calib_wf
  for (int ich=0; ich<calib_nChannel; ich++){
        int channelId = calib_channelId->at(ich);
        // std::cout << "channelId: " << channelId << " anode: " << anodeId(channelId) << std::endl;
        if (channelId>=chmin and channelId<=chmax) {
  		auto hcalib  = (TH1F*)calib_wf->At(ich);
                if (nticks!= hcalib->GetNbinsX()) std::cout << "Warning: nticks != " << nticks << std::endl;
                if (channelId==0) {
                    cout << "channel: " << channelId << " , recob::Wire size: " << hcalib->GetNbinsX() << endl;
                    cout << "bin 0: " << hcalib->GetBinContent(0) << endl;
                }
  		for (int it=0; it<nticks; it++){
  			h1->SetBinContent(channelId-chmin+1, it+1, hcalib->GetBinContent(it+1)); 
                        // if (channelId==0 and it==0) cout << "bin: " << it+1 << " content: " << hcalib->GetBinContent(it+1) << endl;
  		}
        }
  }

  auto h3 = new TH1F("h3","",10000,0,10000);

  // simide
  for (int idx=0; idx<simide_size; idx++){
  	int channelId = simide_channelIdY->at(idx);
  	unsigned int tdc = simide_tdc->at(idx);
        // tdc -= 7600;
  	float nele = simide_numElectrons->at(idx);
        h3->Fill(tdc);
        if (channelId>=chmin and channelId<=chmax) {
                cout << "channel: " << channelId << " tdc: " << tdc << " nele: " << nele << endl;
  		int tbin = (int)(tdc);
  		h2->SetBinContent(channelId-chmin+1, tbin, nele + h2->GetBinContent(channelId-chmin+1, tbin));
  	}
  }

  auto ofile = new TFile("celltree_th2f.root","recreate");
  h1->Write();
  h2->Write();
  h3->Write();
  ofile->Close();

}
