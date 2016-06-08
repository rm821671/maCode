//
// Implementation of the TTemplateFit.
// 
// @author: ralf.meyer@rwth-aachen.de
// @version: 0.8
// 

#include "TTemplateFit.h"


ClassImp(TTemplateFit) ;

TTemplateFit::TTemplateFit(){};

TTemplateFit::TTemplateFit(string name, string title, RooRealVar *x):
    name(name),
    title(title),
    bData(kFALSE),
    bTemplates(kFALSE),
    bFrameBuild(kFALSE),
    bRawFrame(kFALSE),
    bFitted(kFALSE),
    bUnbinnedFit(kFALSE),
    gmean(0.),
    gmean_low(-5.),
    gmean_high(5.),
    gwidth(1.),
    gwidth_low(0.001),
    gwidth_high(5.),
    histOrder(3)
{
    
    Float_t     low,
                up;
    
    //m = x;
    m = new RooRealVar(*x);
    
    low = m->getMin();
    up  = m->getMax();
    
    // initial values for the binning
    NbinsPdf = Int_t((up-low));      // 
    
    NbinsFit = Int_t((up-low)*4);   // fine range for the fit
    
    
}

TTemplateFit::TTemplateFit(const TTemplateFit &other, string name)
{
    // something like a copy constructor
    
}

void TTemplateFit::setK(Int_t val)
{
    k = val;
    return ;
}

Int_t TTemplateFit::getK()
{
    return k;
}

void TTemplateFit::setFitBins(Int_t val)
{
    NbinsFit = val;
    // recreate data histogram with new binning
    rdhData         = new RooDataHist(  ("data_"+name).c_str(), 
                                        "Data", 
                                        *m, 
                                        Import(*(rdsData->createHistogram(m->GetName(), NbinsFit)))
                                        );
    return ;
}

void TTemplateFit::setPdfBins(Int_t val)
{
    NbinsPdf = val;
    // recreate template histograms with new binning
    rdhSigTemplate  = new RooDataHist(  ("sig_"+name).c_str(), 
                                        "Signal", 
                                        *m, 
                                        Import(*(rdsSigTemplate->createHistogram(m->GetName(), NbinsPdf)))
                                        );
    rdhBkgTemplate  = new RooDataHist(  ("bkg_"+name).c_str(), 
                                        "Background", 
                                        *m, 
                                        Import(*(rdsBkgTemplate->createHistogram(m->GetName(), NbinsPdf)))
                                        );
    return ;
}

void TTemplateFit::addData(TH1F* data, TH1F* sig, TH1F* bkg)
{
    // Use this to add data as ROOT::TH1 histograms.
    
    rdhData = new RooDataHist(("data_"+name).c_str(), "Data", *m, Import(*data));
    
    Ntot = rdhData->sumEntries(); // total number of events in the data
    cout << "TTemplateFit::addData(TH1F* data)\t Ntot = " << Ntot << endl;
    
    // set values for signal and background 
    Nsig = new RooRealVar(  ("Nsig_"+name).c_str(), 
                            "Signal events", 
                            0.8*Ntot, 0.4*Ntot, Ntot);
    Nbkg = new RooRealVar(  ("Nbkg_"+name).c_str(), 
                            "Background events", 
                            0.2*Ntot, 0.01*Ntot, 0.6*Ntot);
    rdhSigTemplate = new RooDataHist(("sig_"+name).c_str(), "Signal", *m, Import(*sig));
    rdhBkgTemplate = new RooDataHist(("bkg_"+name).c_str(), "Background", *m, Import(*bkg));
    
    rdhPlotData     = new RooDataHist(  ("plotdata_"+name).c_str(), 
                                        "Data (plot)", 
                                        *m, 
                                        Import(*data)
                                        );
    
    rho_sig = 1./(2.*sqrt(data->GetStdDev())); //0.3162;   // 1/(2*sqrt(sigma))
    rho_bkg = 0.2887;   // 1/sqrt(12)
    
    bTemplates = kTRUE;
    bData = kTRUE;
    return ;
}

