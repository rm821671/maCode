
#include "rooFitHeader.h"

using namespace RooFit;
using namespace std;

/**********************************************************************************************
 * setup functions
 * 
 * 
 */

#define arraysize(ar)  (sizeof(ar) / sizeof(ar[0]))

template <typename T> string NumberToString ( T Number ) {
        // convert numer to string
        ostringstream ss;
        ss << Number;
        return ss.str();
    }

void SystemPath(string &filepath, string & dropbox){
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
    RooRealVar *mcb = new RooRealVar("mcb", "mean crystalball", 0.22, -1., 0.);
    RooRealVar *scb = new RooRealVar("scb", "width crystalball", 0.8, 0.2, 1.5);
    RooRealVar *alphacb = new RooRealVar("alphacb", "alpha crystalball", -3.04, -5., -2.);
    RooRealVar *ncb = new RooRealVar("ncb", "n crystalball", 0.03, 0., 0.5);    
    RooRealVar *mbw = new RooRealVar("mbw", "mean breit wigner", 90.261, 88., 93.);
    RooRealVar *sbw = new RooRealVar("sbw", "width breit wigner", 2.495);//, 2., 4.);    
    RooRealVar *tau = new RooRealVar("tau", "tau", -0.157, -.2, -0.1);    
    RooRealVar *con = new RooRealVar("con", "con", 1., 0., 100.);    
    RooRealVar *p0 = new RooRealVar("p0", "p0", 0.85, 0.5, 1.);
    RooRealVar *p1 = new RooRealVar("p1", "p1", 88.145, 80., 90.);
    RooRealVar *p2 = new RooRealVar("p2", "p2", 4.04, 3., 5.);
    
    RooRealVar *bkgfrac = new RooRealVar("bkgfrac", "fraction of background", 0.1, 0., 1.);
    
    RooRealVar *nbkg = new RooRealVar("nbkg", "Background events", 1000, 100, 2000);
    RooRealVar *nsig = new RooRealVar("nsig", "Signal events", 4500, 4000, 5200);
    
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

RooAbsPdf* modelEG_DCB(RooRealVar& m){
    
    
    // extenden likelihood fit: numbers for signal and background
    RooRealVar *nbkg = new RooRealVar("nbkg", "Background events", 800, 500, 2000);
    RooRealVar *nsig = new RooRealVar("nsig", "Signal events", 4600, 2500, 5200);
    
    // *** SIGNAL ***
    // breit wigner peak
    RooRealVar *mbw = new RooRealVar("mbw", "mean breit wigner", 91.18);
    RooRealVar *sbw = new RooRealVar("sbw", "width breit wigner", 2.495);//, 0., 2.);//, 0., 50.);
    RooBreitWigner *breitwigner = new RooBreitWigner("breitwigner", "Breit Wigner Peak", m, *mbw, *sbw);
    // double sided crystal ball for smearing effects
    RooRealVar *mcb = new RooRealVar("mcb", "mean crystalball", -0.85, -2., 0.);
    RooRealVar *scb = new RooRealVar("scb", "width crystalball", 0.9, 0.5, 4.);//, 0.5, 4.);
    RooRealVar *alphacb1 = new RooRealVar("alphacb1", "alpha crystalball 1", 16, 4., 40.);
    RooRealVar *alphacb2 = new RooRealVar("alphacb2", "alpha crystalball 2", 1.65, 1., 3.);
    RooRealVar *ncb1 = new RooRealVar("ncb1", "n crystalball 1", 2, .5, 10);
    RooRealVar *ncb2 = new RooRealVar("ncb2", "n crystalball 2", 1.24, 1., 3.);
    RooDoubleCBFast *doubleCB = new RooDoubleCBFast("doubleDB", "double sided crystal ball", m, *mcb, *scb, *alphacb1, *ncb1, *alphacb2, *ncb2);
    // convolution of both:
    RooNumConvPdf *sigDCB = new RooNumConvPdf("sigDCB", "Z peak shape with double sided CB", m, *breitwigner, *doubleCB);
    
    // *** BACKGROUND ***
    // cms shape
    RooRealVar *alpha = new RooRealVar("alpha", "alpha", 87.3, 80, 100.);
    RooRealVar *beta = new RooRealVar("beta", "beta", 1./6., 0., 1.);
    RooRealVar *gamma = new RooRealVar("gamma", "gamma", 0.1, 0., 2.);
    RooRealVar *peak = new RooRealVar("peak", "peak", 40.3, 1., 100.);
    RooCMSShape *bkg = new RooCMSShape("bkg", "background shape", m, *alpha, *beta, *gamma, *peak);
    
    // fit model
    RooAddPdf *nmodel = new RooAddPdf("nmodel", "sig+bkg", RooArgList(*bkg, *sigDCB), RooArgList(*nbkg, *nsig));
    
    return nmodel;
    
    /*
    results:
0x5399ec0 RooAddPdf::nmodel = 1.86525 [Auto,Dirty] 
  0x5398ca0/V- RooCMSShape::bkg = 1.22643 [Auto,Dirty] 
    0x7fff266f9380/V- RooRealVar::m = 90
    0x5396a20/V- RooRealVar::alpha = 85.5627 +/- 0.314828
    0x53972c0/V- RooRealVar::beta = 0.209073 +/- 0.0198855
    0x5397b60/V- RooRealVar::gamma = 0.0419733 +/- 0.00539892
    0x5398400/V- RooRealVar::peak = 80.7208 +/- 54.0803
  0x55e2c60/V- RooRealVar::nbkg = 1751.77 +/- 67.9599
  0x55e7090/V- RooNumConvPdf::sigDCB = 2.04211 [Auto,Dirty] 
    0x7fff266f9380/V- RooRealVar::m = 90
    0x55f0d90/V- RooBreitWigner::breitwigner = 0.417991 [Auto,Dirty] 
      0x7fff266f9380/V- RooRealVar::m = 90
      0x55ebcb0/V- RooRealVar::mbw = 91.18
      0x537e700/V- RooRealVar::sbw = 2 +/- 0.0695471
    0x55e5d60/V- RooDoubleCBFast::doubleDB = 0.00026538 [Auto,Dirty] 
      0x7fff266f9380/V- RooRealVar::m = 90
      0x55ecb50/V- RooRealVar::mcb = -0.819378 +/- 0.0386393
      0x55f0670/V- RooRealVar::scb = 1.68443 +/- 0.0444075
      0x55e3780/V- RooRealVar::alphacb1 = 6.47965 +/- 0.955835
      0x55e4c00/V- RooRealVar::ncb1 = 4.93797 +/- 7.8296
      0x55e4360/V- RooRealVar::alphacb2 = 2.54512 +/- 0.199479
      0x55e54a0/V- RooRealVar::ncb2 = 1.02959 +/- 1.79971
  0x537dee0/V- RooRealVar::nsig = 6327.57 +/- 87.3579
    
    
    
    
    */
    
    
} // */

RooAbsPdf* modelEE(RooRealVar& m){
    
    RooRealVar *mcb = new RooRealVar("mcb", "mean crystalball", -0.65);//, -2., 0.);
    RooRealVar *scb = new RooRealVar("scb", "width crystalball", 1.29, 0.1, 2.);
    RooRealVar *alphacb = new RooRealVar("alphacb", "alpha crystalball", -1.58);//, -3., -0.);
    RooRealVar *ncb = new RooRealVar("ncb", "n crystalball", 1.52);//, 0., 3.);
    
    RooRealVar *mbw = new RooRealVar("mbw", "mean breit wigner", 91.18);
    RooRealVar *sbw = new RooRealVar("sbw", "width breit wigner", 2.12);//, 0., 50.);
    
    RooRealVar *alpha = new RooRealVar("alpha", "alpha", 89.3, 0.1, 1.);
    RooRealVar *beta = new RooRealVar("beta", "beta", 5., 0., 10.);
    RooRealVar *gamma = new RooRealVar("gamma", "gamma", 0.1, 0., 10.);
    RooRealVar *peak = new RooRealVar("peak", "peak", 89.3, 85., 95.);
    
    RooRealVar *bkgfrac = new RooRealVar("bkgfrac", "fraction of background", 0.08);//, 0., 0.1);
    
    RooRealVar *nbkg = new RooRealVar("nbkg", "Background events", 30000, 100, 15000);
    RooRealVar *nsig = new RooRealVar("nsig", "Signal events", 340000, 600000, 750000);
    
    
    
    // SIGNAL
    
    // construct crystal ball
    RooCBShape *crystalball = new RooCBShape("crystalball", "Crystal Ball Shape", m, *mcb, *scb, *alphacb, *ncb);
    
    // construct breit wigner
    RooBreitWigner *breitwigner = new RooBreitWigner("breitwigner", "Breit Wigner Peak", m, *mbw, *sbw);
    
    RooRealVar *gmean = new RooRealVar("gmean", "gmean", 0., -2., 2.);
    RooRealVar *gsigma = new RooRealVar("gsigma", "gsigma", 3., 1., 20.);
    
    RooGaussian *gaus = new RooGaussian("gaus", "gaussian peak", m, *gmean, *gsigma);
    
    // convoluted Signal model
    RooNumConvPdf *sig = new RooNumConvPdf("sig", "Z peak shape", m, *breitwigner, *crystalball);
    
    RooNumConvPdf *gsig = new RooNumConvPdf("gsig", "gaussian peak * WB", m, *breitwigner, *gaus);
    
    
    
    // BACKGROUND    
    RooCMSShape *bkg = new RooCMSShape("bkg", "background shape", m, *alpha, *beta, *gamma, *peak);
    
    // fit model
    RooAddPdf *model = new RooAddPdf("model", "sig+bkg", RooArgList(*bkg, *sig), RooArgList(*nbkg, *nsig));
    
    //RooAddPdf *model = new RooAddPdf("model", "sig+bkg", RooArgList(*bkg, *sig), *bkgfrac);
    
    return model;
      
    //return sig;
} // */

RooAbsPdf* modelEE_DCB(RooRealVar& m){
    
    // extended likelihood fit: signal and background contribution
    RooRealVar *nbkg = new RooRealVar("nbkg", "Background events", 9900, 100, 20000);
    RooRealVar *nsig = new RooRealVar("nsig", "Signal events", 400000, 200000, 420000);
    
    // SIGNAL
    // double sided crystal ball
    RooRealVar *mcb = new RooRealVar("mcb", "mean crystalball", -0.68);//, -2., 0.);
    RooRealVar *scb = new RooRealVar("scb", "width crystalball", 1.07, 0.5, 4.);//, 0.5, 4.);
    RooRealVar *alphacb1 = new RooRealVar("alphacb1", "alpha crystalball 1", 15.52, 1., 100.);
    RooRealVar *alphacb2 = new RooRealVar("alphacb2", "alpha crystalball 2", 2.03, 1., 3.);
    RooRealVar *ncb1 = new RooRealVar("ncb1", "n crystalball 1", 0.185, 0.1, 25);
    RooRealVar *ncb2 = new RooRealVar("ncb2", "n crystalball 2", 1.38, 1., 5.);
    RooDoubleCBFast *doubleCB = new RooDoubleCBFast("doubleDB", "double sided crystal ball", m, *mcb, *scb, *alphacb1, *ncb1, *alphacb2, *ncb2);
    // breit wigner
    RooRealVar *mbw = new RooRealVar("mbw", "mean breit wigner", 91.18);
    RooRealVar *sbw = new RooRealVar("sbw", "width breit wigner", 2.495);//, 0., 2.);//, 0., 50.);
    RooBreitWigner *breitwigner = new RooBreitWigner("breitwigner", "Breit Wigner Peak", m, *mbw, *sbw);
    // convoluted Signal model    
    RooNumConvPdf *sigDCB = new RooNumConvPdf("sigDCB", "Z peak shape with double sided CB", m, *breitwigner, *doubleCB);
    
    // BACKGROUND
    // cms shape
    RooRealVar *alpha = new RooRealVar("alpha", "alpha", 85.18);//, 80, 100.);
    RooRealVar *beta = new RooRealVar("beta", "beta", 1.);//, 0., 5.);
    RooRealVar *gamma = new RooRealVar("gamma", "gamma", 0.041);//, 0., 1.);
    RooRealVar *peak = new RooRealVar("peak", "peak", 5.84);//, 1., 100.);
    RooCMSShape *bkg = new RooCMSShape("bkg", "background shape", m, *alpha, *beta, *gamma, *peak);
    
    // fit model
    RooAddPdf *model = new RooAddPdf("model", "sig+bkg", RooArgList(*bkg, *sigDCB), RooArgList(*nbkg, *nsig));
    
    //RooAddPdf *model = new RooAddPdf("model", "sig+bkg", RooArgList(*bkg, *sig), *bkgfrac);
    // */
    
    return model;
} // */

RooAbsPdf* modelEGdata(RooRealVar& m){
    /* old modelling
    RooRealVar *alphacb = new RooRealVar("alphacb", "alpha crystalball", -1.51933);//, -3., -0.);
    RooRealVar *ncb = new RooRealVar("ncb", "n crystalball", 1.538, 0., 3.);
    RooRealVar *bkgfrac = new RooRealVar("bkgfrac", "fraction of background", 0.08);//, 0., 0.1);
    // construct crystal ball
    RooCBShape *crystalball = new RooCBShape("crystalball", "Crystal Ball Shape", m, *mcb, *scb, *alphacb, *ncb);
    RooRealVar *gmean = new RooRealVar("gmean", "gmean", 91., 88., 94.);
    RooRealVar *gsigma = new RooRealVar("gsigma", "gsigma", 3., 1., 20.);
    RooGaussian *gaus = new RooGaussian("gaus", "gaussian peak", m, *gmean, *gsigma);
    // convoluted Signal model
    RooNumConvPdf *sig = new RooNumConvPdf("sig", "Z peak shape", m, *breitwigner, *crystalball);
    RooNumConvPdf *gsig = new RooNumConvPdf("gsig", "gaussian peak * WB", m, *breitwigner, *gaus);
    RooNumConvPdf *gcb = new RooNumConvPdf("gcb", "gaussian * crystal ball", m, *gaus, *crystalball);
    // fit model
    RooAddPdf *model = new RooAddPdf("model", "sig+bkg", RooArgList(*bkg, *sig), RooArgList(*nbkg, *nsig));
    //RooAddPdf *model = new RooAddPdf("model", "sig+bkg", RooArgList(*bkg, *sig), *bkgfrac);
    // */
    
    // extenden likelihood fit: numbers for signal and background
    RooRealVar *nbkg = new RooRealVar("nbkg", "Background events", 2000, 100, 9000);
    RooRealVar *nsig = new RooRealVar("nsig", "Signal events", 13000, 6000, 15000);
    
    // *** SIGNAL ***
    // breit wigner peak
    RooRealVar *mbw = new RooRealVar("mbw", "mean breit wigner", 91.18);
    RooRealVar *sbw = new RooRealVar("sbw", "width breit wigner", 0.893, 0., 2.);//, 0., 50.);
    RooBreitWigner *breitwigner = new RooBreitWigner("breitwigner", "Breit Wigner Peak", m, *mbw, *sbw);
    // double sided crystal ball for smearing effects
    RooRealVar *mcb = new RooRealVar("mcb", "mean crystalball", -0.75, -2., 0.);
    RooRealVar *scb = new RooRealVar("scb", "width crystalball", 1.9, 0.5, 4.);//, 0.5, 4.);
    RooRealVar *alphacb1 = new RooRealVar("alphacb1", "alpha crystalball 1", 6, 4., 7.);
    RooRealVar *alphacb2 = new RooRealVar("alphacb2", "alpha crystalball 2", 1.65, 1., 3.);
    RooRealVar *ncb1 = new RooRealVar("ncb1", "n crystalball 1", 10, 4.5, 20);
    RooRealVar *ncb2 = new RooRealVar("ncb2", "n crystalball 2", 1.24, 1., 3.);
    RooDoubleCBFast *doubleCB = new RooDoubleCBFast("doubleDB", "double sided crystal ball", m, *mcb, *scb, *alphacb1, *ncb1, *alphacb2, *ncb2);
    // convolution of both:
    RooNumConvPdf *sigDCB = new RooNumConvPdf("sigDCB", "Z peak shape with double sided CB", m, *breitwigner, *doubleCB);
    
    // *** BACKGROUND ***
    // cms shape
    RooRealVar *alpha = new RooRealVar("alpha", "alpha", 87.3, 80, 100.);
    RooRealVar *beta = new RooRealVar("beta", "beta", 1./6., 0., 1.);
    RooRealVar *gamma = new RooRealVar("gamma", "gamma", 0.1, 0., 2.);
    RooRealVar *peak = new RooRealVar("peak", "peak", 40.3, 1., 100.);
    RooCMSShape *bkg = new RooCMSShape("bkg", "background shape", m, *alpha, *beta, *gamma, *peak);
    
    // fit model
    RooAddPdf *nmodel = new RooAddPdf("nmodel", "sig+bkg", RooArgList(*bkg, *sigDCB), RooArgList(*nbkg, *nsig));
    
    return nmodel;
    
    
    
    
    
    /*
    results:
0x5399ec0 RooAddPdf::nmodel = 1.86525 [Auto,Dirty] 
  0x5398ca0/V- RooCMSShape::bkg = 1.22643 [Auto,Dirty] 
    0x7fff266f9380/V- RooRealVar::m = 90
    0x5396a20/V- RooRealVar::alpha = 85.5627 +/- 0.314828
    0x53972c0/V- RooRealVar::beta = 0.209073 +/- 0.0198855
    0x5397b60/V- RooRealVar::gamma = 0.0419733 +/- 0.00539892
    0x5398400/V- RooRealVar::peak = 80.7208 +/- 54.0803
  0x55e2c60/V- RooRealVar::nbkg = 1751.77 +/- 67.9599
  0x55e7090/V- RooNumConvPdf::sigDCB = 2.04211 [Auto,Dirty] 
    0x7fff266f9380/V- RooRealVar::m = 90
    0x55f0d90/V- RooBreitWigner::breitwigner = 0.417991 [Auto,Dirty] 
      0x7fff266f9380/V- RooRealVar::m = 90
      0x55ebcb0/V- RooRealVar::mbw = 91.18
      0x537e700/V- RooRealVar::sbw = 2 +/- 0.0695471
    0x55e5d60/V- RooDoubleCBFast::doubleDB = 0.00026538 [Auto,Dirty] 
      0x7fff266f9380/V- RooRealVar::m = 90
      0x55ecb50/V- RooRealVar::mcb = -0.819378 +/- 0.0386393
      0x55f0670/V- RooRealVar::scb = 1.68443 +/- 0.0444075
      0x55e3780/V- RooRealVar::alphacb1 = 6.47965 +/- 0.955835
      0x55e4c00/V- RooRealVar::ncb1 = 4.93797 +/- 7.8296
      0x55e4360/V- RooRealVar::alphacb2 = 2.54512 +/- 0.199479
      0x55e54a0/V- RooRealVar::ncb2 = 1.02959 +/- 1.79971
  0x537dee0/V- RooRealVar::nsig = 6327.57 +/- 87.3579
    
    
    
    
    */
    
    
} // */

RooAbsPdf* modelEEdata(RooRealVar& m){
    
    /* some results:
    ee_try1.pdf
    0x50645e0 RooNumConvPdf::sigDCB = 5.93623 [Auto,Dirty] 
      0x7ffe1f705ae0/V- RooRealVar::m = 90
      0x5060070/V- RooBreitWigner::breitwigner = 0.628234 [Auto,Dirty] 
        0x7ffe1f705ae0/V- RooRealVar::m = 90
        0x5057e80/V- RooRealVar::mbw = 91.18
        0x5058640/V- RooRealVar::sbw = 0.893001 +/- 1.28299
      0x505cb20/V- RooDoubleCBFast::doubleDB = 0.00181886 [Auto,Dirty] 
        0x7ffe1f705ae0/V- RooRealVar::m = 90
        0x4e00110/V- RooRealVar::mcb = -0.734121 +/- 0.00161324
        0x5070520/V- RooRealVar::scb = 2.21325 +/- 0.00946377
        0x4dfed60/V- RooRealVar::alphacb1 = 4.99998 +/- 7.93131e-06
        0x506f0f0/V- RooRealVar::ncb1 = 5.49992 +/- 1.56166e-05
        0x506ecd0/V- RooRealVar::alphacb2 = 1.65407 +/- 0.0043669
        0x4e01490/V- RooRealVar::ncb2 = 1.24242 +/- 0.00293246
    
    ee_try2.pdf
    0x5b44990 RooNumConvPdf::sigDCB = 5.93896 [Auto,Dirty] 
      0x7ffe97184b00/V- RooRealVar::m = 90
      0x5b40420/V- RooBreitWigner::breitwigner = 0.628295 [Auto,Dirty] 
        0x7ffe97184b00/V- RooRealVar::m = 90
        0x5b38230/V- RooRealVar::mbw = 91.18
        0x5b389f0/V- RooRealVar::sbw = 0.892655 +/- 1.22435
      0x5b3ced0/V- RooDoubleCBFast::doubleDB = 0.00180639 [Auto,Dirty] 
        0x7ffe97184b00/V- RooRealVar::m = 90
        0x58e04c0/V- RooRealVar::mcb = -0.734381 +/- 0.000935767
        0x5b508d0/V- RooRealVar::scb = 2.21319 +/- 0.00931009
        0x58df110/V- RooRealVar::alphacb1 = 6.00077 +/- 0.00067377
        0x5b4f4a0/V- RooRealVar::ncb1 = 9.99906 +/- 3.41845e-05
        0x5b4f080/V- RooRealVar::alphacb2 = 1.65246 +/- 0.00297585
        0x58e1840/V- RooRealVar::ncb2 = 1.24597 +/- 0.000845012
    
    
    
    
    
    
    RooRealVar *gmean = new RooRealVar("gmean", "gmean", 91., 88., 94.);
    RooRealVar *gsigma = new RooRealVar("gsigma", "gsigma", 3., 1., 20.);
    
    RooGaussian *gaus = new RooGaussian("gaus", "gaussian peak", m, *gmean, *gsigma);
    
    RooRealVar *alphacb = new RooRealVar("alphacb", "alpha crystalball", -1.51933);//, -3., -0.);
    RooRealVar *ncb = new RooRealVar("ncb", "n crystalball", 1.538, 0., 3.);
    
    RooRealVar *bkgfrac = new RooRealVar("bkgfrac", "fraction of background", 0.08);//, 0., 0.1);
    
    RooNumConvPdf *sig = new RooNumConvPdf("sig", "Z peak shape", m, *breitwigner, *crystalball);
    
    // construct crystal ball
    RooCBShape *crystalball = new RooCBShape("crystalball", "Crystal Ball Shape", m, *mcb, *scb, *alphacb, *ncb);
    
    
    
    RooNumConvPdf *gsig = new RooNumConvPdf("gsig", "gaussian peak * WB", m, *breitwigner, *gaus);
    
    RooNumConvPdf *gcb = new RooNumConvPdf("gcb", "gaussian * crystal ball", m, *gaus, *crystalball);
// */
        
/*    
    0x73ed4e0 RooAddPdf::model = 5.87211 [Auto,Dirty] 
  0x73ec2c0/V- RooCMSShape::bkg = 0.000313944 [Auto,Dirty] 
    0x7ffee4074080/V- RooRealVar::m = 90
    0x73ea040/V- RooRealVar::alpha = 87.9613 +/- 0.0383451
    0x73ea8e0/V- RooRealVar::beta = 0.236981 +/- 0.00156835
    0x73eb180/V- RooRealVar::gamma = 0.160144 +/- 0.00426904
    0x73eba20/V- RooRealVar::peak = 37.0761 +/- 2.80541
  0x6c644a0/V- RooRealVar::nbkg = 47569.6 +/- 955.879
  0x73e8e40/V- RooNumConvPdf::sigDCB = 6.87572 [Auto,Dirty] 
    0x7ffee4074080/V- RooRealVar::m = 90
    0x73e6cb0/V- RooBreitWigner::breitwigner = 0.648436 [Auto,Dirty] 
      0x7ffee4074080/V- RooRealVar::m = 90
      0x73e5e00/V- RooRealVar::mbw = 91.18
      0x73e6410/V- RooRealVar::sbw = 0.774006 +/- 0.00276813
    0x73cd430/V- RooDoubleCBFast::doubleDB = 0.000513543 [Auto,Dirty] 
      0x7ffee4074080/V- RooRealVar::m = 90
      0x6c590b0/V- RooRealVar::mcb = -0.676784 +/- 0.000619648
      0x6c570a0/V- RooRealVar::scb = 2.08985 +/- 0.000210563
      0x73e5060/V- RooRealVar::alphacb1 = 3.62099 +/- 2.03925
      0x73cc1e0/V- RooRealVar::ncb1 = 15.2352 +/- 0.391376
      0x6c58990/V- RooRealVar::alphacb2 = 1.7302 +/- 0.00569883
      0x73cc600/V- RooRealVar::ncb2 = 1.58245 +/- 0.000880789
  0x73ca350/V- RooRealVar::nsig = 278314 +/- 766.442
    
    
    
    
    // ////////////////////////////////////////////////////////////////////////
    // results with extreme large background:
//  0x6908400 RooAddPdf::model = 0.802142 [Auto,Dirty] 
//  0x69071e0/V- RooCMSShape::bkg = 0.0124635 [Auto,Dirty] 
//    0x7ffcbcb24680/V- RooRealVar::m = 90
//    0x6904f60/V- RooRealVar::alpha = 87.1218 +/- 0.0158
//    0x6905800/V- RooRealVar::beta = 0.458729 +/- 0.00376655
//    0x69060a0/V- RooRealVar::gamma = 0.190519 +/- 7.55869e-05
//    0x6906940/V- RooRealVar::peak = 63.511 +/- 40.1882
//  0x617f380/V- RooRealVar::nbkg = 99990.5 +/- 61.3705
//  0x6903d60/V- RooNumConvPdf::sigDCB = 1.15236 [Auto,Dirty] 
//    0x7ffcbcb24680/V- RooRealVar::m = 90
//    0x6901bd0/V- RooBreitWigner::breitwigner = 0.339138 [Auto,Dirty] 
//      0x7ffcbcb24680/V- RooRealVar::m = 90
//      0x6900cc0/V- RooRealVar::mbw = 91.18
//      0x69012d0/V- RooRealVar::sbw = 2.495
//    0x68e82f0/V- RooDoubleCBFast::doubleDB = 3.59131e-06 [Auto,Dirty] 
//      0x7ffcbcb24680/V- RooRealVar::m = 90
//      0x6173f90/V- RooRealVar::mcb = -0.423343 +/- 0.00907257
//      0x6171f80/V- RooRealVar::scb = 1.15912 +/- 0.00547507
//      0x68fff20/V- RooRealVar::alphacb1 = 8.01793 +/- 13.9937
//      0x68e70a0/V- RooRealVar::ncb1 = 1.04 +/- 5.29112
//      0x6173870/V- RooRealVar::alphacb2 = 2.61115 +/- 0.0155526
//      0x68e74c0/V- RooRealVar::ncb2 = 1.98031 +/- 0.00022347
//  0x68e5210/V- RooRealVar::nsig = 225458 +/- 560.456
    
    // extreme large background:
    // extended likelihood fit: signal and background contribution
    RooRealVar *nbkg = new RooRealVar("nbkg", "Background events", 25000, 100, 100000);
    RooRealVar *nsig = new RooRealVar("nsig", "Signal events", 250000, 100000, 350000);
    
    // SIGNAL
    // double sided crystal ball
    RooRealVar *mcb = new RooRealVar("mcb", "mean crystalball", -0.68, -2., 0.);
    RooRealVar *scb = new RooRealVar("scb", "width crystalball", 1.47, 0.5, 4.);//, 0.5, 4.);
    RooRealVar *alphacb1 = new RooRealVar("alphacb1", "alpha crystalball 1", 13.52, 1., 35.);
    RooRealVar *alphacb2 = new RooRealVar("alphacb2", "alpha crystalball 2", 2.03, 1., 3.);
    RooRealVar *ncb1 = new RooRealVar("ncb1", "n crystalball 1", 1.04, 0.1, 25);
    RooRealVar *ncb2 = new RooRealVar("ncb2", "n crystalball 2", 1.38, 1., 5.);
    RooDoubleCBFast *doubleCB = new RooDoubleCBFast("doubleDB", "double sided crystal ball", m, *mcb, *scb, *alphacb1, *ncb1, *alphacb2, *ncb2);
    // construct breit wigner
    RooRealVar *mbw = new RooRealVar("mbw", "mean breit wigner", 91.18);
    RooRealVar *sbw = new RooRealVar("sbw", "width breit wigner", 2.495);//, 0., 2.);//, 0., 50.);
    RooBreitWigner *breitwigner = new RooBreitWigner("breitwigner", "Breit Wigner Peak", m, *mbw, *sbw);
    // convoluted Signal model    
    RooNumConvPdf *sigDCB = new RooNumConvPdf("sigDCB", "Z peak shape with double sided CB", m, *breitwigner, *doubleCB);
    
    // BACKGROUND
    // cms shape
    RooRealVar *alpha = new RooRealVar("alpha", "alpha", 87.96, 80, 100.);
    RooRealVar *beta = new RooRealVar("beta", "beta", 0.237, 0., 1.);
    RooRealVar *gamma = new RooRealVar("gamma", "gamma", 0.16, 0., 1.);
    RooRealVar *peak = new RooRealVar("peak", "peak", 37.3, 1., 100.);
    RooCMSShape *bkg = new RooCMSShape("bkg", "background shape", m, *alpha, *beta, *gamma, *peak);
    
    // fit model
    RooAddPdf *model = new RooAddPdf("model", "sig+bkg", RooArgList(*bkg, *sigDCB), RooArgList(*nbkg, *nsig));
  
  
  
*/  

/* - result is ok... left side doesnt fit really well
    0x70d2970 RooAddPdf::model = 1.34944 [Auto,Dirty] 
      0x70d1750/V- RooCMSShape::bkg = 0.0629399 [Auto,Dirty] 
        0x7ffd465543c0/V- RooRealVar::m = 90
        0x70cf4d0/V- RooRealVar::alpha = 85.1783 +/- 0.107384
        0x70cfd70/V- RooRealVar::beta = 1 +/- 0.0193112
        0x70d0610/V- RooRealVar::gamma = 0.0410973 +/- 0.00514861
        0x70d0eb0/V- RooRealVar::peak = 5.84067 +/- 20.5298
      0x6949730/V- RooRealVar::nbkg = 10000 +/- 14.2724
      0x70ce2d0/V- RooNumConvPdf::sigDCB = 1.39015 [Auto,Dirty] 
        0x7ffd465543c0/V- RooRealVar::m = 90
        0x70cc140/V- RooBreitWigner::breitwigner = 0.339138 [Auto,Dirty] 
          0x7ffd465543c0/V- RooRealVar::m = 90
          0x70cb100/V- RooRealVar::mbw = 91.18
          0x70cb920/V- RooRealVar::sbw = 2.495
        0x70b2700/V- RooDoubleCBFast::doubleDB = 0.000261001 [Auto,Dirty] 
          0x7ffd465543c0/V- RooRealVar::m = 90
          0x693c360/V- RooRealVar::mcb = -0.68
          0x70ca360/V- RooRealVar::scb = 1.47
          0x693db00/V- RooRealVar::alphacb1 = 35.52 +/- 0
          0x70b1480/V- RooRealVar::ncb1 = 0.185753 +/- 16.5695
          0x693e220/V- RooRealVar::alphacb2 = 2.03
          0x70b18a0/V- RooRealVar::ncb2 = 1.38
      0x70af5f0/V- RooRealVar::nsig = 316039 +/- 570.883




*/
    // extended likelihood fit: signal and background contribution
    RooRealVar *nbkg = new RooRealVar("nbkg", "Background events", 9900, 100, 10000);
    RooRealVar *nsig = new RooRealVar("nsig", "Signal events", 316000, 180000, 350000);
    
    // SIGNAL
    // double sided crystal ball
    RooRealVar *mcb = new RooRealVar("mcb", "mean crystalball", -0.68);//, -2., 0.);
    RooRealVar *scb = new RooRealVar("scb", "width crystalball", 1.47);//, 0.5, 4.);//, 0.5, 4.);
    RooRealVar *alphacb1 = new RooRealVar("alphacb1", "alpha crystalball 1", 35.52, 1., 100.);
    RooRealVar *alphacb2 = new RooRealVar("alphacb2", "alpha crystalball 2", 2.03);//, 1., 3.);
    RooRealVar *ncb1 = new RooRealVar("ncb1", "n crystalball 1", 0.185, 0.1, 25);
    RooRealVar *ncb2 = new RooRealVar("ncb2", "n crystalball 2", 1.38);//, 1., 5.);
    RooDoubleCBFast *doubleCB = new RooDoubleCBFast("doubleDB", "double sided crystal ball", m, *mcb, *scb, *alphacb1, *ncb1, *alphacb2, *ncb2);
    // breit wigner
    RooRealVar *mbw = new RooRealVar("mbw", "mean breit wigner", 91.18);
    RooRealVar *sbw = new RooRealVar("sbw", "width breit wigner", 2.495);//, 0., 2.);//, 0., 50.);
    RooBreitWigner *breitwigner = new RooBreitWigner("breitwigner", "Breit Wigner Peak", m, *mbw, *sbw);
    // convoluted Signal model    
    RooNumConvPdf *sigDCB = new RooNumConvPdf("sigDCB", "Z peak shape with double sided CB", m, *breitwigner, *doubleCB);
    
    // BACKGROUND
    // cms shape
    RooRealVar *alpha = new RooRealVar("alpha", "alpha", 85.18, 80, 100.);
    RooRealVar *beta = new RooRealVar("beta", "beta", 1., 0., 5.);
    RooRealVar *gamma = new RooRealVar("gamma", "gamma", 0.041, 0., 1.);
    RooRealVar *peak = new RooRealVar("peak", "peak", 5.84, 1., 100.);
    RooCMSShape *bkg = new RooCMSShape("bkg", "background shape", m, *alpha, *beta, *gamma, *peak);
    
    // fit model
    RooAddPdf *model = new RooAddPdf("model", "sig+bkg", RooArgList(*bkg, *sigDCB), RooArgList(*nbkg, *nsig));
    
    //RooAddPdf *model = new RooAddPdf("model", "sig+bkg", RooArgList(*bkg, *sig), *bkgfrac);
    // */
    
    return model;
} // */

RooAbsPdf* modelEEdata_sCB(RooRealVar& m){
    
    /* 
    RooRealVar *gmean = new RooRealVar("gmean", "gmean", 91., 88., 94.);
    RooRealVar *gsigma = new RooRealVar("gsigma", "gsigma", 3., 1., 20.);
    
    RooGaussian *gaus = new RooGaussian("gaus", "gaussian peak", m, *gmean, *gsigma);
    
    RooRealVar *alphacb = new RooRealVar("alphacb", "alpha crystalball", -1.51933);//, -3., -0.);
    RooRealVar *ncb = new RooRealVar("ncb", "n crystalball", 1.538, 0., 3.);
    
    RooRealVar *bkgfrac = new RooRealVar("bkgfrac", "fraction of background", 0.08);//, 0., 0.1);
    
    RooNumConvPdf *sig = new RooNumConvPdf("sig", "Z peak shape", m, *breitwigner, *crystalball);
    
    // construct crystal ball
    RooCBShape *crystalball = new RooCBShape("crystalball", "Crystal Ball Shape", m, *mcb, *scb, *alphacb, *ncb);
    
    
    
    RooNumConvPdf *gsig = new RooNumConvPdf("gsig", "gaussian peak * WB", m, *breitwigner, *gaus);
    
    RooNumConvPdf *gcb = new RooNumConvPdf("gcb", "gaussian * crystal ball", m, *gaus, *crystalball);
// */
     

    // extended likelihood fit: signal and background contribution
    RooRealVar *nbkg = new RooRealVar("nbkg", "Background events", 9900, 100, 10000);
    RooRealVar *nsig = new RooRealVar("nsig", "Signal events", 316000, 180000, 350000);
    
    // SIGNAL
    
    
    
    
    
    // double sided crystal ball
    RooRealVar *mcb = new RooRealVar("mcb", "mean crystalball", -0.68);//, -2., 0.);
    RooRealVar *scb = new RooRealVar("scb", "width crystalball", 1.47);//, 0.5, 4.);//, 0.5, 4.);
    RooRealVar *alphacb1 = new RooRealVar("alphacb1", "alpha crystalball 1", 35.52, 1., 100.);
    RooRealVar *alphacb2 = new RooRealVar("alphacb2", "alpha crystalball 2", 2.03);//, 1., 3.);
    RooRealVar *ncb1 = new RooRealVar("ncb1", "n crystalball 1", 0.185, 0.1, 25);
    RooRealVar *ncb2 = new RooRealVar("ncb2", "n crystalball 2", 1.38);//, 1., 5.);
    RooDoubleCBFast *doubleCB = new RooDoubleCBFast("doubleDB", "double sided crystal ball", m, *mcb, *scb, *alphacb1, *ncb1, *alphacb2, *ncb2);
    
    // single crystal ball
    RooRealVar *alphacb = new RooRealVar("alphacb", "alpha crystalball", -2.03);//, -3., -0.);
    RooRealVar *ncb = new RooRealVar("ncb", "n crystalball", 1.38);//, 0., 3.);
    RooCBShape *crystalball = new RooCBShape("crystalball", "Crystal Ball Shape", m, *mcb, *scb, *alphacb, *ncb);
    
    // breit wigner
    RooRealVar *mbw = new RooRealVar("mbw", "mean breit wigner", 91.18);
    RooRealVar *sbw = new RooRealVar("sbw", "width breit wigner", 2.495);//, 0., 2.);//, 0., 50.);
    RooBreitWigner *breitwigner = new RooBreitWigner("breitwigner", "Breit Wigner Peak", m, *mbw, *sbw);
    // convoluted Signal model    
    RooNumConvPdf *sigDCB = new RooNumConvPdf("sigDCB", "Z peak shape with double sided CB", m, *breitwigner, *doubleCB);
    RooNumConvPdf *sigSCB = new RooNumConvPdf("sigSCB", "Z peak shape with double sided CB", m, *breitwigner, *crystalball);
    
    // BACKGROUND
    // cms shape
    RooRealVar *alpha = new RooRealVar("alpha", "alpha", 85.18, 80, 100.);
    RooRealVar *beta = new RooRealVar("beta", "beta", 1., 0., 5.);
    RooRealVar *gamma = new RooRealVar("gamma", "gamma", 0.041, 0., 1.);
    RooRealVar *peak = new RooRealVar("peak", "peak", 5.84, 1., 100.);
    RooCMSShape *bkg = new RooCMSShape("bkg", "background shape", m, *alpha, *beta, *gamma, *peak);
    
    // fit model
    RooAddPdf *model = new RooAddPdf("model", "sig+bkg", RooArgList(*bkg, *sigDCB), RooArgList(*nbkg, *nsig));
    
    RooAddPdf *nmodel = new RooAddPdf("nmodel", "sig+bkg", RooArgList(*bkg, *sigSCB), RooArgList(*nbkg, *nsig));
    
    //RooAddPdf *model = new RooAddPdf("model", "sig+bkg", RooArgList(*bkg, *sig), *bkgfrac);
    // */
    
    return nmodel;
} // */

/**********************************************************************************************
 * fits and stuff
 * 
 ***/

void test(){
    
    //TTemplateFit *tfit = new TTemplateFit("tfit", "a template fit object");
    
    
    //tfit->setK(6);
    
    //cout << "setted k: " << tfit->getK() << endl;
    
    
    //tfit->setK(8);
    
    //cout << "tfit->getK = " << tfit->getK() << endl;
    
    
    //RooAddPdf* t;
    
    //cout << "t: " << t << endl;
    
    //if(t==0) cout << "its null!" << endl;
    
    //t = new RooAddPdf("t", "adding pdfs", 
    
    //if(*t) cout << "t!" << endl;
    
    //if(!(*t)) cout << "no t!" << endl;
    
    
    return;
    
    
}

void FitterEG(string dataset){
    cout << "rooFitHeader.FitterEG()" << endl;
    
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
    
    cout << "Nee = " << Nee << endl;
    cout << "Neg = " << Neg << endl;
    
    
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
    
    // pull
    RooHist* hpull_eg = frame_eg->pullHist() ;
    RooPlot* frame_eg3 = m.frame(Title("Pull Distribution")) ;
    frame_eg3->addPlotable((RooPlotable*)hpull_eg,"P") ;
    
/*
    
    // residual:
    RooHist *hresid_eg = frame_eg->residHist();
    RooPlot* frame_eg2 = m.frame(Title("Residual Distribution")) ;
    frame_eg2->addPlotable((RooPlotable*)hresid_eg,"P") ;
    
    
    
    // get models
    RooArgSet* comps_eg = model_eg->getComponents();
    RooAbsPdf* sig_eg   = (RooAbsPdf*)comps_eg->find("sig");
    
    RooAbsReal* N_sig_eg = sig_eg->createIntegral(m) ;
*/
    
   
    
    model_eg->plotOn(frame_eg, Components("bkg"), LineColor(kBlue), LineStyle(kDashed));
    //model_eg->plotOn(frame_eg, Components("sig"), LineColor(8), LineStyle(kDashed)); // smooth green
    
    
    
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
    
    
    int csize = 600;
    TCanvas *c = new TCanvas("c", "c", csize, csize);
    c->cd();
    c->SetLeftMargin(0.19);
    frame_eg->GetYaxis()->SetTitleOffset(2.2);
    frame_eg->GetYaxis()->SetTitle("Events / (0.5 GeV)");
    frame_eg->Draw();
    
    // plot path
    string filepath, dropbox;
    SystemPath(filepath, dropbox);
    string plotpath = dropbox + "code/rooFit/plot/";
    cout << "filepath: " << filepath << endl;
    cout << "dropbox:  " << dropbox << endl;
    
    string stime = "_"+NumberToString(time(0));
    TFile *fsave = new TFile((plotpath+"simulation_fits.root").c_str(), "UPDATE");
    
    model_eg->Write(("eg_model"+stime).c_str());
    c2->Write(("eg_plotCanvas"+stime).c_str());
    
    c->Write(("eg_plotCanvas_noRatio"+stime).c_str());   
    fsave->Close();
    
    
    c2->Close();
    
    
    /*
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
    
    cout << "Nee = " << Nee << endl;
    cout << "Neg = " << Neg << endl;
    
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
    RooAbsPdf *model_ee = modelEE_DCB(m);
    //model_ee->plotOn(frame_ee, LineColor(kGray), LineStyle(kDashed));
    
    // * ********************************************************************* 
    // * FIT with log likelihood
    // * 
    model_ee->fitTo(data_ee, Minimizer("migrad"));
    
    // complete plot
    model_ee->plotOn(frame_ee, LineColor(kBlue));
    
    
    // residual:
    //RooHist *hresid_ee = frame_ee->residHist();
    //RooPlot* frame_ee2 = m.frame(Title("Residual Distribution")) ;
    //frame_ee2->addPlotable((RooPlotable*)hresid_ee,"P") ;
    
    // pull
    RooHist* hpull_ee = frame_ee->pullHist() ;
    RooPlot* frame_ee3 = m.frame(Title("Pull Distribution")) ;
    frame_ee3->addPlotable((RooPlotable*)hpull_ee,"P") ;
    
    // get models
    //RooArgSet* comps_ee = model_ee->getComponents();
    //RooAbsPdf* sig_ee   = (RooAbsPdf*)comps_ee->find("sig");
    
    //RooAbsReal* N_sig_ee = sig_ee->createIntegral(m) ;
    
    //RooArgSet* sigVars_ee = sig_ee->getVariables();
    
    //RooAbsArg* bkg_ee = comps_ee->find("bkg");
    //RooArgSet* bkgVars_ee = bkg_ee->getVariables();
    
    //RooAbsArg *bkgfrac_ee = comps_ee->find("bkgfrac");
    
    //sigVars_ee->Print();
    
    // model plots
    model_ee->plotOn(frame_ee, Components("bkg"), LineColor(kBlue), LineStyle(kDashed));
    //model_ee->plotOn(frame_ee, Components("sig"), LineColor(8), LineStyle(kDashed)); // smooth green
    
    //sig.fitTo(data_ee);
    //sig.plotOn(frame, LineColor(kBlue), LineStyle(kDashed));
    
    
    //RooKeysPdf kest2("kest2", "kest2", m, data_ee, RooKeysPdf::NoMirror) ;
    //kest2->plotOn(frame_ee, LineColor(kRed), LineStyle(kDashed));
    
    
    //*******************************************************************************************
    /// plots
    //
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
    
    
    
    // path
    string filepath, dropbox;
    SystemPath(filepath, dropbox);
    string plotpath = dropbox + "code/rooFit/plot/";
    
    int csize = 600;
    TCanvas *c = new TCanvas("c", "c", csize, csize);
    c->cd();
    c->SetLeftMargin(0.19);
    frame_ee->GetYaxis()->SetTitleOffset(2.2);
    frame_ee->GetYaxis()->SetTitle("Events / (0.5 GeV)");
    frame_ee->Draw();
    
    string stime = "_"+NumberToString(time(0));
    TFile *fsave = new TFile((plotpath+"simulation_fits.root").c_str(), "UPDATE");    
    model_ee->Write(("ee_model"+stime).c_str());
    c1->Write(("ee_plotCanvas"+stime).c_str()); 
    
    c->Write(("ee_plotCanvas_noRatio"+stime).c_str());
    fsave->Close();
    
    c1->Close();
    
    
    
    
    model_ee->printCompactTree();
    
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
    
    // */
    
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

void FitterEGdata(string dataset){
    cout << "rooFitHeader.FitterEGdata()" << endl;
    
    TFile *file = new TFile(dataset.c_str(), "READ");
    
    TH2F *ee_dist = (TH2F*) file->Get("tnp_ee_b2b");
    TH2F *eg_dist = (TH2F*) file->Get("tnp_eg_b2b");
    
    TH1F *ee = (TH1F*) ee_dist->ProjectionX();
    TH1F *eg = (TH1F*) eg_dist->ProjectionX();
    //eg->ProjectionX();
    
    TH1F *ee_n = (TH1F*) ee->Clone();
    TH1F *eg_n = (TH1F*) eg->Clone();
    
    //float Nee = ee_n->Integral();
    //float Neg = eg_n->Integral();
    
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
    
    cout << "Nee = " << Nee << endl;
    cout << "Neg = " << Neg << endl;

    //getchar();
    
    ee->Rebin(5);
    eg->Rebin(5);
    
    
    // define observable
    RooRealVar m("m", "m_{e#gamma} (GeV)", 75., 105.);
    
    /// *******************************************************************************************
    /// eg
    
    // create binned dataset that imports the histogram
    RooDataHist data_eg("dh_eg", "eg sample", m, Import(*eg));
    
    // plot binned dataset
    RooPlot* frame_eg = m.frame(Title("e#gamma invariant mass distribution"));
    data_eg.plotOn(frame_eg);
    
    // construct fit model
    RooAbsPdf *model_eg = modelEGdata(m);
    //model_eg->plotOn(frame_eg, LineColor(kGray), LineStyle(kDashed));
    
    /* ********************************************************************* 
     * FIT with log likelihood
     * */
    model_eg->fitTo(data_eg);//, Minimizer("migrad"));
    
    
    // model plots
    model_eg->plotOn(frame_eg, LineColor(kBlue));
    
    // */
    
    /*
    // residual:
    RooHist *hresid_eg = frame_eg->residHist();
    RooPlot* frame_eg2 = m.frame(Title("Residual Distribution")) ;
    frame_eg2->addPlotable((RooPlotable*)hresid_eg,"P") ;
    */
    // pull
    RooHist* hpull_eg = frame_eg->pullHist() ;
    RooPlot* frame_eg3 = m.frame(Title("Pull Distribution")) ;
    frame_eg3->addPlotable((RooPlotable*)hpull_eg,"P") ;
    
    
    
    
    // get models
    //RooArgSet* comps_eg = model_eg->getComponents();
    //RooAbsPdf* sig_eg   = (RooAbsPdf*)comps_eg->find("sig");
    
    //RooAbsReal* N_sig_eg = sig_eg->createIntegral(m) ;
    
    
    
    
    model_eg->plotOn(frame_eg, Components("bkg"), LineColor(kBlue), LineStyle(kDashed));
    //model_eg->plotOn(frame_eg, Components("sigDCB"), LineColor(8), LineStyle(kDashed)); // smooth green
    // */
    
    //getchar();
    
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
    
    
    int csize = 600;
    TCanvas *c = new TCanvas("c", "c", csize, csize);
    c->cd();
    c->SetLeftMargin(0.19);
    frame_eg->GetYaxis()->SetTitleOffset(2.2);
    frame_eg->GetYaxis()->SetTitle("Events / (0.5 GeV)");
    frame_eg->Draw();
    
    // plot path
    string filepath, dropbox;
    SystemPath(filepath, dropbox);
    string plotpath = dropbox + "code/rooFit/plot/";
    cout << "filepath: " << filepath << endl;
    cout << "dropbox:  " << dropbox << endl;
    
    string stime = "_"+NumberToString(time(0));
    TFile *fsave = new TFile((plotpath+"data_fits.root").c_str(), "UPDATE");
    
    model_eg->Write(("eg_model"+stime).c_str());
    c2->Write(("eg_plotCanvas"+stime).c_str());
    
    c->Write(("eg_plotCanvas_noRatio"+stime).c_str());   
    fsave->Close();
    
    
    c2->Close();
    
    
/*    
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
*/
 
    
// */
}

void FitterEEdata(string dataset){
    cout << "rooFitHeader.FitterEEdata()" << endl;
    
    TFile *file = new TFile(dataset.c_str(), "READ");
    
    TH2F *ee_dist = (TH2F*)file->Get("tnp_ee_b2b")->Clone();
    TH2F *eg_dist = (TH2F*) file->Get("tnp_eg_b2b")->Clone();
    
    //file->Close();
    //delete file;
    
    
    TH1F *ee = (TH1F*) ee_dist->ProjectionX();
    TH1F *eg = (TH1F*) eg_dist->ProjectionX();
    //eg->ProjectionX();
    
    // denominator sample: add ee and eg
    (*ee).Add(eg);
    
    TH1F *ee_n = (TH1F*) ee->Clone();
    TH1F *eg_n = (TH1F*) eg->Clone();
    
    ee_n->Scale(1./ee_n->Integral());
    eg_n->Scale(1./eg_n->Integral());
    
    ee_n->Rebin(10);
    eg_n->Rebin(10);
    
    eg_n->SetLineColor(kRed);
    
    float Nee = ee->GetEntries();
    float Neg = eg->GetEntries();
    
    cout << "Nee = " << Nee << endl;
    cout << "Neg = " << Neg << endl;
    
    
    // define observable
    RooRealVar m("m", "m_{ee} (GeV)", 75., 105.);    
    
    /// *******************************************************************************************
    /// ee
    ee->Rebin(5);
    
    // create binned dataset that imports the histogram
    RooDataHist data_ee("dh_ee", "ee sample", m, Import(*ee));
    
    // plot binned dataset
    RooPlot* frame_ee = m.frame(Title("ee invariant mass distribution"));
    data_ee.plotOn(frame_ee);
    
    
    // construct fit model
    RooAbsPdf *model_ee = modelEEdata(m);
    //model_ee->plotOn(frame_ee, LineColor(kGray), LineStyle(kDashed));
    
    // * ********************************************************************* 
    // * FIT with log likelihood
    // * 
    //model_ee->fitTo(data_ee);//, Minimizer("migrad"));
    
    // complete plot
    model_ee->plotOn(frame_ee, LineColor(kBlue));
    
    
    // residual:
    //RooHist *hresid_ee = frame_ee->residHist();
    //RooPlot* frame_ee2 = m.frame(Title("Residual Distribution")) ;
    //frame_ee2->addPlotable((RooPlotable*)hresid_ee,"P") ;
    
    // pull
    RooHist* hpull_ee = frame_ee->pullHist() ;
    RooPlot* frame_ee3 = m.frame(Title("Pull Distribution")) ;
    frame_ee3->addPlotable((RooPlotable*)hpull_ee,"P") ;
    
    // get models
    //RooArgSet* comps_ee = model_ee->getComponents();
    //RooAbsPdf* sig_ee   = (RooAbsPdf*)comps_ee->find("sig");
    
    //RooAbsReal* N_sig_ee = sig_ee->createIntegral(m) ;
    
    //RooArgSet* sigVars_ee = sig_ee->getVariables();
    
    //RooAbsArg* bkg_ee = comps_ee->find("bkg");
    //RooArgSet* bkgVars_ee = bkg_ee->getVariables();
    
    //RooAbsArg *bkgfrac_ee = comps_ee->find("bkgfrac");
    
    //sigVars_ee->Print();
    
    // model plots
    model_ee->plotOn(frame_ee, Components("bkg"), LineStyle(kDashed), LineColor(kBlue));
    //model_ee->plotOn(frame_ee, Components("sigDCB"), LineColor(8), LineStyle(kDashed)); // smooth green
    
    //sig.fitTo(data_ee);
    //sig.plotOn(frame, LineColor(kBlue), LineStyle(kDashed));
    
    
    //RooKeysPdf kest2("kest2", "kest2", m, data_ee, RooKeysPdf::NoMirror) ;
    //kest2->plotOn(frame_ee, LineColor(kRed), LineStyle(kDashed));
    
    
    /// *******************************************************************************************
    /// plots
    //
    int csize = 600;
    TCanvas *c1 = new TCanvas("c1", "c1", csize, csize);
    
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
    
    
    // path
    string filepath, dropbox;
    SystemPath(filepath, dropbox);
    string plotpath = dropbox + "code/rooFit/plot/";
    cout << "filepath: " << filepath << endl;
    cout << "dropbox:  " << dropbox << endl;
    
    
    
    
    TCanvas *c = new TCanvas("c", "c", csize, csize);
    c->cd();
    c->SetLeftMargin(0.19);
    frame_ee->GetYaxis()->SetTitleOffset(2.2);
    frame_ee->GetYaxis()->SetTitle("Events / (0.5 GeV)");
    frame_ee->Draw();
    
    string stime = "_"+NumberToString(time(0));
    TFile *fsave = new TFile((plotpath+"data_fits.root").c_str(), "UPDATE");    
    model_ee->Write(("ee_model"+stime).c_str());
    c1->Write(("ee_plotCanvas"+stime).c_str()); 
    
    c->Write(("ee_plotCanvas_noRatio"+stime).c_str());
    fsave->Close();
    //*/
    c1->Close();
    model_ee->printCompactTree();
    
    
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
    
    // */
    
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


void templateKernel(string signal, string background, string dataset){
    
    cout << "rooFitHeader.templateKernel()" << endl;
    
    // cross section for DYJetsToLL in pb at NLO:
    float cs = 6025.2;
    // lumi for data in inverse pb (3.81 fb^-1)
    float lum = 3.81e3;
    // total amount of data in DYJetsToLL:
    float Nmc = 7375344;
    // show scale factor
    cout << "Scale factor: " << lum*cs/Nmc << endl;
    
    // templates and data files
    TFile *fSig = new TFile(signal.c_str(), "READ");
    TFile *fBkg = new TFile(background.c_str(), "READ");
    TFile *file = new TFile(dataset.c_str(), "READ");
    
    // read out histograms
    
    // data: 
    TH2F *ee_dist = (TH2F*) file->Get("tnp_ee")->Clone("ee_data");
    TH2F *eg_dist = (TH2F*) file->Get("tnp_eg")->Clone("eg_data");
    
    // background template:
    TH2F *ee_bkg = (TH2F*) fBkg->Get("muon_tnp_ee")->Clone("ee_bkg");
    TH2F *eg_bkg = (TH2F*) fBkg->Get("muon_tnp_eg")->Clone("eg_bkg");
    
    // signal template:
    TH2F *ee_sig = (TH2F*) fSig->Get("tnp_ee")->Clone("ee_sig");
    TH2F *eg_sig = (TH2F*) fSig->Get("tnp_eg")->Clone("eg_sig");
    
    // projections
    map<string,TH1F*> h;
    
    h["ee"] = (TH1F*) ee_dist->ProjectionX();
    h["eg"] = (TH1F*) eg_dist->ProjectionX();
    h["ee_bkg"] = (TH1F*) ee_bkg->ProjectionX();
    h["eg_bkg"] = (TH1F*) eg_bkg->ProjectionX();
    h["ee_sig"] = (TH1F*) ee_sig->ProjectionX();
    h["eg_sig"] = (TH1F*) eg_sig->ProjectionX();
    h["ee_sig+bkg"] = (TH1F*) h["ee_sig"]->Clone("ee_sig+bkg");
    h["ee_sig+bkg"]->Add(h["ee_bkg"]);
    h["eg_sig+bkg"] = (TH1F*) h["eg_sig"]->Clone("eg_sig+bkg");
    h["eg_sig+bkg"]->Add(h["eg_bkg"]);
    
    
    h["ee_bkg_fit"] = (TH1F*) h["ee_bkg"]->Clone("ee_bkg_fit");
    h["eg_bkg_fit"] = (TH1F*) h["eg_bkg"]->Clone("eg_bkg_fit");
    h["ee_sig_fit"] = (TH1F*) h["ee_sig"]->Clone("ee_sig_fit");
    h["eg_sig_fit"] = (TH1F*) h["eg_sig"]->Clone("eg_sig_fit");
    
    
    // rebin
    for(map<string, TH1F*>::iterator it = h.begin(); it != h.end(); it++){
        // it->first = key
        // it->second = value
        // cout << it->first << endl;
        it->second->Rebin(10);
        it->second->SetLineWidth(2);
    }
    
    // scale mc by L(data)/N(mc)*sigma(mc)
    h["ee_sig"]->Scale(lum*cs/Nmc);
    h["eg_sig"]->Scale(lum*cs/Nmc);
    
    
    
    //h["ee_sig+bkg"] = (TH1F*) h["ee_sig"]->Clone();
    //h["eg_sig+bkg"]->Add(h["ee_bkg"]);
    
    h["ee"]->SetLineColor(kBlue);
    h["ee_sig"]->SetLineColor(kRed);
    h["ee_bkg"]->SetLineColor(kGray);
    h["ee_bkg"]->SetFillColor(kGray);
    h["eg"]->SetLineColor(kBlue);
    h["eg_sig"]->SetLineColor(kRed);
    h["eg_bkg"]->SetLineColor(kGray);
    
    h["ee_sig+bkg"]->SetLineColor(kMagenta);
    h["eg_sig+bkg"]->SetLineColor(kMagenta);
    
    
/*
    TCanvas *c1 = new TCanvas("c1", "c1", 1200, 1000);
    //c1->SetLogy();
    c1->Divide(2, 2);
    c1->cd(1);
    
    float offx=-0.1, offy=-0.3;
    TLegend *leg = new TLegend(0.6+offx, 0.75+offy, 0.97+offx, 0.97+offy);
    leg->AddEntry(h["ee"], "Data", "l");
    leg->AddEntry(h["ee_sig"], "Sig template", "l");
    leg->AddEntry(h["ee_bkg"], "Bkg template", "l");
    leg->AddEntry(h["ee_sig+bkg"], "Sig+Bkg", "l");
    
    //gPad->SetLogy();
    h["ee"]->Draw("hist");
    h["ee_sig"]->Draw("hist same");
    h["ee_bkg"]->Draw("hist same");
    leg->Draw();
    
    
    c1->cd(2);
    //gPad->SetLogy();
    h["eg_sig"]->Draw("hist");
    h["eg"]->Draw("hist same");
    h["eg_bkg"]->Draw("hist same");
    
    
    c1->cd(3);
    h["ee_sig+bkg"]->Draw("hist");
    h["ee"]->Draw("hist same");
    
    h["ee_bkg"]->Draw("hist same");
    
    c1->cd(4);
    //gPad->SetLogy();
    h["eg"]->Draw("hist");
    h["eg_sig+bkg"]->Draw("hist same");
    h["eg_bkg"]->Draw("hist same");
*/
    
    
    // RooFit **********************************************************************
    
    // define observable
    RooRealVar m("m", "Invariant Mass", 60., 120.);
    
    
    RooPlot *frame11 = m.frame(Title("templates (eg)"));
    RooPlot *frame21 = m.frame(Title("p.d.f. (eg)"));
    RooPlot *frame31 = m.frame(Title("fit to data (eg)"));
    
    RooPlot *frame12 = m.frame(Title("templates (ee)"));
    RooPlot *frame22 = m.frame(Title("p.d.f. (ee)"));
    RooPlot *frame32 = m.frame(Title("fit to data (ee)"));
    
    
    
    // create binned datasets from the templates
    map<string,RooDataHist*> rdh;
    rdh["eg"] = new RooDataHist("eg", "eg", m, Import(*h["eg"]));
    
    
    RooDataHist data_eg("data_eg", "data eg", m, Import(*h["eg"]));
    RooDataHist data_ee("data_ee", "data ee", m, Import(*h["ee"]));
    
    RooDataHist t_ee_sig("t_ee_sig", "ee sig template", m, Import(*h["ee_sig"]));
    RooDataHist t_ee_bkg("t_ee_bkg", "ee bkg template", m, Import(*h["ee_bkg"]));
    
    RooDataHist t_eg_sig("t_eg_sig", "eg sig template", m, Import(*h["eg_sig"]));
    RooDataHist t_eg_bkg("t_eg_bkg", "eg bkg template", m, Import(*h["eg_bkg"]));
    
    t_eg_sig.plotOn(frame11, MarkerColor(kRed));
    t_eg_bkg.plotOn(frame11, MarkerColor(kGreen+2));
    t_ee_sig.plotOn(frame12, MarkerColor(kRed));
    t_ee_bkg.plotOn(frame12, MarkerColor(kGreen+2));
    
    
    
    // create binned dataset for p.d.f.
    m.setBins(20);
    RooDataHist* hist_eg_s = ((RooDataSet*) &t_eg_sig)->binnedClone() ;
    RooDataHist* hist_ee_s = ((RooDataSet*) &t_ee_sig)->binnedClone() ;
    m.setBins(20);
    RooDataHist* hist_eg_b = ((RooDataSet*) &t_eg_bkg)->binnedClone() ;
    RooDataHist* hist_ee_b = ((RooDataSet*) &t_ee_bkg)->binnedClone() ;
    
    // represent binned datasets as p.d.f. with 2nd order interpolation
    RooHistPdf pdf_eg_s("pdf_eg_s","pdf eg sig",m,*hist_eg_s,2) ;
    RooHistPdf pdf_eg_b("pdf_eg_b","pdf eg bkg",m,*hist_eg_b,2) ;
    RooHistPdf pdf_ee_s("pdf_ee_s","pdf ee sig",m,*hist_ee_s,2) ;
    RooHistPdf pdf_ee_b("pdf_ee_b","pdf ee bkg",m,*hist_ee_b,2) ;
    
    // kernel estimation
    RooKeysPdf kest_ee_sig("kest_ee_sig","kest_ee_sig",m,*((RooDataSet*)&t_ee_sig),RooKeysPdf::MirrorBoth) ;
    RooKeysPdf kest_eg_sig("kest_eg_sig","kest_eg_sig",m,*((RooDataSet*)&t_eg_sig),RooKeysPdf::MirrorBoth) ;
    RooKeysPdf kest_ee_bkg("kest_ee_bkg","kest_ee_bkg",m,*((RooDataSet*)&t_ee_bkg),RooKeysPdf::MirrorBoth) ;
    RooKeysPdf kest_eg_bkg("kest_eg_bkg","kest_eg_bkg",m,*((RooDataSet*)&t_eg_bkg),RooKeysPdf::MirrorBoth) ;
    
    
    
    pdf_eg_s.plotOn(frame21, LineColor(kRed));
    pdf_eg_b.plotOn(frame21, LineColor(kGreen+2));
    pdf_ee_s.plotOn(frame22, LineColor(kRed));
    pdf_ee_b.plotOn(frame22, LineColor(kGreen+2));
    
    
    
    
    // smearing signal with gaussian
    RooRealVar *m_ee = new RooRealVar("m_ee", "mean ee smearing", 90., 0., 100.);
    RooRealVar *s_ee = new RooRealVar("s_ee", "width ee smearing", 3., 0.1, 10.);
    RooRealVar *m_eg = new RooRealVar("m_eg", "mean eg smearing", 90., 0., 100.);
    RooRealVar *s_eg = new RooRealVar("s_eg", "width eg smearing", 3., 0.1, 10.);
    
    RooGaussian *smear_ee = new RooGaussian("smear_ee", "ee gaussian smearing", m, *m_ee, *s_ee);
    RooGaussian *smear_eg = new RooGaussian("smear_eg", "eg gaussian smearing", m, *m_eg, *s_eg);
    
    RooNumConvPdf *sig_ee = new RooNumConvPdf("sig_ee", "smeared signal ee", m, pdf_ee_s, *smear_ee);
    RooNumConvPdf *sig_eg = new RooNumConvPdf("sig_eg", "smeared signal eg", m, pdf_eg_s, *smear_eg);
    
    
    
    RooRealVar *nbkg_eg = new RooRealVar("nbkg", "background events", 2000, 100, 9000);
    RooRealVar *nsig_eg = new RooRealVar("nsig", "signal events", 13000, 5000, 15000);
    RooRealVar *nbkg_ee = new RooRealVar("nbkg", "background events", 9000, 100, 5000);
    RooRealVar *nsig_ee = new RooRealVar("nsig", "signal events", 40000, 1000, 500000);
    
    
    RooAddPdf *model_eg = new RooAddPdf("model_eg", "eg sig+bkg", RooArgList(pdf_eg_b, pdf_eg_s), RooArgList(*nbkg_eg, *nsig_eg));
    RooAddPdf *model_ee = new RooAddPdf("model_ee", "ee sig+bkg", RooArgList(pdf_ee_b, pdf_ee_s), RooArgList(*nbkg_ee, *nsig_ee));
    //RooAddPdf *model_ee = new RooAddPdf("model_ee", "ee sig+bkg", RooArgList(pdf_ee_b, *sig_ee), RooArgList(*nbkg_ee, *nsig_ee));
    
    //RooAddPdf *model_eg = new RooAddPdf("model_eg", "eg sig+bkg", RooArgList(pdf_eg_b, pdf_eg_s), RooArgList(*nbkg_eg, *nsig_eg));
    //RooAddPdf *model_ee = new RooAddPdf("model_ee", "ee sig+bkg", RooArgList(pdf_ee_b, pdf_ee_s), RooArgList(*nbkg_ee, *nsig_ee));
    
    
    // fit to data
    model_eg->fitTo(data_eg);
    model_ee->fitTo(data_ee);
    
    data_eg.plotOn(frame31);
    model_eg->plotOn(frame31);
    
    RooHist* hpull_eg = frame31->pullHist() ;
    RooPlot* fpull_eg = m.frame(Title("Pull Distribution")) ;
    fpull_eg->addPlotable((RooPlotable*)hpull_eg,"P") ;
    
    model_eg->plotOn(frame31, Components("pdf_eg_b"), LineStyle(kDashed));
    
    
    
    data_ee.plotOn(frame32);
    model_ee->plotOn(frame32);
    
    RooHist* hpull_ee = frame32->pullHist() ;
    RooPlot* fpull_ee = m.frame(Title("Pull Distribution")) ;
    fpull_ee->addPlotable((RooPlotable*)hpull_ee,"P") ;
    
    model_ee->plotOn(frame32, Components("pdf_ee_b"), LineStyle(kDashed));
    
    
    TCanvas *c2 = new TCanvas("c2", "c2", 1700, 1000);
    //c1->SetLogy();
    c2->Divide(3, 2);
    c2->cd(1);
    
    gPad->SetLeftMargin(0.15) ; 
    frame11->GetYaxis()->SetTitleOffset(1.4) ; 
    frame11->Draw() ;
    
    
    c2->cd(2);
    
    gPad->SetLeftMargin(0.15) ; 
    frame21->GetYaxis()->SetTitleOffset(1.4) ; 
    frame21->Draw();
    
    
    c2->cd(3);
    
    TPad *pad30 = new TPad("pad30", "pad30", 0, 0.3, 1, 1);
    
    pad30->SetBottomMargin(0.1);
    pad30->SetLeftMargin(0.16);
    pad30->Draw();
    pad30->cd();
    
    frame31->GetYaxis()->SetTitleOffset(1.6);
    frame31->Draw();
    
    pad30->Update();
    
    c2->cd(3);
    
    TPad *pad30p = new TPad("pad30p", "pad30p", 0, 0, 1, 0.3);
    pad30p->SetTopMargin(0);
    pad30p->SetBottomMargin(0.11);
    pad30p->SetGridy();
    pad30p->SetLeftMargin(0.16);
    pad30p->Draw();
    pad30p->cd();
    
    fpull_eg->GetYaxis()->SetTitle("#frac{data-fit}{#sigma_{data}}");
    fpull_eg->GetYaxis()->SetTitleOffset(0.5);
    fpull_eg->GetYaxis()->SetTitleSize(0.11);
    fpull_eg->GetYaxis()->SetNdivisions(5);
    fpull_eg->GetYaxis()->SetLabelSize(0.08);
    
    fpull_eg->GetXaxis()->SetTitle("");
    fpull_eg->GetXaxis()->SetLabelSize(0.0);
    fpull_eg->SetTitle("");
    fpull_eg->Draw();
    
    
    pad30p->Update();
    
    
    
    
    c2->cd(4);
    
    gPad->SetLeftMargin(0.15) ; 
    frame12->GetYaxis()->SetTitleOffset(1.4) ; 
    frame12->Draw() ;
    
    
    c2->cd(5);
    
    gPad->SetLeftMargin(0.15) ; 
    frame22->GetYaxis()->SetTitleOffset(1.4) ; 
    frame22->Draw();
    
    
    c2->cd(6);
    
    TPad *pad31 = new TPad("pad31", "pad31", 0, 0.3, 1, 1);
    
    pad31->SetBottomMargin(0.1);
    pad31->SetLeftMargin(0.16);
    pad31->Draw();
    pad31->cd();
    
    frame32->GetYaxis()->SetTitleOffset(1.6);
    frame32->Draw();
    
    pad31->Update();
    
    c2->cd(6);
    
    TPad *pad32p = new TPad("pad32", "pad32", 0, 0, 1, 0.3);
    pad32p->SetTopMargin(0);
    pad32p->SetBottomMargin(0.11);
    pad32p->SetGridy();
    pad32p->SetLeftMargin(0.16);
    pad32p->Draw();
    pad32p->cd();
    
    fpull_ee->GetYaxis()->SetTitle("#frac{data-fit}{#sigma_{data}}");
    fpull_ee->GetYaxis()->SetTitleOffset(0.5);
    fpull_ee->GetYaxis()->SetTitleSize(0.11);
    fpull_ee->GetYaxis()->SetNdivisions(5);
    fpull_ee->GetYaxis()->SetLabelSize(0.08);
    
    fpull_ee->GetXaxis()->SetTitle("");
    fpull_ee->GetXaxis()->SetLabelSize(0.0);
    fpull_ee->SetTitle("");
    fpull_ee->Draw();
    
    
    pad32p->Update();
    
    
    
    
    
    
    cout << "fit results eg: " << endl;
    model_eg->printCompactTree();
    
    cout << "fit results ee: " << endl;
    model_ee->printCompactTree();
    
    
    cout << "finished!" << endl;
    return;
}

void templateKernelTest(string signal, string background, string dataset){
    
    cout << "rooFitHeader.templateKernel()" << endl;
    
    // cross section for DYJetsToLL in pb at NLO:
    float cs = 6025.2;
    // lumi for data in inverse pb (3.81 fb^-1)
    float lum = 3.81e3;
    // total amount of data in DYJetsToLL:
    float Nmc = 7375344;
    // show scale factor
    cout << "Scale factor: " << lum*cs/Nmc << endl;
    
    // templates and data files
    TFile *fSig = new TFile(signal.c_str(), "READ");
    TFile *fBkg = new TFile(background.c_str(), "READ");
    TFile *file = new TFile(dataset.c_str(), "READ");
    
    // read out histograms
    
    // data: 
    TH2F *ee_dist = (TH2F*) file->Get("tnp_ee")->Clone("ee_data");
    TH2F *eg_dist = (TH2F*) file->Get("tnp_eg")->Clone("eg_data");
    
    // background template:
    TH2F *ee_bkg = (TH2F*) fBkg->Get("muon_tnp_ee")->Clone("ee_bkg");
    TH2F *eg_bkg = (TH2F*) fBkg->Get("muon_tnp_eg")->Clone("eg_bkg");
    
    // signal template:
    TH2F *ee_sig = (TH2F*) fSig->Get("tnp_ee")->Clone("ee_sig");
    TH2F *eg_sig = (TH2F*) fSig->Get("tnp_eg")->Clone("eg_sig");
    
    // projections
    map<string,TH1F*> h;
    
    h["ee"] = (TH1F*) ee_dist->ProjectionX();
    h["eg"] = (TH1F*) eg_dist->ProjectionX();
    h["ee_bkg"] = (TH1F*) ee_bkg->ProjectionX();
    h["eg_bkg"] = (TH1F*) eg_bkg->ProjectionX();
    h["ee_sig"] = (TH1F*) ee_sig->ProjectionX();
    h["eg_sig"] = (TH1F*) eg_sig->ProjectionX();
    h["ee_sig+bkg"] = (TH1F*) h["ee_sig"]->Clone("ee_sig+bkg");
    h["ee_sig+bkg"]->Add(h["ee_bkg"]);
    h["eg_sig+bkg"] = (TH1F*) h["eg_sig"]->Clone("eg_sig+bkg");
    h["eg_sig+bkg"]->Add(h["eg_bkg"]);
    
    
    h["ee_bkg_fit"] = (TH1F*) h["ee_bkg"]->Clone("ee_bkg_fit");
    h["eg_bkg_fit"] = (TH1F*) h["eg_bkg"]->Clone("eg_bkg_fit");
    h["ee_sig_fit"] = (TH1F*) h["ee_sig"]->Clone("ee_sig_fit");
    h["eg_sig_fit"] = (TH1F*) h["eg_sig"]->Clone("eg_sig_fit");
    
    
    // rebin
    for(map<string, TH1F*>::iterator it = h.begin(); it != h.end(); it++){
        // it->first = key
        // it->second = value
        // cout << it->first << endl;
        it->second->Rebin(20);
        it->second->SetLineWidth(2);
    }
    
    // scale mc by L(data)/N(mc)*sigma(mc)
    h["ee_sig"]->Scale(lum*cs/Nmc);
    h["eg_sig"]->Scale(lum*cs/Nmc);
    
    
    
    //h["ee_sig+bkg"] = (TH1F*) h["ee_sig"]->Clone();
    //h["eg_sig+bkg"]->Add(h["ee_bkg"]);
    
    h["ee"]->SetLineColor(kBlue);
    h["ee_sig"]->SetLineColor(kRed);
    h["ee_bkg"]->SetLineColor(kGray);
    h["ee_bkg"]->SetFillColor(kGray);
    h["eg"]->SetLineColor(kBlue);
    h["eg_sig"]->SetLineColor(kRed);
    h["eg_bkg"]->SetLineColor(kGray);
    
    h["ee_sig+bkg"]->SetLineColor(kMagenta);
    h["eg_sig+bkg"]->SetLineColor(kMagenta);
    
    
/*
    TCanvas *c1 = new TCanvas("c1", "c1", 1200, 1000);
    //c1->SetLogy();
    c1->Divide(2, 2);
    c1->cd(1);
    
    float offx=-0.1, offy=-0.3;
    TLegend *leg = new TLegend(0.6+offx, 0.75+offy, 0.97+offx, 0.97+offy);
    leg->AddEntry(h["ee"], "Data", "l");
    leg->AddEntry(h["ee_sig"], "Sig template", "l");
    leg->AddEntry(h["ee_bkg"], "Bkg template", "l");
    leg->AddEntry(h["ee_sig+bkg"], "Sig+Bkg", "l");
    
    //gPad->SetLogy();
    h["ee"]->Draw("hist");
    h["ee_sig"]->Draw("hist same");
    h["ee_bkg"]->Draw("hist same");
    leg->Draw();
    
    
    c1->cd(2);
    //gPad->SetLogy();
    h["eg_sig"]->Draw("hist");
    h["eg"]->Draw("hist same");
    h["eg_bkg"]->Draw("hist same");
    
    
    c1->cd(3);
    h["ee_sig+bkg"]->Draw("hist");
    h["ee"]->Draw("hist same");
    
    h["ee_bkg"]->Draw("hist same");
    
    c1->cd(4);
    //gPad->SetLogy();
    h["eg"]->Draw("hist");
    h["eg_sig+bkg"]->Draw("hist same");
    h["eg_bkg"]->Draw("hist same");
//*/
    
    
    // RooFit **********************************************************************
    
    // define observable
    RooRealVar m("m", "Invariant Mass", 60., 120.);
    
    
    RooPlot *frame11 = m.frame(Title("templates (eg)"));
    RooPlot *frame21 = m.frame(Title("p.d.f. (eg)"));
    RooPlot *frame31 = m.frame(Title("fit to data (eg)"));
    
    RooPlot *frame12 = m.frame(Title("templates (ee)"));
    RooPlot *frame22 = m.frame(Title("p.d.f. (ee)"));
    RooPlot *frame32 = m.frame(Title("fit to data (ee)"));
    
    
    
    // create binned datasets from the templates
    map<string,RooDataHist*> rdh;
    rdh["eg"] = new RooDataHist("eg", "eg", m, Import(*h["eg"]));
    
    
    RooDataHist data_eg("data_eg", "data eg", m, Import(*h["eg"]));
    RooDataHist data_ee("data_ee", "data ee", m, Import(*h["ee"]));
    
    RooDataHist t_ee_sig("t_ee_sig", "ee sig template", m, Import(*h["ee_sig"]));
    RooDataHist t_ee_bkg("t_ee_bkg", "ee bkg template", m, Import(*h["ee_bkg"]));
    
    RooDataHist t_eg_sig("t_eg_sig", "eg sig template", m, Import(*h["eg_sig"]));
    RooDataHist t_eg_bkg("t_eg_bkg", "eg bkg template", m, Import(*h["eg_bkg"]));
    
    t_eg_sig.plotOn(frame11, MarkerColor(kRed));
    t_eg_bkg.plotOn(frame11, MarkerColor(kGreen+2));
    t_ee_sig.plotOn(frame12, MarkerColor(kRed));
    t_ee_bkg.plotOn(frame12, MarkerColor(kGreen+2));
    
    
    
    // create binned dataset for p.d.f.
    m.setBins(20);
    RooDataHist* hist_eg_s = ((RooDataSet*) &t_eg_sig)->binnedClone() ;
    RooDataHist* hist_ee_s = ((RooDataSet*) &t_ee_sig)->binnedClone() ;
    m.setBins(20);
    RooDataHist* hist_eg_b = ((RooDataSet*) &t_eg_bkg)->binnedClone() ;
    RooDataHist* hist_ee_b = ((RooDataSet*) &t_ee_bkg)->binnedClone() ;
    
    // represent binned datasets as p.d.f. with 2nd order interpolation
    RooHistPdf pdf_eg_s("pdf_eg_s","pdf eg sig",m,*hist_eg_s,2) ;
    RooHistPdf pdf_eg_b("pdf_eg_b","pdf eg bkg",m,*hist_eg_b,2) ;
    RooHistPdf pdf_ee_s("pdf_ee_s","pdf ee sig",m,*hist_ee_s,4) ;
    RooHistPdf pdf_ee_b("pdf_ee_b","pdf ee bkg",m,*hist_ee_b,2) ;
    
    // kernel estimation
    float   rho_sig = 1./(2*1.581),
            rho_bkg = 0.2887;
    RooKeysPdf kest_ee_sig("kest_ee_sig","kest_ee_sig",m,*((RooDataSet*)&t_ee_sig),RooKeysPdf::MirrorBoth, rho_sig) ;
    RooKeysPdf kest_eg_sig("kest_eg_sig","kest_eg_sig",m,*((RooDataSet*)&t_eg_sig),RooKeysPdf::MirrorBoth, rho_sig) ;
    RooKeysPdf kest_ee_bkg("kest_ee_bkg","kest_ee_bkg",m,*((RooDataSet*)&t_ee_bkg),RooKeysPdf::MirrorBoth, rho_bkg) ;
    RooKeysPdf kest_eg_bkg("kest_eg_bkg","kest_eg_bkg",m,*((RooDataSet*)&t_eg_bkg),RooKeysPdf::MirrorBoth, rho_bkg) ;
    
    //
    
    pdf_eg_s.plotOn(frame21, LineColor(kRed));
    pdf_eg_b.plotOn(frame21, LineColor(kGreen+2));
    pdf_ee_s.plotOn(frame22, LineColor(kRed));
    pdf_ee_b.plotOn(frame22, LineColor(kGreen+2));
    kest_eg_sig.plotOn(frame21, LineColor(kRed+2));
    kest_eg_bkg.plotOn(frame21, LineColor(kGreen+4));
    kest_ee_sig.plotOn(frame22, LineColor(kRed+2));
    kest_ee_bkg.plotOn(frame22, LineColor(kGreen+4));
    
    
    
    
    // smearing signal with gaussian
    RooRealVar *m_ee = new RooRealVar("m_ee", "mean ee smearing", 90., 0., 100.);
    RooRealVar *s_ee = new RooRealVar("s_ee", "width ee smearing", 3., 0.1, 10.);
    RooRealVar *m_eg = new RooRealVar("m_eg", "mean eg smearing", 90., 0., 100.);
    RooRealVar *s_eg = new RooRealVar("s_eg", "width eg smearing", 3., 0.1, 10.);
    
    RooGaussian *smear_ee = new RooGaussian("smear_ee", "ee gaussian smearing", m, *m_ee, *s_ee);
    RooGaussian *smear_eg = new RooGaussian("smear_eg", "eg gaussian smearing", m, *m_eg, *s_eg);
    
    RooNumConvPdf *sig_ee = new RooNumConvPdf("sig_ee", "smeared signal ee", m, pdf_ee_s, *smear_ee);
    RooNumConvPdf *sig_eg = new RooNumConvPdf("sig_eg", "smeared signal eg", m, pdf_eg_s, *smear_eg);
    
    
    
    RooRealVar *nbkg_eg = new RooRealVar("nbkg", "background events", 2000, 100, 9000);
    RooRealVar *nsig_eg = new RooRealVar("nsig", "signal events", 13000, 5000, 15000);
    RooRealVar *nbkg_ee = new RooRealVar("nbkg", "background events", 9000, 100, 5000);
    RooRealVar *nsig_ee = new RooRealVar("nsig", "signal events", 40000, 1000, 500000);
    
    
    //RooAddPdf *model_eg = new RooAddPdf("model_eg", "eg sig+bkg", RooArgList(pdf_eg_b, pdf_eg_s), RooArgList(*nbkg_eg, *nsig_eg));
    //RooAddPdf *model_ee = new RooAddPdf("model_ee", "ee sig+bkg", RooArgList(pdf_ee_b, pdf_ee_s), RooArgList(*nbkg_ee, *nsig_ee));
    
    
    RooAddPdf *model_eg = new RooAddPdf("model_eg", "eg sig+bkg", RooArgList(kest_eg_bkg, pdf_eg_s), RooArgList(*nbkg_eg, *nsig_eg));
    RooAddPdf *model_ee = new RooAddPdf("model_ee", "ee sig+bkg", RooArgList(kest_ee_bkg, pdf_ee_s), RooArgList(*nbkg_ee, *nsig_ee));
    
    
    // fit to data
    model_eg->fitTo(data_eg);
    model_ee->fitTo(data_ee);
    
    data_eg.plotOn(frame31);
    model_eg->plotOn(frame31);
    
    RooHist* hpull_eg = frame31->pullHist() ;
    RooPlot* fpull_eg = m.frame(Title("Pull Distribution")) ;
    fpull_eg->addPlotable((RooPlotable*)hpull_eg,"P") ;
    
    //model_eg->plotOn(frame31, Components("pdf_eg_b"), LineStyle(kDashed));
    model_eg->plotOn(frame31, Components("kest_eg_bkg"), LineStyle(kDashed));
    
    
    data_ee.plotOn(frame32);
    model_ee->plotOn(frame32);
    
    RooHist* hpull_ee = frame32->pullHist() ;
    RooPlot* fpull_ee = m.frame(Title("Pull Distribution")) ;
    fpull_ee->addPlotable((RooPlotable*)hpull_ee,"P") ;
    
    model_ee->plotOn(frame32, Components("kest_ee_bkg"), LineStyle(kDashed));
    
    
    TCanvas *c2 = new TCanvas("c2", "c2", 1700, 1000);
    //c1->SetLogy();
    c2->Divide(3, 2);
    c2->cd(1);
    
    gPad->SetLeftMargin(0.15) ; 
    frame11->GetYaxis()->SetTitleOffset(1.4) ; 
    frame11->Draw() ;
    
    
    c2->cd(2);
    
    gPad->SetLeftMargin(0.15) ; 
    frame21->GetYaxis()->SetTitleOffset(1.4) ; 
    frame21->Draw();
    
    
    c2->cd(3);
    
    TPad *pad30 = new TPad("pad30", "pad30", 0, 0.3, 1, 1);
    
    pad30->SetBottomMargin(0.1);
    pad30->SetLeftMargin(0.16);
    pad30->Draw();
    pad30->cd();
    
    frame31->GetYaxis()->SetTitleOffset(1.6);
    frame31->Draw();
    
    pad30->Update();
    
    c2->cd(3);
    
    TPad *pad30p = new TPad("pad30p", "pad30p", 0, 0, 1, 0.3);
    pad30p->SetTopMargin(0);
    pad30p->SetBottomMargin(0.11);
    pad30p->SetGridy();
    pad30p->SetLeftMargin(0.16);
    pad30p->Draw();
    pad30p->cd();
    
    fpull_eg->GetYaxis()->SetTitle("#frac{data-fit}{#sigma_{data}}");
    fpull_eg->GetYaxis()->SetTitleOffset(0.5);
    fpull_eg->GetYaxis()->SetTitleSize(0.11);
    fpull_eg->GetYaxis()->SetNdivisions(5);
    fpull_eg->GetYaxis()->SetLabelSize(0.08);
    
    fpull_eg->GetXaxis()->SetTitle("");
    fpull_eg->GetXaxis()->SetLabelSize(0.0);
    fpull_eg->SetTitle("");
    fpull_eg->Draw();
    
    
    pad30p->Update();
    
    
    
    
    c2->cd(4);
    
    gPad->SetLeftMargin(0.15) ; 
    frame12->GetYaxis()->SetTitleOffset(1.4) ; 
    frame12->Draw() ;
    
    
    c2->cd(5);
    
    gPad->SetLeftMargin(0.15) ; 
    frame22->GetYaxis()->SetTitleOffset(1.4) ; 
    frame22->Draw();
    
    
    c2->cd(6);
    
    TPad *pad31 = new TPad("pad31", "pad31", 0, 0.3, 1, 1);
    
    pad31->SetBottomMargin(0.1);
    pad31->SetLeftMargin(0.16);
    pad31->Draw();
    pad31->cd();
    
    frame32->GetYaxis()->SetTitleOffset(1.6);
    frame32->Draw();
    
    pad31->Update();
    
    c2->cd(6);
    
    TPad *pad32p = new TPad("pad32", "pad32", 0, 0, 1, 0.3);
    pad32p->SetTopMargin(0);
    pad32p->SetBottomMargin(0.11);
    pad32p->SetGridy();
    pad32p->SetLeftMargin(0.16);
    pad32p->Draw();
    pad32p->cd();
    
    fpull_ee->GetYaxis()->SetTitle("#frac{data-fit}{#sigma_{data}}");
    fpull_ee->GetYaxis()->SetTitleOffset(0.5);
    fpull_ee->GetYaxis()->SetTitleSize(0.11);
    fpull_ee->GetYaxis()->SetNdivisions(5);
    fpull_ee->GetYaxis()->SetLabelSize(0.08);
    
    fpull_ee->GetXaxis()->SetTitle("");
    fpull_ee->GetXaxis()->SetLabelSize(0.0);
    fpull_ee->SetTitle("");
    fpull_ee->Draw();
    
    
    pad32p->Update();
    
    
    
    
    
    
    cout << "fit results eg: " << endl;
    model_eg->printCompactTree();
    
    cout << "fit results ee: " << endl;
    model_ee->printCompactTree();
    
    
    cout << "finished!" << endl;
    return;
}


void templateKernelClass(string signal, string background, string dataset){
    
    cout << "rooFitHeader.templateKernelClass()" << endl;
    
    // cross section for DYJetsToLL in pb at NLO:
    float cs = 6025.2;
    // lumi for data in inverse pb (3.81 fb^-1)
    float lum = 3.81e3;
    // total amount of data in DYJetsToLL:
    float Nmc = 7375344;
    // show scale factor
    cout << "Scale factor: " << lum*cs/Nmc << endl;
    
    // templates and data files
    TFile *fSig = new TFile(signal.c_str(), "READ");
    TFile *fBkg = new TFile(background.c_str(), "READ");
    TFile *file = new TFile(dataset.c_str(), "READ");
    
    // read out histograms
    
    // data: 
    TH2F *ee_dist = (TH2F*) file->Get("tnp_ee")->Clone("ee_data");
    TH2F *eg_dist = (TH2F*) file->Get("tnp_eg")->Clone("eg_data");
    
    // background template:
    TH2F *ee_bkg = (TH2F*) fBkg->Get("muon_tnp_ee")->Clone("ee_bkg");
    TH2F *eg_bkg = (TH2F*) fBkg->Get("muon_tnp_eg")->Clone("eg_bkg");
    
    // signal template:
    TH2F *ee_sig = (TH2F*) fSig->Get("tnp_ee")->Clone("ee_sig");
    TH2F *eg_sig = (TH2F*) fSig->Get("tnp_eg")->Clone("eg_sig");
    
    // projections
    map<string,TH1F*> h;
    
    h["ee"] = (TH1F*) ee_dist->ProjectionX();
    h["eg"] = (TH1F*) eg_dist->ProjectionX();
    h["ee_bkg"] = (TH1F*) ee_bkg->ProjectionX();
    h["eg_bkg"] = (TH1F*) eg_bkg->ProjectionX();
    h["ee_sig"] = (TH1F*) ee_sig->ProjectionX();
    h["eg_sig"] = (TH1F*) eg_sig->ProjectionX();
    h["ee_sig+bkg"] = (TH1F*) h["ee_sig"]->Clone("ee_sig+bkg");
    h["ee_sig+bkg"]->Add(h["ee_bkg"]);
    h["eg_sig+bkg"] = (TH1F*) h["eg_sig"]->Clone("eg_sig+bkg");
    h["eg_sig+bkg"]->Add(h["eg_bkg"]);
    
    
    h["ee_bkg_fit"] = (TH1F*) h["ee_bkg"]->Clone("ee_bkg_fit");
    h["eg_bkg_fit"] = (TH1F*) h["eg_bkg"]->Clone("eg_bkg_fit");
    h["ee_sig_fit"] = (TH1F*) h["ee_sig"]->Clone("ee_sig_fit");
    h["eg_sig_fit"] = (TH1F*) h["eg_sig"]->Clone("eg_sig_fit");
    
    
    // rebin
    for(map<string, TH1F*>::iterator it = h.begin(); it != h.end(); it++){
        // it->first = key
        // it->second = value
        // cout << it->first << endl;
        it->second->Rebin(20);
        it->second->SetLineWidth(2);
    }
    
    
    // fit class
    //~ TTemplateFit *f_ee = new TTemplateFit("f_ee", "ee Sample");
    //~ f_ee->addFiles(h["ee"], h["ee_sig"], h["ee_bkg"]);
    //~ 
    //~ //f_ee->drawRaw();
    //~ f_ee->fit(kTRUE);
    //~ 
    //~ f_ee->drawFrame();
    
/*
    // scale mc by L(data)/N(mc)*sigma(mc)
    h["ee_sig"]->Scale(lum*cs/Nmc);
    h["eg_sig"]->Scale(lum*cs/Nmc);
    
    
    
    //h["ee_sig+bkg"] = (TH1F*) h["ee_sig"]->Clone();
    //h["eg_sig+bkg"]->Add(h["ee_bkg"]);
    
    h["ee"]->SetLineColor(kBlue);
    h["ee_sig"]->SetLineColor(kRed);
    h["ee_bkg"]->SetLineColor(kGray);
    h["ee_bkg"]->SetFillColor(kGray);
    h["eg"]->SetLineColor(kBlue);
    h["eg_sig"]->SetLineColor(kRed);
    h["eg_bkg"]->SetLineColor(kGray);
    
    h["ee_sig+bkg"]->SetLineColor(kMagenta);
    h["eg_sig+bkg"]->SetLineColor(kMagenta);
    
    
    // RooFit **********************************************************************
    
    // define observable
    RooRealVar m("m", "Invariant Mass", 60., 120.);
    
    
    RooPlot *frame11 = m.frame(Title("templates (eg)"));
    RooPlot *frame21 = m.frame(Title("p.d.f. (eg)"));
    RooPlot *frame31 = m.frame(Title("fit to data (eg)"));
    
    RooPlot *frame12 = m.frame(Title("templates (ee)"));
    RooPlot *frame22 = m.frame(Title("p.d.f. (ee)"));
    RooPlot *frame32 = m.frame(Title("fit to data (ee)"));
    
    
    
    // create binned datasets from the templates
    map<string,RooDataHist*> rdh;
    rdh["eg"] = new RooDataHist("eg", "eg", m, Import(*h["eg"]));
    
    
    RooDataHist data_eg("data_eg", "data eg", m, Import(*h["eg"]));
    RooDataHist data_ee("data_ee", "data ee", m, Import(*h["ee"]));
    
    RooDataHist t_ee_sig("t_ee_sig", "ee sig template", m, Import(*h["ee_sig"]));
    RooDataHist t_ee_bkg("t_ee_bkg", "ee bkg template", m, Import(*h["ee_bkg"]));
    
    RooDataHist t_eg_sig("t_eg_sig", "eg sig template", m, Import(*h["eg_sig"]));
    RooDataHist t_eg_bkg("t_eg_bkg", "eg bkg template", m, Import(*h["eg_bkg"]));
    
    t_eg_sig.plotOn(frame11, MarkerColor(kRed));
    t_eg_bkg.plotOn(frame11, MarkerColor(kGreen+2));
    t_ee_sig.plotOn(frame12, MarkerColor(kRed));
    t_ee_bkg.plotOn(frame12, MarkerColor(kGreen+2));
    
    
    
    // create binned dataset for p.d.f.
    m.setBins(20);
    RooDataHist* hist_eg_s = ((RooDataSet*) &t_eg_sig)->binnedClone() ;
    RooDataHist* hist_ee_s = ((RooDataSet*) &t_ee_sig)->binnedClone() ;
    m.setBins(20);
    RooDataHist* hist_eg_b = ((RooDataSet*) &t_eg_bkg)->binnedClone() ;
    RooDataHist* hist_ee_b = ((RooDataSet*) &t_ee_bkg)->binnedClone() ;
    
    // represent binned datasets as p.d.f. with 2nd order interpolation
    RooHistPdf pdf_eg_s("pdf_eg_s","pdf eg sig",m,t_eg_sig,2) ;
    RooHistPdf pdf_eg_b("pdf_eg_b","pdf eg bkg",m,t_eg_bkg,2) ;
    RooHistPdf pdf_ee_s("pdf_ee_s","pdf ee sig",m,*hist_ee_s,3) ;
    RooHistPdf pdf_ee_b("pdf_ee_b","pdf ee bkg",m,*hist_ee_b,3) ;
    
    // kernel estimation
    float   rho_sig = 1./(2*1.581),
            rho_bkg = 0.2887;
    RooKeysPdf kest_ee_sig("kest_ee_sig","kest_ee_sig",m,*((RooDataSet*)&t_ee_sig),RooKeysPdf::MirrorBoth, rho_sig) ;
    RooKeysPdf kest_eg_sig("kest_eg_sig","kest_eg_sig",m,*((RooDataSet*)&t_eg_sig),RooKeysPdf::MirrorBoth, rho_sig) ;
    RooKeysPdf kest_ee_bkg("kest_ee_bkg","kest_ee_bkg",m,*((RooDataSet*)&t_ee_bkg),RooKeysPdf::MirrorBoth, rho_bkg) ;
    RooKeysPdf kest_eg_bkg("kest_eg_bkg","kest_eg_bkg",m,*((RooDataSet*)&t_eg_bkg),RooKeysPdf::MirrorBoth, rho_bkg) ;
    //RooAbsPdf kest_eg_bkg("kest_eg_bkg","kest_eg_bkg",m,*((RooDataSet*)&t_eg_bkg),RooKeysPdf::MirrorBoth, rho_bkg) ;
    
    //
    
    pdf_eg_s.plotOn(frame21, LineColor(kRed));
    pdf_eg_b.plotOn(frame21, LineColor(kGreen+2));
    pdf_ee_s.plotOn(frame22, LineColor(kRed));
    pdf_ee_b.plotOn(frame22, LineColor(kGreen+2));
    kest_eg_sig.plotOn(frame21, LineColor(kRed+2));
    kest_eg_bkg.plotOn(frame21, LineColor(kGreen+4));
    kest_ee_sig.plotOn(frame22, LineColor(kRed+2));
    kest_ee_bkg.plotOn(frame22, LineColor(kGreen+4));
    
    
    // smearing signal with gaussian
    RooRealVar *m_ee = new RooRealVar("m_ee", "mean ee smearing", 0., -10., 10.);
    RooRealVar *s_ee = new RooRealVar("s_ee", "width ee smearing", 1., 0.001, 5.);
    RooRealVar *m_eg = new RooRealVar("m_eg", "mean eg smearing", 0., -10., 10.);
    RooRealVar *s_eg = new RooRealVar("s_eg", "width eg smearing", 1., 0.001, 5.);
    
    RooGaussian *smear_ee = new RooGaussian("smear_ee", "ee gaussian smearing", m, *m_ee, *s_ee);
    RooGaussian *smear_eg = new RooGaussian("smear_eg", "eg gaussian smearing", m, *m_eg, *s_eg);
    RooGaussian *smear_Kest_ee = new RooGaussian("smear_Kest_ee", "ee gaussian smearing", m, *m_ee, *s_ee);
    RooGaussian *smear_Kest_eg = new RooGaussian("smear_Kest_eg", "eg gaussian smearing", m, *m_eg, *s_eg);
    
    
    RooNumConvPdf *sig_ee = new RooNumConvPdf("sig_ee", "smeared signal ee", m, *smear_ee, pdf_ee_s);
    RooNumConvPdf *sig_eg = new RooNumConvPdf("sig_eg", "smeared signal eg", m, *smear_eg, pdf_eg_s);
    RooNumConvPdf *sig_ee_kest = new RooNumConvPdf("sig_ee_kest", "smeared signal ee", m, *smear_ee, kest_ee_sig);
    RooNumConvPdf *sig_eg_kest = new RooNumConvPdf("sig_eg_kest", "smeared signal eg", m, *smear_eg, kest_eg_sig);
    
    
    RooRealVar *nbkg_eg = new RooRealVar("nbkg", "background events", 2000, 100, 9000);
    RooRealVar *nsig_eg = new RooRealVar("nsig", "signal events", 13000, 5000, 15000);
    RooRealVar *nbkg_ee = new RooRealVar("nbkg", "background events", 9000, 100, 5000);
    RooRealVar *nsig_ee = new RooRealVar("nsig", "signal events", 40000, 1000, 500000);
    
    
    //RooAddPdf *model_eg = new RooAddPdf("model_eg", "eg sig+bkg", RooArgList(pdf_eg_b, pdf_eg_s), RooArgList(*nbkg_eg, *nsig_eg));
    //RooAddPdf *model_ee = new RooAddPdf("model_ee", "ee sig+bkg", RooArgList(pdf_ee_b, pdf_ee_s), RooArgList(*nbkg_ee, *nsig_ee));
    
    
    RooAddPdf *model_eg = new RooAddPdf("model_eg", "eg sig+bkg", RooArgList(kest_eg_bkg, *sig_eg), RooArgList(*nbkg_eg, *nsig_eg));
    //RooAddPdf *model_ee = new RooAddPdf("model_ee", "ee sig+bkg", RooArgList(kest_ee_bkg, pdf_ee_s), RooArgList(*nbkg_ee, *nsig_ee));
    RooAddPdf *model_ee = new RooAddPdf("model_ee", "ee sig+bkg", RooArgList(kest_ee_bkg, *sig_ee), RooArgList(*nbkg_ee, *nsig_ee));
    //RooAddPdf *model_ee = new RooAddPdf("model_ee", "ee sig+bkg", RooArgList(kest_ee_bkg, *sig_ee_kest), RooArgList(*nbkg_ee, *nsig_ee));
    //RooAddPdf *model_ee = new RooAddPdf("model_ee", "ee sig+bkg", RooArgList(kest_ee_bkg, *smear_ee), RooArgList(*nbkg_ee, *nsig_ee));
    
    
    // fit to data
    //model_eg->fitTo(data_eg);
    //model_ee->fitTo(data_ee);
    
    data_eg.plotOn(frame31);
    model_eg->plotOn(frame31);
    
    RooHist* hpull_eg = frame31->pullHist() ;
    RooPlot* fpull_eg = m.frame(Title("Pull Distribution")) ;
    fpull_eg->addPlotable((RooPlotable*)hpull_eg,"P") ;
    
    //model_eg->plotOn(frame31, Components("pdf_eg_b"), LineStyle(kDashed));
    model_eg->plotOn(frame31, Components("kest_eg_bkg"), LineStyle(kDashed));
    
    
    data_ee.plotOn(frame32);
    model_ee->plotOn(frame32);
    
    RooHist* hpull_ee = frame32->pullHist() ;
    RooPlot* fpull_ee = m.frame(Title("Pull Distribution")) ;
    fpull_ee->addPlotable((RooPlotable*)hpull_ee,"P") ;
    
    model_ee->plotOn(frame32, Components("kest_ee_bkg"), LineStyle(kDashed));
    
    
    TCanvas *c2 = new TCanvas("c2", "c2", 1700, 1000);
    //c1->SetLogy();
    c2->Divide(3, 2);
    c2->cd(1);
    
    gPad->SetLeftMargin(0.15) ; 
    frame11->GetYaxis()->SetTitleOffset(1.4) ; 
    frame11->Draw() ;
    
    
    c2->cd(2);
    
    gPad->SetLeftMargin(0.15) ; 
    frame21->GetYaxis()->SetTitleOffset(1.4) ; 
    frame21->Draw();
    
    
    c2->cd(3);
    
    TPad *pad30 = new TPad("pad30", "pad30", 0, 0.3, 1, 1);
    
    pad30->SetBottomMargin(0.1);
    pad30->SetLeftMargin(0.16);
    pad30->Draw();
    pad30->cd();
    
    frame31->GetYaxis()->SetTitleOffset(1.6);
    frame31->Draw();
    
    pad30->Update();
    
    c2->cd(3);
    
    TPad *pad30p = new TPad("pad30p", "pad30p", 0, 0, 1, 0.3);
    pad30p->SetTopMargin(0);
    pad30p->SetBottomMargin(0.11);
    pad30p->SetGridy();
    pad30p->SetLeftMargin(0.16);
    pad30p->Draw();
    pad30p->cd();
    
    fpull_eg->GetYaxis()->SetTitle("#frac{data-fit}{#sigma_{data}}");
    fpull_eg->GetYaxis()->SetTitleOffset(0.5);
    fpull_eg->GetYaxis()->SetTitleSize(0.11);
    fpull_eg->GetYaxis()->SetNdivisions(5);
    fpull_eg->GetYaxis()->SetLabelSize(0.08);
    
    fpull_eg->GetXaxis()->SetTitle("");
    fpull_eg->GetXaxis()->SetLabelSize(0.0);
    fpull_eg->SetTitle("");
    fpull_eg->Draw();
    
    
    pad30p->Update();
    
    
    
    
    c2->cd(4);
    
    gPad->SetLeftMargin(0.15) ; 
    frame12->GetYaxis()->SetTitleOffset(1.4) ; 
    frame12->Draw() ;
    
    
    c2->cd(5);
    
    gPad->SetLeftMargin(0.15) ; 
    frame22->GetYaxis()->SetTitleOffset(1.4) ; 
    frame22->Draw();
    
    
    c2->cd(6);
    
    TPad *pad31 = new TPad("pad31", "pad31", 0, 0.3, 1, 1);
    
    pad31->SetBottomMargin(0.1);
    pad31->SetLeftMargin(0.16);
    pad31->Draw();
    pad31->cd();
    
    frame32->GetYaxis()->SetTitleOffset(1.6);
    frame32->Draw();
    
    pad31->Update();
    
    c2->cd(6);
    
    TPad *pad32p = new TPad("pad32", "pad32", 0, 0, 1, 0.3);
    pad32p->SetTopMargin(0);
    pad32p->SetBottomMargin(0.11);
    pad32p->SetGridy();
    pad32p->SetLeftMargin(0.16);
    pad32p->Draw();
    pad32p->cd();
    
    fpull_ee->GetYaxis()->SetTitle("#frac{data-fit}{#sigma_{data}}");
    fpull_ee->GetYaxis()->SetTitleOffset(0.5);
    fpull_ee->GetYaxis()->SetTitleSize(0.11);
    fpull_ee->GetYaxis()->SetNdivisions(5);
    fpull_ee->GetYaxis()->SetLabelSize(0.08);
    
    fpull_ee->GetXaxis()->SetTitle("");
    fpull_ee->GetXaxis()->SetLabelSize(0.0);
    fpull_ee->SetTitle("");
    fpull_ee->Draw();
    
    
    pad32p->Update();
    
    
    
    
    
    
    cout << "fit results eg: " << endl;
    model_eg->printCompactTree();
    
    cout << "fit results ee: " << endl;
    model_ee->printCompactTree();
    
    
    cout << "finished!" << endl;
*/
    return;
}



void templateKernelUnbinned(string signal, string background, string dataset){
    
    cout << "rooFitHeader.templateKernelUnbinned()" << endl;
    
    //rt.gROOT.SetBatch(rt.kTRUE) # dont show the canvases
    gROOT->SetBatch(kTRUE); // dont show canvases
    
    // cross section for DYJetsToLL in pb at NLO:
    float cs = 6025.2;
    // lumi for data in inverse pb (3.81 fb^-1)
    float lum = 3.81e3;
    // total amount of data in DYJetsToLL:
    float Nmc = 7375344;
    // show scale factor
    cout << "Scale factor: " << lum*cs/Nmc << endl;
    
    // templates and data files
    TFile *fSig = new TFile(signal.c_str(), "READ");
    TFile *fBkg = new TFile(background.c_str(), "READ");
    TFile *fDat = new TFile(dataset.c_str(), "READ");
    
    // read out trees
    map<string, TTree*> t;
    
    string treeNames[] =    {   "data_num",
                                "data_den",
                                "template_bkg_num",
                                "template_bkg_den",
                                "template_sig",
                            };
    Int_t NtreeNames = arraysize(treeNames);
    
    t["data_num"] =            (TTree*) fDat->Get("data_num");
    t["data_den"] =            (TTree*) fDat->Get("data_den");
    t["template_bkg_num"] =    (TTree*) fBkg->Get("template_bkg_num");
    t["template_bkg_den"] =    (TTree*) fBkg->Get("template_bkg_den");
    t["template_sig"] =        (TTree*) fSig->Get("template_sig");
    
    cout << "data_num.Entries()         = " << t["data_num"]->GetEntries() << endl;
    cout << "data_den.Entries()         = " << t["data_den"]->GetEntries() << endl;
    cout << "template_bkg_num.Entries() = " << t["template_bkg_num"]->GetEntries() << endl;
    cout << "template_bkg_den.Entries() = " << t["template_bkg_den"]->GetEntries() << endl;
    cout << "template_sig.Entries()     = " << t["template_sig"]->GetEntries() << endl;
    
    
    //              0       1
    string ts[] = {"den", "num"};
    Int_t Nts = arraysize(ts);
    
    // ******************* class objects for the fittings here
    
    // template fit class
    RooRealVar *m = new RooRealVar("m", "m (GeV)", 60., 120.);
    //m->SetNameTitle("m", "m (GeV)");
    RooRealVar *pt   = new RooRealVar("pt",   "p_{T}^{probe} (GeV)", 0., 1000.);
    RooRealVar *nvtx = new RooRealVar("nvtx", "N_{vertex}", 0., 35.);
    RooRealVar *ntrk = new RooRealVar("ntrk", "N_{track}", 0., 240.);
    RooRealVar *eta  = new RooRealVar("eta",  "|#eta^{probe}|", 0., 1.5);
    
    
    
    // root file to store fit results
    TFile *fResults = new TFile("results_pt1.root", "update");
    
    // template fit objects
    map<string, TTemplateFit*> tf;
    
    // global fits
    //~ for(int i=0; i<Nts; i++){
        //~ // create template fit objects
        //~ cout << "initialize tf["<< ts[i] << "]:" << endl;
        //~ tf[ts[i]] = new TTemplateFit(ts[i], ts[i], m);
        //~ tf[ts[i]]->addData(t["data_"+ts[i]], t["template_sig"], t["template_bkg_"+ts[i]]);
        //~ 
        //~ // fit
        //~ tf[ts[i]]->buildPdf("hist", "fit");
    //~ }
    //~ 
    //~ for(int i=0; i<Nts; i++){
        //~ cout << "draw and save tf["<< ts[i] << "]:" << endl;
        //~ tf[ts[i]]->drawRaw(fResults);
        //~ tf[ts[i]]->drawFrame(fResults);
        //~ tf[ts[i]]->save(fResults);
    //~ }
    //~ 
    //~ 
    //~ 
    
    // datasets with pt
    map<string, RooDataSet*> rdsPt;
    for(int i=0; i<NtreeNames; i++){
        rdsPt[treeNames[i]] = new RooDataSet(   treeNames[i].c_str(), 
                                                treeNames[i].c_str(), 
                                                RooArgList(*m, *pt), 
                                                Import(*t[treeNames[i]]));
    }
    
    // datasets with Ntracks
    map<string, RooDataSet*> rdsNtrk;
    for(int i=0; i<NtreeNames; i++){
        rdsNtrk[treeNames[i]] = new RooDataSet( treeNames[i].c_str(), 
                                                treeNames[i].c_str(), 
                                                RooArgList(*m, *ntrk), 
                                                Import(*t[treeNames[i]]));
    }
    
    // datasets with Pt and Ntracks
    map<string, RooDataSet*> rdsPtNtrk;
    for(int i=0; i<NtreeNames; i++){
        rdsPtNtrk[treeNames[i]] = new RooDataSet(   treeNames[i].c_str(), 
                                                    treeNames[i].c_str(), 
                                                    RooArgList(*m, *pt, *ntrk), 
                                                    Import(*t[treeNames[i]]));
    }
    
    // map for all roo data sets
    map<string, RooDataSet*> rds;
    
    // ////////////////////////////////////////////////////////////////////////////////////
    // Pt 
    //                          0   1       2       3       4     5     6     7        8     9     10
    Double_t bins_pt[] =    {  40. , 42. ,  44.,   46.,   48.,   51.,   55.,   61.,    72.,  90. , 250.};
    string bins_pt_pdf[] =  {"hist","hist","hist","hist","hist","hist","hist","hist","hist","hist","hist"};
    //string bins_pt_pdf[] =  {"kernel","kernel","kernel","kernel","hist","hist","hist","hist","hist","hist","hist"};
    
    string sBin, sName;
    string sLow, sUp;
    
    Int_t Npt = arraysize(bins_pt);
    cout << "Npt = " << Npt << endl;
    
    
    Int_t fitPtBin = 7;
    Int_t fitFraction = 1; // 0: den, 1: num
    
    string fSubfolder = "pt";
    // loop nominator and denominator:  
    // - for each, create a dataset within the pt bins of above;
    for(int i=0; i<Nts; i++){
    //for(int i=fitFraction; i<=fitFraction; i++){
        
        for(int bin=0; bin < (Npt-1); bin++){
        //for(int bin=fitPtBin ; bin <= fitPtBin; bin++){
            sLow = NumberToString(bins_pt[bin]);
            sUp =  NumberToString(bins_pt[bin+1]);
            sBin = sLow+"to"+sUp;
            sName = ts[i]+"_pt_"+sBin;
            //cout << sName << endl;
            
            rds["data_"+ts[i]] =         (RooDataSet*)  rdsPt["data_"+ts[i]]->reduce(*m, ("pt>="+sLow+" && pt<"+sUp).c_str());
            rds["template_sig"] =        (RooDataSet*)  rdsPt["template_sig"]->reduce(*m, ("pt>="+sLow+" && pt<"+sUp).c_str());
            rds["template_bkg_"+ts[i]] = (RooDataSet*)  rdsPt["template_bkg_"+ts[i]]->reduce(*m, ("pt>="+sLow+" && pt<"+sUp).c_str());
            
            cout << "Entries in bin "+sBin << " = " << rds["data_"+ts[i]]->numEntries() << endl;
            
            // build pdf s and draw frames
            tf[sName] = new TTemplateFit(sName, sName, m);
            tf[sName]->addData(rds["data_"+ts[i]], rds["template_sig"], rds["template_bkg_"+ts[i]]);
            tf[sName]->buildPdf(bins_pt_pdf[bin], "fit");
            tf[sName]->drawRaw(fResults, fSubfolder);
            tf[sName]->drawFrame(fResults, fSubfolder);
            tf[sName]->save(fResults, fSubfolder);
            
        }
        
    }
    

    
    
    
    //~ 
    //~ // close files
    fSig->Close();
    fBkg->Close();
    fDat->Close(); 
    fResults->Close();
    
    return;
}

void templateKernelUnbinned_PtNtrk(string signal, string background, string dataset){
    
    cout << "rooFitHeader.templateKernelUnbinned_PtNtrk()" << endl;
    
    //rt.gROOT.SetBatch(rt.kTRUE) # dont show the canvases
    gROOT->SetBatch(kTRUE); // dont show canvases
    
    // cross section for DYJetsToLL in pb at NLO:
    float cs = 6025.2;
    // lumi for data in inverse pb (3.81 fb^-1)
    float lum = 3.81e3;
    // total amount of data in DYJetsToLL:
    float Nmc = 7375344;
    // show scale factor
    cout << "Scale factor: " << lum*cs/Nmc << endl;
    
    // templates and data files
    TFile *fSig = new TFile(signal.c_str(), "READ");
    TFile *fBkg = new TFile(background.c_str(), "READ");
    TFile *fDat = new TFile(dataset.c_str(), "READ");
    
    // read out trees
    map<string, TTree*> t;
    
    string treeNames[] =    {   "data_num",
                                "data_den",
                                "template_bkg_num",
                                "template_bkg_den",
                                "template_sig",
                            };
    Int_t NtreeNames = arraysize(treeNames);
    
    t["data_num"] =            (TTree*) fDat->Get("data_num");
    t["data_den"] =            (TTree*) fDat->Get("data_den");
    t["template_bkg_num"] =    (TTree*) fBkg->Get("template_bkg_num");
    t["template_bkg_den"] =    (TTree*) fBkg->Get("template_bkg_den");
    t["template_sig"] =        (TTree*) fSig->Get("template_sig");
    
    cout << "data_num.Entries()         = " << t["data_num"]->GetEntries() << endl;
    cout << "data_den.Entries()         = " << t["data_den"]->GetEntries() << endl;
    cout << "template_bkg_num.Entries() = " << t["template_bkg_num"]->GetEntries() << endl;
    cout << "template_bkg_den.Entries() = " << t["template_bkg_den"]->GetEntries() << endl;
    cout << "template_sig.Entries()     = " << t["template_sig"]->GetEntries() << endl;
    
    
    //              0       1
    string ts[] = {"den", "num"};
    Int_t Nts = arraysize(ts);
    
    // ******************* class objects for the fittings here
    
    // template fit class
    RooRealVar *m = new RooRealVar("m", "m (GeV)", 60., 120.);
    //m->SetNameTitle("m", "m (GeV)");
    RooRealVar *pt   = new RooRealVar("pt",   "p_{T}^{probe} (GeV)", 0., 1000.);
    RooRealVar *nvtx = new RooRealVar("nvtx", "N_{vertex}", 0., 35.);
    RooRealVar *ntrk = new RooRealVar("ntrk", "N_{track}", 0., 240.);
    RooRealVar *eta  = new RooRealVar("eta",  "|#eta^{probe}|", 0., 1.5);
    
    
    
    // root file to store fit results
    TFile *fResults = new TFile("results_ptntrk.root", "update");
    
    // template fit objects
    map<string, TTemplateFit*> tf;
    
    // global fits
    //~ for(int i=0; i<Nts; i++){
        //~ // create template fit objects
        //~ cout << "initialize tf["<< ts[i] << "]:" << endl;
        //~ tf[ts[i]] = new TTemplateFit(ts[i], ts[i], m);
        //~ tf[ts[i]]->addData(t["data_"+ts[i]], t["template_sig"], t["template_bkg_"+ts[i]]);
        //~ 
        //~ // fit
        //~ tf[ts[i]]->buildPdf("hist", "fit");
    //~ }
    //~ 
    //~ for(int i=0; i<Nts; i++){
        //~ cout << "draw and save tf["<< ts[i] << "]:" << endl;
        //~ tf[ts[i]]->drawRaw(fResults);
        //~ tf[ts[i]]->drawFrame(fResults);
        //~ tf[ts[i]]->save(fResults);
    //~ }
    //~ 
    //~ 
    //~ 
    
    // datasets with pt
    map<string, RooDataSet*> rdsPt;
    for(int i=0; i<NtreeNames; i++){
        rdsPt[treeNames[i]] = new RooDataSet(   treeNames[i].c_str(), 
                                                treeNames[i].c_str(), 
                                                RooArgList(*m, *pt), 
                                                Import(*t[treeNames[i]]));
    }
    
    // datasets with Ntracks
    map<string, RooDataSet*> rdsNtrk;
    for(int i=0; i<NtreeNames; i++){
        rdsNtrk[treeNames[i]] = new RooDataSet( treeNames[i].c_str(), 
                                                treeNames[i].c_str(), 
                                                RooArgList(*m, *ntrk), 
                                                Import(*t[treeNames[i]]));
    }
    
    // datasets with Pt and Ntracks
    map<string, RooDataSet*> rdsPtNtrk;
    for(int i=0; i<NtreeNames; i++){
        rdsPtNtrk[treeNames[i]] = new RooDataSet(   treeNames[i].c_str(), 
                                                    treeNames[i].c_str(), 
                                                    RooArgList(*m, *pt, *ntrk), 
                                                    Import(*t[treeNames[i]]));
    }
    
    // map for all roo data sets
    map<string, RooDataSet*> rds;
    
    // ////////////////////////////////////////////////////////////////////////////////////
    // Pt and Ntracks
    
    //                          0      1       2       3       4     5     6     7        8     9     10
    Double_t bins_ntrk[] =    {  0. , 20. ,  30.,     40.,   50.,   60.,   70.,   80.,   90.,   110., 240.};
    string bins_ntrk_pdf[] =  {"hist","hist","hist","hist","hist","hist","hist","hist","hist","hist","hist"};
    
    //                          0    1      2       3     4      5      6       7     8      9     10
    Double_t bins_pt[] =    {  40. , 42. ,  44.,   46.,   51.,   61.,   250.};
    string bins_pt_pdf[] =  {"hist","hist","hist","hist","hist","hist","hist"};//,"hist","hist","hist","hist"};
    
    
    string sBin, sName;
    
    string sLowTrk, sUpTrk, sLowPt, sUpPt;
    
    Int_t Ntrk = arraysize(bins_ntrk);
    Int_t Npt  = arraysize(bins_pt);
    
    cout << "Ntrk = " << Ntrk << endl;
    cout << "Npt  = " << Npt << endl;
    
    Int_t fitPtNtrkBin = 2;
    
    string fSubfolder = "ptntrk";
    // loop nominator and denominator:  
    // - for each, create a dataset within the pt bins of above;
    Int_t bincounter = 0;
    for(int i=0; i<Nts; i++){
        // i = 0 den
        // i = 1 num
        
        for(int binTrk=0; binTrk<(Ntrk-1); binTrk++){
            for(int binPt=0; binPt<(Npt-1); binPt++){
                bincounter++;
                
                sLowTrk = NumberToString(bins_ntrk[binTrk]);
                sUpTrk  = NumberToString(bins_ntrk[binTrk+1]);
                sLowPt  = NumberToString(bins_pt[binPt]);
                sUpPt   = NumberToString(bins_pt[binPt+1]);
                
                sBin    = "ntrk"+sLowTrk+"to"+sUpTrk+"_pt"+sLowPt+"to"+sUpPt;
                sName   = ts[i]+"_"+sBin;
                
                //cout << bincounter << " sName = " << sName << endl;
                
                rds["data_"+ts[i]] =         (RooDataSet*) rdsPtNtrk["data_"+ts[i]]->reduce(*m, ("ntrk>="+sLowTrk+" && ntrk<"+sUpTrk+" && pt>="+sLowPt+" && pt<"+sUpPt).c_str());
                rds["template_sig"] =        (RooDataSet*) rdsPtNtrk["template_sig"]->reduce(*m, ("ntrk>="+sLowTrk+" && ntrk<"+sUpTrk+" && pt>="+sLowPt+" && pt<"+sUpPt).c_str());
                rds["template_bkg_"+ts[i]] = (RooDataSet*) rdsPtNtrk["template_bkg_"+ts[i]]->reduce(*m, ("ntrk>="+sLowTrk+" && ntrk<"+sUpTrk+" && pt>="+sLowPt+" && pt<"+sUpPt).c_str());
                
                cout << "Entries in " +sName << " = " << rds["data_"+ts[i]]->numEntries() << endl;
                
                //~ // build pdf s and draw frames
                tf[sName] = new TTemplateFit(sName, sName, m);
                tf[sName]->addData(rds["data_"+ts[i]], rds["template_sig"], rds["template_bkg_"+ts[i]]);
                tf[sName]->buildPdf(bins_ntrk_pdf[binTrk], "fit");
                tf[sName]->drawRaw(fResults, fSubfolder);
                tf[sName]->drawFrame(fResults, fSubfolder);
                tf[sName]->save(fResults, fSubfolder);
                
            }
        }
        
    }
    
    
    
    
    
        //~ // ////////////////////////////////////////////////////////////////////////////////////
    //~ // Ntracks
    //~ 
    //~ //                          0   1       2       3       4     5     6     7        8     9     10
    //~ Double_t bins_ntrk[] =    {  0. , 20. ,  30.,     40.,   50.,   60.,   70.,   80.,   90.,   110., 240.};
    //~ string bins_ntrk_pdf[] =  {"hist","hist","hist","hist","hist","hist","hist","hist","hist","hist","hist"};
    //~ //string bins_pt_pdf[] =  {"kernel","kernel","kernel","kernel","hist","hist","hist","hist","hist","hist","hist"};
    //~ string sBin, sName;
    //~ string sLow, sUp;
    //~ 
    //~ Int_t Ntrk = arraysize(bins_ntrk);
    //~ cout << "Ntrk = " << Ntrk << endl;
    //~ 
    //~ Int_t fitNtrkBin = 2;
    //~ 
    //~ string fSubfolder = "ntrk";
    //~ // loop nominator and denominator:  
    //~ // - for each, create a dataset within the pt bins of above;
    //~ for(int i=0; i<Nts; i++){
        //~ 
        //~ for(int bin=0; bin < (Ntrk-1); bin++){
        //~ // for(int bin=fitNtrkBin ; bin <= fitNtrkBin; bin++){
            //~ sLow = NumberToString(bins_ntrk[bin]);
            //~ sUp =  NumberToString(bins_ntrk[bin+1]);
            //~ sBin = sLow+"to"+sUp;
            //~ sName = ts[i]+"_ntrk_"+sBin;
            //~ //cout << sName << endl;
            //~ 
            //~ rds["data_"+ts[i]] =         (RooDataSet*)  rdsNtrk["data_"+ts[i]]->reduce(*m, ("ntrk>="+sLow+" && ntrk<"+sUp).c_str());
            //~ rds["template_sig"] =        (RooDataSet*)  rdsNtrk["template_sig"]->reduce(*m, ("ntrk>="+sLow+" && ntrk<"+sUp).c_str());
            //~ rds["template_bkg_"+ts[i]] = (RooDataSet*)  rdsNtrk["template_bkg_"+ts[i]]->reduce(*m, ("ntrk>="+sLow+" && ntrk<"+sUp).c_str());
            //~ 
            //~ cout << "Entries in bin "+sBin << " = " << rds["data_"+ts[i]]->numEntries() << endl;
            //~ 
            //~ // build pdf s and draw frames
            //~ tf[sName] = new TTemplateFit(sName, sName, m);
            //~ tf[sName]->addData(rds["data_"+ts[i]], rds["template_sig"], rds["template_bkg_"+ts[i]]);
            //~ tf[sName]->buildPdf(bins_ntrk_pdf[bin], "");
            //~ tf[sName]->drawRaw(fResults, fSubfolder);
            //~ tf[sName]->drawFrame(fResults, fSubfolder);
            //~ tf[sName]->save(fResults, fSubfolder);
            //~ 
        //~ }
        //~ 
    //~ }
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    //~ 
    //~ // close files
    fSig->Close();
    fBkg->Close();
    fDat->Close(); 
    fResults->Close();
    
    return;
}



void readFitResults(string dropbox){
    cout << "rooFitHeader.readFitResults()" << endl;
    
    // dropbox = /user/rmeyer/Dropbox/data_uni/
    
    string ts[] = {"den", "num"};
    Int_t Nts = arraysize(ts);
    
    Double_t bins_pt[] =    {  40. , 45.  , 50.  , 55.  , 60.  , 65.  , 70.  , 80.  , 100. , 120. , 250.};
    string bins_pt_pdf[] =  {"hist","hist","hist","hist","hist","hist","hist","hist","hist","hist","hist"};  
    Int_t Npt = arraysize(bins_pt) - 1;
    
    
    // open results
    string results = dropbox + "code/rooFit/egamma/results.root";
    TFile *fRes = new TFile(results.c_str(), "read");
    
    //~ map<string, RooFitResult*>  rs;
    //~ map<string, RooPlot*>       rp;
    //~ map<string, RooAbsPdf*>     rm;
    //~ 
    //~ string ss = "den";
    //~ 
    //~ rs[ss] = (RooFitResult*)    fRes->Get((ss+"/fitresult").c_str());
    //~ rp[ss] = (RooPlot*)         fRes->Get((ss+"/frame").c_str());
    //~ rm[ss] = (RooAbsPdf*)       fRes->Get((ss+"/model").c_str());
    //~ 
    //~ 
    //~ rs["den"]->Print();
    //~ 
    //~ RooArgSet *pars = (RooArgSet*) &(rs[ss]->floatParsFinal());
    //~ 
    //~ 
    //~ cout << "nsig = " << ((RooRealVar*)pars->find("Nsig_den"))->getVal() << endl;
    //~ cout << "err = " <<  ((RooRealVar*)pars->find("Nsig_den"))->getError() << endl;
    
    
    // histograms for efficiency plot
    map<string, TH1F*> h;
    h["num"] = new TH1F("num", "efficiency;p_{T} (GeV);fakerate", Npt, bins_pt);
    h["den"] = new TH1F("den", "denominator", Npt, bins_pt);
    
    h["eff"] = new TH1F("eff", "efficiency;fakerate;p_{T} (GeV)", Npt, bins_pt);
    
    string  sLow, sUp, sBin, sName;
    Double_t binMidth = 0;
    
    RooFitResult *rFit;
    RooArgSet *rPars;
    Double_t value, err;
    // fill bins
    for(int bin=1; bin<Npt+1; bin++){
        for(int i=0; i<Nts; i++){
            
            binMidth = bins_pt[bin-1]+(bins_pt[bin]-bins_pt[bin-1])/2.;
            sLow = NumberToString(bins_pt[bin-1]);
            sUp =  NumberToString(bins_pt[bin]);
            sBin = sLow+"to"+sUp;
            sName = ts[i]+"_pt_"+sBin;
            
            // read out fit result
            rFit =  (RooFitResult*)    fRes->Get((sName+"/fitresult").c_str());
            rPars = (RooArgSet*) &(rFit->floatParsFinal());
            value = ((RooRealVar*)rPars->find(("Nsig_"+sName).c_str()))->getVal();
            err =   ((RooRealVar*)rPars->find(("Nsig_"+sName).c_str()))->getError();
            
            cout << "sName: " << sName << "\tval:   " << (value) << "\terr:   " << (err) << endl;
            
            //~ for(int k = 0; k < int(value*100); k++){
                //~ h[ts[i]]->Fill(binMidth);
            //~ }
            h[ts[i]]->SetBinContent(bin, (value));
            h[ts[i]]->SetBinError(bin, (err));
            
        }
    }
    
    //h["den"]->Sumw2();
    //h["num"]->Sumw2();
    
    
    //rt.TEfficiency.CheckConsistency(self.pas, self.tot):
    //TEfficiency *heff = new TEfficiency
    //cout << "Consistent = " << TEfficiency::CheckConsistency(*h["num"], *h["den"]) << endl;
    
    
    TCanvas *t = new TCanvas("t", "t", 800, 400);
    t->Divide(2, 1);
    t->cd(1);
    h["den"]->Draw("hist e");
    t->cd(2);
    h["num"]->Draw("hist e");
    //t->Update();
    
    h["eff"] = (TH1F*) h["num"]->Clone();
    h["eff"]->Divide(h["den"]);
    
    TCanvas *c = new TCanvas("c", "c", 700, 700);
    c->cd();
    h["eff"]->Draw("hist e");
    
    
    //cout << "chi2/dof  = " << rp[ss]->chiSquare() << endl;
    //cout << "chi2/ndof = " << rp[ss]->chiSquare(4) << endl;
    
    
    
    
    // model parameter:
    // "Nsig_"+name
    
    // RooArgSet* params = model->getVariables() ;
    // RooRealVar* c0 = (RooRealVar*) params->find(“c0”) ;
    // params->setRealValue(“c0”,5.3) ;
    // getRealValue()
    
    //cout << "Nsig_"+ss+" = " << rm[ss]->getVariables()->getRealValue(("Nsig_"+ss).c_str()) << endl; 
    
    
    
    
    
    
    //fRes->Close();
    
    
    //~ TFile *f = new TFile("mkdirtest.root", "update");
    //~ 
    //~ if(!f->cd("hello")){
        //~ cout << "Create dir... " << endl;
        //~ f->mkdir("hello");
    //~ }
    //~ 
    //~ f->cd("hello");
    
    
    
    return ;
}

void influenceBinningFit(string dropbox){
    // create some toy monte carlo, create different binned histograms and fit model to them
    // to investigate the effect of "binning".
    
    gROOT->SetBatch(kTRUE); // dont show canvases
    
    cout << "influenceBinningFit()" << endl;
    
    string results = dropbox + "code/rooFit/egamma/results.root";
    TFile *fRes = new TFile(results.c_str(), "update");
    
    Double_t offset = -0.;
    Double_t rangeMin = 60. + offset;
    Double_t rangeMax = 120.+ offset;
    
    Double_t    mean = (rangeMax-rangeMin)/2.+rangeMin,
                width = 2.5;
    
    Double_t Nevents = 1000000;
    
    
    
    fRes->cd();
    if(!fRes->cd("binningTests"))
    {
        cout << "Create dir in tree... " << endl;
        //fRes->mkdir(("binningTests_"+NumberToString(rangeMin)+"to"+NumberToString(rangeMax)).c_str());
        fRes->mkdir("binningTests");
    }
    //fRes->cd(("binningTests_"+NumberToString(rangeMin)+"to"+NumberToString(rangeMax)).c_str());
    fRes->cd("binningTests");
    
    cout << "check1" << endl;
    
    
    // create variables and gaussian :
    RooRealVar *x     = new RooRealVar("x", "x (Unit)", rangeMin, rangeMax);
    RooRealVar *gm    = new RooRealVar("gm", "mean" , mean, mean/4., mean*4.);
    RooRealVar *gw    = new RooRealVar("gw", "width", width, 0.1, width*4.);
    RooGaussian *gaus = new RooGaussian("gaus", "gaussian p.d.f",*x, *gm, *gw); 
    //RooLandau *gaus = new RooLandau("landau", "landau p.d.f.", *x, *gm, *gw);
    
    
    for(Nevents = 100; Nevents <= 1000001; Nevents = Nevents*10){
        string plotname = NumberToString(rangeMin)+"to"+NumberToString(rangeMax)+"_gen"+NumberToString(Nevents);
        
        // create toy monte carlo events:
        RooDataSet *data  = gaus->generate(*x, Nevents);
        
        cout << "check2" << endl;
        
                //~ //RooRealVar *x = new RooRealVar("m", "m (GeV)", rangeMin, rangeMax);
                //~ // extended likelihood fit: signal and background contribution
                //~ RooRealVar *nbkg = new RooRealVar("nbkg", "Background events", Nevents*0.2, Nevents*0.01, Nevents*0.5);
                //~ RooRealVar *nsig = new RooRealVar("nsig", "Signal events", Nevents*0.8, Nevents*0.5, Nevents);
                //~ 
                //~ // SIGNAL
                //~ // double sided crystal ball
                //~ RooRealVar *mcb = new RooRealVar("mcb", "mean crystalball", 0., -2., 2.);
                //~ RooRealVar *scb = new RooRealVar("scb", "width crystalball", 1., 0.1, 4.);//, 0.5, 4.);
                //~ RooRealVar *alphacb1 = new RooRealVar("alphacb1", "alpha crystalball 1", 15, 1., 100.);
                //~ RooRealVar *alphacb2 = new RooRealVar("alphacb2", "alpha crystalball 2", 2., 1., 3.);
                //~ RooRealVar *ncb1 = new RooRealVar("ncb1", "n crystalball 1", 0.2, 0.1, 10);
                //~ RooRealVar *ncb2 = new RooRealVar("ncb2", "n crystalball 2", 1.4, 1., 3.);
                //~ RooDoubleCBFast *doubleCB = new RooDoubleCBFast("doubleDB", "double sided crystal ball", *x, *mcb, *scb, *alphacb1, *ncb1, *alphacb2, *ncb2);
                //~ // breit wigner
                //~ RooRealVar *mbw = new RooRealVar("mbw", "mean breit wigner", 91.18);
                //~ RooRealVar *sbw = new RooRealVar("sbw", "width breit wigner", 2.495);//, 0., 2.);//, 0., 50.);
                //~ RooBreitWigner *breitwigner = new RooBreitWigner("breitwigner", "Breit Wigner Peak", *x, *mbw, *sbw);
                //~ // convoluted Signal model    
                //~ RooNumConvPdf *sigDCB = new RooNumConvPdf("sigDCB", "Z peak shape with double sided CB", *x, *breitwigner, *doubleCB);
                //~ 
                //~ // BACKGROUND
                //~ // cms shape
                //~ RooRealVar *alpha = new RooRealVar("alpha", "alpha", 85., 80, 100.);
                //~ RooRealVar *beta = new RooRealVar("beta", "beta", 1., 0., 5.);
                //~ RooRealVar *gamma = new RooRealVar("gamma", "gamma", 0.04, 0., 1.);
                //~ RooRealVar *peak = new RooRealVar("peak", "peak", 5.84, 1., 100.);
                //~ RooCMSShape *bkg = new RooCMSShape("bkg", "background shape", *x, *alpha, *beta, *gamma, *peak);
                //~ 
                //~ // fit model
                //~ RooAddPdf *gaus = new RooAddPdf("model", "sig+bkg", RooArgList(*bkg, *sigDCB), RooArgList(*nbkg, *nsig));
                //~ 
                //~ cout << "check3" << endl;
                //~ 
                //~ RooDataSet *data = gaus->generate(*x, Nevents);
        
        cout << "check4" << endl;
    /*
        frame = m->frame(Title(name.c_str()), Name((name+"_data").c_str()));
        fpull = m->frame(Title((name+" pull distribution").c_str()), Name((name+"_pull").c_str()));
        
        rdhData->plotOn(frame, Name(("data_"+name).c_str()));
        
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
    */
        
        
        RooPlot *f1 = x->frame(Title("Fit Binning 1"));
        RooPlot *f2 = x->frame(Title("Fit binning 2"));
        RooPlot *f3 = x->frame(Title("Fit binning 3"));
        RooPlot *f1p = x->frame();
        RooPlot *f2p = x->frame();
        RooPlot *f3p = x->frame();
        
        
        // unbinned dataset and fit to it
        //~ data->plotOn(f1, Name("data"));
        //~ gaus->fitTo(*data);
        //~ gaus->plotOn(f1, Name("model"), LineColor(kBlue));
        //~ gaus->paramOn(f1, Layout(0.6, 0.9, 0.9));
        
        // integrals:
        Double_t IntGaus;
        
        // binned histograms
        TH1 *hbin;
        hbin = (TH1*) (data->createHistogram(x->GetName(), 5)); hbin->SetName("hbin_5");
        RooDataHist *rdh1 = new RooDataHist("rdh1", "Data", *x, Import(*hbin));
        hbin = (TH1*) (data->createHistogram(x->GetName(), 50)); hbin->SetName("hbin_10");
        RooDataHist *rdh2 = new RooDataHist("rdh2", "Data", *x, Import(*hbin));
        hbin = (TH1*) (data->createHistogram(x->GetName(), 200)); hbin->SetName("hbin_30");
        RooDataHist *rdh3 = new RooDataHist("rdh3", "Data", *x, Import(*hbin));
        
        rdh1->plotOn(f1, Name("data"));
        gaus->fitTo(*rdh1, FitOptions("I"));
        gaus->plotOn(f1, Name("model"), LineColor(kBlue));
        gaus->paramOn(f1, Layout(0.6, 0.9, 0.9));
        
        
        
        rdh2->plotOn(f2, Name("data"));
        gaus->fitTo(*rdh2, FitOptions("I"));
        gaus->plotOn(f2, Name("model"), LineColor(kBlue));
        gaus->paramOn(f2, Layout(0.6, 0.9, 0.9));
        
        rdh3->plotOn(f3, Name("data"));
        gaus->fitTo(*rdh3, FitOptions("I"));
        gaus->plotOn(f3, Name("model"), LineColor(kBlue));
        gaus->paramOn(f3, Layout(0.6, 0.9, 0.9));
        
        // pull
        RooHist *rh1 = f1->pullHist("data", "model");
        f1p->addPlotable(rh1, "P");
        RooHist *rh2 = f2->pullHist("data", "model");
        f2p->addPlotable(rh2, "P");
        RooHist *rh3 = f3->pullHist("data", "model");
        f3p->addPlotable(rh3, "P");
        
        TCanvas *c1 = new TCanvas("c1", "c1", 1500, 600);
        c1->Divide(3, 1);
        
        c1->cd(1);
        TPad *p11 = new TPad("p11", "p11", 0., 0.3, 1., 1.);
        p11->SetLeftMargin(0.16);
        p11->Draw();
        p11->cd();
        f1->GetYaxis()->SetTitleOffset(1.6);
        f1->Draw();
        
        c1->cd(1);
        TPad *p12 = new TPad("p12", "p12", 0., 0., 1., 0.3);
        p12->SetLeftMargin(0.16);
        p12->SetGridy();
        p12->Draw();
        p12->cd();
        f1p->GetYaxis()->SetNdivisions(5);
        f1p->GetYaxis()->SetLabelSize(0.08);
        f1p->Draw();
        
        
        c1->cd(2);
        TPad *p21 = new TPad("p21", "p21", 0., 0.3, 1., 1.);
        p21->SetLeftMargin(0.16);
        p21->Draw();
        p21->cd();
        f2->GetYaxis()->SetTitleOffset(1.6);
        f2->Draw();
        
        c1->cd(2);
        TPad *p22 = new TPad("p22", "p22", 0., 0., 1., 0.3);
        p22->SetLeftMargin(0.16);
        p22->SetGridy();
        p22->Draw();
        p22->cd();
        f2p->GetYaxis()->SetNdivisions(5);
        f2p->GetYaxis()->SetLabelSize(0.08);
        f2p->Draw();
        
        
        c1->cd(3);
        TPad *p31 = new TPad("p31", "p31", 0., 0.3, 1., 1.);
        p31->SetLeftMargin(0.16);
        p31->Draw();
        p31->cd();
        f3->GetYaxis()->SetTitleOffset(1.6);
        f3->Draw();
        
        c1->cd(3);
        TPad *p32 = new TPad("p32", "p32", 0., 0., 1., 0.3);
        p32->SetLeftMargin(0.16);
        p32->SetGridy();
        p32->Draw();
        p32->cd();
        f3p->GetYaxis()->SetNdivisions(5);
        f3p->GetYaxis()->SetLabelSize(0.08);
        f3p->Draw();
        
        
        
        c1->Write(("canvasExmpl_"+plotname).c_str(), TObject::kOverwrite);
        c1->Close();
        
        
        // integral
        RooAbsReal *igx = gaus->createIntegral(*x);
        cout << "gaus_Int(x) = " << igx->getVal() << endl;
        
        
        
        //~ // **************************************************************************************
        //~ // Create a plot of relative difference of fitted width to generated width.
        //~ // - initialize binned histogram:
        RooDataHist* rdh = NULL;
        //map<Int_t, RooRealVar*> fitWidth;
        TH1 *h1 = NULL;
        //~ 
        //RooArgSet* params = model->getVariables() ;
        //RooRealVar* c0 = (RooRealVar*) params->find(“c0”) ;
        //params->setRealValue(“c0”,5.3) ;
        //getRealValue()
        //~ 
        
        map<Int_t, Double_t> relWidth;
        map<Int_t, Double_t> relWidthErr;
        
        Double_t    dTempVal = 0.,
                    dTempErr = 0.;
        // different binnings
        
        Int_t   binStart = fabs(rangeMax-rangeMin)/12.,
                binMax = fabs(rangeMax-rangeMin)*20,
                binStep = binStart;
                
        Int_t count = 0;
        
        for(int Nbin = binStart; Nbin < binMax; Nbin+=binStep){
            
            h1 = (TH1*) data->createHistogram(x->GetName(), Nbin);
            h1->SetName(("h1_"+NumberToString(Nbin)).c_str());
            rdh = new RooDataHist(  
                                    ("rdh_"+NumberToString(Nbin)).c_str(),
                                    "data",
                                    *x,
                                    Import(*h1)
                                );
            gaus->fitTo(*rdh, FitOptions("I"));
            // ((RooRealVar*)rPars->find(("Nsig_"+sName).c_str()))->getVal();
            dTempVal = ((RooRealVar*)gaus->getVariables()->find("gw"))->getVal();
            relWidth[Nbin] = fabs(dTempVal-width)/width;
            dTempErr = ((RooRealVar*)gaus->getVariables()->find("gw"))->getError();
            relWidthErr[Nbin] = sqrt(pow(dTempErr*1./width*(dTempVal-width)/fabs(width-dTempVal), 2));
            count ++;
            
        }
        
        TGraphErrors *tge = new TGraphErrors(count);
        tge->SetNameTitle("tge", "Fitted width as function of number of bins");
        
        count = 0;
        for(int Nbin = binStart; Nbin < binMax; Nbin+=binStep){
            cout << "Nbin = " << Nbin << "\t relWidth[" << Nbin << "] = " << relWidth[Nbin] << endl;
            tge->SetPoint(count, Float_t(Nbin)/fabs(rangeMax-rangeMin), relWidth[Nbin]);
            tge->SetPointError(count, 0., relWidthErr[Nbin]);
            // SetPoint (Int_t i, Double_t x, Double_t y)
            // SetPointError (Int_t i, Double_t ex, Double_t ey)
            count++;
        }
        cout << "Fitted values: " << count << endl;
        
        //TF1 *func1 = new TF1("func1", "[0]+[1]*pow(x+[2], [3])+[4]*exp([5]*(x+[6]))", Float_t(binStart)/20., Float_t(binMax)/20.+Float_t(binMax-binStart)/20.*0.1);
        //tge->Fit(func1, "MR");
        
        TCanvas *c2 = new TCanvas("c2", "c2", 800, 800);
        c2->cd();
        c2->SetLogy();
        c2->SetGridy();
        c2->SetGridx();
        c2->SetLeftMargin(0.19);
        c2->SetBottomMargin(0.16);
        tge->SetMarkerStyle(kFullDotLarge);
        tge->GetXaxis()->SetTitle("#frac{No. of Bins}{1 GeV}");
        tge->GetXaxis()->SetTitleOffset(1.4);
        tge->GetYaxis()->SetTitle("#left|#frac{width(fitted)-width(init)}{width(init)}#right|");
        tge->GetYaxis()->SetTitleOffset(1.8);
        tge->Draw("AP");
        
        
        c2->Write(("canvasFittedWidth_"+plotname).c_str(), TObject::kOverwrite);
        c2->Close();
    }
    
    fRes->Close();
    
    
    return ;
    
}

void influenceSignalBackground(string dropbox){
    
    // create some toy monte carlo, create different binned histograms and fit model to them
    // to investigate the effect of "binning".
    
    gROOT->SetBatch(kTRUE); // dont show canvases
    
    cout << "influenceSignalBackground()" << endl;
    
    string results = dropbox + "code/rooFit/egamma/results.root";
    TFile *fRes = new TFile(results.c_str(), "update");
    
    string sExtra;
    
    sExtra = "_fitI";
    //sExtra = "";
    
    
    Double_t offset = -0.;
    Double_t rangeMin = 60. + offset;
    Double_t rangeMax = 120.+ offset;
    
    Double_t    mean = (rangeMax-rangeMin)/2.+rangeMin,
                width = 2.5;
    
    
    
    fRes->cd();
    if(!fRes->cd("SignalBackgroundTests"))
    {
        cout << "Create dir in tree... " << endl;
        //fRes->mkdir(("binningTests_"+NumberToString(rangeMin)+"to"+NumberToString(rangeMax)).c_str());
        fRes->mkdir("SignalBackgroundTests");
    }
    //fRes->cd(("binningTests_"+NumberToString(rangeMin)+"to"+NumberToString(rangeMax)).c_str());
    fRes->cd("SignalBackgroundTests");
    
    
    // construct model signal+background, then fit. compare integral
    Double_t    Ntot = 100;
    
    for(Ntot = 100; Ntot <= 1000001; Ntot = Ntot*10){
        
        Double_t    fSig = 0.75,
                    Nsig = fSig*Ntot,
                    Nbkg = (1-fSig)*Ntot;
        
        string plotname = "_gen_"+NumberToString(Ntot);
        
        // create variables and gaussian :
        RooRealVar *x     = new RooRealVar("x", "x (Unit)", rangeMin, rangeMax);
        RooRealVar *gm    = new RooRealVar("gm", "mean" , mean, mean/4., mean*4.);
        RooRealVar *gw    = new RooRealVar("gw", "width", width, 0.1, width*4.);
        RooGaussian *gaus = new RooGaussian("gaus", "gaussian p.d.f",*x, *gm, *gw); 
        
        // create background:
        RooRealVar *a0 = new RooRealVar("a0","a0",0.5, 0., 2.) ;
        RooRealVar *a1 = new RooRealVar("a1","a1",-0.2 ,-2., 2.) ;
        RooChebychev *bkg1 = new RooChebychev("bkg1","Background 1",*x,RooArgSet(*a0,*a1)) ;
        
        // fit model
        RooRealVar *nbkg = new RooRealVar("nbkg", "Background events", (1-fSig)*Ntot, 0.01*Ntot, 0.5*Ntot);
        RooRealVar *nsig = new RooRealVar("nsig", "Signal events", fSig*Ntot, 0.5*Ntot, Ntot);
        RooAddPdf *model = new RooAddPdf("model", "sig+bkg", RooArgList(*gaus, *bkg1), RooArgList(*nsig, *nbkg));
        
        // create data
        RooDataSet *dSig = gaus->generate(*x, Nsig);
        RooDataSet *dBkg = bkg1->generate(*x, Nbkg);
        
        RooDataSet *dModel = new RooDataSet(*dSig, "fullData");
        dModel->append(*dBkg);
        
        Int_t   Nbin = 30;
        RooDataHist *hModel = NULL;
        
        hModel = new RooDataHist(   "hModel", 
                                    "binned data", 
                                    *x,
                                    Import(*((TH1*)dModel->createHistogram(x->GetName(), Nbin)))
                                    );
        
        model->fitTo(*hModel, Extended(kTRUE), FitOptions("I"));
        
        // plots
        RooPlot *tframe = x->frame(Title("testframe"));
        RooPlot *dataframe = x->frame(Title("data plot"));
        RooPlot *fitframe = x->frame(Title("fit plot"));
        
        dBkg->plotOn(dataframe, MarkerColor(kBlue));
        dModel->plotOn(dataframe);
        
        gaus->plotOn(tframe, LineColor(kRed));
        bkg1->plotOn(tframe, LineStyle(kDashed));
        
        hModel->plotOn(fitframe);
        model->plotOn(fitframe);
        model->plotOn(fitframe, Components("bkg1"), LineStyle(kDashed));
        model->paramOn(fitframe, Layout(0.6, 0.9, 0.9));
        
        TCanvas *c1 = new TCanvas(("c1"+sExtra).c_str(), "c1", 1400, 500);
        c1->Divide(2, 1);
        
        //~ c1->cd(1);
        //~ tframe->Draw();
        
        c1->cd(1);
        dataframe->Draw();
        
        c1->cd(2);
        fitframe->Draw();
        
        c1->Update();
        c1->Write(("exampleFittedNsig"+sExtra+"_"+NumberToString(Nbin)+plotname).c_str(), TObject::kOverwrite);
        c1->Close();
        
        //~ // ***************************************************************************************
        //~ // Create a plot of relative difference of fitted width to generated width.
        //~ // - initialize binned histogram:
        RooDataHist* rdh = NULL;
        //map<Int_t, RooRealVar*> fitWidth;
        TH1 *h1 = NULL;
        //~ 
        //RooArgSet* params = model->getVariables() ;
        //RooRealVar* c0 = (RooRealVar*) params->find(“c0”) ;
        //params->setRealValue(“c0”,5.3) ;
        //getRealValue()
        //~ 
        
        map<Int_t, Double_t> relNsig;
        map<Int_t, Double_t> relNsigErr;
        
        Double_t    dTempVal = 0.,
                    dTempErr = 0.;
        // different binnings
        
        Int_t   binStart = fabs(rangeMax-rangeMin)/12.,
                binMax = fabs(rangeMax-rangeMin)*20,
                binStep = binStart;
        
        Int_t count = 0;
        
        for(Nbin = binStart; Nbin < binMax; Nbin+=binStep){
            
            h1 = (TH1*) dModel->createHistogram(x->GetName(), Nbin);
            h1->SetName(("h1_"+NumberToString(Nbin)+sExtra).c_str());
            rdh = new RooDataHist(  
                                    ("rdh_"+NumberToString(Nbin)+sExtra).c_str(),
                                    "binned data",
                                    *x,
                                    Import(*h1)
                                );
            model->fitTo(*rdh, Extended(kTRUE), FitOptions("I"));
            dTempVal = ((RooRealVar*)model->getVariables()->find("nsig"))->getVal();
            relNsig[Nbin] = fabs(dTempVal-Nsig)/Nsig;
            dTempErr = ((RooRealVar*)model->getVariables()->find("nsig"))->getError();
            relNsigErr[Nbin] = sqrt(pow(dTempErr*1./Nsig*(dTempVal-Nsig)/fabs(Nsig-dTempVal), 2));
            count ++;
            
        }
        
        TGraphErrors *tge = new TGraphErrors(count);
        tge->SetNameTitle(("tge"+sExtra).c_str(), "Relative Nsig deviation as function of number of bins");
        
        count = 0;
        for(Nbin = binStart; Nbin < binMax; Nbin+=binStep){
            cout << "Nbin = " << Nbin << "\t relNsig[" << Nbin << "] = " << relNsig[Nbin] << endl;
            tge->SetPoint(count, Float_t(Nbin)/fabs(rangeMax-rangeMin), relNsig[Nbin]);
            tge->SetPointError(count, 0., relNsigErr[Nbin]);
            // SetPoint (Int_t i, Double_t x, Double_t y)
            // SetPointError (Int_t i, Double_t ex, Double_t ey)
            count++;
        }
        cout << "Fitted values: " << count << endl;
        
        //TF1 *func1 = new TF1("func1", "[0]+[1]*pow(x+[2], [3])+[4]*exp([5]*(x+[6]))", Float_t(binStart)/20., Float_t(binMax)/20.+Float_t(binMax-binStart)/20.*0.1);
        //tge->Fit(func1, "MR");
        
        TCanvas *c2 = new TCanvas(("c2"+sExtra).c_str(), "c2", 800, 800);
        c2->cd();
        c2->SetLogy();
        c2->SetGridy();
        c2->SetGridx();
        c2->SetLeftMargin(0.19);
        c2->SetBottomMargin(0.16);
        tge->SetMarkerStyle(kFullDotLarge);
        tge->GetXaxis()->SetTitle("#frac{No. of Bins}{1 GeV}");
        tge->GetXaxis()->SetTitleOffset(1.4);
        tge->GetYaxis()->SetTitle("#left|#frac{Nsig(fitted)-Nsig(init)}{Nsig(init)}#right|");
        tge->GetYaxis()->SetTitleOffset(1.8);
        tge->Draw("AP");
        
        
        c2->Update();
        c2->Write(("canvasFittedNsig"+sExtra+plotname).c_str(), TObject::kOverwrite);
        c2->Close();
        
        // *******************************************************************************************
    
    
    }
    
    
    
    
    fRes->Close();
    
    
    //~ cout << "Ntot = " << Ntot << endl;
    //~ cout << "Nsig = " << Nsig << endl;
    //~ cout << "Nbkg = " << Nbkg << endl;
    
    return ;
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



void readValues(){
    
    // filename:
    string filepath, dropbox, plotpath, filename;
    SystemPath(filepath, dropbox);
    plotpath = dropbox + "code/rooFit/plot/";
    
    filename = "data_fits.root";
    
    TFile *file = new TFile((plotpath+filename).c_str(), "READ");
    
    
    RooAbsPdf *model_eg = (RooAbsPdf*) file->Get("eg_model_1461688228");
    RooAbsPdf *model_ee = (RooAbsPdf*) file->Get("ee_model_1461690428");
    
    //RooRealVar *Neg = (RooRealVar*)model_eg->getVariables()->find("nsig");
    
    
    
    float neg = model_eg->getVariables()->getRealValue("nsig");
    float nee = model_ee->getVariables()->getRealValue("nsig");
    
    
    cout << "neg = " << neg << endl;
    cout << "nee = " << nee << endl;
    
    float f;
    f = neg/(neg+nee);
    cout << "f   = " << f << endl;
    
    
}


// alter code
/*








    
    //~ // ////////////////////////////////////////////////////////////////////////////////////
    //~ // Ntracks
    //~ 
    //~ //                          0   1       2       3       4     5     6     7        8     9     10
    //~ Double_t bins_ntrk[] =    {  0. , 15. ,  25.,     28.,   31.,   34.,   38.,   42.,  46.,  51. , 240.};
    //~ string bins_ntrk_pdf[] =  {"hist","hist","hist","hist","hist","hist","hist","hist","hist","hist","hist"};
    //~ //string bins_pt_pdf[] =  {"kernel","kernel","kernel","kernel","hist","hist","hist","hist","hist","hist","hist"};
    //~ string sBin, sName;
    //~ string sLow, sUp;
    //~ 
    //~ Int_t Ntrk = arraysize(bins_ntrk);
    //~ cout << "Ntrk = " << Ntrk << endl;
    //~ 
    //~ Int_t fitNtrkBin = 2;
    //~ 
    //~ string fSubfolder = "ntrk";
    //~ // loop nominator and denominator:  
    //~ // - for each, create a dataset within the pt bins of above;
    //~ for(int i=0; i<Nts; i++){
        //~ 
        //~ for(int bin=0; bin < (Ntrk-1); bin++){
        //~ // for(int bin=fitNtrkBin ; bin <= fitNtrkBin; bin++){
            //~ sLow = NumberToString(bins_ntrk[bin]);
            //~ sUp =  NumberToString(bins_ntrk[bin+1]);
            //~ sBin = sLow+"to"+sUp;
            //~ sName = ts[i]+"_ntrk_"+sBin;
            //~ //cout << sName << endl;
            //~ 
            //~ rds["data_"+ts[i]] =         (RooDataSet*)  rdsNtrk["data_"+ts[i]]->reduce(*m, ("ntrk>="+sLow+" && ntrk<"+sUp).c_str());
            //~ rds["template_sig"] =        (RooDataSet*)  rdsNtrk["template_sig"]->reduce(*m, ("ntrk>="+sLow+" && ntrk<"+sUp).c_str());
            //~ rds["template_bkg_"+ts[i]] = (RooDataSet*)  rdsNtrk["template_bkg_"+ts[i]]->reduce(*m, ("ntrk>="+sLow+" && ntrk<"+sUp).c_str());
            //~ 
            //~ cout << "Entries in bin "+sBin << " = " << rds["data_"+ts[i]]->numEntries() << endl;
            //~ 
            //~ // build pdf s and draw frames
            //~ tf[sName] = new TTemplateFit(sName, sName, m);
            //~ tf[sName]->addData(rds["data_"+ts[i]], rds["template_sig"], rds["template_bkg_"+ts[i]]);
            //~ tf[sName]->buildPdf(bins_ntrk_pdf[bin], "");
            //~ tf[sName]->drawRaw(fResults, fSubfolder);
            //~ tf[sName]->drawFrame(fResults, fSubfolder);
            //~ tf[sName]->save(fResults, fSubfolder);
            //~ 
        //~ }
        //~ 
    //~ }
    
    //~ // ////////////////////////////////////////////////////////////////////////////////////
    //~ // Pt and Ntracks
    //~ 
    //~ //                          0   1       2       3       4     5     6     7        8     9     10
    //~ Double_t bins_ntrk[] =    {  0. , 15. ,  25.,     28.,   31.,   34.,   38.,   42.,  46.,  51. , 240.};
    //~ string bins_ntrk_pdf[] =  {"hist","hist","hist","hist","hist","hist","hist","hist","hist","hist","hist"};
    //~ 
    //~ Double_t bins_pt[] =    {  40. , 42. ,  44.,   46.,   48.,   51.,   55.,   61.,    72.,  90. , 250.};
    //~ string bins_pt_pdf[] =  {"hist","hist","hist","hist","hist","hist","hist","hist","hist","hist","hist"};
    //~ 
    //~ 
    //~ string sBin, sName;
    //~ string sLow, sUp;
    //~ 
    //~ Int_t Ntrk = arraysize(bins_ptntrk);
    //~ cout << "Ntrk = " << Ntrk << endl;
    //~ 
    //~ Int_t fitPtNtrkBin = 2;
    //~ 
    //~ string fSubfolder = "ptntrk";
    //~ // loop nominator and denominator:  
    //~ // - for each, create a dataset within the pt bins of above;
    //~ for(int i=0; i<Nts; i++){
        //~ 
        //~ for(int bin=0; bin < (Ntrk-1); bin++){
        //~ // for(int bin=fitPtNtrkBin ; bin <= fitPtNtrkBin; bin++){
            //~ sLow = NumberToString(bins_ntrk[bin]);
            //~ sUp =  NumberToString(bins_ntrk[bin+1]);
            //~ sBin = sLow+"to"+sUp;
            //~ sName = ts[i]+"_ntrk_"+sBin;
            //~ //cout << sName << endl;
            //~ 
            //~ rds["data_"+ts[i]] =         (RooDataSet*)  rdsPtNtrk["data_"+ts[i]]->reduce(*m, ("ntrk>="+sLow+" && ntrk<"+sUp).c_str());
            //~ rds["template_sig"] =        (RooDataSet*)  rdsPtNtrk["template_sig"]->reduce(*m, ("ntrk>="+sLow+" && ntrk<"+sUp).c_str());
            //~ rds["template_bkg_"+ts[i]] = (RooDataSet*)  rdsPtNtrk["template_bkg_"+ts[i]]->reduce(*m, ("ntrk>="+sLow+" && ntrk<"+sUp).c_str());
            //~ 
            //~ cout << "Entries in bin "+sBin << " = " << rds["data_"+ts[i]]->numEntries() << endl;
            //~ 
            //~ // build pdf s and draw frames
            //~ tf[sName] = new TTemplateFit(sName, sName, m);
            //~ tf[sName]->addData(rds["data_"+ts[i]], rds["template_sig"], rds["template_bkg_"+ts[i]]);
            //~ tf[sName]->buildPdf(bins_ntrk_pdf[bin], "");
            //~ tf[sName]->drawRaw(fResults, fSubfolder);
            //~ tf[sName]->drawFrame(fResults, fSubfolder);
            //~ tf[sName]->save(fResults, fSubfolder);
            //~ 
        //~ }
        //~ 
    //~ }
    
    
    
    
    
    
    
    //~ 
    
    
    //~ 
    //~ //pt->setRange(100., 150.);
    //~ RooDataSet *rds = new RooDataSet("rds", "rds", RooArgList(*m, *pt), Import(*t["data_num"]));//, Cut("pt>100"));//, Cut("120<pt<130"));
    //~ 
    //~ // rf402_datahandling.C
    //~ // RooDataHist* dh2 = (RooDataHist*) dh.reduce(y,"x>0") ;
    //~ Double_t low = 100.;
    //~ Double_t up = 150.;
    //~ cout << "NumberToString(low) = " << NumberToString(low) << endl;
    //~ RooDataSet *rdsm = (RooDataSet*) rds->reduce(*m, ("pt>"+NumberToString(low)+" && pt<"+NumberToString(up)).c_str());
    //~ 
    //~ 
    //~ //cout << "reduced entries: " << rdsm->numEntries() << endl;
    //~ 
    //~ RooPlot *f1 = pt->frame();
    //~ RooPlot *f2 = m->frame();
    //~ RooPlot *f3 = m->frame();
    //~ 
    //~ 
    //~ rds->plotOn(f1);
    //~ rds->plotOn(f2);
    //~ rdsm->plotOn(f3);
    //~ 
    //~ TCanvas *c1 = new TCanvas("c1", "c1", 1400, 500);
    //~ c1->Divide(3, 1);
    //~ 
    //~ c1->cd(1);
    //~ f1->Draw();
    //~ 
    //~ c1->cd(2);
    //~ f2->Draw();
    //~ 
    //~ c1->cd(3);
    //~ f3->Draw();
    //~ 
    //~ cout << "rds: " << endl;
    //~ rds->Print();
    //~ cout << "rdsm: " << endl;
    //~ rdsm->Print();
    //~ 



























// test with roodatasets with cuts
    RooRealVar *m    = new RooRealVar("m", "m (GeV)", 60., 120.);
    RooRealVar *pt   = new RooRealVar("pt", "p_{T} (GeV)", 0., 700.); 
    
    
    pt->setRange(100., 150.);
    RooDataSet *rds = new RooDataSet("rds", "rds", RooArgList(*m, *pt), Import(*t["data_num"]));//, Cut("pt>100"));//, Cut("120<pt<130"));
    
    // rf402_datahandling.C
    // RooDataHist* dh2 = (RooDataHist*) dh.reduce(y,"x>0") ;
    RooDataSet *rdsm = (RooDataSet*) rds->reduce(*m);
    
    
    //cout << "reduced entries: " << rdsm->numEntries() << endl;
    
    RooPlot *f1 = pt->frame();
    RooPlot *f2 = m->frame();
    RooPlot *f3 = m->frame();
    
    
    rds->plotOn(f1);
    rds->plotOn(f2);
    rdsm->plotOn(f3);
    
    TCanvas *c1 = new TCanvas("c1", "c1", 1400, 500);
    c1->Divide(3, 1);
    
    c1->cd(1);
    f1->Draw();
    
    c1->cd(2);
    f2->Draw();
    
    c1->cd(3);
    f3->Draw();
    
    cout << "rds: " << endl;
    rds->Print();
    cout << "rdsm: " << endl;
    rdsm->Print();









    
    //~ tf["den"]->fit("hist");
    //~ tf["den"]->drawFrame(fResults);
    //~ tf["den"]->save(fResults);
    //~ 
    
    
    
    
    //~ 
    //~ RooFitResult *r = gaus->fitTo(*rdsGaus, Save());
    //~ r->Print();
    //~ 
    //~ // test histogram converting:
    //~ RooDataSet *rds = new RooDataSet("rds", "rds", *m, Import(*t["data_num"]));
    //~ TH1F* h1 = (TH1F*) rds->createHistogram("m", 480);
    //~ 
    //~ RooDataHist *rdh;
    //~ 
    //~ rdh = new RooDataHist("rdh", "rdh", *m, Import(*(rds->createHistogram(m->GetName(), 120))));
    //~ 
    //~ RooPlot *frame = NULL;
    //~ if(frame == NULL){
        //~ cout << "frame is null!" << endl;
    //~ }
    //~ 
    //~ //cout << "frame->GetName(): " << frame->GetName() << endl;
    //~ frame = m->frame(Title("frame"));
    //~ 
    //~ 
    //~ 
    //~ TCanvas *c1 = new TCanvas("c1", "c1", 600, 600);
    //~ c1->cd();
    //~ //h1->Draw("hist");
    //~ rdsGaus->plotOn(frame);
    //~ gaus->plotOn(frame);
    //~ 
    //~ // Layout(Double_t xmin,
    //~ //   Double_t xmax, Double_t ymax)
    //~ //gaus->paramOn(frame, Parameters(RooArgList(*gmean)), Layout(0.6, 0.9, 0.9));
    //~ gaus->paramOn(frame, Layout(0.6, 0.9, 0.9));
    //~ frame->getAttText()->SetTextSize(0.025) ; 
    //~ frame->Draw();
    //~ 
    //~ TFile *f = new TFile("tests.root", "RECREATE");
    //~ 
    //~ f->cd();
    //~ f->mkdir("hello");
    //~ f->cd("hello");
    //~ 
    //~ frame->Write("plotframe", TObject::kOverwrite); // 
    //~ 
    //~ f->Close();
    //~ 
    //~ 
    //~ cout << "final value of floating parameters:" << endl ;
    //~ r->floatParsFinal().Print("s") ;
    
    
    //~ 
    
    
    //TTemplateFit *f_den = new TTemplateFit("f_den", "Denominator", m);
    //f_den->addTrees(tree_data_den, tree_template_sig, tree_template_bkg_den);
    
    //f_den->drawRaw();
    
    //f_den->fit("hist");
    //f_den->drawRaw();
    //f_den->drawFrame();
    
    //~ cout << "range of m: " << m->getMin() << " to " << m->getMax() << endl;
    //~ 
    //~ m->setMin(58.);
    //~ m->setMax(118.);
    //~ 
    //~ cout << "range of m: " << m->getMin() << " to " << m->getMax() << endl;
    
    //delete f_den;
    
    
    
    
    
    
    
    // ******************* some tests
    //RooRealVar *m = new RooRealVar("m", "m (GeV)");//, 60., 120.);
    
    
    //~ RooDataSet *d_num = new RooDataSet("d_num", "Data numerator", *m, Import(*tree_data_num));
    //~ RooDataSet *d_den = new RooDataSet("d_den", "Data denominator", *m, Import(*tree_data_den));
    //~ RooDataSet *t_bkg_num = new RooDataSet("t_bkg_num", "Bkg numerator", *m, Import(*tree_template_bkg_num));
    //~ RooDataSet *t_bkg_den = new RooDataSet("t_bkg_den", "Bkg denominator", *m, Import(*tree_template_bkg_den));
    //~ RooDataSet *t_sig = new RooDataSet("t_sig", "Sig", *m, Import(*tree_template_sig));
    //~ 
    //~ 
    //~ 
    //~ 
    //~ RooPlot *frame_data_num = m->frame(Title("Data Numerator"));
    //~ RooPlot *frame_data_den = m->frame(Title("Data Denominator"));
    //~ RooPlot *frame_template_bkg_num = m->frame(Title("Template Background Numerator"));
    //~ RooPlot *frame_template_bkg_den = m->frame(Title("Template Background Denominator"));
    //~ RooPlot *frame_template_sig = m->frame(Title("Template Signal"));
    //~ 
    //~ RooPlot *frame_pullDen = m->frame(Title("pull distribution"));
    //~ RooPlot *frame_pullNum = m->frame(Title("pull distribution"));
    //~ 
    //~ d_num->plotOn(frame_data_num, MarkerColor(kBlack));
    //~ d_den->plotOn(frame_data_den, MarkerColor(kRed));
    //~ 
    //~ // frame->pullHist(("data_"+name).c_str(), ("model_"+name).c_str());
    //~ 
    //~ m->setBins(60);
    //~ RooDataHist *rdhDataNum = d_num->binnedClone();
    //~ RooDataHist *rdhDataDen = d_den->binnedClone();
    //~ 
    //~ RooDataHist *rdhSig = t_sig->binnedClone();
    //~ RooHistPdf *rhpSig0 = new RooHistPdf("rhpSig0", "rhpSig0", *m, *rdhSig, 0);
    //~ RooHistPdf *rhpSig2 = new RooHistPdf("rhpSig2", "rhpSig2", *m, *rdhSig, 3);
    //~ 
    //~ //RooKeysPdf *rkpSig0 = new RooKeysPdf("rkpSig0", "rkpSig0", *m, *t_sig, RooKeysPdf::MirrorBoth);
    //~ 
    //~ //m->setBins(10);
    //~ //RooDataHist *rdhBkgDen = t_bkg_den->binnedClone();
    //~ // RooKeysPdf kest1("kest1","kest1",x,*data1,RooKeysPdf::MirrorBoth) ;
    //~ RooKeysPdf *rkpBkgDen0 = new RooKeysPdf("rkpBkgDen0", "rkpBkgDen0", *m, *t_bkg_den, RooKeysPdf::MirrorBoth);
    //~ 
    //~ //m->setBins(10);
    //~ //RooDataHist *rdhBkgNum = t_bkg_num->binnedClone();
    //~ RooKeysPdf *rkpBkgNum0 = new RooKeysPdf("rkpBkgNum0", "rkpBkgNum0", *m, *t_bkg_num, RooKeysPdf::MirrorBoth);
    //~ 
    //~ // template plots
    //~ t_sig->plotOn(frame_template_sig, Binning(120), MarkerColor(kBlack));
    //~ rhpSig0->plotOn(frame_template_sig, LineStyle(kDashed));
    //~ rhpSig2->plotOn(frame_template_sig, LineColor(kBlue));
    //~ //rkpSig0->plotOn(frame_template_sig, LineStyle(kDashed), LineColor(kRed));
    //~ 
    //~ t_bkg_den->plotOn(frame_template_bkg_den, Binning(120), MarkerColor(kBlack));
    //~ rkpBkgDen0->plotOn(frame_template_bkg_den, LineColor(kBlue));
    //~ 
    //~ t_bkg_num->plotOn(frame_template_bkg_num, Binning(120), MarkerColor(kBlack));
    //~ rkpBkgNum0->plotOn(frame_template_bkg_num, LineColor(kBlue));
    //~ 
    //~ 
    //~ // fit model
    //~ string name = "try";
    //~ 
    //~ Double_t    gmean(0.),
                //~ gmean_low(-5.),
                //~ gmean_high(5.),
                //~ gwidth(1.),
                //~ gwidth_low(0.001),
                //~ gwidth_high(5.);
    //~ 
    //~ RooRealVar *NbkgDen = new RooRealVar("NbkgDen", "nbkg den", 0.2*NtotDen, 0.01*NtotDen, 0.6*NtotDen);
    //~ RooRealVar *NsigDen = new RooRealVar("NsigDen", "nsig num", 0.8*NtotDen, 0.4*NtotDen, NtotDen);
    //~ RooRealVar *NbkgNum = new RooRealVar("NbkgNum", "nbkg num", 0.2*NtotNum, 0.01*NtotNum, 0.6*NtotNum);
    //~ RooRealVar *NsigNum = new RooRealVar("NsigNum", "nsig num", 0.8*NtotNum, 0.4*NtotNum, NtotNum);
    //~ 
    //~ 
    //~ RooRealVar *gm = new RooRealVar(("gm_"+name).c_str(), "Gaussian mean", gmean, gmean_low, gmean_high);
    //~ RooRealVar *gs = new RooRealVar(("gs_"+name).c_str(), "Gaussian width", gwidth, gwidth_low, gwidth_high);
    //~ 
    //~ Float_t rho_sig = 0.3162,   // 1/(2*sqrt(sigma))
            //~ rho_bkg = 0.2887;   // 1/sqrt(12)
    //~ 
    //~ RooGaussian *gaus        = new RooGaussian(("gaus_"+name).c_str(), "Gaussian smearing", *m, *gm, *gs);
    //~ RooNumConvPdf *smearedSig  = new RooNumConvPdf(("smearedSig_"+name).c_str(), "Smeared Z Signal", *m, *gaus, *rhpSig2);
    //~ 
    //~ RooAddPdf *modelDen       = new RooAddPdf(    ("modelDen_"+name).c_str(), 
                                    //~ "Signal + background", 
                                    //~ RooArgList(*rkpBkgDen0, *smearedSig), 
                                    //~ RooArgList(*NbkgDen, *NsigDen));
    //~ 
    //~ RooAddPdf *modelNum       = new RooAddPdf(    ("modelNum_"+name).c_str(), 
                                    //~ "Signal + background", 
                                    //~ RooArgList(*rkpBkgNum0, *smearedSig), 
                                    //~ RooArgList(*NbkgNum, *NsigNum));
    //~ 
    //~ 
    //~ 
    //~ 
    //~ //modelNum->plotOn(frame_data_num, LineColor(kBlue));
    //~ //modelNum->plotOn(frame_data_num, Components("rkpBkgNum0"), LineColor(kBlue), LineStyle(kDashed));
    //~ 
    //~ 
    //~ // **************************************************************************************************************** fit
    //~ cout << "press key to start fit... " << endl;
    //~ getchar();
    //~ 
    //~ modelNum->fitTo(*rdhDataNum, NumCPU(6), Timer(kTRUE)) ;
    //~ modelDen->fitTo(*rdhDataDen, NumCPU(6), Timer(kTRUE)) ;
    //~ 
    //~ 
    //~ // hpull = frame->pullHist(("data_"+name).c_str(), ("model_"+name).c_str());
    //~ // fpull->addPlotable(hpull, "P");
    //~ modelNum->plotOn(frame_data_num, LineColor(kBlue));
    //~ RooHist *NumPull = frame_data_num->pullHist();
    //~ frame_pullNum->addPlotable(NumPull, "P");
    //~ 
    //~ modelNum->plotOn(frame_data_num, Components("rkpBkgNum0"), LineColor(kBlue), LineStyle(kDashed));
    //~ 
    //~ 
    //~ modelDen->plotOn(frame_data_den, LineColor(kBlue));
    //~ RooHist *DenPull = frame_data_den->pullHist();
    //~ frame_pullDen->addPlotable(DenPull, "P");
    //~ 
    //~ modelDen->plotOn(frame_data_den, Components("rkpBkgDen0"), LineColor(kBlue), LineStyle(kDashed));
    //~ 
    //~ 
    //~ 
    //~ 
    //~ TCanvas *c1 = new TCanvas("c1", "c1", 1300, 750);
    //~ c1->Divide(2, 1);
    //~ 
    //~ c1->cd(1);
    //~ TPad *p11 = new TPad("p11", "p11", 0., 0.3, 1., 1.);
    //~ p11->SetLeftMargin(0.16);
    //~ p11->Draw();
    //~ p11->cd();
    //~ frame_data_den->GetYaxis()->SetTitleOffset(1.8);
    //~ 
    //~ frame_data_den->Draw();
    //~ 
    //~ c1->cd(1);
    //~ TPad *p12 = new TPad("p12", "p12", 0., 0., 1., 0.3);
    //~ p12->SetLeftMargin(0.16);
    //~ p12->Draw();
    //~ p12->cd();
    //~ 
    //~ frame_pullDen->Draw();
    //~ 
    //~ c1->cd(2);
    //~ TPad *p21 = new TPad("p21", "p21", 0., 0.3, 1., 1.);
    //~ p21->SetLeftMargin(0.16);
    //~ p21->Draw();
    //~ p21->cd();
    //~ frame_data_num->GetYaxis()->SetTitleOffset(1.8);
    //~ frame_data_num->Draw();
    //~ 
    //~ c1->cd(2);
    //~ TPad *p22 = new TPad("p22", "p22", 0., 0., 1., 0.3);
    //~ p22->SetLeftMargin(0.16);
    //~ p22->Draw();
    //~ p22->cd();
    //~ 
    //~ frame_pullNum->Draw();
    //~ 
    //~ 
    //~ 
    //~ 
    //~ 
    //~ TCanvas *c2 = new TCanvas("c2", "c2", 1500, 500);
    //~ c2->Divide(3, 1);
    //~ 
    //~ c2->cd(1);
    //~ frame_template_sig->Draw();
    //~ 
    //~ c2->cd(2);
    //~ frame_template_bkg_den->Draw();
    //~ 
    //~ c2->cd(3);
    //~ frame_template_bkg_num->Draw();
    //~ 
    //~ 
    //~ 
    //~ cout << "Numerator: " << endl;
    //~ modelNum->printCompactTree();
    //~ 
    //~ cout << "Denominator: " << endl;
    //~ modelDen->printCompactTree();
    
    









*/



