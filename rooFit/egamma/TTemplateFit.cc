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
    sType("data"),
    bData(kFALSE),
    bTemplates(kFALSE),
    bFrameBuild(kFALSE),
    bRawFrame(kFALSE),
    bFitted(kFALSE),
    bUnbinnedFit(kFALSE),
    bConvolveSignal(kTRUE),
    rho_sig(0.3162), // redefined when data are added
    rho_bkg(0.2887), // 1/sqrt(12))
    gmean(0.),
    gmean_low(-5.),
    gmean_high(5.),
    gwidth(1.),
    gwidth_low(0.001),
    gwidth_high(5.),
    histOrder(3),
    NbinsPlot(60)
{
    
    Float_t     low,
                up;
    
    //m = x;
    m = new RooRealVar(*x);
    
    low = m->getMin();
    up  = m->getMax();
    
    // initial values for the binning
    NbinsPdf = Int_t((up-low));     // for pdf modeling
    
    NbinsFit = Int_t((up-low)*4);   // fine range for the fit << OBSOLETE
    
    
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

void TTemplateFit::addData(TH1F* data, TTree* sig, TTree* bkg)
{
    // Use this to add data as ROOT::TH1 and templates as ROOT::TTree.
    
    rdhData = new RooDataHist(("data_"+name).c_str(), "Data", *m, Import(*data));
    //rdsData = (RooDataSet*) rdhData;
    rdsSigTemplate = new RooDataSet(("sig_unbinned_"+name).c_str(), "Signal", *m, Import(*sig));
    rdsBkgTemplate = new RooDataSet(("bkg_unbinned_"+name).c_str(), "Background", *m, Import(*bkg));
    
    Ntot = Int_t(data->Integral()); // total number of events in the data
    cout << "TTemplateFit::addData(TH1F* data)\t Ntot = " << Ntot << endl;
    
    // set values for signal and background 
    Nsig = new RooRealVar(  ("Nsig_"+name).c_str(), 
                            "Signal events", 
                            0.8*Ntot, 0.4*Ntot, Ntot);
    Nbkg = new RooRealVar(  ("Nbkg_"+name).c_str(), 
                            "Background events", 
                            0.2*Ntot, 0.01*Ntot, 0.6*Ntot);
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
    
    rdhPlotData     = new RooDataHist(  *rdhData,
                                        ("plotdata_"+name).c_str() 
                                        );
    rdhPlotData->SetTitle("Data (plot)");
    
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
                                        Import(*(rdsData->createHistogram(m->GetName(), NbinsPlot)))
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
    
    //~ // The answer is 42!
    
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
                                        Import(*(rdsData->createHistogram(m->GetName(), NbinsPlot)))
                                        );
    
    rho_sig = 1./(2.*sqrt((rdsData->createHistogram(m->GetName(), NbinsFit))->GetStdDev())); //0.3162;   // 1/(2*sqrt(sigma))
    rho_bkg = 0.2887;   // 1/sqrt(12)
    
    
    if(Ntot<250){
        bUnbinnedFit = kTRUE;
    }
    
    bData = kTRUE;
    bTemplates = kTRUE;
    
    return ;
}

void TTemplateFit::addData(RooDataSet* data, TTree* sig, TTree* bkg)
{
    // Use this to add data as binned or unbinned RooDataSet and the templates
    // as TTrees.
    // Binning can changed however you like it.
    // The unbinned data are _not_ used in the fit; instead it is recommended
    // to use a fine binning. The p.d.f. construction from the templates
    // is done by the unbinned trees in case of kernel density estimation
    // or the binned histograms in case of histogram based interpolation.
    
    rdsData = new RooDataSet(*data, ("data_unbinned_"+name).c_str());
    rdsSigTemplate = new RooDataSet(("sig_unbinned_"+name).c_str(), "Signal", *m, Import(*sig));
    rdsBkgTemplate = new RooDataSet(("bkg_unbinned_"+name).c_str(), "Background", *m, Import(*bkg));
    
    rdsData->SetTitle("Data");
    
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
                                        Import(*(rdsData->createHistogram(m->GetName(), NbinsPlot)))
                                        );
    
    rho_sig = 1./(2.*sqrt((rdsData->createHistogram(m->GetName(), NbinsFit))->GetStdDev())); //0.3162;   // 1/(2*sqrt(sigma))
    rho_bkg = 0.2887;   // 1/sqrt(12)
    
    bData = kTRUE;
    bTemplates = kTRUE;
    
    return;
}