void TTemplateFit::addData(TTree* data, TTree* sig, TTree* bkg)
{
    // Use this to add data as trees; binning can changed however you like it.
    // The unbinned data are _not_ used in the fit; instead it is recommended
    // to use a fine binning. The p.d.f. construction from the templates
    // is done by the unbinned trees in case of kernel density estimation
    // or the binned histograms in case of histogram based interpolation.
    
    rdsData = new RooDataSet(("data_unbinned_"+name).c_str(), "Data", *m, Import(*data));
    rdsSigTemplate = new RooDataSet(("sig_unbinned_"+name).c_str(), "Signal", *m, Import(*sig));
    rdsBkgTemplate = new RooDataSet(("bkg_unbinned_"+name).c_str(), "Background", *m, Import(*bkg));
    
    Ntot = data->GetEntries();
    
    // set values for signal and background 
    Nsig = new RooRealVar(  ("Nsig_"+name).c_str(), 
                            "Signal events", 
                            0.8*Ntot, 0.4*Ntot, Ntot);
    Nbkg = new RooRealVar(  ("Nbkg_"+name).c_str(), 
                            "Background events", 
                            0.2*Ntot, 0.01*Ntot, 0.6*Ntot);
    
    // create binned RooDataHist in variable name
    // - rdhData is used for the fit
    // - binned templates are used in case of histogram 
    rdhData         = new RooDataHist(  ("data_"+name).c_str(), 
                                        "Data", 
                                        *m, 
                                        Import(*(rdsData->createHistogram(m->GetName(), NbinsFit)))
                                        );
    rdhSigTemplate  = new RooDataHist(  ("sig_"+name).c_str(), 
                                        "Signal", 
                                        *m, 
                                        Import(*(rdsSigTemplate->createHistogram(m->GetName(), NbinsPdf)))
                                        );
    rdhBkgTemplate  = new RooDataHist(  ("bkg_"+name).c_str(), 
                                        "Background", 
                                        *m, 
                                        Import(*(rdsBkgTemplate->createHistogram(m->GetName(), NbinsPdf)))
                                        );
    rdhPlotData     = new RooDataHist(  ("plotdata_"+name).c_str(), 
                                        "Data (plot)", 
                                        *m, 
                                        Import(*(rdsData->createHistogram(m->GetName(), 60)))
                                        );
    
    rho_sig = 1./(2.*sqrt((rdsData->createHistogram(m->GetName(), NbinsFit))->GetStdDev())); //0.3162;   // 1/(2*sqrt(sigma))
    rho_bkg = 0.2887;   // 1/sqrt(12)
    
    bData = kTRUE;
    bTemplates = kTRUE;
    
    return;
}

void TTemplateFit::addData(RooDataSet* data, RooDataSet* sig, RooDataSet* bkg)
{
    // Use this to add data as binned or unbinned RooDataSets.
    // Note: if the RooDataSets are binned, it might occur some problems
    //       when the binned RooDataHist are created below if the 
    //       number of requested bins is below the number of bins
    //       included in the RooDataSet. Not tested yet.
    
    rdsData = new RooDataSet(*data, ("data_unbinned_"+name).c_str());
    rdsSigTemplate = new RooDataSet(*sig, ("sig_unbinned_"+name).c_str());
    rdsBkgTemplate = new RooDataSet(*bkg, ("bkg_unbinned_"+name).c_str());
    
    rdsData->SetTitle("Data");
    rdsSigTemplate->SetTitle("Signal");
    rdsBkgTemplate->SetTitle("Background");
    
    Ntot = data->numEntries();
    
    // set values for signal and background 
    Nsig = new RooRealVar(  ("Nsig_"+name).c_str(), 
                            "Signal events", 
                            0.8*Ntot, 0.4*Ntot, Ntot);
    Nbkg = new RooRealVar(  ("Nbkg_"+name).c_str(), 
                            "Background events", 
                            0.2*Ntot, 0.01*Ntot, 0.6*Ntot);
    
    // create binned RooDataHist in variable name
    // - rdhData is used for the fit
    // - binned templates are used in case of histogram based pdf (only for signal)
    rdhData         = new RooDataHist(  ("data_"+name).c_str(), 
                                        "Data", 
                                        *m, 
                                        Import(*(rdsData->createHistogram(m->GetName(), NbinsFit)))
                                        );
    rdhSigTemplate  = new RooDataHist(  ("sig_"+name).c_str(), 
                                        "Signal", 
                                        *m, 
                                        Import(*(rdsSigTemplate->createHistogram(m->GetName(), NbinsPdf)))
                                        );
    rdhBkgTemplate  = new RooDataHist(  ("bkg_"+name).c_str(), 
                                        "Background", 
                                        *m, 
                                        Import(*(rdsBkgTemplate->createHistogram(m->GetName(), NbinsPdf)))
                                        );
    rdhPlotData     = new RooDataHist(  ("plotdata_"+name).c_str(), 
                                        "Data (plot)", 
                                        *m, 
                                        Import(*(rdsData->createHistogram(m->GetName(), 120)))
                                        );
    
    rho_sig = 1./(2.*sqrt((rdsData->createHistogram(m->GetName(), NbinsFit))->GetStdDev())); //0.3162;   // 1/(2*sqrt(sigma))
    rho_bkg = 0.2887;   // 1/sqrt(12)
    
    
    if(Ntot<250){
        bUnbinnedFit = kTRUE;
    }
    
    bData = kTRUE;
    bTemplates = kTRUE;
    
    return;
}

