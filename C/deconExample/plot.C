void plot(){
  std::ifstream in("channel_93.dat");
  std::string dummyLine;
  std::getline(in, dummyLine); // skip a line

  std::vector<double> v1, v2;
  int ntick;
  double y1,y2;
  while(in >> ntick){
    in >> y1; in >> y2;
    v1.push_back(y1);
    v2.push_back(y2);
  }

  TH1F* h1 = new TH1F("h1","",ntick,0,ntick);
  TH1F* h2 = new TH1F("h1","",ntick,0,ntick);
  h2->SetLineColor(2);
  for(int i=0; i<ntick; i++){
    h1->SetBinContent(i+1,v1.at(i));
    // decon offset 120
    int ibin = i+120;
    if (ibin<ntick) 
      h2->SetBinContent(ibin+1,v2.at(i) * 200);
  }

  h1->Draw();
  h2->Draw("same");

}