void TTemplateFit::fitToPdf(RooAbsPdf *pdf, TFile* f, string fSubfolder, string sFit, string sPlotStyle)
{
    // Use this to fit any given pdf to the data.
    // The results are saved in f/fSubfolder/name/pdfFit/
    // and contains:
    //  frame/      --> RooPlot frame
    //  pullframe/  --> RooPlot frame of the pull distribution
    //  canvas/     --> plot as canvas
    //  fitresult/  --> RooFitResult
    //  modelpdf/   --> the RooAbsPdf after the fit
    cout << "TTemplateFit::fitToPdf(RooAbsPdf *pdf, TFile* f, string fSubfolder, string sPlotStyle)" << endl;
    
    string folder;
    folder = fSubfolder + "/" + name + "/pdfFit";
    cout << "folder = " << folder << endl;
    
    
    if(f == NULL || f->IsZombie())
    {
        cout << "TTemplateFit::fitToPdf(RooAbsPdf* pdf, TFile *f, string fSubfolder) needs an initialized TFile pointer!" << endl;
        return ;
    }
    
    if(pdf == NULL || pdf->IsZombie())
    {
        cout << "TTemplateFit::fitToPdf(RooAbsPdf* pdf, TFile *f, string fSubfolder) needs an initialized RooAbsPdf pointer!" << endl;
        return ;
    }
    
    // set the signal/background fractions for numerator or denumerator
    
    // RooArgSet* params = model->getVariables() ;
    // params->setRealValue(“c0”,5.3) ;
    // parametrizedModel->getVariables()->setRealValue("", 80);
    //~ Double_t frac;
    //~ if(name.find("num") != string::npos){
        //~ frac = 0.338;
        //~ ((RooRealVar*)pdf->getVariables()->find("nbkg"))->setVal(frac*Ntot);
        //~ ((RooRealVar*)pdf->getVariables()->find("nbkg"))->setMin(frac*Ntot*0.9);
        //~ ((RooRealVar*)pdf->getVariables()->find("nbkg"))->setMax(frac*Ntot*1.1);
        //~ ((RooRealVar*)pdf->getVariables()->find("nsig"))->setVal((1-frac)*Ntot);
        //~ ((RooRealVar*)pdf->getVariables()->find("nsig"))->setMin((1-frac)*Ntot*0.9);
        //~ ((RooRealVar*)pdf->getVariables()->find("nsig"))->setMax((1-frac)*Ntot*1.1);
        //~ 
        //~ cout << "This is the numerator!" << endl;
    //~ }
    //~ if(name.find("den") != string::npos){
        //~ frac = 0.022;
        //~ ((RooRealVar*)pdf->getVariables()->find("nbkg"))->setVal(frac*Ntot);
        //~ ((RooRealVar*)pdf->getVariables()->find("nbkg"))->setMin(frac*Ntot*0.9);
        //~ ((RooRealVar*)pdf->getVariables()->find("nbkg"))->setMax(frac*Ntot*1.1);
        //~ ((RooRealVar*)pdf->getVariables()->find("nsig"))->setVal((1-frac)*Ntot);
        //~ ((RooRealVar*)pdf->getVariables()->find("nsig"))->setMin((1-frac)*Ntot*0.9);
        //~ ((RooRealVar*)pdf->getVariables()->find("nsig"))->setMax((1-frac)*Ntot*1.1);
        //~ 
        //~ cout << "THis is the denominator!" << endl;
    //~ }
    
    
    // is pdf and file are valid, do a fit
    RooFitResult *res = NULL;
    
    if(!sFit.compare("fit")){
        if(bUnbinnedFit && rdsData != NULL){
            res = pdf->fitTo(*rdsData, Extended(kTRUE), Timer(kTRUE), Save(kTRUE));
        } else{
            res = pdf->fitTo(*rdhData, Extended(kTRUE), Timer(kTRUE), Save(kTRUE));
        }
    } else{
        cout << "TTemplateFit::fitToPdf() - no Fit will be performed, only draw initial pdf." << endl;
    }
    
    // now create some result objects
    RooPlot *rooframe = NULL;
    RooPlot *roopull  = NULL;
    
    // The plot is done in the following way:
    // The full pdf is plotted and the pulls correspond to this,
    // and in addition if a sPlotStyle string corresponding to a component is given
    // additionally plot this in the same frame using a dashed line.
    rooframe = m->frame(Title((name+"_pdf").c_str()), Name((name+"_data_pdf").c_str()));
    roopull  = m->frame(Title((name+"_pull").c_str()), Name((name+"_pull_pdf").c_str()));
    
    // first: plot the data and fitted full pdf
    rdhPlotData->plotOn(    rooframe, 
                            Name(("data_"+name).c_str())
                            );
    pdf->plotOn(    rooframe,
                    Name(("model_"+name).c_str()), 
                    LineColor(kBlue));
    
    if(!sPlotStyle.compare("")){
        // do nothing
        (void)0;
    } else if(!sPlotStyle.compare("bkg")){
        pdf->plotOn(    rooframe,
                        Name(("bkg_"+name).c_str()),
                        Components("bkg"),
                        LineColor(kBlue),
                        LineStyle(kDashed));
    } else {
        cout << "No plotstyle given. Only the full model is plotted." << endl;
    }
    
    pdf->paramOn(rooframe, Layout(0.6, 0.9, 0.9));
    
    // create pull distribution
    RooHist *_pull = getPullHist(rooframe);
    roopull->addPlotable(_pull, "P");
    
    
    // create a canvas
    //TCanvas *ct = new TCanvas("ct", "pdf fit canvas", 600, 600);
    
    
    
    
    
    // save the results
    f->cd();
    if(!f->cd( (folder).c_str() ))
    {
        cout << "Create dir in tree... " << endl;
        f->mkdir( (folder).c_str() );
    }
    
    f->cd( (folder).c_str() );
    
    // plots
    if(rooframe != NULL)    rooframe->Write(   //frame->GetName(), 
                                            "frame", 
                                            TObject::kOverwrite);
    if(roopull != NULL)     roopull->Write(   //fpull->GetName(), 
                                            "pullframe", 
                                            TObject::kOverwrite);
    // save fitted model
    if(pdf != NULL)         pdf->Write(   "model",
                                            TObject::kOverwrite);
    // fit result
    if(res != NULL)         res->Write( //(name+"_fitResult").c_str(), 
                                            "fitresult", 
                                            TObject::kOverwrite);
    
    return ;
}

