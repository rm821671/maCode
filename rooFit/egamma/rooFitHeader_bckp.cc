
#include "rooFitHeader.h"

using namespace RooFit;
using namespace std;

/**********************************************************************************************
 * setup functions
 * 
 * 
 */

template <typename T> string NumberToString ( T Number ) {
        // convert numer to string
        ostringstream ss;
        ss << Number;
        return ss.str();
    }


void SystemPath(string & filepath, string & dropbox){
    // check operating system and define the file paths
    struct utsname uts;
    uname(&uts);
    /*
    cout << "uts.sysname: " << uts.sysname << endl;
    cout << "uts.nodename: " << uts.nodename << endl;
    cout << "uts.release: " << uts.release << endl;
    cout << "uts.version: " << uts.version << endl;
    cout << "uts.machine: " << uts.machine << endl;
    */
    string path = "/user/rmeyer/mergedNTuples/"; // default
    
    string version(uts.version);
    string ubuntu = "Ubuntu";   // if the version string contains ubuntu, 
                                // its the laptop
    cout << "System Version: " << version << endl;
    
    // search for ubuntu, otherwise its scientific
    if (version.find(ubuntu) != string::npos) {
        //  cout << "ubuntu found!" << '\n';
        path = "/home/jack/";
        filepath = path + "data_uni/mergedntuples/";
    }
    else {
        //  cout << "ubuntu not found! choose scientific" << '\n';
        path = "/user/rmeyer/";
        filepath = path + "mergedNTuples/";
    }
    
    dropbox = path + "Dropbox/data_uni/";
    
} // SystemPath()

/**********************************************************************************************
 * models
 * 
 ***/

RooAbsPdf* modelEG(RooRealVar& m){    
    RooRealVar *mcb = new RooRealVar("mcb", "mean crystalball", 0.22, -1., 1.);
    RooRealVar *scb = new RooRealVar("scb", "width crystalball", 1.1, 0.9, 1.5);
    RooRealVar *alphacb = new RooRealVar("alphacb", "alpha crystalball", -3.04, -5., -2.);
    RooRealVar *ncb = new RooRealVar("ncb", "n crystalball", 0.03, 0., 0.5);    
    RooRealVar *mbw = new RooRealVar("mbw", "mean breit wigner", 90.261, 88., 93.);
    RooRealVar *sbw = new RooRealVar("sbw", "width breit wigner", 3.01, 2., 4.);    
    RooRealVar *tau = new RooRealVar("tau", "tau", -0.157, -.2, -0.1);    
    RooRealVar *con = new RooRealVar("con", "con", 1., 0., 100.);    
    RooRealVar *p0 = new RooRealVar("p0", "p0", 0.85, 0.5, 1.);
    RooRealVar *p1 = new RooRealVar("p1", "p1", 86.145, 80., 90.);
    RooRealVar *p2 = new RooRealVar("p2", "p2", 4.04, 3., 5.);
    
    RooRealVar *bkgfrac = new RooRealVar("bkgfrac", "fraction of background", 0.1, 0., 1.);
    
    RooRealVar *nbkg = new RooRealVar("nbkg", "Background events", 2000, 100, 4000);
    RooRealVar *nsig = new RooRealVar("nsig", "Signal events", 10000, 6000, 12000);
    
    // construct crystal ball
    RooCBShape *crystalball = new RooCBShape("crystalball", "Crystal Ball Shape", m, *mcb, *scb, *alphacb, *ncb);
    
    // construct breit wigner
    RooBreitWigner *breitwigner = new RooBreitWigner("breitwigner", "Breit Wigner Peak", m, *mbw, *sbw);
    //RooBreitWigner *breitwigner = new RooGaussian("breitwigner", "Breit Wigner Peak", m, *mbw, *sbw);
    
    // convoluted Signal model
    RooNumConvPdf *sig = new RooNumConvPdf("sig", "Z peak shape", m, *breitwigner, *crystalball);
    //RooGaussian *sig = *breitwigner;
        
    // error function times exponential
    RooExponential *exponential = new RooExponential("exponential", "exponential bkg", m, *tau);
    RooFormulaVar *erf = new RooFormulaVar("erf", "p0*(TMath::Erf((m-p1)/p2)+1)", RooArgList(m, *p0, *p1, *p2));
    
    RooEffProd *bkg = new RooEffProd("bkg", "background shape", *exponential, *erf);
    // fit model
    RooAddPdf *model = new RooAddPdf("model", "sig+bkg", RooArgList(*bkg, *sig), RooArgList(*nbkg, *nsig));
    //RooAddPdf *model = new RooAddPdf("model", "sig+bkg", RooArgList(*bkg, *sig), *bkgfrac);
    
    return model;    
} // */

