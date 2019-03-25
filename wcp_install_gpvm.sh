#!/bin/bash
source /cvmfs/dune.opensciencegrid.org/products/dune/setup_dune.sh
setup dunetpc v08_11_00 -q e17:prof
setup cmake v3_13_2
setup eigen v3_3_4a

WC_PATH=`pwd`/wcp
WC_EXTERNAL=`pwd`/external
UPS_EXTERNAL=/cvmfs/larsoft.opensciencegrid.org/products/

#--- GLPK
cd $WC_EXTERNAL
wget http://ftp.gnu.org/gnu/glpk/glpk-4.65.tar.gz
tar -xzvf glpk-4.65.tar.gz
cd glpk-4.65
./configure --prefix=$WC_EXTERNAL
make
make install

#--- wcp
git clone https://github.com/BNLIF/wire-cell.git wcp
cd $WC_PATH
git submodule init
git submodule update
alias waf=`pwd`/waf-tools/waf
mkdir install

waf --prefix=$WC_PATH/install --with-fftw=$UPS_EXTERNAL/fftw/v3_3_6_pl2/Linux64bit+2.6-2.12-prof --with-fftw-include=$UPS_EXTERNAL/fftw/v3_3_6_pl2/Linux64bit+2.6-2.12-prof/include --with-fftw-lib=$UPS_EXTERNAL/fftw/v3_3_6_pl2/Linux64bit+2.6-2.12-prof/lib --with-glpk=$WC_EXTERNAL --with-glpk-include=$WC_EXTERNAL/include --with-glpk-lib=$WC_EXTERNAL/lib --boost-includes=$UPS_EXTERNAL/boost/v1_66_0a/Linux64bit+2.6-2.12-e17-prof/include --boost-libs=$UPS_EXTERNAL/boost/v1_66_0a/Linux64bit+2.6-2.12-e17-prof/lib --boost-mt --with-eigen=$UPS_EXTERNAL/eigen/v3_3_4a configure

waf build
waf install

echo
echo "Successfull installatin for WCP."
echo
echo "Also, please add these lines in your ~/.bashrc."
echo
echo "export WC_PATH=$WC_PATH"
echo "export WC_EXTERNAL=$WC_EXTERNAL"
echo "alias waf=\$WC_PATH/waf-tools/waf"
echo "export PATH=\$WC_PATH/install/bin:\$PATH"
echo "export LD_LIBRARY_PATH=\$WC_PATH/install/lib:\$WC_PATH/install/lib64:\$WC_EXTERNAL/lib:\$LD_LIBRARY_PATH"
echo "export PYTHONPATH=\$WC_PATH/install:\$PYTHONPATH"
