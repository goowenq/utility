void convolve(){
  int nsample = 100;
  auto h1 = new TH1F("h1","original signal",nsample,0,10);
  auto h2 = new TH1F("h2","convolved",nsample,0,10);
  h2->SetLineColor(1);

  // response function: gaussian
  auto f1 = new TF1("f1","gaus(0)",0,10);
  f1->SetParameter(0,25.0); // amplitude
  f1->SetParameter(1,2.0); // mu
  f1->SetParameter(2,0.4); // sigma
  f1->Draw();

  // fill data points
  vector<double> signal, response;
  for (int i=0; i<nsample; i++) {
    double xvalue = i*0.1;
    if(xvalue == 1.5 or xvalue == 3.5) {
      signal.push_back(1);
      h1->SetBinContent(i+1, 1.0); 
    }
    else {
      signal.push_back(0);
    }

    response.push_back(f1->Eval(xvalue));
  }

  h1->Draw("same");

  // fourier transform (FT) for signal
  vector<double> signal_re(nsample), signal_im(nsample); // real and imagnary
  auto fft_r2c = TVirtualFFT::FFT(1, &nsample, "R2C ES K");
  fft_r2c->SetPoints(signal.data());
  fft_r2c->Transform();
  fft_r2c->GetPointsComplex(signal_re.data(), signal_im.data());

  // FT for response function
  vector<double> resp_re(nsample), resp_im(nsample); // real and imagnary
  fft_r2c->SetPoints(response.data());
  fft_r2c->Transform();
  fft_r2c->GetPointsComplex(resp_re.data(), resp_im.data());

  // signal * response in frequency domain
  vector<double> re(nsample), im(nsample);
  for (int i=0; i<nsample; i++) {
    TComplex c_signal(signal_re.at(i), signal_im.at(i));
    TComplex c_resp(resp_re.at(i), resp_im.at(i));
    TComplex c_conv_signal = c_signal * c_resp;
    re.at(i) = c_conv_signal.Re();
    im.at(i) = c_conv_signal.Im();
  }

  // inverse FT: complex to real
  auto fft_c2r = TVirtualFFT::FFT(1,&nsample, "C2R M K");
  fft_c2r->SetPointsComplex(re.data(), im.data());
  fft_c2r->Transform();
  std::vector<double> conv_signal(nsample);
  fft_c2r->GetPoints(conv_signal.data());
  
  for(int i=0; i<nsample; i++){
    h2->SetBinContent(i+1, conv_signal.at(i) / ((double)nsample));
  }
  h2->Draw("same");

  delete fft_r2c;
  delete fft_c2r;

  auto lg = new TLegend(0.6,0.6,0.9,0.8);
  lg->AddEntry(h1, "original signal", "l")->SetTextColor(4);
  lg->AddEntry(f1, "response function", "l")->SetTextColor(2);
  lg->AddEntry(h2, "convolved signal", "l")->SetTextColor(1);
  lg->Draw();

  cout << " --> writing convolved signal to a root file. " << endl;
  auto file = new TFile("conv_signal.root","RECREATE");
  h2->Write();
  file->Close();
}
