/* Helper class for FFT interpolation via ROOT
*  Wenqiang Gu (goowenq@gmail.com)
*
*  Example:
*	FFTHelper* fft = new FFTHelper();
*	fft->initialize(vector<> x,vector<>y, x.size(), sample_length);
*	fft->translate(double dt);
*	for(int i=0; i<fft->_newx.size(); i++){
*		double xx = fft->_newx[i];
*		double yy = fft->_newy[i];
*
*	}
*	delete fft;
*
*   Warning:
*    Too small shift(<1E-3) will not be translated, see FFTHelper::translate()
*/

#include <vector>
#include "TVirtualFFT.h"
#include "TComplex.h"
#include "TMath.h"

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
	if(std::abs(dt) < 1e-3){ // do NOT translate for too small shift
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