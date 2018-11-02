
void plot_mitigation(int, TCanvas*, TH2I*, TH2F*, TH2F*);

void plot_mitigation_all(){

	TFile* file1 = TFile::Open("magnify_4696_358_dl9.root");
	TFile* file2 = TFile::Open("mitigated_magnify_4696_358_dl9.root");
	TH2I* hu_orig = (TH2I*)file1->Get("hu_orig");
	TH2I* hv_orig = (TH2I*)file1->Get("hv_orig");
	TH2I* hw_orig = (TH2I*)file1->Get("hw_orig");
	
	TH2F* hu_linear = (TH2F*)file2->Get("hu_linear");
	TH2F* hv_linear = (TH2F*)file2->Get("hv_linear");
	TH2F* hw_linear = (TH2F*)file2->Get("hw_linear");
	TH2F* hu_mitigated = (TH2F*)file2->Get("hu_mitigated");
	TH2F* hv_mitigated = (TH2F*)file2->Get("hv_mitigated");
	TH2F* hw_mitigated = (TH2F*)file2->Get("hw_mitigated");
	
	hu_orig->Add(hv_orig); hu_orig->Add(hw_orig);
	hu_linear->Add(hv_linear); hu_linear->Add(hw_linear);
	hu_mitigated->Add(hv_mitigated); hu_mitigated->Add(hw_mitigated);


	TCanvas* c1 = new TCanvas("c1","c1",1200,600);
	c1->Divide(3,2);

	c1->Print("mitigation.pdf(","pdf");
	for(int i=100; i<200; i++){
		if(i%100==0) std::cout << i << std::endl; 
		plot_mitigation(i, c1, hu_orig, hu_linear, hu_mitigated);
	}
	c1->Print("mitigation.pdf)","pdf");

}

void plot_mitigation(int chn, TCanvas* c1, TH2I* hu_orig, TH2F* hu_linear, TH2F* hu_mitigated){

	TH1F* horig = new TH1F("horig",Form("Origninal Channel= %d",chn),6000,0,6000);
	TH1F* hmod = new TH1F("hmod","ADC % 64",6000,0,6000);
	TH1F* hlinear = new TH1F("hlinear","Linear Interpolation",6000,0,6000);
	TH1F* hmitigated = new TH1F("hmitigated","Mitigated",6000,0,6000);
	horig->GetXaxis()->SetTitle("ticks");
	hmod->GetXaxis()->SetTitle("ticks");
	hlinear->GetXaxis()->SetTitle("ticks");
	hmitigated->GetXaxis()->SetTitle("ticks");

	int nticks=6000;
	for(int i=0; i!=nticks; i++){
		horig->SetBinContent(i+1, hu_orig->GetBinContent(chn+1, i+1));
		int ADC = hu_orig->GetBinContent(chn+1, i+1);
		hmod->SetBinContent(i+1, ADC % 64);
		hlinear->SetBinContent(i+1, hu_linear->GetBinContent(chn+1, i+1));
		hmitigated->SetBinContent(i+1, hu_mitigated->GetBinContent(chn+1, i+1));
	}

	// noise distribution
	int origmax = horig->GetMaximum();
	int origmin = horig->GetMinimum();
	TH1F* horig_n = new TH1F("horig_n","Original", 20+origmax-origmin, origmin-10, origmax+10);
	TH1F* hmiti_n = new TH1F("hmiti_n","Mitigated", 20+origmax-origmin, origmin-10, origmax+10);
	horig_n->SetLineColor(4);
	hmiti_n->SetLineColor(2);
	for(int i=0; i!=nticks; i++){
		horig_n->Fill(horig->GetBinContent(i+1));
		hmiti_n->Fill(hmitigated->GetBinContent(i+1));
	}

	//TCanvas* c1 = new TCanvas("c1","c1",1200,600);
	//c1->Divide(3,2);
	c1->cd(1); horig->Draw();
	c1->cd(2); hmod->Draw();
	c1->cd(4); hlinear->SetLineColor(8); hlinear->Draw();
	c1->cd(5); hmitigated->SetLineColor(2); horig->Draw(); hmitigated->Draw("same");
	c1->cd(6); horig_n->Draw(); hmiti_n->Draw("same");
	TLegend* lg = new TLegend(0.6,0.6,0.8,0.8);
	lg->AddEntry(horig,"Original", "l")->SetTextColor(4);
	lg->AddEntry(hmitigated,"Mitigated", "l")->SetTextColor(2);
	lg->SetBorderSize(0);
	lg->SetFillColor(0);
	lg->Draw();

	c1->Print("mitigation.pdf","pdf");

	horig->Delete();
	hmod->Delete();
	hlinear->Delete();
	hmitigated->Delete();
	horig_n->Delete();
	hmiti_n->Delete();
	lg->Delete();
}