RooAbsPdf* modelEE(RooRealVar& m){
    
    RooRealVar *mcb = new RooRealVar("mcb", "mean crystalball", -0.65, -2., 0.);
    RooRealVar *scb = new RooRealVar("scb", "width crystalball", 1.4, 0.01, 4.);
    RooRealVar *alphacb = new RooRealVar("alphacb", "alpha crystalball", -2, -10., -0.);
    RooRealVar *ncb = new RooRealVar("ncb", "n crystalball", 2., 0., 3.);
    
    RooRealVar *mbw = new RooRealVar("mbw", "mean breit wigner", 91.18);
    RooRealVar *sbw = new RooRealVar("sbw", "width breit wigner", 2.497, 0., 5.);
    
    
    
    RooRealVar *alpha = new RooRealVar("alpha", "alpha", 0.3, 0.1, 1.);
    RooRealVar *beta = new RooRealVar("beta", "beta", -0.278, -0.5, 0.);
    RooRealVar *gamma = new RooRealVar("gamma", "gamma", 0.3, 0., 10.);
    RooRealVar *peak = new RooRealVar("peak", "peak", 89.375, 85., 95.);
    
    RooRealVar *bkgfrac = new RooRealVar("bkgfrac", "fraction of background", 0.08);//, 0., 0.1);
    
    RooRealVar *nbkg = new RooRealVar("nbkg", "Background events", 30000, 100, 15000);
    RooRealVar *nsig = new RooRealVar("nsig", "Signal events", 340000, 600000, 750000);
    
    // SIGNAL
    
    // construct crystal ball
    RooCBShape *crystalball = new RooCBShape("crystalball", "Crystal Ball Shape", m, *mcb, *scb, *alphacb, *ncb);
    
    // construct breit wigner
    RooBreitWigner *breitwigner = new RooBreitWigner("breitwigner", "Breit Wigner Peak", m, *mbw, *sbw);
    //RooGaussian *breitwigner = new RooGaussian("breitwigner", "Breit Wigner Peak", m, *mbw, *sbw);
    
    // convoluted Signal model
    RooNumConvPdf *sig = new RooNumConvPdf("sig", "Z peak shape", m, *breitwigner, *crystalball);
    
    // BACKGROUND
    
    RooCMSShape *bkg = new RooCMSShape("bkg", "background shape", m, *alpha, *beta, *gamma, *peak);
    
    /*  
         Double_t RooCMSShape::evaluate() const 
         { 
          // ENTER EXPRESSION IN TERMS OF VARIABLE ARGUMENTS HERE 

          Double_t erf = RooMath::erfc((alpha - x) * beta);
          Double_t u = (x - peak)*gamma;

          if(u < -70) u = 1e20;
          else if( u>70 ) u = 0;
          else u = exp(-u);   //exponential decay
          return erf*u;
         } 
        
        const char *name, const char *title,
        RooAbsReal& _x,
        RooAbsReal& _alpha,
        RooAbsReal& _beta,
        RooAbsReal& _gamma,
        RooAbsReal& _peak);
    */
    
    // fit model
    RooAddPdf *model = new RooAddPdf("model", "sig+bkg", RooArgList(*bkg, *sig), RooArgList(*nbkg, *nsig));
    //RooAddPdf *model = new RooAddPdf("model", "sig+bkg", RooArgList(*bkg, *sig), *bkgfrac);
    
    return model;    
} // */

