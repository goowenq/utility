void deconvolve(){

  auto file = TFile::Open("conv_signal.root");
  auto h2 = (TH2F*)file->Get("h2");
  h2->Draw();

  int nsample = h2->GetNbinsX();

  // exactly same response function
  // copied from convolve.C
  auto f1 = new TF1("f1","gaus(0)",0,10);
  f1->SetParameter(0,25.0); // amplitude
  f1->SetParameter(1,2.0); // mu
  f1->SetParameter(2,0.4); // sigma
  // f1->Draw("same");

  // prepare data for FFT
  vector<double> signal, response;
  for (int i=0; i<nsample; i++) {
    signal.push_back(h2->GetBinContent(i+1));
    double xvalue = 0.1 * i;
    response.push_back(f1->Eval(xvalue));
  }

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

  // signal devided by response in frequency domain
  vector<double> re(nsample), im(nsample);
  for (int i=0; i<nsample; i++) {
    TComplex c_signal(signal_re.at(i), signal_im.at(i));
    TComplex c_resp(resp_re.at(i), resp_im.at(i));
    TComplex c_deconv_signal = c_signal / c_resp;
    re.at(i) = c_deconv_signal.Re();
    im.at(i) = c_deconv_signal.Im();
  }

  // inverse FT: complex to real
  auto fft_c2r = TVirtualFFT::FFT(1,&nsample, "C2R M K");
  fft_c2r->SetPointsComplex(re.data(), im.data());
  fft_c2r->Transform();
  std::vector<double> deconv_signal(nsample);
  fft_c2r->GetPoints(deconv_signal.data());
  
  
  auto h1 = new TH1F("h1","deconv signal",nsample,0,10);
  for(int i=0; i<nsample; i++){
    h1->SetBinContent(i+1, deconv_signal.at(i) / ((double)nsample));
  }
  h1->Draw("same");

  delete fft_r2c;
  delete fft_c2r;

  auto lg = new TLegend(0.6,0.6,0.9,0.8);
  lg->AddEntry(h2, "original signal", "l")->SetTextColor(1);
  // lg->AddEntry(f1, "response function", "l")->SetTextColor(2);
  lg->AddEntry(h1, "deconvolved signal", "l")->SetTextColor(4);
  lg->Draw();

}
