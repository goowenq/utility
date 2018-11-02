// Created on 11/2/2018
// by Wenqiang Gu (wgu@bnl.gov)
// to compile it standalone: g++ -o xxx.exe FTmitigation.cc `root-config --cflags --libs`
// usage: ./xxx.exe magnify_xxx.root

#include <vector>
#include <iostream>
#include <tuple>
#include <map>
#include <algorithm>
#include <cmath>
#include "TFile.h"
#include "TTree.h"
#include "TVirtualFFT.h"
#include "TComplex.h"
#include "TMath.h"
#include "TH1F.h"
#include "TH2I.h"
#include "TGraph.h"
#include "TString.h"


class FFTHelper{
	public:
	std::vector<double> _x;
	std::vector<double> _y;
	std::vector<double> _re;
	std::vector<double> _im;
	std::vector<double> _newx;
	std::vector<double> _newy;

	int m_nsmp;
	double m_length;
	TVirtualFFT* m_fft_forward;
	TVirtualFFT* m_fft_backward;

	~FFTHelper() {delete m_fft_forward; delete m_fft_backward;}
	void initialize(std::vector<double>&, std::vector<double>&, int, double);
	void translate(double t0);
};

void FFTHelper::initialize(std::vector<double>& x, std::vector<double>& y, int nsmp, double length){
	m_nsmp = nsmp;
	m_length = length;
	_x = x;
	_y = y;
	_re.resize(m_nsmp);
	_im.resize(m_nsmp);
	_newx.resize(m_nsmp);
	_newy.resize(m_nsmp);
	if(m_fft_forward==0) m_fft_forward = TVirtualFFT::FFT(1, &m_nsmp, "R2C ES K");
	// FT transform
	m_fft_forward->SetPoints(_y.data());
	m_fft_forward->Transform();
	m_fft_forward->GetPointsComplex(_re.data(), _im.data());
}

void FFTHelper::translate(double dt){
	if(std::abs(dt) < 1e-3){
		_newx = _x;
		_newy = _y;
	}
	else{
		double f0 = 1./m_length;
		for(int i=0; i<m_nsmp; i++){
			double fi= i * f0;
			double w= 2*TMath::Pi()*fi;
			TComplex z(_re[i], _im[i]);
			TComplex z1(0,w*dt);
			TComplex z2 = z * TComplex::Exp(z1);
			_re[i] = z2.Re();
			_im[i] = z2.Im();
		}

		if(m_fft_backward==0) m_fft_backward = TVirtualFFT::FFT(1, &m_nsmp, "C2R M K");
		m_fft_backward->SetPointsComplex(_re.data(), _im.data());
		m_fft_backward->Transform();
		m_fft_backward->GetPoints(_newy.data());
		for(int i=0; i<m_nsmp; i++){
			_newx[i] = _x[i] + dt;
			_newy[i] /= double(m_nsmp);
		}
	}
}




