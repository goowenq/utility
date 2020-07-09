/* ROOT script for searching peaks
 * 
 */

// #include "PdfHelper.h"
#include "TFile.h"
#include "TH1F.h"
#include "TSpectrum.h"
#include <iostream>
#include <string>

class PeakFinder{
	std::string m_name;
	TFile* m_file;
	TSpectrum* m_spectrum;
	TH1F* m_currHist;
	int m_currId;

public:
	PeakFinder(std::string name="");
	std::vector<int> Analyze(TH1F*, float threshold, float absthr, int bin0=0, int bin1=0);
	void Close();

};

PeakFinder::PeakFinder(std::string name){
        m_name = name;
	m_currId = -1;
        if (!name.empty()){
	  auto fname = name + ".root";
	  m_file = new TFile(fname.c_str(), "recreate");
        }
	m_spectrum = new TSpectrum();
}

std::vector<int> PeakFinder::Analyze(TH1F* h, float threshold, float absthr, int bin0, int bin1){ // thredhold: 0~1
  // std::vector<int> vpulse;
  // std::vector<int> vbuffer;
  // const int nticks = h->GetNbinsX();
  // for(int itk=0; itk<nticks;itk++){
  //   double content = h->GetBinContent(itk+1);
  //   if(content> threshold){
  //     vbuffer.push_back(itk);
  //   }
  //   else{
  //     if(vbuffer.size()>0){ // dump the peak position
  //       vpulse.push_back(vbuffer[0]);
  //       vbuffer.clear();
  //     }
  //   }
  // }

  int nfound = m_spectrum->Search(h, 2, "", threshold);
  std::vector<int> ret;

  // for(auto p: vpulse){
  double* xpeaks = m_spectrum->GetPositionX();
  for(int p=0; p<nfound; p++){
  	double xpos = xpeaks[p];
  	int pbin = h->FindBin(xpos);
        if(bin0>0 and bin1>0){
           if (pbin<bin0 or pbin>bin1) continue; 
        } 
        std::cout << "one peak at " << pbin << std::endl;
        ret.push_back(pbin);

  	m_currId ++;
  	auto name = TString::Format("No%d", m_currId);
  	m_currHist = new TH1F(name.Data(), name.Data(), 100,0,100);
  	for(int i=0; i<100; i++){
                double content = h->GetBinContent(pbin-40 + i);
                //if(content>absthr)
  		m_currHist->SetBinContent(i+1, content);
  	}

        // save the peaks in TFile/TH1F
        if (!m_name.empty()){
          m_file->cd();
  	  m_currHist->Write();
        }

  }

  return ret;
}

void PeakFinder::Close(){
	std::cout << "Total # of peaks: " << m_currId+1 << endl;
	if(!m_name.empty()) m_file->Close();
        delete m_spectrum;
}