void TTemplateFit::setGaus( Float_t mean,   Float_t mean_low,   Float_t mean_high,
                            Float_t width,  Float_t width_low,  Float_t width_high)
{
    // Use this to change the initial values for the gaussian which is used
    // to convolve it with the signal p.d.f.
    gmean = mean;
    gmean_low = mean_low;
    gmean_high = mean_high;
    
    gwidth = width;
    gwidth_low = width_low;
    gwidth_high = width_high;
    
    //~ setVal (Double_t value)
    //~ setRange (Double_t min, Double_t max)
    //~ SetTitle (const char *title="")
    //~ SetName (const char *name)
    //~ SetNameTitle (const char *name, const char *title)
    
    return ;
}

void TTemplateFit::buildPdf(string method, string fit)
{
    // method = "kernel" --> signal kernel estimation
    // method = "hist"   --> signal histo based estimation
    cout << "TTemplateFit::fit(string method) --> method = " << method << "\t--> ";
    
    if(!(bData && bTemplates)){
        cout << "No data or templates added! Use addData(), addTemplates() or addFiles() for both." << endl;
        return;
    }
    
    if(!method.compare("kernel")){
        cout << "kernel p.d.f. estimation!" << endl;
        kernelFit();
        constructModel(kestSig);
    } else if (!method.compare("hist")){
        cout << "histogram based p.d.f. estimation!" << endl;
        histoFit(histOrder);
        constructModel(histSig);
    } else{
        cout << "No method given to construct the signal pdf from signal template. Choose \"kernel\" or \"hist\"." << endl;
        return;
    }
    
    //getchar();
    bRawFrame = kTRUE;
    //drawRaw();
    //getchar();
    
    if(!fit.compare("fit")){
        cout << "start fitting the model..." << endl;
        fitModel();
    }
    
    buildFrame();
    
    return;
}

void TTemplateFit::kernelFit()
{
    cout << "TTemplateFit::kernelFit()" << endl;
    // fit signal plus background using a gaussian adaptive kernel estimator
    // to form the signal p.d.f.
    kestSig = new RooKeysPdf(   ("kestSig_"+name).c_str(), 
                                "Kernel estimated signal p.d.f.",
                                *m,
                                //*((RooDataSet*) rdhSigTemplate),
                                *rdsSigTemplate,
                                RooKeysPdf::MirrorBoth,
                                rho_sig);
    
    return ;
}

void TTemplateFit::histoFit(Int_t order)
{
    cout << "TTemplateFit::histoFit(" << order << ")" << endl;
    // fit signal plus background using a histogram based interpolation of order "order"
    // to form the signal p.d.f.
    histSig = new RooHistPdf(   ("histSig_"+name).c_str(),
                                "Histogram based signal p.d.f.",
                                *m,
                                //*rdhSigTemplate,
                                *rdhSigTemplate,
                                order);
    
    return ;
}

void TTemplateFit::constructModel(RooAbsPdf* sig)
{
    cout << "TTemplateFit::constructModel()" << endl;
    
    gm = new RooRealVar(("gm_"+name).c_str(), "Gaussian mean", gmean, gmean_low, gmean_high);
    gs = new RooRealVar(("gs_"+name).c_str(), "Gaussian width", gwidth, gwidth_low, gwidth_high);
    
    kestBkg = new RooKeysPdf(   ("kestBkg_"+name).c_str(), 
                                "Kernel estimated background p.d.f.",
                                *m,
                                //*((RooDataSet*) rdhBkgTemplate),
                                *rdsBkgTemplate,
                                RooKeysPdf::MirrorBoth,
                                rho_bkg);
    
    gaus        = new RooGaussian(("gaus_"+name).c_str(), "Gaussian smearing", *m, *gm, *gs);
    smearedSig  = new RooNumConvPdf(("smearedSig_"+name).c_str(), "Smeared Z Signal", *m, *gaus, *sig);
    
    model       = new RooAddPdf(    ("model_"+name).c_str(), 
                                    "Signal + background", 
                                    RooArgList(*kestBkg, *smearedSig), 
                                    RooArgList(*Nbkg, *Nsig));
    
    return ;
}

