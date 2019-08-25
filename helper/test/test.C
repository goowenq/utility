#include "PdfHelper.h"
#include "FFTHelper.h"

void test(){
// Example of PdfHelper
// PdfHelper* pdf = new PdfHelper("pdf", 4,4, 37); // name, columns per page , rows per page, max pages per PDF file
// for(int i=0; i<3442; i++){
//  TH1F* h = new TH1F(Form("h%d",i), Form("h%d",i), 100, -10, 10);
//  h->FillRandom("gaus");
//  pdf->Next();
//  h->Draw();
//  cout << "drawing " << h->GetName() << endl;
// }
// pdf->Close();

// Example of FFT Interpolation Helper
TH1F* h = new TH1F("h","h",100,-5,5);
h->FillRandom("gaus");
h->Draw();
vector<double> x,y;
for(int i=1; i<=100; i++){
	double content = h->GetBinContent(i);
	x.push_back(h->GetBinCenter(i));
	y.push_back(content);
}
FFTHelper* fft = new FFTHelper();
fft->initialize(x,y, x.size(), 10);
fft->translate(0.03);
TGraph* g1 = new TGraph();
for(int i=0; i<fft->_newx.size(); i++){
	double xx = fft->_newx[i];
	double yy = fft->_newy[i];
	g1->SetPoint(g1->GetN(), xx,yy);
}
delete fft;
g1->SetMarkerColor(2);
g1->SetLineColor(2);
g1->Draw("PLsame");

}
