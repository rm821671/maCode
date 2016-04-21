#ifndef MY_ROO_HEADER
#define MY_ROO_HEADER

#ifndef __CINT__
#include "RooGlobalFunc.h"
#endif
#include "RooRealVar.h"
#include "RooConstVar.h"

#include "RooDataSet.h"
#include "RooDataHist.h"
#include "RooGaussian.h"

#include "RooAbsPdf.h"
#include "RooAbsArg.h"
#include "RooRealProxy.h"

//#include "Roo.h"
#include "RooCBShape.h"
#include "RooBreitWigner.h"
#include "RooExponential.h"
#include "RooNumConvPdf.h"
#include "RooEffProd.h"
#include "RooAddPdf.h"
#include "RooKeysPdf.h"

// own density functions
#include "RooCBExGaussShape.h"
#include "RooCMSShape.h"
#include "RooDoubleCBFast.h"


#include "TROOT.h"
#include "TChain.h"
#include "TFile.h"

//#include "TSelector.h"
//#include "TTreeReader.h"
//#include "TTreeReaderValue.h"
//#include "TTreeReaderArray.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TProfile.h"
#include "TEfficiency.h"

#include "TRandom.h"
#include "TRandom3.h"


#include "TCanvas.h"
#include "TAxis.h"
#include "RooPlot.h"
#include "RooHist.h"

#include "TTree.h"
#include "TH1D.h"
#include "TRandom.h"

#include <ctime>
#include <sstream>
#include <iostream>
#include <fstream>
#include <iomanip>

#include <algorithm>
#include <string>


#include <map>
#include <sys/utsname.h>


using namespace std;
using namespace RooFit;


/**********************************************************************************************
 * setup functions 
 * 
 */

template <typename T> string NumberToString ( T Number );
void SystemPath(string & filepath, string & dropbox);

/**********************************************************************************************
 * models
 * 
 ***/

RooAbsPdf* modelEG(RooRealVar& m);
RooAbsPdf* modelEE(RooRealVar& m);

/**********************************************************************************************
 * fits and stuff
 * 
 ***/

void FitterEG(string dataset);
void FitterEE(string dataset);

void kernelEst(string dataset);









#endif