void TTemplateFit::scaleBackgroundTemplate(TTree* mcBkg, TTree* mcBkgTrue, TFile* f, string fSubfolder)
{
    // Note: the intentional idea for this method was to 
    // rescale the background template in data using the differences 
    // in the monte carlo background template to the monte carlo truth template.
    // 
    // It turned out that this rescaling gives no physical resonable result since
    // 
    // 
    
    string folder;
    folder = fSubfolder + "/" + name;
    cout << "folder = " << folder << endl;
    
    if(f == NULL || f->IsZombie())
    {
        cout << "TTemplateFit::scaleBackground(TTree* mcBkg, TTree* mcBkgTrue, TFile* f, string fSubfolder) needs an initialized TFile pointer!" << endl;
        return ;
    }
    
    Int_t NbinsBkgScaling(60);
    
    rdsBkgTemplateMC     = new RooDataSet(("bkg_mc_"+name).c_str(), "MC bkg template", *m, Import(*mcBkg));
    rdsBkgTemplateMCtrue = new RooDataSet(("bkg_mc_true_"+name).c_str(), "MC bkg true template", *m, Import(*mcBkgTrue));
    
    TH1F* hBkgMc     = (TH1F*)(rdsBkgTemplateMC->createHistogram(m->GetName(), NbinsBkgScaling));
    TH1F* hBkgMcTrue = (TH1F*)(rdsBkgTemplateMCtrue->createHistogram(m->GetName(), NbinsBkgScaling));
    
    // now scale hBkgMc to hBkgMcTrue
    hBkgMc->Scale(hBkgMcTrue->Integral()/hBkgMc->Integral());
    
    // create data background histogram
    TH1F* hBkg      = (TH1F*)(rdsBkgTemplate->createHistogram(m->GetName(), NbinsBkgScaling));
    hBkg->Scale(hBkgMc->Integral()/hBkg->Integral());
    
    TH1F* hBkgScaled = (TH1F*)(rdsBkgTemplate->createHistogram(m->GetName(), NbinsBkgScaling));
    hBkgScaled->Scale(hBkgMc->Integral()/hBkgScaled->Integral());
    
    hBkg->Sumw2();
    hBkgScaled->Sumw2();
    
    // scale every bin with the bin difference factor of the mc bkg template to the true mc bkg template
    Int_t binmin = hBkg->GetMinimumBin();
    Int_t binmax = hBkg->GetMaximumBin();
    Int_t Nbins  = hBkg->GetSize() - 2;
    
    Double_t scalefactor(0);
    Double_t val(0);
    Double_t x(0);
    
    for(int i=1; i<Nbins+1; i++){
        val = hBkg->GetBinContent(i);
        x   = hBkg->GetBinCenter(i);
        scalefactor = hBkgMcTrue->GetBinContent(i)/hBkgMc->GetBinContent(i);
        hBkgScaled->SetBinContent(i, val*scalefactor);
        
        cout << "i = " << i << "\t x = " << x << "\t scalefactor = " << scalefactor << endl;
    }
    
    
    rdhBkgTemplateMC     = new RooDataHist(  ("bkg_mc_"+name).c_str(), 
                                        "Binned MK bkg template", 
                                        *m, 
                                        Import(*hBkgMc)
                                        );
    rdhBkgTemplateMCtrue = new RooDataHist(  ("bkg_mc_true_"+name).c_str(), 
                                        "Binned true MK bkg template", 
                                        *m, 
                                        Import(*hBkgMcTrue)
                                        );
    rdhBkgTemplateUnscaled  = new RooDataHist(  ("bkg_mc_unscaled_"+name).c_str(), 
                                        "Binned bkg template unscaled", 
                                        *m, 
                                        Import(*hBkg)
                                        );
    rdhBkgTemplateScaled  = new RooDataHist(  ("bkg_mc_scaled_"+name).c_str(), 
                                        "Binned bkg template scaled", 
                                        *m, 
                                        Import(*hBkgScaled)
                                        );
    
    // finally redefine the rdsBkgTemplate using the created scaled histogram
    rdsBkgTemplate = (RooDataSet*)rdhBkgTemplateScaled;
    //new RooDataSet(("bkg_unbinned_"+name).c_str(), "Background", *m, Import(*(RooDataSet*)rdhBkgTemplateScaled));
    
    RooDataSet* rdsBkgTemplateUnscaled = (RooDataSet*)rdhBkgTemplateUnscaled;
    //new RooDataSet(("bkg_unbinned_unscaled_"+name).c_str(), "Background unscaled", *m, Import(*hBkg));
    
    // some plots
    RooPlot *f1 = m->frame(Title((name+"_bkg_scaling").c_str()), Name((name+"_bkg_scaling").c_str()));
    RooPlot *f2 = m->frame(Title((name+"_pdf_comparison").c_str()), Name((name+"_pdf_comparison").c_str()));
    
    
    
    // create kernel pdf's (for comparison plot only)
    RooAbsPdf* pdfUnscaled = new RooKeysPdf(   ("kestBkgUnscaled_"+name).c_str(), 
                                "Kernel estimated background p.d.f.",
                                *m,
                                //*((RooDataSet*) rdhBkgTemplate),
                                *(RooDataSet*)(rdhBkgTemplateMC),
                                RooKeysPdf::MirrorBoth,
                                rho_bkg);
    RooAbsPdf* pdfScaled = new RooKeysPdf(   ("kestBkgScaled_"+name).c_str(), 
                                "True Kernel estimated background p.d.f.",
                                *m,
                                //*((RooDataSet*) rdhBkgTemplate),
                                *(RooDataSet*)(rdhBkgTemplateMCtrue),
                                RooKeysPdf::MirrorBoth,
                                rho_bkg);
    
    rdhBkgTemplateMC->plotOn(f1, MarkerColor(kRed));
    rdhBkgTemplateMCtrue->plotOn(f1, MarkerColor(kBlue));
    pdfUnscaled->plotOn(f1, LineColor(kRed));
    pdfScaled->plotOn(f1, LineColor(kBlue));
    
    //rdsBkgTemplateUnscaled->plotOn(f1, MarkerColor(kBlack));
    //rdsBkgTemplate->plotOn(f1, MarkerColor(kMagenta));
    
    rdhBkgTemplate->plotOn(f2, MarkerColor(kBlack));
    pdfUnscaled->plotOn(f2, LineColor(kRed));
    pdfScaled->plotOn(f2, LineColor(kBlue));
    
    // save stuff
    
    f->cd();
    
    if(!f->cd( (folder).c_str() ))
    {
        cout << "Create dir in tree... " << endl;
        f->mkdir( (folder).c_str() );
    }
    
    f->cd( (folder).c_str() );
    
    if(f1 != NULL)       f1->Write(     //frame->GetName(), 
                                        "scalingframe", 
                                        TObject::kOverwrite);
    if(f2 != NULL)       f2->Write(     //frame->GetName(), 
                                        "comparison", 
                                        TObject::kOverwrite);
    
    return ;
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
        cout << "No data or templates added! Use addData()." << endl;
        return;
    }
    
    if(!method.compare("kernel")){
        cout << "kernel p.d.f. estimation!" << endl;
        kernelFit();
    } else if (!method.compare("hist")){
        cout << "histogram based p.d.f. estimation!" << endl;
        histoFit(histOrder);
    } else{
        cout << "No method given to construct the signal pdf from signal template. Choose \"kernel\" or \"hist\"." << endl;
        return;
    }
    
    constructModel();
    
    bRawFrame = kTRUE;
    
    if(!fit.compare("fit")){
        cout << "start fitting the model..." << endl;
        fitModel();
    }
    
    buildFrame();
    
    return ;
}

