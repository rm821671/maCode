//
// TAuxiliary.h
//
// @author: ralf.meyer@rwth-aachen.de
// 
// A class containing helper functions.
// I.e. set plot options, create ratios, ...
// Make some stuff with datatypes from RooFit...
//
// Includes all needed variables to handle from function to function.
// 
// 


#ifndef AUXILIARYCLASS
#define AUXILIARYCLASS



// strange behaviour:
// ACliC can not compile this without the including of some
// ROOT classes explicitely here
// (although they are included in rooFitHeader.h as well...)
#include "rooFitHeader.h"

#include "TTemplateFit.h"

#include "TPad.h"
#include "THStack.h"


struct labeledRooDataSet{
        RooDataSet* r;
        string name;
    };

class TAuxiliary{
    
    
    
public:
    // ~ constructor, destructor
    TAuxiliary();
    inline virtual ~TAuxiliary() {}
    
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // methods
    
    void drawCMS(TPad& p);
    
    void setTH1PlotOptions(TH1& h);
    void setTH1RatioPlotOptions(TH1& h);
    
    
    void setPadStyle(TPad& p);
    void setCanvasStyle(TCanvas& c);
    
    
    // methods for handling roo data sets
    map<Int_t, map<string, RooDataSet*>> SortRooDataSets(map<string, RooDataSet*> r);
    THStack* RooTStack(string name, string title, map<Int_t, map<string, RooDataSet*>> r, RooRealVar& x, Int_t Nbins);
    
    
    
    
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // member data objects
    
    // templates and data files
    map<string, TFile*> f;
    string fname;
    
    // read out trees
    map<string, TTree*> t;
    string *treeNames = NULL; // this should point to a string array
    Int_t NtreeNames;         // should be the length of the array! otherwise segmenation faults...
    
    string *ts = NULL;  // same
    //~ Int_t Nts;          // - " -
    
    // template fit class objects
    map<string, TTemplateFit*>  tf;
    
    // map for all roo data sets
    map<string, RooDataSet*>    rds;
    
    // map for roo real vars
    map<string, RooRealVar*>    rv;
    
    // roodatasets for monte carlo
    map<string, RooDataSet*>    rdsMc;
    map<string, RooDataSet*>    rdsMcW;
    string mcname;
    
    //~ // create datasets in each variable
    map<string, RooDataSet*>    rdsM;      // map with invariant mass dependency data
    map<string, RooDataSet*>    rdsPt;     // pt
    map<string, RooDataSet*>    rdsNtrk;   // ntrk
    map<string, RooDataSet*>    rdsPtNtrk; // pt and ntrk
    map<string, RooDataSet*>    rdsNvtx;   // nvtx
    map<string, RooDataSet*>    rdsEta;    // Eta
    map<string, RooDataSet*>    rdsW;      // weightes
    
    // binned 1d montecarlo histograms (to apply weights)
    map<string, TH1F*> thMc;
    
    //~ // define binnings for 1d variable plots
    // Pt 
    Double_t *bins_pt       = NULL;
    string *bins_pt_pdf     = NULL;
    Int_t Npt;
    // Nvtx
    Double_t *bins_nvtx     = NULL;
    string *bins_nvtx_pdf   = NULL;
    Int_t Nvtx;
    // Ntrk
    Double_t *bins_ntrk     = NULL;
    string *bins_ntrk_pdf   = NULL;
    Int_t Ntrk;
    // eta
    Double_t *bins_eta      = NULL;
    string *bins_eta_pdf    = NULL;
    Int_t Neta;
    
    string sBin, sName;
    string sLow, sUp;
    
    Int_t fitPtBin = 7;
    Int_t fitFraction = 1; // 0: den, 1: num
    
    // subfolder for each fit
    string fSubfolder;
    string unique;
    
    // pointer on roo distributions
    RooAbsPdf* parametrizedModel;
    RooArgSet* parametrizedPars;
    
    
    
    
    
    ClassDef(TAuxiliary, 1)
};
#endif
