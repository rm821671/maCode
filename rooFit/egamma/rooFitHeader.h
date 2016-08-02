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
#include "RooFFTConvPdf.h"
#include "RooEffProd.h"
#include "RooAddPdf.h"
#include "RooKeysPdf.h"
#include "RooFitResult.h"

// own density functions
#include "RooCBExGaussShape.h"
#include "RooCMSShape.h"
#include "RooDoubleCBFast.h"

// auxiliary class with helpfull method and a lot of data members
#include "TAuxiliary.h"

// classes for fit and plotting and all that
#include "TTemplateFit.h"

#include "TROOT.h"
#include "TChain.h"
#include "TFile.h"
#include "TLatex.h"

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
#include "THStack.h"

#include "TRandom.h"
#include "TRandom3.h"

#include "TPad.h"
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
 * parametrized models
 * 
 ***/

RooAbsPdf *getModelPdf(RooRealVar& m, Int_t N, string component, string fraction);

/**********************************************************************************************
 * fits and stuff
 * 
 ***/


void workflowHandler(map<string, string> set);

void test(map<string, string> &set);

void templateKernelUnbinned(map<string, string> set);

void readFitResults(string dropbox);


void closurePlot(map<string, string> set);


void readValues();






#endif