int mitigate_chn(TH2I* hu_orig, int ch, TH2F* hu_linear, TH2F* hu_mitigated){

  const Int_t nticks = hu_orig->GetNbinsY();
  // orig
  TH1F* horig = new TH1F("horig","orig",nticks,0,nticks);
  for (Int_t i=0;i!=nticks;i++){
    horig->SetBinContent(i+1,hu_orig->GetBinContent(ch+1,i+1));
  }
  if(horig->Integral()==0){
  	horig->Delete();
  	return -1;
  }
  // mod 64
  TH1F* hmod = new TH1F("hmod","mod64",nticks,0,nticks);
  for (Int_t i=0;i!=nticks;i++){
    int content = horig->GetBinContent(i+1);
    hmod->SetBinContent(i+1,content%64);
  }  


  // linear interpolation
  std::vector<std::pair<int,int>> pairs;
  for (Int_t i=0;i!=nticks;i++){
    int mod_res = hmod->GetBinContent(i+1);
    if (mod_res == 0 || mod_res == 1 || mod_res==63){
      if (pairs.size()==0){
  	pairs.push_back(std::make_pair(i,i));
      }else if ( (pairs.back().second + 1) == i){
  	pairs.back().second = i;
      }else{
  	pairs.push_back(std::make_pair(i,i));
      }
    }
  }
  
  TH1F* hlinear = (TH1F*)horig->Clone("hlinear"); // linear interpolation
  // figure out the boundary  and do the correction ... 
  for (size_t j = 0; j!=pairs.size();j++){
    int start = pairs.at(j).first -1 ;
    int end = pairs.at(j).second + 1 ;
    if (start >=0 && end < nticks){
      double start_content = hlinear->GetBinContent(start+1);
      double end_content = hlinear->GetBinContent(end+1);
      for (int i = start+1; i!=end;i++){
  	    double content = start_content + (end_content - start_content) /(end-start) *(i-start);
  	    hlinear->SetBinContent(i+1,content);
      }
    }
    //std::cout << start << " " << end << std::endl;
  }


  	// interpolation
	std::vector<double> xeven, yeven;
	std::vector<double> xodd, yodd;
	for(int i=0; i!=nticks; i++){
		if(i%2==0){
			xeven.push_back(hlinear->GetBinCenter(i+1));	
			yeven.push_back(hlinear->GetBinContent(i+1));	
		}
		else{
			xodd.push_back(hlinear->GetBinCenter(i+1));	
			yodd.push_back(hlinear->GetBinContent(i+1));
		}

	}


	FFTHelper* fft1 = new FFTHelper();
	fft1->initialize(xeven,yeven, xeven.size(), 6000.0);
	fft1->translate(-1);

	FFTHelper* fft2 = new FFTHelper();
	fft2->initialize(xodd,yodd, xodd.size(), 6000.0);
	fft2->translate(+1);

	TH1F* hFT = new TH1F("hFT","FT interpolation",nticks, 0, nticks);
	for(int i=0; i<fft1->_newx.size(); i++){
		double xx = fft1->_newx[i];
		double yy = fft1->_newy[i];
		int xbin = hFT->FindBin(xx);
		hFT->SetBinContent(xbin, yy);
	}
	for(int i=0; i<fft2->_newx.size(); i++){
		double xx = fft2->_newx[i];
		double yy = fft2->_newy[i];
		int xbin = hFT->FindBin(xx);
		hFT->SetBinContent(xbin, yy);
	}
	delete fft1;
	delete fft2;

	// replace the sticky-code
    TH1F* hmitigated = (TH1F*)horig->Clone("hmitigated");
    for (Int_t i=0;i!=nticks;i++){
      int mod_res = hmod->GetBinContent(i+1);
      if (mod_res == 0 || mod_res == 1 || mod_res==63){
        if(i>0 && i<nticks){
        	hmitigated->SetBinContent(i+1,hFT->GetBinContent(i+1));
        	if(fabs(hlinear->GetBinContent(i+1) - horig->GetBinContent(i+1))>15)
        	std::cout
        	<< " chn: " << ch
        	<< " tick: " << i
        	<< " orig: " << horig->GetBinContent(i+1)
        	<< " linear: " << hlinear->GetBinContent(i+1)
        	<< " FT interp: " << hmitigated->GetBinContent(i+1)
        	<< std::endl;
        }

      }
    }


    for(int i=0; i!=nticks; i++){
	    hu_linear->SetBinContent(ch+1, i+1, hlinear->GetBinContent(i+1));
	    hu_mitigated->SetBinContent(ch+1, i+1, hmitigated->GetBinContent(i+1));
    }

    horig->Delete();
    hlinear->Delete();
    hmod->Delete();
    hFT->Delete();
    hmitigated->Delete();
    return 1;
}

