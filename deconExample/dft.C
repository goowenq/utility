#include "response.h"

double filter_time(double freq){ 
    double a = 0.143555;
    double b = 4.95096;
    return (freq>0)*exp(-0.5*pow(freq/a,b));
}

double filter_low_loose(double freq){
    return  1-exp(-pow(freq/0.005,2));
}

void dft(){
  // get median waveform
  std::ifstream in("channel_93.dat");
  std::string dummyLine;
  std::getline(in, dummyLine); // skip a line

  std::vector<double> vMedian, v2;
  int inSmps;
  double y1,y2;
  while(in >> inSmps){
    in >> y1; in >> y2;
    vMedian.push_back(y1);
    v2.push_back(y2);
  }

  if(inSmps!=6000) {
    std::cout << "Error! Not 6000 ticks." << std::endl;
    return;
  }

  // fft
  TH1F* hMedian = new TH1F("hMedian","",inSmps,0,inSmps);
  TH1F* hu_resp = new TH1F("hu_resp","",inSmps,0,inSmps);
  u_resp.resize(inSmps); // U response extends to 6000 ticks
  for(int i=0; i<inSmps; i++){
    hMedian->SetBinContent(i+1, vMedian.at(i));
    hu_resp->SetBinContent(i+1, u_resp.at(i));
  }
  TH1* hm = hMedian->FFT(0,"MAG"); // amplitude
  TH1* hp = hMedian->FFT(0,"PH"); // phase
  TH1* hm1 = hu_resp->FFT(0,"MAG1");
  TH1* hp1 = hu_resp->FFT(0,"PH1");

  std::vector<double> vRe(inSmps), vIm(inSmps);
  for(int i=0; i<inSmps; i++){
    double rho = hm->GetBinContent(i+1);
    double phi = hp->GetBinContent(i+1);
    double rho1 = hm1->GetBinContent(i+1);
    double phi1 = hp1->GetBinContent(i+1);

    double freq;
    // assuming 2 MHz digitization
    if (i <inSmps/2.){
        freq = i/(1.*inSmps)*2.;
    }else{
        freq = (inSmps- i)/(1.*inSmps)*2.;
    }
    double factor = filter_time(freq)*filter_low_loose(freq);
    rho *= factor; // apply filter

    rho /= rho1; // deconvolution
    phi -= phi1;
    vRe.at(i) = rho * cos(phi);  
    vIm.at(i) = rho * sin(phi);  
  }

  // inverse FFT of vRe & vIm
  TVirtualFFT* fft_c2r = TVirtualFFT::FFT(1,&inSmps, "C2R M K");
  fft_c2r->SetPointsComplex(vRe.data(), vIm.data()); 
  fft_c2r->Transform();
  std::vector<double> vReOut(inSmps);
  fft_c2r->GetPoints(vReOut.data());

  TH1F* hout = new TH1F("hout","",inSmps,0,inSmps);
  for(int i=0; i<inSmps; i++){
    double content = vReOut.at(i) / (1.0*inSmps);
    hout->SetBinContent(i+1, content * 200);
  }

  hout->Draw();
  
}
