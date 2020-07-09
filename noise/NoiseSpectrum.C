void NoiseSpectrum(){
  
    //TFile *file = new TFile("/data1/csarasty/7GeV/magnify_5205_101_dl1.root");
    TFile *file = new TFile("protodune-run5817.root");
    TH2F *hu = (TH2F*)file->Get("hw_raw0"); 
    const int nticks = hu->GetNbinsY();
    const int nchs   = hu->GetNbinsX();
    //TH2F *h1 = new TH2F("h1","h1",nticks,0,nticks,nchs,0,nchs);
    TH2F *h1 = new TH2F("h1","h1",nticks,0,nticks,nchs,0,nchs);
    for(int i=0; i!=nchs; i++){
      for(int j=0; j!=nticks; j++){
	  double content = hu->GetBinContent(i+1,j+1);
         if(TMath::Abs(content)<15){h1->SetBinContent(j+1,i+1,content);}
         else{h1->SetBinContent(j+1,i+1,0);}
      
      }
    }
    const int nchs1 = h1->GetNbinsY(); 
    TH2F *fft = new TH2F("fft","fft",nticks,0,nticks,nchs1,0,nchs1);
    for(int i=480; i!=960; i++){ // i:ch, j: tick
      TH1F *htemp = new TH1F(Form("htemp_%d",i),Form("htemp_%d",i),nticks,0,nticks);
      for(int j=0; j!=nticks; j++){
      htemp->SetBinContent(j+1,h1->GetBinContent(j+1,i+1)); 
      }
      TH1 *hm= htemp->FFT(0,Form("MAG_%d",i));
      hm->SetBinContent(1,0);
      for(int l=0; l!=nticks; l++){
        fft->SetBinContent(l+1,i+1,hm->GetBinContent(l+1));
      }
    } 
    cout << "nchs1 " << nchs1 << endl;
    cout << "nticks " << nticks << endl;
    TH1F *avgfft = new TH1F("avgFFt","avgFFT",nticks,0,nticks);
    for(int a=0; a!=nticks; a++){
      for(int b=0; b!=nchs1; b++){
	//cout << "tick " << a << "ch " << b << endl;
        double content =+ fft->GetBinContent(a+1,b+1)/nchs1;
        //cout << content << endl;   
        avgfft->SetBinContent(a+1,content);
      }
    }
   // fft->Draw("COLZ");
    avgfft->Draw();

    avgfft->Rebin();
}