int main(int argc, char** argv){

	const char* filename = argv[1];
  	// TFile *file = new TFile("magnify_4368_82.root");
  	// TFile *file = TFile::Open("magnify_3506_42.root");
  	TFile *file = TFile::Open(filename);
  	TH2I * hu_orig = (TH2I*)file->Get("hu_orig");
  	TH2I * hv_orig = (TH2I*)file->Get("hv_orig");
  	TH2I * hw_orig = (TH2I*)file->Get("hw_orig");


  	TString oname;
  	oname.Form("mitigated_%s", filename);
	// TFile* ofile = new TFile("mitigated_magnify_3506_42.root","recreate");
	TFile* ofile = new TFile(oname.Data(),"recreate");

  	int nchans = hu_orig->GetNbinsX();
  	int nticks = hu_orig->GetNbinsY();
  	TH2F* hu_linear = new TH2F("hu_linear","hu_linear",nchans,-0.5,nchans-0.5,nticks,0,nticks);
  	TH2F* hv_linear = new TH2F("hv_linear","hv_linear",nchans,-0.5,nchans-0.5,nticks,0,nticks);
  	TH2F* hw_linear = new TH2F("hw_linear","hw_linear",nchans,-0.5,nchans-0.5,nticks,0,nticks);
  	TH2F* hu_mitigated = new TH2F("hu_mitigated","hu_mitigated",nchans,-0.5,nchans-0.5,nticks,0,nticks);
  	TH2F* hv_mitigated = new TH2F("hv_mitigated","hv_mitigated",nchans,-0.5,nchans-0.5,nticks,0,nticks);
  	TH2F* hw_mitigated = new TH2F("hw_mitigated","hw_mitigated",nchans,-0.5,nchans-0.5,nticks,0,nticks);

  	//nchans = 15360;
  	// nchans = 10;
  	for(int i=0; i<nchans; i++){
  		if(i%100==0) std::cout << i << std::endl;
  		mitigate_chn(hu_orig, i, hu_linear, hu_mitigated);
  		mitigate_chn(hv_orig, i, hv_linear, hv_mitigated);
  		mitigate_chn(hw_orig, i, hw_linear, hw_mitigated);
  	}

	ofile->Write();
	ofile->Close();

	return 0;
}

	// TFile* file = new TFile("test.root","recreate");
	// TH1F* h1 = new TH1F("h1","h1",200,-10,10);
	// h1->SetLineColor(4);
	// h1->FillRandom("gaus",50000);

	// std::vector<double> xeven,yeven;
	// std::vector<double> xodd,yodd;
	// for(int i=1; i<=200; i++){
	// 	if(i%2==0){
	// 		xeven.push_back(h1->GetBinCenter(i));	
	// 		yeven.push_back(h1->GetBinContent(i));	
	// 	}
	// 	else{
	// 		xodd.push_back(h1->GetBinCenter(i));	
	// 		yodd.push_back(h1->GetBinContent(i));
	// 	}

	// }

	// FFTHelper* fft1 = new FFTHelper();
	// fft1->initialize(xeven,yeven, xeven.size(), 20);
	// fft1->translate(-0.1);

	// FFTHelper* fft2 = new FFTHelper();
	// fft2->initialize(xodd,yodd, xeven.size(), 20);
	// fft2->translate(+0.1);


	// TGraph* g1 = new TGraph(fft1->_newx.size(), fft1->_newx.data(), fft1->_newy.data());
	// g1->SetMarkerStyle(3);
	// g1->SetMarkerColor(1);
	// g1->SetName("FTeven");

	// TGraph* g2 = new TGraph(fft2->_newx.size(), fft2->_newx.data(), fft2->_newy.data());
	// g2->SetMarkerStyle(2);
	// g2->SetMarkerColor(2);
	// g2->SetName("FTodd");
	// delete fft1;
	// delete fft2;

	// h1->Write();
	// g1->Write();
	// g2->Write();
	// file->Close();