/**********************************************************************************************
 * fits and stuff
 * 
 ***/

void FitterEG(string dataset){
    cout << "rooFitHeader.secondTry_eg_only()" << endl;
    
    TFile *file = new TFile(dataset.c_str(), "READ");
    
    TH2F *ee_dist = (TH2F*) file->Get("tnp_ee");
    TH2F *eg_dist = (TH2F*) file->Get("tnp_eg");
    
    TH1F *ee = (TH1F*) ee_dist->ProjectionX();
    TH1F *eg = (TH1F*) eg_dist->ProjectionX();
    //eg->ProjectionX();
    
    TH1F *ee_n = (TH1F*) ee->Clone();
    TH1F *eg_n = (TH1F*) eg->Clone();
    
    ee_n->Scale(1./ee_n->Integral());
    eg_n->Scale(1./eg_n->Integral());
    
    ee_n->Rebin(10);
    eg_n->Rebin(10);
    
    eg_n->SetLineColor(kRed);
    
    
    
    
    //TCanvas *ct = new TCanvas("ct", "ct", 600, 600);
    //ct->cd();
    //eg->Draw("hist");
    
    float Nee = ee->GetEntries();
    float Neg = eg->GetEntries();
    
    
    
    
    ee->Rebin(5);
    eg->Rebin(5);
    
    
    // define observable
    RooRealVar m("m", "Invariant Mass", 75., 105.);
    
    /// *******************************************************************************************
    /// eg
    
    // create binned dataset that imports the histogram
    RooDataHist data_eg("dh_eg", "eg sample", m, Import(*eg));
    
    // plot binned dataset
    RooPlot* frame_eg = m.frame(Title("eg invariant mass sepectrum"));
    data_eg.plotOn(frame_eg);
    
    // construct fit model
    RooAbsPdf *model_eg = modelEG(m);
    //model_eg->plotOn(frame_eg, LineColor(kGray), LineStyle(kDashed));
    
    /* ********************************************************************* 
     * FIT with log likelihood
     * */
    model_eg->fitTo(data_eg);
    
    
    
    
    
    
    
    // model plots
    model_eg->plotOn(frame_eg, LineColor(kBlue));
    
    // */
    
    
    // residual:
    RooHist *hresid_eg = frame_eg->residHist();
    RooPlot* frame_eg2 = m.frame(Title("Residual Distribution")) ;
    frame_eg2->addPlotable((RooPlotable*)hresid_eg,"P") ;
    
    // pull
    RooHist* hpull_eg = frame_eg->pullHist() ;
    RooPlot* frame_eg3 = m.frame(Title("Pull Distribution")) ;
    frame_eg3->addPlotable((RooPlotable*)hpull_eg,"P") ;
    
    // get models
    RooArgSet* comps_eg = model_eg->getComponents();
    RooAbsPdf* sig_eg   = (RooAbsPdf*)comps_eg->find("sig");
    
    RooAbsReal* N_sig_eg = sig_eg->createIntegral(m) ;
    
    
   
    
    model_eg->plotOn(frame_eg, Components("bkg"), LineColor(kMagenta), LineStyle(kDashed));
    model_eg->plotOn(frame_eg, Components("sig"), LineColor(8), LineStyle(kDashed)); // smooth green
    
    
    
    TCanvas *c2 = new TCanvas("c2", "c2", 800, 800);
    TPad *pad21 = new TPad("pad21", "pad21", 0, 0.3, 1, 1);
    
    pad21->SetBottomMargin(0.1);
    pad21->SetLeftMargin(0.16);
    pad21->Draw();
    pad21->cd();
    
    frame_eg->GetYaxis()->SetTitleOffset(1.6);
    frame_eg->Draw();
    
    pad21->Update();
    
    c2->cd();
    
    TPad *pad22 = new TPad("pad22", "pad22", 0, 0, 1, 0.3);
    pad22->SetTopMargin(0);
    pad22->SetBottomMargin(0.11);
    pad22->SetGridy();
    pad22->SetLeftMargin(0.16);
    pad22->Draw();
    pad22->cd();
    
    
    frame_eg3->GetYaxis()->SetTitle("#frac{data-fit}{#sigma_{data}}");
    frame_eg3->GetYaxis()->SetTitleOffset(0.5);
    frame_eg3->GetYaxis()->SetTitleSize(0.11);
    frame_eg3->GetYaxis()->SetNdivisions(5);
    frame_eg3->GetYaxis()->SetLabelSize(0.08);
    
    frame_eg3->GetXaxis()->SetTitle("");
    frame_eg3->GetXaxis()->SetLabelSize(0.0);
    frame_eg3->SetTitle("");
    frame_eg3->Draw();
    
    frame_eg3->Draw();
    
    pad22->Update();
    c2->Update();
    
    
    // */
    

    cout << endl;
    model_eg->printCompactTree();
    
    
    cout << endl;
    
    
    float   sig_ee_val,
            sig_ee_int,
            bkg_ee_val,
            bkg_ee_int,
            
            sig_eg_val,
            sig_eg_int;
    
    
    
    sig_eg_val = sig_eg->getVal();
    sig_eg_int = N_sig_eg->getVal();
    
    cout << endl;
    
    cout << "sig_eg        = " << sig_eg_val << endl;
    cout << "sig_eg_Int(m) = " << sig_eg_int << endl;
    
    cout << "Nee = " << Nee << endl;
    cout << "Neg = " << Neg << endl;
    
// */

}

