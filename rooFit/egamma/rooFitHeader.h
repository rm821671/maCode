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


#include "RooHistPdf.h"
//#include "Roo.h"
#include "RooCBShape.h"
#include "RooBreitWigner.h"
#include "RooExponential.h"
#include "RooChebychev.h"
#include "RooLandau.h"
#include "RooNumConvPdf.h"
#include "RooEffProd.h"
#include "RooAddPdf.h"
#include "RooKeysPdf.h"
#include "RooFitResult.h"

// own density functions
#include "RooCBExGaussShape.h"
#include "RooCMSShape.h"
#include "RooDoubleCBFast.h"

// classes for fit and plotting and all that
#include "TTemplateFit.h"


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
#include "TGraphErrors.h"
#include "TF1.h"

#include "TRandom.h"
#include "TRandom3.h"


#include "TCanvas.h"
#include "TAxis.h"
#include "RooPlot.h"
#include "RooHist.h"

#include "TTree.h"
#include "TH1D.h"
#include "TRandom.h"
#include "TLegend.h"

#include <ctime>
#include <sstream>
#include <iostream>
#include <fstream>
#include <iomanip>
//#include <string>

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

void test();

void FitterEG(string dataset);
void FitterEE(string dataset);

void FitterEGdata(string dataset);
void FitterEEdata(string dataset);

void kernelEst(string dataset);


void templateKernel(string signal, string background, string dataset);

void templateKernelTest(string signal, string background, string dataset);
void templateKernelClass(string signal, string background, string dataset);

void templateKernelUnbinned(string signal, string background, string dataset);
void templateKernelUnbinned_PtNtrk(string signal, string background, string dataset);

void readFitResults(string dropbox);
void influenceBinningFit(string dropbox);
void influenceSignalBackground(string dropbox);

void readValues();







#endif
