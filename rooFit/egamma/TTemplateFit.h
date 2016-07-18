//
// TTemplateFit.h
//
// @author: ralf.meyer@rwth-aachen.de
// @version: 0.8
// 
// A class to easily fit two template based p.d.f.s (signal and background)
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
    
    // some test methods
    void setK(Int_t val) ;
    Int_t getK() ;
    
    // methods to setup the fit
    inline void setType(string type) { sType = type; return ;}
    inline void setSignalConvolution(Bool_t val) { bConvolveSignal = val; return ;}
    inline void setUnbinnedFit(Bool_t val) { bUnbinnedFit = val; return ;}
    inline void setRhoBkg(Double_t val) {rho_bkg = val; return ;}
    inline void setRhoSig(Double_t val) {rho_sig = val; return ;}
    void setFitBins(Int_t val);
    void setPdfBins(Int_t val);
    inline void setHistOrder(Int_t val) { histOrder = val; return ;}
    inline Int_t getHistOrder() { return histOrder; }
    inline Double_t getNtot() { return Ntot; }
    
    // add data as TH1, TTree or RooDataSet
    void addData(TH1F* data,        TH1F* sig,       TH1F* bkg);
    void addData(TH1F* data,        TTree* sig,      TTree* bkg);
    void addData(RooDataSet* data,  TTree* sig,      TTree* bkg);
    void addData(TTree* data,       TTree* sig,      TTree* bkg);
    void addData(RooDataSet* data,  RooDataSet* sig, RooDataSet* bkg);
    
    // perform a fit of the data to a given pdf
    void fitToPdf(RooAbsPdf *pdf, TFile* f, string fSubfolder, string sFit, string sPlotStyle);
    
    //
    void scaleBackgroundTemplate(TTree* mcBkg, TTree* mcBkgTrue, TFile* f, string fSubfolder);
    
    // change default values of the gaussian signal smearing function
    void setGaus(   Float_t mean,   Float_t mean_low,   Float_t mean_high,
                    Float_t width,  Float_t width_low,  Float_t width_high);
    
    void buildPdf(string method, string fit);
    
    // internal functions
    void kernelFit();
    void histoFit(Int_t order);
    void constructModel(); // background is always gaussian kernel est.
    void fitModel();
    
    void buildFrame();
    
    void drawFrame(TFile *f, string fSubfolder);
    void drawRaw(TFile *f, string fSubfolder);
    void save(TFile *f, string fSubfolder);
    
protected:
    
    string name;
    string title;
    
    // residual graph creator
    RooHist* getPullHist(RooPlot *frame);
    
private:
    
    Int_t       k;    // test
    Double_t    Ntot; // total number of events in data
    
    // gaussian smearing parameters
    Double_t    gmean,  gmean_low,  gmean_high, 
                gwidth, gwidth_low, gwidth_high; 
    
    //  data or monte carlo (mc)
    string sType;
    
    // control booleans
    Bool_t bData;
    Bool_t bTemplates;
    Bool_t bFrameBuild;
    Bool_t bRawFrame;
    Bool_t bFitted;
    
    // unbinned fit option
    Bool_t bUnbinnedFit;
    
    // option: convolve signal with gaus or not
    Bool_t bConvolveSignal;
    
    // define observable
    RooRealVar* m = NULL;
    
    // rho parameter for gaussian adaptive kernel estimator
    Float_t rho_sig, // = 0.3162,   // redefined when data are added
            rho_bkg; //= 0.2887;   // 1/sqrt(12)
    
    // binnings for:
    // - the fit to the data Nbins
    // - the pdf construction from the signal in case of histogram based interpolation
    Int_t       NbinsFit;
    Int_t       NbinsPdf;
    Int_t       NbinsPlot;
    
    // order of interpolation in case of histogram based p.d.f. construction
    Int_t       histOrder;
    
    // signal and background events
    RooRealVar* Nsig = NULL;
    RooRealVar* Nbkg = NULL;
    
    // the class contains two templates and one dataset
    // binned data sets
    RooDataHist* rdhData        = NULL; // 
    RooDataHist* rdhSigTemplate = NULL;
    RooDataHist* rdhBkgTemplate = NULL;
    
    // unbinned data sets
    //RooDataSet* rdsData = NULL;
    RooDataSet* rdsData        = NULL;
    RooDataSet* rdsSigTemplate = NULL;
    RooDataSet* rdsBkgTemplate = NULL;
    
    // binned data histogram only for plot
    RooDataHist* rdhPlotData = NULL;
    
    // scaling data sets for background variation
    RooDataSet* rdsBkgTemplateMC     = NULL;
    RooDataSet* rdsBkgTemplateMCtrue = NULL;
    
    RooDataHist* rdhBkgTemplateMC        = NULL;
    RooDataHist* rdhBkgTemplateMCtrue    = NULL;
    RooDataHist* rdhBkgTemplateUnscaled  = NULL;
    RooDataHist* rdhBkgTemplateScaled    = NULL;
    
    // pdf.s for signal and background
    // - background is always modelled with a Kernel Estimator
    // - signal is kernel estimated or histogram based
    RooAbsPdf* pdfBkg       = NULL;
    RooAbsPdf* pdfSig       = NULL;
    RooAbsPdf* smearedSig   = NULL;
    
    // smearing gaussian for signal
    RooRealVar* gm = NULL;
    RooRealVar* gs = NULL;
    // RooGaussian *gaus = NULL;
    RooAbsPdf *gaus = NULL;

    // complete pdf
    RooAbsPdf* model = NULL;
    
    // fit result of the template fit
    RooFitResult* fitResult = NULL;
    
    // pull distribution
    RooHist* hpull = NULL;
    
    // plot frames
    RooPlot *frame = NULL;
    RooPlot *fpull = NULL;
    
    
    ClassDef(TTemplateFit, 1)
    
};
#endif
