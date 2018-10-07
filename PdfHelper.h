/* Example:
*  PdfHelper* pdf = new PdfHelper("OutputName", nColumn, nRow, MaxPage);
*  for(int i=0; i<100; i++){
*	 TH1F* h = new TH1F();
*	 pdf->Next();
*	 h->Draw();
*  }
*  pdf->Close();
*/
#include "TCanvas.h"
#include <string>

class PdfHelper{
	std::string m_pdfName;
	std::string m_fullPdfName;
	int m_columns;
	int m_rows;
	int m_maxPages;
	int i_page;
	int i_pdf;
	TCanvas* m_canvas;
	int i_pad;

public:
	PdfHelper(const char* name, int col, int row, int pages);
	void Next();
	void Print();
	void Close();

};

PdfHelper::PdfHelper(const char* name, int col, int row, int pages)
:m_pdfName(name), m_columns(col), m_rows(row), m_maxPages(pages){
	i_page = 0;
	i_pdf = 0;
	i_pad = 0;
	m_canvas = new TCanvas("c1","c1",800, 600);
	m_canvas->Divide(m_columns, m_rows);
	m_fullPdfName = m_pdfName +"_"+i_pdf+".pdf";
}

void PdfHelper::Next(){
	if(i_pad == m_columns * m_rows){
		i_pad =1;
		i_page ++;
		std::string filename = m_fullPdfName;
		if(i_page==1){
			filename += "("; // first page
		}
		else if(i_page == m_maxPages){
			filename += ")"; // last page
			i_page=0;
			i_pdf ++;
			m_fullPdfName = m_pdfName +"_"+i_pdf+".pdf";			
		}
		m_canvas->Print(filename.c_str(),"pdf");
		m_canvas->Clear();
		m_canvas->Divide(m_columns, m_rows);	
	}
	else{
		i_pad ++;
	}
	m_canvas->cd(i_pad);
}

void PdfHelper::Close(){
	std::string filename = m_fullPdfName+")";
	m_canvas->Print(filename.c_str(),"pdf");
}