void TTemplateFit::fitModel()
{
    cout << "TTemplateFit::fitModel()" << endl;
    if(!(bData && bTemplates)){
        cout << "No data added!" << endl;
        return;
    }
    
    // fitResult = model->fitTo(*rdhData, NumCPU(4), Timer(kTRUE), Save());
    //
    // FitOptions(const char* optStr)
    // Options: see 
    // https://root.cern.ch/root/htmldoc/guides/users-guide/FittingHistograms.html#the-th1fit-method
    if(bUnbinnedFit && rdsData != NULL){
        fitResult = model->fitTo(*rdsData, FitOptions("I"), Extended(kTRUE), Timer(kTRUE), Save());
    }
    else{
        fitResult = model->fitTo(*rdhData, FitOptions("I"), Extended(kTRUE), Timer(kTRUE), Save());
    }
    
    
    bFitted = kTRUE;
    return ;
}

void TTemplateFit::buildFrame()
{
    cout << "TTemplateFit::buildFrame()" << endl;
    // 
    frame = m->frame(Title(name.c_str()), Name((name+"_data").c_str()));
    fpull = m->frame(Title((name+" pull distribution").c_str()), Name((name+"_pull").c_str()));
    
    rdhPlotData->plotOn(frame, Name(("data_"+name).c_str()));
    
    model->plotOn(  frame,
                    Name(("model_"+name).c_str()), 
                    LineColor(kBlue));
    
    model->paramOn(frame, Layout(0.6, 0.9, 0.9));
    frame->getAttText()->SetTextSize(0.025) ; 
    
    // pullHist (const char *histname=0, const char *pdfname=0, bool useAverage=false) const 
    hpull = frame->pullHist(("data_"+name).c_str(), ("model_"+name).c_str());
    fpull->addPlotable(hpull, "P");
    
    model->plotOn(  frame, 
                    Name(("bkg_"+name).c_str()), 
                    Components(("kestBkg_"+name).c_str()),
                    LineColor(kBlue), 
                    LineStyle(kDashed));
    
    bFrameBuild = kTRUE;
    
    return ;
}

void TTemplateFit::drawFrame(TFile *f, string fSubfolder)
{
    string folder;
    folder = fSubfolder + "/" + name;
    
    cout << "TTemplateFit::drawFrame()" << endl;
    
    if(f == NULL || f->IsZombie()){
        cout << "TTemplateFit::drawFrame(TFile *f) received an uninitialized TFile pointer, plot won't be saved!" << endl;
    }
    
    if(!bFrameBuild){
        cout << "Cannot draw frame before building it using buildFrame()." << endl;
        return ;
    }
    
    Int_t   csize   = 600,
            cheight = 600;
    TCanvas* c = new TCanvas(("c_"+name).c_str(), ("Canvas "+name).c_str(), csize, cheight);
    
    TPad* pFit = new TPad(("pFit_"+name).c_str(), "pFit", 0., 0.3, 1., 1.);
    pFit->SetBottomMargin(0.1);
    pFit->SetLeftMargin(0.16);
    pFit->Draw();
    pFit->cd();
    
    frame->GetYaxis()->SetTitleOffset(1.6);
    frame->Draw();
    
    pFit->Update();
    
    c->cd();
    
    TPad* pPull = new TPad(("pPull_"+name).c_str(), "pPull", 0., 0., 1., 0.3);
    pPull->SetTopMargin(0);
    pPull->SetBottomMargin(0.15);
    pPull->SetGridy();
    pPull->SetLeftMargin(0.16);
    pPull->Draw();
    pPull->cd();
    
    fpull->GetYaxis()->SetTitle("#frac{data-fit}{#sigma_{data}}");
    fpull->GetYaxis()->SetTitleOffset(0.5);
    fpull->GetYaxis()->SetTitleSize(0.11);
    fpull->GetYaxis()->SetNdivisions(5);
    fpull->GetYaxis()->SetLabelSize(0.08);
    fpull->GetXaxis()->SetTitle("");
    fpull->GetXaxis()->SetLabelSize(0.08);
    //fpull->GetXaxis()->SetRangeUser(60., 120.);
    fpull->SetTitle("");
    fpull->Draw();
    
    pPull->Update();
    
    c->Update();
    
    if(f == NULL || f->IsZombie()){
        return;
    }
    
    f->cd();
    
    if(!f->cd( (folder).c_str() ))
    {
        cout << "Create dir in tree... " << endl;
        f->mkdir( (folder).c_str() );
    }
    
    f->cd( (folder).c_str() );
    
    c->Write("canvas", TObject::kOverwrite);
    return;
}