void FitterEE(string dataset){
    cout << "rooFitHeader.FitterEE()" << endl;
    
    TFile *file = new TFile(dataset.c_str(), "READ");
    
    TH2F *ee_dist = (TH2F*) file->Get("tnp_ee");
    TH2F *eg_dist = (TH2F*) file->Get("tnp_eg");
    
    TH1F *ee = (TH1F*) ee_dist->ProjectionX();
    TH1F *eg = (TH1F*) eg_dist->ProjectionX();
    //eg->ProjectionX();
    
    TH1F *ee_n = (TH1F*) ee->Clone();
    TH1F *eg_n = (TH1F*) eg->Clone();
    
    ee_n->Scale(1./ee_n->Integral());
    eg_n->Scale(1./eg_n->Integral());
    
    ee_n->Rebin(10);
    eg_n->Rebin(10);
    
    eg_n->SetLineColor(kRed);
    
    float Nee = ee->GetEntries();
    float Neg = eg->GetEntries();
    
    
    
    
    
    // define observable
    RooRealVar m("m", "Invariant Mass", 75., 105.);    
    
    /// *******************************************************************************************
    /// ee
    ee->Rebin(5);
    
    // create binned dataset that imports the histogram
    RooDataHist data_ee("dh_ee", "ee sample", m, Import(*ee));
    
    // plot binned dataset
    RooPlot* frame_ee = m.frame(Title("ee invariant mass sepectrum"));
    data_ee.plotOn(frame_ee);
    
    
    // construct fit model
    RooAbsPdf *model_ee = modelEE(m);
    //model_ee->plotOn(frame_ee, LineColor(kGray), LineStyle(kDashed));
    
    // * ********************************************************************* 
    // * FIT with log likelihood
    // * 
    //model_ee->fitTo(data_ee);
    
    // complete plot
    model_ee->plotOn(frame_ee, LineColor(kBlue));
    
    // residual:
    RooHist *hresid_ee = frame_ee->residHist();
    RooPlot* frame_ee2 = m.frame(Title("Residual Distribution")) ;
    frame_ee2->addPlotable((RooPlotable*)hresid_ee,"P") ;
    
    // pull
    RooHist* hpull_ee = frame_ee->pullHist() ;
    RooPlot* frame_ee3 = m.frame(Title("Pull Distribution")) ;
    frame_ee3->addPlotable((RooPlotable*)hpull_ee,"P") ;
    
    // get models
    RooArgSet* comps_ee = model_ee->getComponents();
    RooAbsPdf* sig_ee   = (RooAbsPdf*)comps_ee->find("sig");
    
    RooAbsReal* N_sig_ee = sig_ee->createIntegral(m) ;
    
    //RooArgSet* sigVars_ee = sig_ee->getVariables();
    
    //RooAbsArg* bkg_ee = comps_ee->find("bkg");
    //RooArgSet* bkgVars_ee = bkg_ee->getVariables();
    
    //RooAbsArg *bkgfrac_ee = comps_ee->find("bkgfrac");
    
    //sigVars_ee->Print();
    
    // model plots
    model_ee->plotOn(frame_ee, Components("bkg"), LineColor(kMagenta), LineStyle(kDashed));
    model_ee->plotOn(frame_ee, Components("sig"), LineColor(8), LineStyle(kDashed)); // smooth green
    
    //sig.fitTo(data_ee);
    //sig.plotOn(frame, LineColor(kBlue), LineStyle(kDashed));
    
    
    //RooKeysPdf kest2("kest2", "kest2", m, data_ee, RooKeysPdf::NoMirror) ;
    //kest2->plotOn(frame_ee, LineColor(kRed), LineStyle(kDashed));
    
    
    /// *******************************************************************************************
    /// plots
    //*
    TCanvas *c1 = new TCanvas("c1", "c1", 800, 800);
    
    TPad *pad11 = new TPad("pad11", "pad11", 0, 0.3, 1, 1);
    
    pad11->SetBottomMargin(0.1);
    pad11->SetLeftMargin(0.16);
    pad11->Draw();
    pad11->cd();
    
    frame_ee->GetYaxis()->SetTitleOffset(1.6);
    frame_ee->Draw();
    
    pad11->Update();
    
    c1->cd();
    
    TPad *pad12 = new TPad("pad12", "pad12", 0, 0, 1, 0.3);
    pad12->SetTopMargin(0);
    pad12->SetBottomMargin(0.11);
    pad12->SetGridy();
    pad12->SetLeftMargin(0.16);
    pad12->Draw();
    pad12->cd();
    
    frame_ee3->GetYaxis()->SetTitle("#frac{data-fit}{#sigma_{data}}");
    frame_ee3->GetYaxis()->SetTitleOffset(0.5);
    frame_ee3->GetYaxis()->SetTitleSize(0.11);
    frame_ee3->GetYaxis()->SetNdivisions(5);
    frame_ee3->GetYaxis()->SetLabelSize(0.08);
    
    frame_ee3->GetXaxis()->SetTitle("");
    frame_ee3->GetXaxis()->SetLabelSize(0.0);
    frame_ee3->SetTitle("");
    frame_ee3->Draw();
    
    pad12->Update();
    
    c1->Update();
    
    
    //createCanvas("ee sample", *frame, *hpull);
    
    //createCanvas("eg sample", *frame, *hresid);
    
    
    /*
    c1->Divide(3);
    
    c1->cd(1);
    gPad->SetLeftMargin(0.15);
    //ee->Draw("hist");
    //eg_n->Draw("hist same");
    
    c1->cd(2);
    gPad->SetLeftMargin(0.15);
    frame2->GetYaxis()->SetTitleOffset(1.6);
    frame2->Draw();
    
    c1->cd(3);
    gPad->SetLeftMargin(0.15);
    frame3->GetYaxis()->SetTitleOffset(1.6);
    frame3->Draw();
    */
    
    // print all values to terminal
    model_ee->printCompactTree();
    cout << endl;
    //model_eg->printCompactTree();
    
    
    cout << endl;
    
    
    float   sig_ee_val,
            sig_ee_int,
            bkg_ee_val,
            bkg_ee_int,
            
            sig_eg_val,
            sig_eg_int;
    
    
    sig_ee_val = sig_ee->getVal();
    sig_ee_int = N_sig_ee->getVal();
    
    //sig_eg_val = sig_eg->getVal();
    //sig_eg_int = N_sig_eg->getVal();
    
    cout << endl;
    
    cout << "sig_ee        = " << sig_ee_val << endl;
    cout << "sig_ee_Int(m) = " << sig_ee_int << endl;
    
    cout << "sig_eg        = " << sig_eg_val << endl;
    cout << "sig_eg_Int(m) = " << sig_eg_int << endl;
    
    
    //cout << "fakerate = " << sig_eg_int/(sig_eg_int+sig_ee_int) << endl;
    
    
    cout << "Nee = " << Nee << endl;
    cout << "Neg = " << Neg << endl;
    
    /*
    RooRealVar *mcb = (RooRealVar*)sigVars->find("mcb");
    RooRealVar *scb = (RooRealVar*)sigVars->find("scb");
    RooRealVar *alphacb = (RooRealVar*)sigVars->find("alphacb");
    RooRealVar *ncb = (RooRealVar*)sigVars->find("ncb");
    RooRealVar *mbw = (RooRealVar*)sigVars->find("mbw");
    RooRealVar *sbw = (RooRealVar*)sigVars->find("sbw");
    RooRealVar *tau = (RooRealVar*)bkgVars->find("tau");
    RooRealVar *p0 = (RooRealVar*)bkgVars->find("p0");
    RooRealVar *p1 = (RooRealVar*)bkgVars->find("p1");
    RooRealVar *p2 = (RooRealVar*)bkgVars->find("p2");
    
    mcb->Print();
    scb->Print();
    alphacb->Print();
    ncb->Print();
    mbw->Print();
    sbw->Print();
    tau->Print();
    p0->Print();
    p1->Print();
    p2->Print();
    //bkgfrac->Print();
    //bkgfrac->Print();
    // */
}