void TTemplateFit::kernelFit()
{
    cout << "TTemplateFit::kernelFit()" << endl;
    // fit signal plus background using a gaussian adaptive kernel estimator
    // to form the signal p.d.f.
    pdfSig  = new RooKeysPdf(   ("kestSig_"+name).c_str(), 
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
    pdfSig  = new RooHistPdf(   ("histSig_"+name).c_str(),
                                "Histogram based signal p.d.f.",
                                *m,
                                //*rdhSigTemplate,
                                *rdhSigTemplate,
                                order);
    
    return ;
}

void TTemplateFit::constructModel()
{
    cout << "TTemplateFit::constructModel()" << endl;
    
    // setPlotLabel (const char *label)
    gm = new RooRealVar(("gm_"+name).c_str(), "Gaussian mean", gmean, gmean_low, gmean_high);
    gs = new RooRealVar(("gs_"+name).c_str(), "Gaussian width", gwidth, gwidth_low, gwidth_high);
    
    pdfBkg  = new RooKeysPdf(   ("kestBkg_"+name).c_str(), 
                                "Kernel estimated background p.d.f.",
                                *m,
                                //*((RooDataSet*) rdhBkgTemplate),
                                *rdsBkgTemplate,
                                RooKeysPdf::MirrorBoth,
                                rho_bkg);
    
    gaus        = new RooGaussian(("gaus_"+name).c_str(), "Gaussian smearing", *m, *gm, *gs);
    
    m->setBins(10000, "cache"); // bins for fft sampling
    smearedSig  = new RooFFTConvPdf(("smearedSig_"+name).c_str(), "Smeared Z Signal", *m, *pdfSig, *gaus);
    
    
    
    if(bConvolveSignal){
        model       = new RooAddPdf(    ("model_"+name).c_str(), 
                                        "Signal + background", 
                                        RooArgList(*pdfBkg, *smearedSig), 
                                        RooArgList(*Nbkg, *Nsig));
    }
    
    // model without gaus
    if(!bConvolveSignal){
        model       = new RooAddPdf(    ("model_"+name).c_str(), 
                                        "Signal + background", 
                                        RooArgList(*pdfBkg, *pdfSig), 
                                        RooArgList(*Nbkg, *Nsig));
    }
    
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
    
    if(!sType.compare("data")){
        if(bUnbinnedFit && rdsData != NULL){
            fitResult = model->fitTo(*rdsData, Extended(kTRUE), Timer(kTRUE), Save(kTRUE));
        }
        else{
            fitResult = model->fitTo(*rdhData, Extended(kTRUE), Timer(kTRUE), Save(kTRUE));
        }
    } else if(!sType.compare("mc")){
        // in case of monte carlo weights, the errors have to be corrected, which is in first order
        // done by the covariance matrix 
        //      V' = V C-1 V
        // where V is the covariance matrix estimed by the likelihood with the weights
        // as factorial per logarithm: -logL = - sum [ w_i log f(x_i) ] 
        // while C is estimated as the covariance matrix from
        // squared weights by  -logL' = -sum [ w_i^2 log f(x_i) ]
        //
        if(bUnbinnedFit && rdsData != NULL){
            fitResult = model->fitTo(*rdsData, Extended(kTRUE), Timer(kTRUE), Save(kTRUE), SumW2Error(kTRUE));
        }
        else{
            fitResult = model->fitTo(*rdhData, Extended(kTRUE), Timer(kTRUE), Save(kTRUE), SumW2Error(kTRUE));
        }
    } else{
        cout << "No valid data type. Use setType(string type) for type=\"data\" or type=\"mc\"." << endl;
        return;
    }
    
    bFitted = kTRUE;
    return ;
}

void TTemplateFit::buildFrame()
{
    cout << "TTemplateFit::buildFrame()" << endl;
    
    // plot labels for the variables
    gm->setPlotLabel("gausMean");
    gs->setPlotLabel("gausWidth");
    Nsig->setPlotLabel("Nsig");
    Nbkg->setPlotLabel("Nbkg");
    
    frame = m->frame(Title(name.c_str()), Name((name+"_data").c_str()));
    fpull = m->frame(Title((name+" pull distribution").c_str()), Name((name+"_pull").c_str()));
    
    rdhPlotData->plotOn(frame, Name(("data_"+name).c_str()));
    
    model->plotOn(  frame,
                    Name(("model_"+name).c_str()), 
                    LineColor(kBlue));
    
    model->paramOn(frame, Layout(0.6, 0.9, 0.9));
    frame->getAttText()->SetTextSize(0.025) ; 
    
    // pullHist (const char *histname=0, const char *pdfname=0, bool useAverage=false) const 
    // hpull = frame->pullHist(("data_"+name).c_str(), ("model_"+name).c_str());
    
    hpull = getPullHist(frame); // own pull creator using integral per bin
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
    return ;
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
    rdhPlotData->plotOn(frameDat, MarkerColor(kBlue));
    
    
    pdfSig->plotOn(frameSig, LineColor(kBlue), LineStyle(kDashed));
    //smearedSig->plotOn(frameSig, LineColor(kBlack));
    pdfBkg->plotOn(frameBkg, LineColor(kBlack));
    model->plotOn(frameDat, LineColor(kBlack));
    model->plotOn(frameDat, Components(("kestBkg_"+name).c_str()), LineColor(kBlack), LineStyle(kDashed));
    
    
    
    TCanvas* c_raw = new TCanvas(("c_raw_"+name).c_str(), "Raw plot", 1500, 500);
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
    
    
    return ;
}

void TTemplateFit::save(TFile *f, string fSubfolder)
{
    string folder;
    folder = fSubfolder + "/" + name;
    
    if(f == NULL || f->IsZombie())
    {
        cout << "TTemplateFit::save(TFile *f, string fSubfolder) needs an initialized TFile pointer!" << endl;
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

RooHist* TTemplateFit::getPullHist(RooPlot *_frame)
{
    // Method to calculate the residuals for the plot frame.
    // Initially ported from: https://root.cern.ch/doc/master/RooHist_8cxx_source.html#l00702
    // 
    // The goal is to create pulls where the difference f(x)-x is 
    // calculated by the integral per bin:
    // If x is the midth of the bin:
    // binLower = x-binWidth/2
    // binUpper = x+binWidth/2
    // Pull(x) = Int(f(x), x=binLower..binUpper)/(binUpper-binLower) - Entries(Bin(x))
    
    
    
    // create empty histogram
    Double_t _nominalBinWidth = 1.;
    RooHist* hist = new RooHist(_nominalBinWidth);
    // Extract the curve and the data histogram from the frame.
    // The frame has to be builed for this!
    if(_frame == NULL){
        cout << "The pull histogram is empty. Probably the frame has not been builded!" << endl;
        return hist;
    }
    RooCurve *curve  = (RooCurve*) (_frame->getCurve( ("model_"+name).c_str() ));
    RooHist  *hist1  = (RooHist*)  (_frame->getHist(  ("data_"+name).c_str()  ));
    // Copy all non-content properties from hist1
    hist->SetName(Form("pull_%s_%s", hist1->GetName(), curve->GetName())) ;
    hist->SetTitle(Form("Pull of %s and %s", hist1->GetTitle(), curve->GetTitle()));
    // Determine range of curve 
    Double_t xstart,xstop,y ;
    curve->GetPoint(0, xstart, y) ;
    curve->GetPoint(curve->GetN()-1, xstop, y) ;
    // Add histograms, calculate Poisson confidence interval on sum value
    for(Int_t i=0; i < hist1->GetN(); i++) {    
        Double_t x, point;
        hist1->GetPoint(i, x, point) ;
        // Only calculate pull for bins inside curve range
        if (x < xstart || x > xstop) continue ;
        Double_t yy;
        Double_t intValue;
        Double_t exl = hist1->GetErrorXlow(i);
        Double_t exh = hist1->GetErrorXhigh(i);
        if (exl<=0 ) exl = hist1->GetErrorX(i);
        if (exh<=0 ) exh = hist1->GetErrorX(i);
        if (exl<=0 ) exl = 0.5*hist1->getNominalBinWidth();
        if (exh<=0 ) exh = 0.5*hist1->getNominalBinWidth();
        // integrate curve over the bin and divide in by the bin width
        intValue = curve->average(x-exl, x+exh);
        yy = point - intValue;
        //~ // no average:
        //~ yy = point - curve->interpolate(x) ;
        Double_t dyl = hist1->GetErrorYlow(i) ;
        Double_t dyh = hist1->GetErrorYhigh(i) ;
        Double_t norm = (yy>0?dyl:dyh);
        if (norm == 0.) {
            yy  = 0;
            dyh = 0;
            dyl = 0;
        } else {
            yy  /= norm;
            dyh /= norm;
            dyl /= norm;
        }
        hist->addBinWithError(x, yy, dyl, dyh);
    }
    return hist;
}










