#!/bin/bash
export WC_PATH=`pwd`/wcp
export WC_EXTERNAL=`pwd`/external

clear
echo
echo "Start to install Wire-Cell Prototype (WCP)."
echo
read -p "Press [Enter] key to continue..."

mkdir -p $WC_EXTERNAL/package
pushd $WC_EXTERNAL/package
  #--- eigen
  wget https://gitlab.com/libeigen/eigen/-/archive/3.3.7/eigen-3.3.7.tar.gz
  tar -xzvf eigen-3.3.7.tar.gz
  pushd eigen-3.3.7
  mkdir build
  cd build
  cmake -DCMAKE_INSTALL_PREFIX=$WC_EXTERNAL ..
  make install
  popd
  
  #--- boost
  wget https://dl.bintray.com/boostorg/release/1.69.0/source/boost_1_69_0.tar.bz2
  tar --bzip2 -xf boost_1_69_0.tar.bz2
  pushd boost_1_69_0
  ./bootstrap.sh --prefix=$WC_EXTERNAL
  ./b2 install
  popd
  
  #--- fftw3
  wget http://www.fftw.org/fftw-3.3.8.tar.gz
  tar -xzvf fftw-3.3.8.tar.gz
  pushd fftw-3.3.8
  ./configure --prefix=$WC_EXTERNAL --enable-shared
  make
  make install
  popd
  
  #--- GLPK
  wget http://ftp.gnu.org/gnu/glpk/glpk-4.65.tar.gz
  tar -xzvf glpk-4.65.tar.gz
  pushd glpk-4.65
  ./configure --prefix=$WC_EXTERNAL
  make
  make install
  popd

  #--- ROOT
  wget https://root.cern.ch/download/root_v6.12.06.source.tar.gz
  tar -xzvf root_v6.12.06.source.tar.gz
  pushd root-6.12.06
  mkdir install
  cd install
  cmake -Dminuit2=ON -Dpython=ON -Dcxx11=ON -DFFTW_LIBRARY=$WC_EXTERNAL/lib/libfftw3.so ..
  make -j4
  popd

popd

#--- wcp
git clone https://github.com/BNLIF/wire-cell.git wcp
cd wcp/
./switch-git-urls dev
git submodule init
git submodule update
alias waf=`pwd`/waf-tools/waf
mkdir install
source $WC_EXTERNAL/package/root-6.12.06/install/bin/thisroot.sh
export LD_LIBRARY_PATH=$WC_EXTERNAL/lib:$LD_LIBRARY_PATH
waf --prefix=$WC_PATH/install --with-fftw=$WC_EXTERNAL --with-fftw-include=$WC_EXTERNAL/include --with-fftw-lib=$WC_EXTERNAL/lib --with-glpk=$WC_EXTERNAL --with-glpk-include=$WC_EXTERNAL/include --with-glpk-lib=$WC_EXTERNAL/lib --boost-includes=$WC_EXTERNAL/include --boost-libs=$WC_EXTERNAL/lib --boost-mt --with-eigen=$WC_EXTERNAL configure
waf build
waf install

echo
echo "Successfull installatin for WCP."
echo
echo "Also, please add these lines in your ~/.bashrc."
echo
echo "export WC_PATH=$WC_PATH"
echo "export WC_EXTERNAL=$WC_EXTERNAL"
echo "source \$WC_EXTERNAL/package/root-6.12.06/install/bin/thisroot.sh"
echo "alias waf=\$WC_PATH/waf-tools/waf"
echo "export PATH=\$WC_PATH/install/bin:\$PATH"
echo "export LD_LIBRARY_PATH=\$WC_PATH/install/lib:\$WC_PATH/install/lib64:\$WC_EXTERNAL/lib:\$LD_LIBRARY_PATH"
echo "export PYTHONPATH=\$WC_PATH/install:\$PYTHONPATH"