void kernelEst(string dataset){
    cout << "rooFitHeader.kernelEst()" << endl;
    
    TFile *file = new TFile(dataset.c_str(), "READ");
    
    TH2F *ee_dist = (TH2F*) file->Get("tnp_ee");
    TH2F *eg_dist = (TH2F*) file->Get("tnp_eg");
    
    

    TH1F *ee = (TH1F*) ee_dist->ProjectionX();//("eerb").c_str(),1, 10);
    TH1F *eg = (TH1F*) eg_dist->ProjectionX();//("egrb").c_str(),1, 10);
    //eg->ProjectionX();
    
    TH1F *ee_n = (TH1F*) ee->Clone();
    TH1F *eg_n = (TH1F*) eg->Clone();
    
    ee_n->Scale(1./ee_n->Integral());
    eg_n->Scale(1./eg_n->Integral());
    
    ee_n->Rebin(10);
    eg_n->Rebin(10);
    
    eg_n->SetLineColor(kRed);
    
    // rebin
    ee->Rebin(10);
    eg->Rebin(10);
    
    
    
    // define observable
    RooRealVar m("m", "Invariant Mass", 60., 120.);
    
    

    
    
    // create binned datasets that imports the histogram
    RooDataHist data_ee("dh_ee", "ee sample", m, Import(*ee));
    RooDataHist data_eg("dh_eg", "eg sample", m, Import(*eg));
    
    
    RooDataSet* dsee = (RooDataSet*) &data_ee;
    RooDataSet* dseg = (RooDataSet*) &data_eg;
    
    // plot binned dataset
    RooPlot* frame_ee = m.frame(Title("ee invariant mass sepectrum"));
    RooPlot* frame_eg = m.frame(Title("eg invariant mass sepectrum"));
    
    
    //data_ee.plotOn(frame_ee);
    dsee->plotOn(frame_ee);
    dseg->plotOn(frame_eg);
    
    
    
    // kernel density estimations
    
    RooKeysPdf kest_ee("kest_ee", "kest_ee", m, *dseg, RooKeysPdf::MirrorBoth, 0.3) ;
    
    kest_ee.plotOn(frame_ee, LineColor(kRed), LineStyle(kDashed));
    
    RooKeysPdf kest_eg("kest_eg", "kest_eg", m, *dsee, RooKeysPdf::NoMirror, 0.4) ;
    kest_eg.plotOn(frame_eg, LineColor(kRed), LineStyle(kDashed));
    

    //TH1 *f = kest_ee.createHistogram("test", 100);
    
    
    // pull distributions
    // 
    
    RooHist* hpull_ee = frame_ee->pullHist() ;
    RooPlot* frame_ee1 = m.frame(Title("Pull Distribution")) ;
    frame_ee1->addPlotable((RooPlotable*)hpull_ee,"P") ;
    
    RooHist* hpull_eg = frame_eg->pullHist() ;
    RooPlot* frame_eg1 = m.frame(Title("Pull Distribution")) ;
    //frame_eg1->addPlotable((RooPlotable*)hpull_eg,"P") ;
    frame_eg1->addPlotable(hpull_eg,"P") ;
    
    
    
    TCanvas *c1 = new TCanvas("c1", "c1", 1200, 600);
    c1->Divide(2);
    
    
    c1->cd(1);
    
    TPad *pad11 = new TPad("pad11", "pad11", 0, 0.3, 1, 1);
    
    pad11->SetBottomMargin(0.1);
    pad11->SetLeftMargin(0.16);
    pad11->Draw();
    pad11->cd();
    
    frame_ee->GetYaxis()->SetTitleOffset(1.6);
    frame_ee->Draw();
    
    pad11->Update();
    
    c1->cd(1);
    
    TPad *pad12 = new TPad("pad12", "pad12", 0, 0, 1, 0.3);
    pad12->SetTopMargin(0);
    pad12->SetBottomMargin(0.11);
    pad12->SetGridy();
    pad12->SetLeftMargin(0.16);
    pad12->Draw();
    pad12->cd();

    frame_ee1->GetYaxis()->SetTitle("#frac{data-fit}{#sigma_{data}}");
    frame_ee1->GetYaxis()->SetTitleOffset(0.5);
    frame_ee1->GetYaxis()->SetTitleSize(0.11);
    frame_ee1->GetYaxis()->SetNdivisions(5);
    frame_ee1->GetYaxis()->SetLabelSize(0.08);
    
    frame_ee1->GetXaxis()->SetTitle("");
    frame_ee1->GetXaxis()->SetLabelSize(0.0);
    frame_ee1->SetTitle("");
    frame_ee1->Draw();

    pad12->Update();
    
    //c1->Update();
     
    c1->cd(2);
    
    TPad *pad21 = new TPad("pad21", "pad21", 0, 0.3, 1, 1);
    
    pad21->SetBottomMargin(0.1);
    pad21->SetLeftMargin(0.16);
    pad21->Draw();
    pad21->cd();
    
    frame_eg->GetYaxis()->SetTitleOffset(1.6);
    frame_eg->Draw();
    
    pad21->Update();
    
    c1->cd(2);
    
    TPad *pad22 = new TPad("pad22", "pad22", 0, 0, 1, 0.3);
    pad22->SetTopMargin(0);
    pad22->SetBottomMargin(0.11);
    pad22->SetGridy();
    pad22->SetLeftMargin(0.16);
    pad22->Draw();
    pad22->cd();

    frame_eg1->GetYaxis()->SetTitle("#frac{data-fit}{#sigma_{data}}");
    frame_eg1->GetYaxis()->SetTitleOffset(0.5);
    frame_eg1->GetYaxis()->SetTitleSize(0.11);
    frame_eg1->GetYaxis()->SetNdivisions(5);
    frame_eg1->GetYaxis()->SetLabelSize(0.08);
    
    frame_eg1->GetXaxis()->SetTitle("");
    frame_eg1->GetXaxis()->SetLabelSize(0.0);
    frame_eg1->SetTitle("");
    frame_eg1->Draw();

    pad22->Update();
    
    
    
    
    
    // print all values to terminal
    //model_ee->printCompactTree();
    cout << "end" << endl;
    //model_eg->printCompactTree();

// */

}







