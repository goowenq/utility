int anodeId(int channelId){
  return (int)(channelId / 2560);
}

void celltree_simchan_x(std::string filename="celltree.root", int chmin=0, int chmax=2559, int nticks=6000){ // [chmin, chmax]
  auto in = TFile::Open(filename.c_str());
  auto t = dynamic_cast<TTree*>(in->Get("Event/Sim"));

  int simide_size;
  std::vector<int>* simide_channelIdY = 0;
  std::vector<unsigned short>* simide_tdc = 0;
  std::vector<float>* simide_numElectrons = 0;
  std::vector<float>* simide_x = 0;
  std::vector<float>* simide_y = 0;
  std::vector<float>* simide_z = 0;
  t->SetBranchAddress("simide_size", &simide_size);
  t->SetBranchAddress("simide_channelIdY", &simide_channelIdY);
  t->SetBranchAddress("simide_tdc", &simide_tdc);
  t->SetBranchAddress("simide_numElectrons", &simide_numElectrons);
  t->SetBranchAddress("simide_x", &simide_x);
  t->SetBranchAddress("simide_y", &simide_y);
  t->SetBranchAddress("simide_z", &simide_z);


  t->GetEntry(0);
  std::cout << simide_size << std::endl; 

  auto h1 = new TH2F("h1","tdc vs channel", chmax-chmin+1, chmin-0.5, chmax+0.5, 6000,0,6000);
  auto h2 = new TH2F("h2","x vs channel", chmax-chmin+1, chmin-0.5, chmax+0.5, 7200, -360,360);
  auto h3 = new TH2F("h3","x vs z", 7200, 0, 720, 7200, -360,360);
  auto h4 = new TH2F("h4","y vs z", 7200, 0, 720, 7200,  0,720);

  // simide
  for (int idx=0; idx<simide_size; idx++){
  	unsigned short tdc = simide_tdc->at(idx);
  	float x = simide_x->at(idx);
  	float y = simide_y->at(idx);
  	float z = simide_z->at(idx);
        // if ( (tdc + (360-fabs(x))/0.16/0.5) > 6000)
        if ( tdc>6000 )
        {
          // cout << tdc << endl;
          continue;
        }
  	float nele = simide_numElectrons->at(idx);
        h3->Fill(z,x,nele);
        h4->Fill(z,y,nele);
        // if (fabs(x)>360) cout << x << endl;
  	int channelId = simide_channelIdY->at(idx);
        if (channelId>=chmin and channelId<=chmax) {
  		// int tbin = (int)(tdc);
  		int locbin = h2->GetYaxis()->FindBin(x);
  		h2->SetBinContent(channelId-chmin+1, locbin, 1 + h2->GetBinContent(channelId-chmin+1, locbin));
  	}
  }

  h2->Draw("colz");

  // auto ofile = new TFile("simchan_x.root","recreate");
  // h2->Write();
  // ofile->Close();


}
