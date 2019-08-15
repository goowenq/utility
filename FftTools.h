void FftShiftSampling(TH1F* hin, int tbin /*shift bins*/, TH1F* hout){
  std::vector<double> vin;
  int inSmps = hin->GetNbinsX();
  for(int i=0; i<inSmps; i++){
    vin.push_back(hin->GetBinContent(i+1));
  }

  std::vector<double> vRe(inSmps), vIm(inSmps);
  TVirtualFFT* fft_r2c = TVirtualFFT::FFT(1, &inSmps, "R2C ES K");
  fft_r2c->SetPoints(vin.data());  
  fft_r2c->Transform();
  fft_r2c->GetPointsComplex(vRe.data(), vIm.data());

  // FFT shift
  double f0 = 1./(2.*inSmps);
  for(int i=0; i<inSmps; i++){
    double fi = i * f0;
    double w = 2*TMath::Pi()*fi;
    TComplex z(vRe[i], vIm[i]);
    TComplex z1(0,w*tbin); // phase shift
    TComplex z2 = z * TComplex::Exp(z1);
    vRe[i] = z2.Re();
    vIm[i] = z2.Im();
  }

  std::vector<double> vReOut(inSmps);
  auto fft_c2r = TVirtualFFT::FFT(1, &inSmps, "C2R M K");
  fft_c2r->SetPointsComplex(vRe.data(), vIm.data());
  fft_c2r->Transform();
  fft_c2r->GetPoints(vReOut.data());

  for(int i=0; i<inSmps; i++){
    hout->SetBinContent(i+1+tbin/2, vReOut[i] / (1.0*inSmps)); // corresponding phase shift
  }

  delete fft_r2c;
  delete fft_c2r;
}

void FftUpSampling(TH1F* hin, int inSmps1, TH1F* hout, int outSmps1){

  int inSmps = inSmps1;
  int outSmps = outSmps1;

  if(outSmps<=inSmps){
  	for(int i=0; i<outSmps; i++){
  		if(i<inSmps) hout->SetBinContent(i+1, hin->GetBinContent(i+1));
  		else hout->SetBinContent(i+1, hin->GetBinContent(inSmps+1));
  	}
  	std::cout << "Warning: output sample size should be greater than the input." << std::endl;
  	return;
  }

  std::vector<double> vin(inSmps);
  for(int i=0; i<inSmps; i++){
    vin[i] = hin->GetBinContent(i+1);
  }
  std::vector<double> vRe(inSmps), vIm(inSmps);
  TVirtualFFT* fft_r2c = TVirtualFFT::FFT(1, &inSmps, "R2C ES K");
  fft_r2c->SetPoints(vin.data());  
  fft_r2c->Transform();
  fft_r2c->GetPointsComplex(vRe.data(), vIm.data());

  // extend sample size
  vRe.resize(outSmps);
  vIm.resize(outSmps);

  // inverse FFT
  TVirtualFFT* fft_c2r = TVirtualFFT::FFT(1,&outSmps, "C2R M K");
  fft_c2r->SetPointsComplex(vRe.data(), vIm.data()); 
  fft_c2r->Transform();
  std::vector<double> vReOut(outSmps);
  fft_c2r->GetPoints(vReOut.data());

  for(int i=0; i<outSmps; i++){
    hout->SetBinContent(i+1, vReOut[i] / (1.0*inSmps));
  }

  delete fft_r2c;
  delete fft_c2r;
}