void TTemplateFit::drawRaw(TFile *f, string fSubfolder)
{
    string folder;
    folder = fSubfolder + "/" + name;
    
    cout << "TTemplateFit::drawRaw()" << endl;
    
    if(f == NULL || f->IsZombie()){
        cout << "TTemplateFit::drawRaw(TFile *f) received an uninitialized TFile pointer, plot won't be saved!" << endl;
    }
    
    if(!bRawFrame){
        cout << "Cannot draw raw frame, some parts are missing." << endl;
        return ;
    }
    
    RooPlot *frameSig = m->frame(Title("Signal Template"));
    RooPlot *frameBkg = m->frame(Title("Background Template"));
    RooPlot *frameDat = m->frame(Title("Data"));
    
    rdhSigTemplate->plotOn(frameSig, MarkerColor(kRed));
    //rdsSigTemplate->plotOn(frameSig, MarkerColor(kBlue));
    
    rdsBkgTemplate->plotOn(frameBkg, MarkerColor(kBlue));
    rdsData->plotOn(frameDat, MarkerColor(kBlue));
    
    
    histSig->plotOn(frameSig, LineColor(kBlue), LineStyle(kDashed));
    //smearedSig->plotOn(frameSig, LineColor(kBlack));
    kestBkg->plotOn(frameBkg, LineColor(kBlack));
    model->plotOn(frameDat, LineColor(kBlack));
    model->plotOn(frameDat, Components(("kestBkg_"+name).c_str()), LineColor(kBlack), LineStyle(kDashed));
    
    
    
    TCanvas* c_raw = new TCanvas(("c_raw"+name).c_str(), "Raw plot", 1500, 500);
    c_raw->Divide(3, 1);
    c_raw->cd(1);
    frameBkg->Draw();
    
    
    c_raw->cd(2);
    frameSig->Draw();
    
    c_raw->cd(3);
    frameDat->Draw();
    
    c_raw->cd(4);
    
    c_raw->Update();
    
    // save
    if(f == NULL || f->IsZombie()){
        return ;
    }
    
    f->cd();
    
    if(!f->cd( (folder).c_str() ))
    {
        cout << "Create dir in tree... " << endl;
        f->mkdir( (folder).c_str() );
    }
    
    f->cd( (folder).c_str() );
    
    frameSig->Write("raw_sig", TObject::kOverwrite);
    frameBkg->Write("raw_bkg", TObject::kOverwrite);
    frameDat->Write("raw_dat", TObject::kOverwrite);
    c_raw->Write("raw_canvas", TObject::kOverwrite);
    
    
    return;
}

void TTemplateFit::save(TFile *f, string fSubfolder)
{
    string folder;
    folder = fSubfolder + "/" + name;
    
    if(f == NULL || f->IsZombie())
    {
        cout << "TTemplateFit::save(TFile *f) needs an initialized TFile pointer!" << endl;
        return ;
    }
    
    f->cd();
    
    if(!f->cd( (folder).c_str() ))
    {
        cout << "Create dir in tree... " << endl;
        f->mkdir( (folder).c_str() );
    }
    
    f->cd( (folder).c_str() );
    
    if(!bFrameBuild){
        cout << "Warning while calling " << name <<".save(): no frames available!" << endl;
    }
    if(!bFitted){
        cout << "Warning while calling " << name <<".save(): no fitresult available!" << endl;
    }
    
    // plots
    if(frame != NULL) frame->Write(     //frame->GetName(), 
                                        "frame", 
                                        TObject::kOverwrite);
    if(fpull != NULL) fpull->Write(     //fpull->GetName(), 
                                        "pullframe", 
                                        TObject::kOverwrite);
    // model
    if(model != NULL) model->Write(     "model",
                                        TObject::kOverwrite);
    // fit result
    if(fitResult != NULL) fitResult->Write( //(name+"_fitResult").c_str(), 
                                            "fitresult", 
                                            TObject::kOverwrite);
    
    cout << "... results from \"" << name << "\" written to " << f->GetName() << endl;
    return ;
}












