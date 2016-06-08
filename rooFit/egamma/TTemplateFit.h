//
// TTemplateFit.h
//
// @author: ralf.meyer@rwth-aachen.de
// @version: 0.8
// 
// A class to easly fit two template based p.d.f.s (signal and background)
// to a dataset and save the results in a given TFile.
// 


#ifndef ROOTEMPLATEFIT
#define ROOTEMPLATEFIT

#include "rooFitHeader.h"

//#include "RooDataSet.h"
//#include "RooDataHist.h"
//#include "RooRealVar.h"

//#include "TH1F.h"

class TTemplateFit{
    
    
    
public:
    TTemplateFit();
    TTemplateFit(string name, string title, RooRealVar *x);
    TTemplateFit(const TTemplateFit& other, string name="");
    
    virtual TTemplateFit* clone(string newname) const {return new TTemplateFit(*this, newname);}
    inline virtual ~TTemplateFit() { /*cout << "delete TTemplateFitter!" << endl;*/ }
    
    //
    Float_t rho_sig = 0.3162,   // redefined when data are added
            rho_bkg = 0.2887;   // 1/sqrt(12)
    
    // some test methods
    void setK(Int_t val) ;
    Int_t getK() ;
    
    void setFitBins(Int_t val);
    void setPdfBins(Int_t val);
    inline void setHistOrder(Int_t val) { histOrder = val; }
    inline Int_t getHistOrder() { return histOrder; }
    
    // add data as TH1, TTree or RooDataSet
    void addData(TH1F* data, TH1F* sig, TH1F* bkg);
    void addData(TTree* data, TTree* sig, TTree* bkg);
    void addData(RooDataSet* data, RooDataSet* sig, RooDataSet* bkg);
    
    // change default values of the gaussian signal smearing function
    void setGaus(   Float_t mean,   Float_t mean_low,   Float_t mean_high,
                    Float_t width,  Float_t width_low,  Float_t width_high);
    
    void buildPdf(string method, string fit);
    
    // internal functions
    void kernelFit();
    void histoFit(Int_t order);
    void constructModel(RooAbsPdf* sig); // background is always gaussian kernel est.
    void fitModel();
    
    void buildFrame();
    
    void drawFrame(TFile *f, string fSubfolder);
    void drawRaw(TFile *f, string fSubfolder);
    void save(TFile *f, string fSubfolder);
    
protected:
    
    string name;
    string title;
    
private:
    
    Int_t       k;
    Double_t    Ntot; // total number of events in data
    
    // gaussian smearing parameters
    Double_t    gmean,  gmean_low,  gmean_high, 
                gwidth, gwidth_low, gwidth_high; 
    
    // control booleans
    Bool_t bData;
    Bool_t bTemplates;
    Bool_t bFrameBuild;
    Bool_t bRawFrame;
    Bool_t bFitted;
    
    // unbinned fit option
    Bool_t bUnbinnedFit;
    
    // define observable
    RooRealVar* m = NULL;
    
    // binnings for:
    // - the fit to the data Nbins
    // - the pdf construction from the signal in case of histogram based interpolation
    Int_t       NbinsFit,
                NbinsPdf;
    
    // order of interpolation in case of histogram based p.d.f. construction
    Int_t histOrder;
    
    // signal and background events
    RooRealVar* Nsig = NULL;
    RooRealVar* Nbkg = NULL;
    
    // the class contains two templates and one dataset
    RooDataHist* rdhData = NULL; // 
    RooDataHist* rdhSigTemplate = NULL;
    RooDataHist* rdhBkgTemplate = NULL;
    
    // data histogram only for plot
    RooDataHist* rdhPlotData = NULL;
    
    // unbinned data sets
    RooDataSet* rdsData = NULL;
    RooDataSet* rdsSigTemplate = NULL;
    RooDataSet* rdsBkgTemplate = NULL;
    
    // pdf.s for signal and background
    // - background is always modelled in a Kernel Estimator
    // - signal is KernelEstimated or histogram based
    RooAbsPdf* kestBkg      = NULL;
    RooAbsPdf* kestSig      = NULL;
    RooAbsPdf* histSig      = NULL;
    RooAbsPdf* smearedSig   = NULL;
    
    // smearing gaussian for signal
    RooRealVar* gm = NULL;
    RooRealVar* gs = NULL;
    // RooGaussian *gaus = NULL;
    RooAbsPdf *gaus = NULL;

    // complete pdf
    RooAbsPdf* model = NULL;
    
    // fit result
    RooFitResult* fitResult = NULL;
    
    // pull distribution
    RooHist* hpull = NULL;
    
    // plot frames
    RooPlot *frame = NULL;
    RooPlot *fpull = NULL;
    
    
    ClassDef(TTemplateFit, 1)
    
};
#endif
