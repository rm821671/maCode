
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

RooAbsPdf* getModelPdf(RooRealVar& m, Int_t N, string component, string fraction)
{   // parametrized fit model(s)
    
    // model: 
    //  signal = breit wigner , convoluted with gaussian OR double cb
    //  background = cms shape
    
    
    
    Double_t Ntot = N;      // total events
    Double_t f;       // signal fraction
    
    Double_t m_z = 91.1876; // Z mass in GeV
    Double_t w_z = 2.495;   // Z decay width in GeV
    
    // initialize signal and background fraction
    RooRealVar *nbkg = NULL;//new RooRealVar("nbkg", "Background events", (1-f)*Ntot);//, 0.01*Ntot, 0.2*Ntot);
    RooRealVar *nsig = NULL;//new RooRealVar("nsig", "Signal events", f*Ntot);//, 0.8*Ntot, Ntot);
    
    //fractions for nominator/denumerator
    if(fraction.find("den") != string::npos){
        cout << "Model for denominator!!!!~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
        f = 0.978;
        nbkg = new RooRealVar("nbkg", "Background events", (1-f)*Ntot, (1-f)*Ntot*0.9, (1-f)*Ntot*1.1);
        nsig = new RooRealVar("nsig", "Signal events", f*Ntot, f*Ntot*0.9, Ntot);
        //~ nbkg->setVal((1-f)*Ntot);
        //~ nbkg->setMin((1-f)*Ntot*0.9);
        //~ nbkg->setMax((1-f)*Ntot*1.1);
        //~ nsig->setVal(f*Ntot);
        //~ nsig->setMin(f*Ntot*0.9);
        //~ nsig->setMax(Ntot);
    }
    //
    if(fraction.find("num") != string::npos){
        cout << "Model for numerator!!!!~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
        f = 0.662;
        nbkg = new RooRealVar("nbkg", "Background events", (1-f)*Ntot, (1-f)*Ntot*0.9, (1-f)*Ntot*1.1);
        nsig = new RooRealVar("nsig", "Signal events", f*Ntot, f*Ntot*0.9, f*Ntot*1.1);
        //~ nbkg->setVal((1-f)*Ntot);
        //~ nbkg->setMin((1-f)*Ntot*0.9);
        //~ nbkg->setMax((1-f)*Ntot*1.1);
        //~ nsig->setVal(f*Ntot);
        //~ nsig->setMin(f*Ntot*0.9);
        //~ nsig->setMax(f*Ntot*1.1);
    }
    
    
    // gaussian
    RooRealVar *mg  = new RooRealVar("mg", "Mean gaussian", 0., -2., 2.);
    RooRealVar *sg  = new RooRealVar("sg", "Width gaussian", 1., 0.1, 2.);
    RooAbsPdf  *gaussian    = new RooGaussian("gaussian", "Gaussian smearing pdf", m, *mg, *sg);
    
    // double sided crystal ball
    RooRealVar *mcb         = new RooRealVar("mcb", "mean crystalball", -0.68, -2., 2.);
    RooRealVar *scb         = new RooRealVar("scb", "width crystalball", 1.07, 0.5, 4.);//, 0.5, 4.);
    RooRealVar *alphacb1    = new RooRealVar("alphacb1", "alpha crystalball 1", 4.5, 1., 5.);
    RooRealVar *alphacb2    = new RooRealVar("alphacb2", "alpha crystalball 2", 2.03, 1., 5.);
    RooRealVar *ncb1        = new RooRealVar("ncb1", "n crystalball 1", 0.8, 0.1, 25);
    RooRealVar *ncb2        = new RooRealVar("ncb2", "n crystalball 2", 1.38, 1., 5.);
    RooDoubleCBFast *doubleCB = new RooDoubleCBFast("doubleCB", "double sided crystal ball", m, *mcb, *scb, *alphacb1, *ncb1, *alphacb2, *ncb2);
    
    // breit wigner
    RooRealVar *mbw = new RooRealVar("mbw", "Mean breit wigner", m_z);  // fix mean to Z mass
    RooRealVar *sbw = new RooRealVar("sbw", "Width breit wigner", w_z); // fix width to Z decay width
    RooAbsPdf  *breitwigner = new RooBreitWigner("breitwigner", "Breit Wigner Peak", m, *mbw, *sbw);
    
    // convoluted signals
    m.setBins(10000, "cache");
    RooAbsPdf  *signalGaus = new RooFFTConvPdf("signalGaus", "Z peak shape BW(*)Gaus", m, *breitwigner, *gaussian);
    RooAbsPdf  *signalDCB  = new RooFFTConvPdf("signalDCB", "Z peak shape BW(*)DCB", m, *breitwigner, *doubleCB);
    
    // cms shape (errorfunction times falling exponential)
    RooRealVar *alpha   = new RooRealVar("alpha", "alpha", 85.18, 80., 100.);
    RooRealVar *beta    = new RooRealVar("beta", "beta", 0.14, 0., .8);
    RooRealVar *gamma   = new RooRealVar("gamma", "gamma", 0.041, 0., 5.);
    RooRealVar *peak    = new RooRealVar("peak", "peak", 5.84, 1., 10.);
    if(fraction.find("den") != string::npos){
        beta->setVal(0.14);
        beta->setMin(0.);
        beta->setMax(0.2);
        gamma->setVal(0.04);
        gamma->setMin(0.);
        gamma->setMax(0.1);
    }
    if(fraction.find("num") != string::npos){
        beta->setVal(0.19);
        beta->setMin(0.05);
        beta->setMax(0.25);
        gamma->setVal(0.03);
        gamma->setMin(0.);
        gamma->setMax(0.1);
    }
    
    RooCMSShape *bkg = new RooCMSShape("bkg", "background shape", m, *alpha, *beta, *gamma, *peak);
    
    // complete model
    RooAbsPdf  *modelGaus = new RooAddPdf("modelGaus", "Signal+Background", RooArgList(*signalGaus, *bkg), RooArgList(*nsig, *nbkg));
    RooAbsPdf  *modelDCB  = new RooAddPdf("modelDCB", "Signal+Background", RooArgList(*signalDCB, *bkg), RooArgList(*nsig, *nbkg));
    
    if(     !component.compare("signalGaus"))    return signalGaus;
    else if(!component.compare("signalDCB"))     return signalDCB;
    else if(!component.compare("modelGaus"))     return modelGaus;
    else if(!component.compare("modelDCB"))      return modelDCB;
    else if(!component.compare("background"))    return bkg;
    else{
        cout << "no parameter given while calling getModelPdf(), modelGaus returned!" << endl;
        return modelGaus;
    }
}

/**********************************************************************************************
 * fits and stuff
 * 
 ***/

void workflowHandler(map<string, string> set){
    
    
    cout << "rooFitHeader.workflowHandler()" << endl;
    
    string filepath, dropbox, ss;
    
    // filepath and dropbox path changes, depending on system
    SystemPath(filepath, dropbox);
    
    // subpath for selected files
    ss = "root_selectorFiles/";
    
    cout << "filepath = " << filepath << endl;
    cout << "dropbox  = " << dropbox << endl;
    cout << "dropbox/root_selectorFiles = " << dropbox+ss << endl;
    
    // append filenames with the correct path:
    for(auto& mapIt: set){
        set[mapIt.first] = dropbox+ss+mapIt.second;
    }
    //~ for(auto& mapIt: set){
        //~ cout << mapIt.second << endl;
    //~ }
    
    //~  // content of set:
    //~ 
    //~ // templates, events, etc
    //~ set["templateSignal"]
    //~ set["templateBackground"]
    //~ set["templateBackgroundMC_stack"]
    //~ set["eventsData"]
    //~ set["eventsMC_stack"]
    //~ set["closureMC_stack"]
    //~ // mc templates for background seperated
    //~ set["templateBackgroundMC_WJetsToLNu"]
    //~ set["templateBackgroundMC_DYJetsToLL"]
    //~ set["templateBackgroundMC_TTGJets"
    //~ set["templateBackgroundMC_WGToLNuG"]
    //~ set["templateBackgroundMC_ZGTo2LG"]
    //~ set["templateBackgroundMC_TTJets"]
    //~ // mc events seperated
    //~ set["eventsMC_WJetsToLNu"]
    //~ set["eventsMC_DYJetsToLL"]
    //~ set["eventsMC_TTGJets"]
    //~ set["eventsMC_WGToLNuG"]
    //~ set["eventsMC_ZGTo2LG"]
    //~ set["eventsMC_TTJets"]
    
    ///////////////////////////////////////////////////////////////////////////////////////////
    cout << "start!" << endl;
    //test();
    
    //~ templateKernelUnbinned(set); 
    
    readFitResults(dropbox);
    //~ closurePlot(set);
    
    return ;
}

void test(map<string, string> &set){
    
    
    Int_t a[] = {4, 5, 2, 3};
    
    string p[] = {"a", "b", "c", "d", "e", "f", "g", "h", "i", "j"};
    
    
    for(const Int_t &ar : a){
        cout << "arrayvalue: " << ar << "\t string[" << ar << "] = " << p[ar] << endl;
        
    }
    
    string texts[] = {"Apple", "Banana", "Orange"};
    for(auto &&text : texts)
        cout << "value of text: " << text << endl;
    
    
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

// ***************************************************************************************************************************
// * all fits and stuff
// *
// ***************************************************************************************************************************
void templateKernelUnbinned(map<string, string> set){
//void templateKernelUnbinned(string signal, string background, string mcbackground, string dataset, string mcset){
    
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
    
    //~ // *** read out data trees ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    string signal       = set["templateSignal"];
    string background   = set["templateBackground"];
    string dataset      = set["eventsData"];
    string mcset        = set["eventsMC_stack"];
    string mcbackground = set["templateBackgroundMC_stack"];
    
    //~ cout << "mcset = " << mcset << endl;
    //~ getchar();
    
    // templates and data files
    TFile *fSig   = new TFile(signal.c_str(), "READ");
    TFile *fBkg   = new TFile(background.c_str(), "READ");
    TFile *fDat   = new TFile(dataset.c_str(), "READ");
    TFile *f_Mc   = new TFile(mcset.c_str(), "READ");
    TFile *fBkgMc = new TFile(mcbackground.c_str(), "READ");
    
    // read out trees
    map<string, TTree*> t;
    
    string treeNames[] =    {   "data_num",
                                "data_den",
                                "mc_num",
                                "mc_den",
                                "template_bkg_num",
                                "template_bkg_den",
                                "template_bkg_num_MC",
                                "template_bkg_den_MC",
                                
                                "template_sig",
                            };
    Int_t NtreeNames = arraysize(treeNames);
    
    t["data_num"] =            (TTree*) fDat->Get("data_num");
    t["data_den"] =            (TTree*) fDat->Get("data_den");
    t["mc_num"]   =            (TTree*) f_Mc->Get("mc_num");
    t["mc_den"]   =            (TTree*) f_Mc->Get("mc_den");
    t["template_bkg_num"] =    (TTree*) fBkg->Get("template_bkg_num");
    t["template_bkg_den"] =    (TTree*) fBkg->Get("template_bkg_den");
    t["template_sig"] =        (TTree*) fSig->Get("template_sig");
    
    t["template_bkg_num_MC"]      = (TTree*) fBkgMc->Get("template_bkg_num_MC");
    t["template_bkg_den_MC"]      = (TTree*) fBkgMc->Get("template_bkg_den_MC");
    t["template_bkg_num_MC_true"] = (TTree*) fBkgMc->Get("template_bkg_num_MC_true");
    t["template_bkg_den_MC_true"] = (TTree*) fBkgMc->Get("template_bkg_den_MC_true");
    
    cout << "data_num.Entries()         = " << t["data_num"]->GetEntries() << endl;
    cout << "data_den.Entries()         = " << t["data_den"]->GetEntries() << endl;
    cout << "template_bkg_num.Entries() = " << t["template_bkg_num"]->GetEntries() << endl;
    cout << "template_bkg_den.Entries() = " << t["template_bkg_den"]->GetEntries() << endl;
    cout << "template_sig.Entries()     = " << t["template_sig"]->GetEntries() << endl;
    
    //              0       1
    string ts[] = {"den", "num"};
    Int_t Nts = arraysize(ts);
    
    
    // *** class objects for the fittings ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // template fit class objects
    map<string, TTemplateFit*> tf;
    // map for all roo data sets
    map<string, RooDataSet*> rds;
    
    //TAuxiliary aux;
    
    // invariant mass
    RooRealVar *m = new RooRealVar("m", "m (GeV)", 60., 120.);
    // kinematic variables
    RooRealVar *pt   = new RooRealVar("pt",   "p_{T}^{probe} (GeV)", 0., 1000.);
    RooRealVar *nvtx = new RooRealVar("nvtx", "N_{vertex}", 0., 35.);
    RooRealVar *ntrk = new RooRealVar("ntrk", "N_{track}", 0., 240.);
    RooRealVar *eta  = new RooRealVar("eta",  "|#eta^{probe}|", 0., 1.5);
    RooRealVar *w    = new RooRealVar("w",    "weights", -200., 200.);
    
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // for montecarlo:
    //      > create RooDataSets with weights
    // (check tutorial rf403)
    //   // Instruct dataset wdata in interpret w as event weight rather than as observable
    //      RooDataSet wdata(   data->GetName(),
    //                          data->GetTitle(),
    //                          data,
    //                          *data->get(),
    //                          0,
    //                          w->GetName()) ;
    map<string, RooDataSet*> rdsMc;
    map<string, RooDataSet*> rdsMcW;
    string mcname;
    mcname = "mc_num";
    rdsMc[mcname] = new RooDataSet( mcname.c_str(), 
                                    mcname.c_str(), 
                                    RooArgList(*m, *w), 
                                    Import(*t[mcname])); 
    mcname = "mc_den";
    rdsMc[mcname] = new RooDataSet( mcname.c_str(),
                                    mcname.c_str(), 
                                    RooArgList(*m, *w), 
                                    Import(*t[mcname]));
    // now use the weight-branch (w) to make roofit interpret 
    // them as a weight for the other branches
    mcname = "mc_num";
    rdsMcW[mcname] = new RooDataSet(    rdsMc[mcname]->GetName(),
                                        rdsMc[mcname]->GetTitle(),
                                        rdsMc[mcname], 
                                        *rdsMc[mcname]->get(), 
                                        0,
                                        w->GetName());
    mcname = "mc_den";
    rdsMcW[mcname] = new RooDataSet(    rdsMc[mcname]->GetName(), 
                                        rdsMc[mcname]->GetTitle(), 
                                        rdsMc[mcname], 
                                        *rdsMc[mcname]->get(), 
                                        0, 
                                        w->GetName());
    
    //~ cout << "weightes stuff... " << endl;
    //~ getchar();
    //~ mcname = "babedibubedi";
    //~ rdsMc[mcname] =         new RooDataSet( mcname.c_str(), 
                                //~ mcname.c_str(), 
                                //~ RooArgList(*m, *pt, *w), 
                                //~ Import(*t[mcname])); 
    //~ rdsMc[mcname] =         new RooDataSet(    
                                //~ rdsMc[mcname]->GetName(),
                                //~ rdsMc[mcname]->GetTitle(),
                                //~ rdsMc[mcname], 
                                //~ *rdsMc[mcname]->get(), 
                                //~ 0,
                                //~ w->GetName());
    //~ cout << "weightes stuff... done" << endl;
    //~ getchar();
    
    
    map<string, TH1F*> thMc;
    mcname = "mc_num";
    thMc[mcname] = (TH1F*) rdsMcW[mcname]->createHistogram(m->GetName(), 60);
    mcname = "mc_den";
    thMc[mcname] = (TH1F*) rdsMcW[mcname]->createHistogram(m->GetName(), 60);
    
    
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //~ // create datasets in each variable
    map<string, RooDataSet*> rdsM;      // map with invariant mass dependency data
    map<string, RooDataSet*> rdsPt;     // pt
    map<string, RooDataSet*> rdsNtrk;   // ntrk
    map<string, RooDataSet*> rdsPtNtrk; // pt and ntrk
    map<string, RooDataSet*> rdsNvtx;   // nvtx
    map<string, RooDataSet*> rdsEta;    // Eta
    map<string, RooDataSet*> rdsW;      // weightes
    
    for(int i=0; i<NtreeNames; i++){
        cout << "treeNames["<<i<<"] = " << treeNames[i] << endl;
        //~ getchar();
        
        //~ rdsM[treeNames[i]]         = new RooDataSet(treeNames[i].c_str(),
                                                    //~ treeNames[i].c_str(),
                                                    //~ *m,
                                                    //~ Import(*t[treeNames[i]]));
        //~ 
        //~ rdsPt[treeNames[i]]        = new RooDataSet(treeNames[i].c_str(), 
                                                    //~ treeNames[i].c_str(), 
                                                    //~ *pt, 
                                                    //~ Import(*t[treeNames[i]]));
        //~ rdsPt[treeNames[i]]->merge(rdsM[treeNames[i]]);
        
        //~ if (s1.find(s2) != std::string::npos) {
            //~ std::cout << "found!" << '\n';
        //~ }
        
        if(treeNames[i].find("mc") != string::npos || treeNames[i].find("MC") != string::npos){
            // if monte carlo, use weights
            cout << "treeNames = " << treeNames[i] << " is Simulation! --> weights applied" << endl;
            rdsW[treeNames[i]]        = new RooDataSet( treeNames[i].c_str(), 
                                                        treeNames[i].c_str(), 
                                                        RooArgList(*m, *pt, *w), 
                                                        Import(*t[treeNames[i]]));
            rdsPt[treeNames[i]]       = new RooDataSet( rdsW[treeNames[i]]->GetName(),
                                                        rdsW[treeNames[i]]->GetTitle(),
                                                        rdsW[treeNames[i]], 
                                                        *rdsW[treeNames[i]]->get(), 
                                                        0,
                                                        w->GetName());
            
            rdsW[treeNames[i]]      = new RooDataSet(   treeNames[i].c_str(), 
                                                        treeNames[i].c_str(), 
                                                        RooArgList(*m, *ntrk, *w), 
                                                        Import(*t[treeNames[i]]));
            rdsNtrk[treeNames[i]]       = new RooDataSet( rdsW[treeNames[i]]->GetName(),
                                                        rdsW[treeNames[i]]->GetTitle(),
                                                        rdsW[treeNames[i]], 
                                                        *rdsW[treeNames[i]]->get(), 
                                                        0,
                                                        w->GetName());
            
            rdsW[treeNames[i]]    = new RooDataSet(treeNames[i].c_str(), 
                                                        treeNames[i].c_str(), 
                                                        RooArgList(*m, *pt, *ntrk, *w), 
                                                        Import(*t[treeNames[i]]));
            rdsPtNtrk[treeNames[i]]       = new RooDataSet( rdsW[treeNames[i]]->GetName(),
                                                        rdsW[treeNames[i]]->GetTitle(),
                                                        rdsW[treeNames[i]], 
                                                        *rdsW[treeNames[i]]->get(), 
                                                        0,
                                                        w->GetName());
            
            rdsW[treeNames[i]]      = new RooDataSet(treeNames[i].c_str(), 
                                                        treeNames[i].c_str(), 
                                                        RooArgList(*m, *nvtx, *w), 
                                                        Import(*t[treeNames[i]]));
            rdsNvtx[treeNames[i]]       = new RooDataSet( rdsW[treeNames[i]]->GetName(),
                                                        rdsW[treeNames[i]]->GetTitle(),
                                                        rdsW[treeNames[i]], 
                                                        *rdsW[treeNames[i]]->get(), 
                                                        0,
                                                        w->GetName());
            
            rdsW[treeNames[i]]       = new RooDataSet(treeNames[i].c_str(), 
                                                        treeNames[i].c_str(), 
                                                        RooArgList(*m, *eta, *w), 
                                                        Import(*t[treeNames[i]]));
            rdsEta[treeNames[i]]       = new RooDataSet( rdsW[treeNames[i]]->GetName(),
                                                        rdsW[treeNames[i]]->GetTitle(),
                                                        rdsW[treeNames[i]], 
                                                        *rdsW[treeNames[i]]->get(), 
                                                        0,
                                                        w->GetName());
            rdsEta[treeNames[i]]->Print();
            
            
        } else {
            //~ cout << "treeNames = " << treeNames[i] << " is Data!" << endl;
            rdsPt[treeNames[i]]        = new RooDataSet(treeNames[i].c_str(), 
                                                        treeNames[i].c_str(), 
                                                        RooArgList(*m, *pt), 
                                                        Import(*t[treeNames[i]]));
            rdsNtrk[treeNames[i]]      = new RooDataSet(treeNames[i].c_str(), 
                                                        treeNames[i].c_str(), 
                                                        RooArgList(*m, *ntrk), 
                                                        Import(*t[treeNames[i]]));
            rdsPtNtrk[treeNames[i]]    = new RooDataSet(treeNames[i].c_str(), 
                                                        treeNames[i].c_str(), 
                                                        RooArgList(*m, *pt, *ntrk), 
                                                        Import(*t[treeNames[i]]));
            rdsNvtx[treeNames[i]]      = new RooDataSet(treeNames[i].c_str(), 
                                                        treeNames[i].c_str(), 
                                                        RooArgList(*m, *nvtx), 
                                                        Import(*t[treeNames[i]]));
            rdsEta[treeNames[i]]       = new RooDataSet(treeNames[i].c_str(), 
                                                        treeNames[i].c_str(), 
                                                        RooArgList(*m, *eta), 
                                                        Import(*t[treeNames[i]]));
        }
    }
    
    //~ // ** create file for the results ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //~ // and subfolder 
    // root file to store fit results
    TFile *fResults = new TFile("results_fits.root", "update");
    // subfolder for each fit
    string fSubfolder;
    string unique;
    
    unique = "";
    
    RooAbsPdf* parametrizedModel;
    RooArgSet* parametrizedPars;
    
    
     // global fits
    
    // global fits on data ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    for(int i=0; i<Nts; i++){
        // create template fit objects
        cout << "initialize tf["<< ts[i] << "]:" << endl;
        tf[ts[i]] = new TTemplateFit(ts[i], ts[i], m);
        tf[ts[i]]->addData(t["data_"+ts[i]], t["template_sig"], t["template_bkg_"+ts[i]]);
        
        tf[ts[i]]->setRhoBkg(1.);         // set rho to 1 (smoothness prefered over detail)
        tf[ts[i]]->setUnbinnedFit(kTRUE); // unbinned fit
        
        fSubfolder = unique+"global/withSigConvFFT";
        
        //~ // fit with convolution
        tf[ts[i]]->buildPdf("hist", "fit");
        tf[ts[i]]->drawRaw(fResults, fSubfolder);
        tf[ts[i]]->drawFrame(fResults, fSubfolder);
        tf[ts[i]]->save(fResults, fSubfolder);
        
        //~ // parametrized model fit
        parametrizedModel = getModelPdf(*m, tf[ts[i]]->getNtot(), "modelDCB", ts[i]);
        tf[ts[i]]->setUnbinnedFit(kFALSE); // binned fit
        tf[ts[i]]->fitToPdf(parametrizedModel, fResults, fSubfolder, "fit", "bkg");
        
        tf[ts[i]]->setUnbinnedFit(kTRUE); // unbinned fit
        
        // no convolution
        fSubfolder = unique+"global/noSigConvFFT";
        tf[ts[i]]->setSignalConvolution(kFALSE); // set convolution off
        tf[ts[i]]->buildPdf("hist", "fit");     // build template based pdfs (histogram based for signal) and fit them
        tf[ts[i]]->drawRaw(fResults, fSubfolder);
        tf[ts[i]]->drawFrame(fResults, fSubfolder);
        tf[ts[i]]->save(fResults, fSubfolder);
        
        //~ // scale the background template
        fSubfolder = unique+"global/backgroundVariation/scalingPlots";
        tf[ts[i]]->scaleBackgroundTemplate(t["template_bkg_"+ts[i]+"_MC"], t["template_bkg_"+ts[i]+"_MC_true"], fResults, fSubfolder);
        // now fit again
        fSubfolder = unique+"global/backgroundVariation/templateFit";
        tf[ts[i]]->setSignalConvolution(kTRUE); // set convolution off
        tf[ts[i]]->setUnbinnedFit(kFALSE); // binned fit
        tf[ts[i]]->buildPdf("hist", "fit");
        tf[ts[i]]->drawRaw(fResults, fSubfolder);
        tf[ts[i]]->drawFrame(fResults, fSubfolder);
        tf[ts[i]]->save(fResults, fSubfolder);
        
    }
    // global fits on mc   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    for(int i=0; i<Nts; i++){
        // create template fit objects
        cout << "initialize tf["<< ts[i] << "]:" << endl;
        tf[ts[i]] = new TTemplateFit(ts[i], ts[i], m);
        tf[ts[i]]->setType("mc"); // initialize as montecarlo fitter
        
        //tf[ts[i]]->addData(rdsMcW["mc_"+ts[i]], t["template_sig"], t["template_bkg_"+ts[i]]);
        tf[ts[i]]->addData(thMc["mc_"+ts[i]], t["template_sig"], t["template_bkg_"+ts[i]+"_MC"]);
        
        tf[ts[i]]->setRhoBkg(1.);          // set rho to 1 (smoothness prefered over detail)
        tf[ts[i]]->setUnbinnedFit(kFALSE); // binned fit
        
        // fit with convolution
        fSubfolder = unique+"mc_global/withSigConvFFT";
        tf[ts[i]]->buildPdf("hist", "fit");
        tf[ts[i]]->drawRaw(fResults, fSubfolder);
        tf[ts[i]]->drawFrame(fResults, fSubfolder);
        tf[ts[i]]->save(fResults, fSubfolder);
        
        // fit with true background template for systematic uncertainty
        tf[ts[i]]->addData(thMc["mc_"+ts[i]], t["template_sig"], t["template_bkg_"+ts[i]+"_MC_true"]);
        tf[ts[i]]->setRhoBkg(1.);          // set rho to 1 (smoothness prefered over detail)
        tf[ts[i]]->setUnbinnedFit(kFALSE); // binned fit
        fSubfolder = unique+"mc_global/withTrueBkg";
        tf[ts[i]]->buildPdf("hist", "fit");
        tf[ts[i]]->drawRaw(fResults, fSubfolder);
        tf[ts[i]]->drawFrame(fResults, fSubfolder);
        tf[ts[i]]->save(fResults, fSubfolder);
        
    }
    
    // */
    
    
    
    //~ // *** define binnings for 1d variable plots ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    
    // Pt 
    //                          0     1     2       3       4     5     6     7        8     9     10
    Double_t bins_pt[] =    {  40. , 45. ,  50.,   60.,   70.,   90.,   110., 140.,   250.};
    string bins_pt_pdf[] =  {"hist","hist","hist","hist","hist","hist","hist","hist","hist"};
    Int_t Npt = arraysize(bins_pt);
    // Nvtx
    //                          0     1     2       3       4     5     6     7     8      9       10     11      12    13    14    15    16    17    18    19
    Double_t bins_nvtx[] =  {   1.,   2.,  3.,     4.,      5.,    6.,  7.,   8.,   9.,    10.,    11.,   12.,  13.,    14.,  15.,  16.,  17.,  18.,  19.,  20.  };
    string bins_nvtx_pdf[] ={"hist","hist","hist","hist","hist","hist","hist","hist","hist","hist","hist","hist","hist","hist","hist","hist","hist","hist", "hist"};
    Int_t Nvtx = arraysize(bins_nvtx);
    // Ntrk
    //                          0     1     2       3       4     5     6     7        8     9     10
    Double_t bins_ntrk[] =  {  0.,   20.,   40.,   60.,   80.,   100.,  120.,  140.,  160.};
    string bins_ntrk_pdf[] ={"hist","hist","hist","hist","hist","hist","hist","hist","hist"};
    Int_t Ntrk = arraysize(bins_ntrk);
    // eta
    //                          0     1     2       3       4     5     6     7        8     9     10
    Double_t bins_eta[] =   {  0.,    0.2,  0.4,   0.6,    0.8,   1.,   1.2,  1.4};
    string bins_eta_pdf[] = {"hist","hist","hist","hist","hist","hist","hist","hist"};
    Int_t Neta = arraysize(bins_eta);
    
    
    
    string sBin, sName;
    string sLow, sUp;
    
    cout << "Npt  = " << Npt  << endl;
    cout << "Ntrk = " << Ntrk << endl;
    cout << "Nvtx = " << Nvtx << endl;
    cout << "Neta = " << Neta << endl;
    
    Int_t fitPtBin = 7;
    Int_t fitFraction = 1; // 0: den, 1: num
    
    //~ // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //~ // tests...
    //~ RooPlot *abc = m->frame();
    //~ RooPlot* a2  = m->frame();
    //~ RooPlot* a3  = m->frame();
    //~ rdsPt["template_bkg_den_MC"]->plotOn(abc);
    //~ 
    //~ TCanvas *myTestCanvas = new TCanvas("myTestCanvas", "...", 600, 600);
    //~ myTestCanvas->Divide(2, 2);
    //~ myTestCanvas->cd(1);
    //~ abc->Draw();
    //~ 
    //~ myTestCanvas->cd(2);
    //~ rdsPt["mc_num"]->plotOn(a2);
    //~ a2->Draw();
    //~ 
    //~ myTestCanvas->cd(3);
    //~ rdsPt["template_bkg_num_MC"]->plotOn(a3);
    //~ a3->Draw();
    //~ 
    //~ myTestCanvas->SaveAs("testPlot.png");
    
    
    
    //getchar();
    
    
    
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Data
    unique = "data_";
    // loop nominator and denominator:  
    // - for each, create a dataset within the pt bins of above;
    for(int i=0; i<Nts; i++){
    //for(int i=fitFraction; i<=fitFraction; i++){
        for(int bin=0; bin < (Npt-1); bin++){
        //for(int bin=fitPtBin ; bin <= fitPtBin; bin++){
            sLow = NumberToString(bins_pt[bin]);
            sUp =  NumberToString(bins_pt[bin+1]);
            sBin = sLow+"-"+sUp;
            sName = ts[i]+"_pt_"+sBin;
            //cout << sName << endl;
            
            rds["data_"+ts[i]] =         (RooDataSet*)  rdsPt["data_"+ts[i]]->reduce(*m,           ("pt>="+sLow+" && pt<"+sUp).c_str());
            rds["template_sig"] =        (RooDataSet*)  rdsPt["template_sig"]->reduce(*m,          ("pt>="+sLow+" && pt<"+sUp).c_str());
            rds["template_bkg_"+ts[i]] = (RooDataSet*)  rdsPt["template_bkg_"+ts[i]]->reduce(*m,   ("pt>="+sLow+" && pt<"+sUp).c_str());
            
            cout << "Entries in "+sName << " = " << rds["data_"+ts[i]]->numEntries() << endl;
            
            fSubfolder = unique+"pt/binning1";
            //fSubfolder = "pt/pullTests/withAverage";
            // build pdf s and draw frames
            tf[sName] = new TTemplateFit(sName, sName, m);
            tf[sName]->addData(rds["data_"+ts[i]], rds["template_sig"], rds["template_bkg_"+ts[i]]);
            // now perform an unbinned likelihood fit
            tf[sName]->setUnbinnedFit(kTRUE);
            tf[sName]->setRhoBkg(1.);
            tf[sName]->buildPdf(bins_pt_pdf[bin], "fit");
            tf[sName]->drawRaw(fResults, fSubfolder);
            tf[sName]->drawFrame(fResults, fSubfolder);
            tf[sName]->save(fResults, fSubfolder);
            
            //del tf[sName];
        }
        
        //goto jumperEnd;
        
        for(int bin=0; bin < (Nvtx-1); bin++){
        //for(int bin=fitPtBin ; bin <= fitPtBin; bin++){
            sLow = NumberToString(bins_nvtx[bin]); 
            sUp =  NumberToString(bins_nvtx[bin+1]);
            sBin = sLow+"-"+sUp;
            sName = ts[i]+"_nvtx_"+sBin;
            //cout << sName << endl;
            
            rds["data_"+ts[i]] =         (RooDataSet*)  rdsNvtx["data_"+ts[i]]->reduce(*m,         ("nvtx>="+sLow+" && nvtx<"+sUp).c_str());
            rds["template_sig"] =        (RooDataSet*)  rdsNvtx["template_sig"]->reduce(*m,        ("nvtx>="+sLow+" && nvtx<"+sUp).c_str());
            rds["template_bkg_"+ts[i]] = (RooDataSet*)  rdsNvtx["template_bkg_"+ts[i]]->reduce(*m, ("nvtx>="+sLow+" && nvtx<"+sUp).c_str());
            
            cout << "Entries in "+sName << " = " << rds["data_"+ts[i]]->numEntries() << endl;
            
            fSubfolder = unique+"nvtx/binning1";
            // build pdf s and draw frames
            tf[sName] = new TTemplateFit(sName, sName, m);
            tf[sName]->addData(rds["data_"+ts[i]], rds["template_sig"], rds["template_bkg_"+ts[i]]);
            // now perform an unbinned likelihood fit
            tf[sName]->setUnbinnedFit(kTRUE);
            tf[sName]->setRhoBkg(1.);
            tf[sName]->buildPdf(bins_nvtx_pdf[bin], "fit");
            tf[sName]->drawRaw(fResults, fSubfolder);
            tf[sName]->drawFrame(fResults, fSubfolder);
            tf[sName]->save(fResults, fSubfolder);
            
            
        }
        
        for(int bin=0; bin < (Ntrk-1); bin++){
        //for(int bin=fitPtBin ; bin <= fitPtBin; bin++){
            sLow = NumberToString(bins_ntrk[bin]);
            sUp =  NumberToString(bins_ntrk[bin+1]);
            sBin = sLow+"-"+sUp;
            sName = ts[i]+"_ntrk_"+sBin;
            //cout << sName << endl;
            
            rds["data_"+ts[i]] =         (RooDataSet*)  rdsNtrk["data_"+ts[i]]->reduce(*m,         ("ntrk>="+sLow+" && ntrk<"+sUp).c_str());
            rds["template_sig"] =        (RooDataSet*)  rdsNtrk["template_sig"]->reduce(*m,        ("ntrk>="+sLow+" && ntrk<"+sUp).c_str());
            rds["template_bkg_"+ts[i]] = (RooDataSet*)  rdsNtrk["template_bkg_"+ts[i]]->reduce(*m, ("ntrk>="+sLow+" && ntrk<"+sUp).c_str());
            
            cout << "Entries in "+sName << " = " << rds["data_"+ts[i]]->numEntries() << endl;
            
            fSubfolder = unique+"ntrk/binning1";
            // build pdf s and draw frames
            tf[sName] = new TTemplateFit(sName, sName, m);
            tf[sName]->addData(rds["data_"+ts[i]], rds["template_sig"], rds["template_bkg_"+ts[i]]);
            // now perform an unbinned likelihood fit
            tf[sName]->setUnbinnedFit(kTRUE);
            tf[sName]->setRhoBkg(1.);
            tf[sName]->buildPdf(bins_ntrk_pdf[bin], "fit");
            tf[sName]->drawRaw(fResults, fSubfolder);
            tf[sName]->drawFrame(fResults, fSubfolder);
            tf[sName]->save(fResults, fSubfolder);
            
        }
        
        for(int bin=0; bin < (Neta-1); bin++){
            //for(int bin=fitPtBin ; bin <= fitPtBin; bin++){
            sLow = NumberToString(bins_eta[bin]);
            sUp =  NumberToString(bins_eta[bin+1]);
            sBin = sLow+"-"+sUp;
            sName = ts[i]+"_eta_"+sBin;
            //cout << sName << endl;
            
            rds["data_"+ts[i]] =         (RooDataSet*)  rdsEta["data_"+ts[i]]->reduce(*m,         ("eta>="+sLow+" && eta<"+sUp).c_str());
            rds["template_sig"] =        (RooDataSet*)  rdsEta["template_sig"]->reduce(*m,        ("eta>="+sLow+" && eta<"+sUp).c_str());
            rds["template_bkg_"+ts[i]] = (RooDataSet*)  rdsEta["template_bkg_"+ts[i]]->reduce(*m, ("eta>="+sLow+" && eta<"+sUp).c_str());
            
            cout << "Entries in "+sName << " = " << rds["data_"+ts[i]]->numEntries() << endl;
            
            fSubfolder = unique+"eta/binning1";
            // build pdf s and draw frames
            tf[sName] = new TTemplateFit(sName, sName, m);
            tf[sName]->addData(rds["data_"+ts[i]], rds["template_sig"], rds["template_bkg_"+ts[i]]);
            // now perform an unbinned likelihood fit
            tf[sName]->setUnbinnedFit(kTRUE);
            tf[sName]->setRhoBkg(1.);
            tf[sName]->buildPdf(bins_eta_pdf[bin], "fit");
            tf[sName]->drawRaw(fResults, fSubfolder);
            tf[sName]->drawFrame(fResults, fSubfolder);
            tf[sName]->save(fResults, fSubfolder);
            
        }
        
    }
    
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Simulation
    unique = "ww_mc_";
    
    
    //                        {   "data_num",
    //                            "data_den",
    //                            "mc_num",
    //                            "mc_den",
    //                            "template_bkg_num",
    //                            "template_bkg_den",
    //                            "template_bkg_num_MC",
    //                            "template_bkg_den_MC",
    //                            "template_sig",
    //                        };
    
    
    // loop nominator and denominator:  
    // - for each, create a dataset within the pt bins of above;
    for(int i=0; i<Nts; i++){
    //for(int i=fitFraction; i<=fitFraction; i++){
        
        for(int bin=0; bin < (Npt-1); bin++){
        //for(int bin=fitPtBin ; bin <= fitPtBin; bin++){
            sLow = NumberToString(bins_pt[bin]);
            sUp =  NumberToString(bins_pt[bin+1]);
            sBin = sLow+"-"+sUp;
            sName = ts[i]+"_pt_"+sBin;
            //cout << sName << endl;
            
            rds["mc_"+ts[i]] =                 (RooDataSet*)  rdsPt["mc_"+ts[i]]->reduce(*m,           ("pt>="+sLow+" && pt<"+sUp).c_str());
            rds["template_sig"] =              (RooDataSet*)  rdsPt["template_sig"]->reduce(*m,          ("pt>="+sLow+" && pt<"+sUp).c_str());
            rds["template_bkg_"+ts[i]+"_MC"] = (RooDataSet*)  rdsPt["template_bkg_"+ts[i]+"_MC"]->reduce(*m,   ("pt>="+sLow+" && pt<"+sUp).c_str());
            
            cout << "Entries in "+sName << " = " << rds["mc_"+ts[i]]->numEntries() << endl;
            
            fSubfolder = unique+"pt/binning1";
            //fSubfolder = "pt/pullTests/withAverage";
            // build pdf s and draw frames
            tf[sName] = new TTemplateFit(sName, sName, m);
            tf[sName]->setType("mc"); // initialize as montecarlo fitter
            tf[sName]->addData(rds["mc_"+ts[i]], rds["template_sig"], rds["template_bkg_"+ts[i]+"_MC"]);
            // now perform an unbinned likelihood fit
            tf[sName]->setUnbinnedFit(kTRUE);
            tf[sName]->setRhoBkg(1.);
            tf[sName]->buildPdf(bins_pt_pdf[bin], "fit");
            tf[sName]->drawRaw(fResults, fSubfolder);
            tf[sName]->drawFrame(fResults, fSubfolder);
            tf[sName]->save(fResults, fSubfolder);
            
            
        }
        
        
        for(int bin=0; bin < (Nvtx-1); bin++){
        //for(int bin=fitPtBin ; bin <= fitPtBin; bin++){
            sLow = NumberToString(bins_nvtx[bin]);
            sUp =  NumberToString(bins_nvtx[bin+1]);
            sBin = sLow+"-"+sUp;
            sName = ts[i]+"_nvtx_"+sBin;
            //cout << sName << endl;
            
            rds["mc_"+ts[i]] =         (RooDataSet*)  rdsNvtx["mc_"+ts[i]]->reduce(*m,         ("nvtx>="+sLow+" && nvtx<"+sUp).c_str());
            rds["template_sig"] =        (RooDataSet*)  rdsNvtx["template_sig"]->reduce(*m,        ("nvtx>="+sLow+" && nvtx<"+sUp).c_str());
            rds["template_bkg_"+ts[i]+"_MC"] = (RooDataSet*)  rdsNvtx["template_bkg_"+ts[i]+"_MC"]->reduce(*m, ("nvtx>="+sLow+" && nvtx<"+sUp).c_str());
            
            cout << "Entries in "+sName << " = " << rds["mc_"+ts[i]]->numEntries() << endl;
            
            fSubfolder = unique+"nvtx/binning1";
            // build pdf s and draw frames
            tf[sName] = new TTemplateFit(sName, sName, m);
            tf[sName]->setType("mc"); // initialize as montecarlo fitter
            tf[sName]->addData(rds["mc_"+ts[i]], rds["template_sig"], rds["template_bkg_"+ts[i]+"_MC"]);
            // now perform an unbinned likelihood fit
            tf[sName]->setUnbinnedFit(kTRUE);
            tf[sName]->setRhoBkg(1.);
            tf[sName]->buildPdf(bins_nvtx_pdf[bin], "fit");
            tf[sName]->drawRaw(fResults, fSubfolder);
            tf[sName]->drawFrame(fResults, fSubfolder);
            tf[sName]->save(fResults, fSubfolder);
            
        }
        
        for(int bin=0; bin < (Ntrk-1); bin++){
        //for(int bin=fitPtBin ; bin <= fitPtBin; bin++){
            sLow = NumberToString(bins_ntrk[bin]);
            sUp =  NumberToString(bins_ntrk[bin+1]);
            sBin = sLow+"-"+sUp;
            sName = ts[i]+"_ntrk_"+sBin;
            //cout << sName << endl;
            
            rds["mc_"+ts[i]] =         (RooDataSet*)  rdsNtrk["mc_"+ts[i]]->reduce(*m,         ("ntrk>="+sLow+" && ntrk<"+sUp).c_str());
            rds["template_sig"] =        (RooDataSet*)  rdsNtrk["template_sig"]->reduce(*m,        ("ntrk>="+sLow+" && ntrk<"+sUp).c_str());
            rds["template_bkg_"+ts[i]+"_MC"] = (RooDataSet*)  rdsNtrk["template_bkg_"+ts[i]+"_MC"]->reduce(*m, ("ntrk>="+sLow+" && ntrk<"+sUp).c_str());
            
            cout << "Entries in "+sName << " = " << rds["mc_"+ts[i]]->numEntries() << endl;
            
            fSubfolder = unique+"ntrk/binning1";
            // build pdf s and draw frames
            tf[sName] = new TTemplateFit(sName, sName, m);
            tf[sName]->setType("mc"); // initialize as montecarlo fitter
            tf[sName]->addData(rds["mc_"+ts[i]], rds["template_sig"], rds["template_bkg_"+ts[i]+"_MC"]);
            // now perform an unbinned likelihood fit
            tf[sName]->setUnbinnedFit(kTRUE);
            tf[sName]->setRhoBkg(1.);
            tf[sName]->buildPdf(bins_ntrk_pdf[bin], "fit");
            tf[sName]->drawRaw(fResults, fSubfolder);
            tf[sName]->drawFrame(fResults, fSubfolder);
            tf[sName]->save(fResults, fSubfolder);
            
        }
        
        for(int bin=0; bin < (Neta-1); bin++){
            //for(int bin=fitPtBin ; bin <= fitPtBin; bin++){
            sLow = NumberToString(bins_eta[bin]);
            sUp =  NumberToString(bins_eta[bin+1]);
            sBin = sLow+"-"+sUp;
            sName = ts[i]+"_eta_"+sBin;
            //cout << sName << endl;
            
            rds["mc_"+ts[i]] =         (RooDataSet*)  rdsEta["mc_"+ts[i]]->reduce(*m,         ("eta>="+sLow+" && eta<"+sUp).c_str());
            rds["template_sig"] =        (RooDataSet*)  rdsEta["template_sig"]->reduce(*m,        ("eta>="+sLow+" && eta<"+sUp).c_str());
            rds["template_bkg_"+ts[i]+"_MC"] = (RooDataSet*)  rdsEta["template_bkg_"+ts[i]+"_MC"]->reduce(*m, ("eta>="+sLow+" && eta<"+sUp).c_str());
            
            cout << "Entries in "+sName << " = " << rds["mc_"+ts[i]]->numEntries() << endl;
            
            fSubfolder = unique+"eta/binning1";
            // build pdf s and draw frames
            tf[sName] = new TTemplateFit(sName, sName, m);
            tf[sName]->setType("mc"); // initialize as montecarlo fitter
            tf[sName]->addData(rds["mc_"+ts[i]], rds["template_sig"], rds["template_bkg_"+ts[i]+"_MC"]);
            // now perform an unbinned likelihood fit
            tf[sName]->setUnbinnedFit(kTRUE);
            tf[sName]->setRhoBkg(1.);
            tf[sName]->buildPdf(bins_eta_pdf[bin], "fit");
            tf[sName]->drawRaw(fResults, fSubfolder);
            tf[sName]->drawFrame(fResults, fSubfolder);
            tf[sName]->save(fResults, fSubfolder);
            
        }
        
    }
    
    
    
    
    //~ 
    //~ goto jumperEnd;
    //~ 
    //~ // close files
    jumperEnd:
    fSig->Close();
    fBkg->Close();
    fDat->Close(); 
    f_Mc->Close();
    fResults->Close();
    
    return;
    }

// ***************************************************************************************************************************
// * create fake rate dependency plots
// *
// ***************************************************************************************************************************
void readFitResults(string dropbox){
    cout << "rooFitHeader.readFitResults()" << endl;
    // dropbox = /user/rmeyer/Dropbox/data_uni/
    
    gROOT->SetBatch(kTRUE); // dont show canvases
    
    
    string ts[] = {"den", "num"};
    Int_t Nts = arraysize(ts);
    
    
    //~ // open results ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    string results = dropbox + "code/rooFit/egamma/results_fits.root";
    TFile *fRes = new TFile(results.c_str(), "read");
    
    string closure = dropbox;
    
    //~ // file to save plots
    string plot_results = dropbox + "code/rooFit/egamma/results_plots.root";
    TFile *fPlots = new TFile(plot_results.c_str(), "update");
    
    
    string unique;
    string subfolder;
    string path;
    
    //~ // *** define binnings for 1d variable plots ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    
    // Pt 
    //                          0     1     2       3       4     5     6       7       8     9     10
    Double_t bins_pt[] =    {  40. , 45. ,  50.,   60.,   70.,   90.,   110.,  140.,   250};
    string bins_pt_pdf[] =  {"hist","hist","hist","hist","hist","hist","hist","hist","hist"};
    Int_t Npt = arraysize(bins_pt);
    // Nvtx
    //                          0     1     2       3       4     5     6     7     8      9       10     11      12    13    14    15    16    17    18    19
    Double_t bins_nvtx[] =  {   1.,   2.,  3.,     4.,      5.,    6.,  7.,   8.,   9.,    10.,    11.,   12.,  13.,    14.,  15.,  16.,  17.,  18.,  19.,  20.  };
    string bins_nvtx_pdf[] ={"hist","hist","hist","hist","hist","hist","hist","hist","hist","hist","hist","hist","hist","hist","hist","hist","hist","hist", "hist"};
    Int_t Nvtx = arraysize(bins_nvtx);
    // Ntrk
    //                          0     1     2       3       4     5     6     7        8     9     10
    Double_t bins_ntrk[] =  {  0.,   20.,   40.,   60.,   80.,   100.,  120.,  140.,  160.};
    string bins_ntrk_pdf[] ={"hist","hist","hist","hist","hist","hist","hist","hist","hist"};
    Int_t Ntrk = arraysize(bins_ntrk);
    // eta
    //                          0     1     2       3       4     5     6     7        8     9     10
    Double_t bins_eta[] =   {  0.,    0.2,  0.4,   0.6,    0.8,   1.,   1.2,  1.4};
    string bins_eta_pdf[] = {"hist","hist","hist","hist","hist","hist","hist","hist"};
    Int_t Neta = arraysize(bins_eta);
    
    
    
    //~ // read out variables for bins in variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    
    string sBin, sName;
    string sLow, sUp;
    
    Double_t binMidth;
    
    string fSubfolder;
    
    cout << "Npt  = " << Npt << endl;
    cout << "Ntrk = " << Ntrk << endl;
    cout << "Nvtx = " << Nvtx << endl;
    cout << "Neta = " << Neta << endl;
    
    Int_t fitPtBin = 7;
    Int_t fitFraction = 1; // 0: den, 1: num
    
    // read out variables for the results
    RooFitResult *rFit;
    RooArgSet    *rPars;
    Double_t      value;
    Double_t      err;
    RooPlot      *rPlot;
    TCanvas      *tPlot;
    
    // fakerate histograms
    map<string, TH1F*> h;
    
    string var;
    
    var = "pt";
    h["num_"+var] = new TH1F(("num_"+var).c_str(), "numerator;p_{T} (GeV);events",   Npt-1, bins_pt);
    h["den_"+var] = new TH1F(("den_"+var).c_str(), "denominator;p_{T} (GeV);events", Npt-1, bins_pt);
    var = "ntrk";
    h["num_"+var] = new TH1F(("num_"+var).c_str(), "numerator;p_{T} (GeV);events",   Ntrk-1, bins_ntrk);
    h["den_"+var] = new TH1F(("den_"+var).c_str(), "denominator;p_{T} (GeV);events", Ntrk-1, bins_ntrk);
    var = "nvtx";
    h["num_"+var] = new TH1F(("num_"+var).c_str(), "numerator;p_{T} (GeV);events",   Nvtx-1, bins_nvtx);
    h["den_"+var] = new TH1F(("den_"+var).c_str(), "denominator;p_{T} (GeV);events", Nvtx-1, bins_nvtx);
    var = "eta";
    h["num_"+var] = new TH1F(("num_"+var).c_str(), "numerator;p_{T} (GeV);events",   Neta-1, bins_eta);
    h["den_"+var] = new TH1F(("den_"+var).c_str(), "denominator;p_{T} (GeV);events", Neta-1, bins_eta);
    
    
    unique = "";
    
    
    
    
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // do a closure plot
    // fSubfolder = unique+"global/withSigConvFFT";
    
    // read out results of the global fit
    //closurePlot(TFile *fRes, TFile *fClos);
    
    
    
    
    //goto jumperEnd;
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    
    
    
    //~ // loop nominator and denominator:  
    //~ // - red out results
    
    
    unique = "data_";
    //~ unique = "mc_";
    
    
    for(int i=0; i<Nts; i++){
    //for(int i=fitFraction; i<=fitFraction; i++){
        for(int bin=0; bin < (Npt-1); bin++){
            
            binMidth = bins_pt[bin]+(bins_pt[bin+1]-bins_pt[bin])/2.;
            sLow = NumberToString(bins_pt[bin]);
            sUp =  NumberToString(bins_pt[bin+1]);
            sBin = sLow+"-"+sUp;
            sName = ts[i]+"_pt_"+sBin;
            
            fSubfolder = unique+"pt/binning1";
            
            // read out fit result
            path = fSubfolder+"/"+sName+"/fitresult";
            
            //~ rModel = (RooAbsPdf*) fRes->Get((path).c_str());
            //~ rPars  = (RooArgSet*) rModel->getVariables();
            //~ value  = ((RooRealVar*)rPars->find(("Nsig_"+sName).c_str()))->getVal();
            //~ err    = ((RooRealVar*)rPars->find(("Nsig_"+sName).c_str()))->getError();
            rFit =  (RooFitResult*)    fRes->Get((path).c_str());
            rPars = (RooArgSet*) &(rFit->floatParsFinal());
            value = ((RooRealVar*)rPars->find(("Nsig_"+sName).c_str()))->getVal();
            err =   ((RooRealVar*)rPars->find(("Nsig_"+sName).c_str()))->getError();
            
            
            
            var = "pt";
            for(int k = 0; k < int(value); k++){
                h[ts[i]+"_"+var]->Fill(binMidth);
            }
             
            //h[ts[i]]->SetBinContent(bin, (value));
            //h[ts[i]]->SetBinError(bin, (err));
            
            // save picture
            path = fSubfolder+"/"+sName+"/frame";
            rPlot = (RooPlot*) fRes->Get((path).c_str());
            tPlot = new TCanvas(sName.c_str(), sName.c_str(), 800, 700);
            tPlot->cd();
            tPlot->SetRightMargin(0.16);
            rPlot->Draw();
            tPlot->SaveAs((dropbox+"thesis/ma_SusyMeeting/2016_07_14/plots/"+unique+var+"/"+sName+".png").c_str());
            tPlot->Close();
            
        }
        for(int bin=0; bin < (Nvtx-1); bin++){
            
            binMidth = bins_nvtx[bin]+(bins_nvtx[bin+1]-bins_nvtx[bin])/2.;
            sLow = NumberToString(bins_nvtx[bin]);
            sUp =  NumberToString(bins_nvtx[bin+1]);
            sBin = sLow+"-"+sUp;
            sName = ts[i]+"_nvtx_"+sBin;
            
            fSubfolder = unique+"nvtx/binning1";
            
            // read out fit result
            path = fSubfolder+"/"+sName+"/fitresult";
            
            rFit =  (RooFitResult*)    fRes->Get((path).c_str());
            rPars = (RooArgSet*) &(rFit->floatParsFinal());
            value = ((RooRealVar*)rPars->find(("Nsig_"+sName).c_str()))->getVal();
            err =   ((RooRealVar*)rPars->find(("Nsig_"+sName).c_str()))->getError();
            
            var = "nvtx";
            for(int k = 0; k < int(value); k++){
                h[ts[i]+"_"+var]->Fill(binMidth);
            }
             
            //h[ts[i]]->SetBinContent(bin, (value));
            //h[ts[i]]->SetBinError(bin, (err));
            
            // save picture
            path = fSubfolder+"/"+sName+"/frame";
            rPlot = (RooPlot*) fRes->Get((path).c_str());
            tPlot = new TCanvas(sName.c_str(), sName.c_str(), 700, 700);
            tPlot->cd();
            tPlot->SetRightMargin(0.16);
            rPlot->Draw();
            tPlot->SaveAs((dropbox+"thesis/ma_SusyMeeting/2016_07_14/plots/"+unique+var+"/"+sName+".png").c_str());
            tPlot->Close();
            
        }
        for(int bin=0; bin < (Ntrk-1); bin++){
            
            binMidth = bins_ntrk[bin]+(bins_ntrk[bin+1]-bins_ntrk[bin])/2.;
            sLow = NumberToString(bins_ntrk[bin]);
            sUp =  NumberToString(bins_ntrk[bin+1]);
            sBin = sLow+"-"+sUp;
            sName = ts[i]+"_ntrk_"+sBin;
            
            fSubfolder = unique+"ntrk/binning1";
            
            // read out fit result
            path = fSubfolder+"/"+sName+"/fitresult";
            
            rFit =  (RooFitResult*)    fRes->Get((path).c_str());
            rPars = (RooArgSet*) &(rFit->floatParsFinal());
            value = ((RooRealVar*)rPars->find(("Nsig_"+sName).c_str()))->getVal();
            err =   ((RooRealVar*)rPars->find(("Nsig_"+sName).c_str()))->getError();
            
            var = "ntrk";
            for(int k = 0; k < int(value); k++){
                h[ts[i]+"_"+var]->Fill(binMidth);
            }
            
            //h[ts[i]]->SetBinContent(bin, (value));
            //h[ts[i]]->SetBinError(bin, (err));
            
            // save picture
            path = fSubfolder+"/"+sName+"/frame";
            rPlot = (RooPlot*) fRes->Get((path).c_str());
            tPlot = new TCanvas(sName.c_str(), sName.c_str(), 700, 700);
            tPlot->cd();
            tPlot->SetRightMargin(0.16);
            rPlot->Draw();
            tPlot->SaveAs((dropbox+"thesis/ma_SusyMeeting/2016_07_14/plots/"+unique+var+"/"+sName+".png").c_str());
            tPlot->Close();
            
        }
        for(int bin=0; bin < (Neta-1); bin++){
            
            binMidth = bins_eta[bin]+(bins_eta[bin+1]-bins_eta[bin])/2.;
            sLow = NumberToString(bins_eta[bin]);
            sUp =  NumberToString(bins_eta[bin+1]);
            sBin = sLow+"-"+sUp;
            sName = ts[i]+"_eta_"+sBin;
            
            fSubfolder = unique+"eta/binning1";
            
            // read out fit result
            path = fSubfolder+"/"+sName+"/fitresult";
            
            rFit =  (RooFitResult*)    fRes->Get((path).c_str());
            rPars = (RooArgSet*) &(rFit->floatParsFinal());
            value = ((RooRealVar*)rPars->find(("Nsig_"+sName).c_str()))->getVal();
            err =   ((RooRealVar*)rPars->find(("Nsig_"+sName).c_str()))->getError();
            
            var = "eta";
            for(int k = 0; k < int(value); k++){
                h[ts[i]+"_"+var]->Fill(binMidth);
            }
             
            //h[ts[i]]->SetBinContent(bin, (value));
            //h[ts[i]]->SetBinError(bin, (err));
            
            // save picture
            path = fSubfolder+"/"+sName+"/frame";
            rPlot = (RooPlot*) fRes->Get((path).c_str());
            tPlot = new TCanvas(sName.c_str(), sName.c_str(), 700, 700);
            tPlot->cd();
            tPlot->SetRightMargin(0.16);
            rPlot->Draw();
            tPlot->SaveAs((dropbox+"thesis/ma_SusyMeeting/2016_07_14/plots/"+unique+var+"/"+sName+".png").c_str());
            tPlot->Close();
            
        }
    }
    
    
    cout << "done..." << endl;
    cout << ".. create efficiencies" << endl;
    
    //rt.TEfficiency.CheckConsistency(self.pas, self.tot):
    // self.eff = rt.TEfficiency(self.pas, self.tot)
    //TEfficiency *heff = new TEfficiency
    //cout << "Consistent = " << TEfficiency::CheckConsistency(*h["num"], *h["den"]) << endl;
    map<string, TEfficiency*> eff;
    map<string, TGraphAsymmErrors*> tGraph;
    
    var = "pt";
    cout << var << " consistency = " << TEfficiency::CheckConsistency(*h["num_"+var], *h["den_"+var]) << endl;
    eff[var] = new TEfficiency(*h["num_"+var], *h["den_"+var]);
    tGraph[var] = (TGraphAsymmErrors*) eff[var]->CreateGraph();
    tGraph[var]->SetTitle("");
    tGraph[var]->GetXaxis()->SetTitle("p_{T}^{(probe)} (GeV)");
    tGraph[var]->GetYaxis()->SetTitle("fakerate");
    
    var = "ntrk";
    cout << var << " consistency = " << TEfficiency::CheckConsistency(*h["num_"+var], *h["den_"+var]) << endl;
    eff[var] = new TEfficiency(*h["num_"+var], *h["den_"+var]);
    tGraph[var] = (TGraphAsymmErrors*) eff[var]->CreateGraph();
    tGraph[var]->SetTitle("");
    tGraph[var]->GetXaxis()->SetTitle("N_{tracks}");
    tGraph[var]->GetYaxis()->SetTitle("fakerate");
    
    var = "nvtx";
    cout << var << " consistency = " << TEfficiency::CheckConsistency(*h["num_"+var], *h["den_"+var]) << endl;
    eff[var] = new TEfficiency(*h["num_"+var], *h["den_"+var]);
    tGraph[var] = (TGraphAsymmErrors*) eff[var]->CreateGraph();
    tGraph[var]->SetTitle("");
    tGraph[var]->GetXaxis()->SetTitle("N_{vertex}");
    tGraph[var]->GetYaxis()->SetTitle("fakerate");
    
    var = "eta";
    cout << var << " consistency = " << TEfficiency::CheckConsistency(*h["num_"+var], *h["den_"+var]) << endl;
    eff[var] = new TEfficiency(*h["num_"+var], *h["den_"+var]);
    tGraph[var] = (TGraphAsymmErrors*) eff[var]->CreateGraph();
    tGraph[var]->SetTitle("");
    tGraph[var]->GetXaxis()->SetTitle("|#eta^{(probe)}|");
    tGraph[var]->GetYaxis()->SetTitle("fakerate");
    
    
    fPlots->cd();
    // canvases
    map<string, TCanvas*> c;
    
    
    
    //unique = "ww_mc_";
    //~ unique = "";
    
    var = "pt";
    c[var] = new TCanvas(("c_"+var).c_str(), ("c_"+var).c_str(), 700, 700);
    c[var]->cd();
    tGraph[var]->Draw("ap");
    tGraph[var]->Write(("tGraph_"+unique+var).c_str(), TObject::kOverwrite);
    c[var]->Write(("canvas_"+unique+var).c_str(), TObject::kOverwrite);
    c[var]->SaveAs((dropbox+"thesis/ma_SusyMeeting/2016_07_14/fakerate/"+unique+var+".png").c_str());
    
    var = "nvtx";
    c[var] = new TCanvas(("c_"+var).c_str(), ("c_"+var).c_str(), 700, 700);
    c[var]->cd();
    tGraph[var]->Draw("ap");
    tGraph[var]->Write(("tGraph_"+unique+var).c_str(), TObject::kOverwrite);
    c[var]->Write(("canvas_"+unique+var).c_str(), TObject::kOverwrite);
    c[var]->SaveAs((dropbox+"thesis/ma_SusyMeeting/2016_07_14/fakerate/"+unique+var+".png").c_str());
    
    var = "ntrk";
    c[var] = new TCanvas(("c_"+var).c_str(), ("c_"+var).c_str(), 700, 700);
    c[var]->cd();
    tGraph[var]->Draw("ap");
    tGraph[var]->Write(("tGraph_"+unique+var).c_str(), TObject::kOverwrite);
    c[var]->Write(("canvas_"+unique+var).c_str(), TObject::kOverwrite);
    c[var]->SaveAs((dropbox+"thesis/ma_SusyMeeting/2016_07_14/fakerate/"+unique+var+".png").c_str());
    
    var = "eta";
    c[var] = new TCanvas(("c_"+var).c_str(), ("c_"+var).c_str(), 700, 700);
    c[var]->cd();
    tGraph[var]->Draw("ap");
    tGraph[var]->Write(("tGraph_"+unique+var).c_str(), TObject::kOverwrite);
    c[var]->Write(("canvas_"+unique+var).c_str(), TObject::kOverwrite);
    c[var]->SaveAs((dropbox+"thesis/ma_SusyMeeting/2016_07_14/fakerate/"+unique+var+".png").c_str());
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    //~ // histograms for efficiency plot
    //~ map<string, TH1F*> h;
    //~ h["num"] = new TH1F("num", "efficiency;p_{T} (GeV);fakerate", Npt, bins_pt);
    //~ h["den"] = new TH1F("den", "denominator", Npt, bins_pt);
    //~ 
    //~ h["eff"] = new TH1F("eff", "efficiency;fakerate;p_{T} (GeV)", Npt, bins_pt);
    //~ 
    //~ string  sLow, sUp, sBin, sName;
    //~ Double_t binMidth = 0;
    //~ 
    //~ RooFitResult *rFit;
    //~ RooArgSet *rPars;
    //~ Double_t value, err;
    //~ 
    //~ RooAbsPdf *rModel;
    //~ 
    //~ // fill bins
    //~ 
    //~ 
    //~ for(int bin=1; bin<Npt+1; bin++){
        //~ 
        //~ for(int i=0; i<Nts; i++){
            //~ 
            //~ binMidth = bins_pt[bin-1]+(bins_pt[bin]-bins_pt[bin-1])/2.;
            //~ sLow = NumberToString(bins_pt[bin-1]);
            //~ sUp =  NumberToString(bins_pt[bin]);
            //~ sBin = sLow+"to"+sUp;
            //~ sName = ts[i]+"_pt_"+sBin;
            //~ 
            //~ // read out fit result
            //~ path = subfolder+"/"+sName+"/fitresult";
            //~ //cout << "path = " << path << endl;
            //~ 
            //~ rFit =  (RooFitResult*)    fRes->Get((subfolder+"/"+sName+"/fitresult").c_str());
            //~ rPars = (RooArgSet*) &(rFit->floatParsFinal());
            //~ value = ((RooRealVar*)rPars->find(("Nsig_"+sName).c_str()))->getVal();
            //~ err =   ((RooRealVar*)rPars->find(("Nsig_"+sName).c_str()))->getError();
            //~ 
            //~ //cout << "sName: " << sName << "\tval:   " << (value) << "\terr:   " << (err) << endl;
            //~ 
            //~ // for(int k = 0; k < int(value*100); k++){
                //~ // h[ts[i]]->Fill(binMidth);
            //~ // }
            //~ // 
            //~ h[ts[i]]->SetBinContent(bin, (value));
            //~ h[ts[i]]->SetBinError(bin, (err));
            //~ 
            //~ 
        //~ }
    //~ }
    //~ 
    //~ 
    //h["den"]->Sumw2();
    //h["num"]->Sumw2();
    
    
    //rt.TEfficiency.CheckConsistency(self.pas, self.tot):
    //TEfficiency *heff = new TEfficiency
    //cout << "Consistent = " << TEfficiency::CheckConsistency(*h["num"], *h["den"]) << endl;
    
    //~ 
    //~ TCanvas *t = new TCanvas("t", "t", 800, 400);
    //~ t->Divide(2, 1);
    //~ t->cd(1);
    //~ h["den"]->Draw("hist e");
    //~ t->cd(2);
    //~ h["num"]->Draw("hist e");
    //~ //t->Update();
    //~ 
    //~ h["eff"] = (TH1F*) h["num"]->Clone();
    //~ h["eff"]->SetTitle("Fakerate");
    //~ h["eff"]->Divide(h["den"]);
    //~ h["eff"]->SetMarkerStyle(kFullCircle);
    //~ h["eff"]->SetMarkerColor(kBlack);
    //~ 
    //~ 
    //~ TCanvas *c = new TCanvas("c", "c", 700, 700);
    //~ c->cd();
    //~ c->SetGrid();
    //~ h["eff"]->Draw("hist pe");
    //~ 
    
    
    
    
    // /////////////////////////////////////////////////////////////////////////////////////////////
    // 2d readout
    
    //~ //                       0      1       2       3       4     5     6     7        8     9     10
    //~ Double_t bins_ntrk[] =    {  0. , 20. ,  30.,     40.,   50.,   60.,   70.,   80.,   90.,   110., 240.};
    //~ string bins_ntrk_pdf[] =  {"hist","hist","hist","hist","hist","hist","hist","hist","hist","hist","hist"};
    //~ 
    //~ //                             0    1      2       3     4      5      6       7     8      9     10
    //~ Double_t bins_pt2[]     =  {  40. , 42. ,  44.,   46.,   51.,   61.,   250.};
    //~ string bins_pt2_pdf[]   =  {"hist","hist","hist","hist","hist","hist","hist"};//,"hist","hist","hist","hist"};
    //~ 
    //~ Int_t Ntrk = arraysize(bins_ntrk);
          //~ Npt  = arraysize(bins_pt2);
    //~ 
    //~ 
    //~ // open results
    //~ results = dropbox + "code/rooFit/egamma/results_ptntrk.root";
    //~ TFile *fRes2 = new TFile(results.c_str(), "read");
    //~ 
    //~ 
    //~ // TH2F (const char *name, const char *title, Int_t nbinsx, Double_t xlow, Double_t xup, Int_t nbinsy, Double_t ylow, Double_t yup)
    //~ // TH2F (const char *name, const char *title, Int_t nbinsx, const Double_t *xbins, Int_t nbinsy, const Double_t *ybins)
    //~ map<string, TH2*> h2;
    //~ 
    //~ h2["num"] = new TH2F("num_ptntrk", "numerator;p_{T} (GeV);N_{trk};events",   Npt-1, bins_pt2, Ntrk-1, bins_ntrk);
    //~ h2["den"] = new TH2F("den_ptntrk", "denominator;p_{T} (GeV);N_{trk};events", Npt-1, bins_pt2, Ntrk-1, bins_ntrk);
    //~ h2["eff"] = new TH2F("eff_ptntrk", "fakerate;p_{T} (GeV);N_{trk};fakerate",  Npt-1, bins_pt2, Ntrk-1, bins_ntrk);
    //~ 
    //~ subfolder = "ptntrk1";
    //~ 
    //~ string sLowTrk, sUpTrk, sLowPt, sUpPt;
    //~ 
    //~ map<string, RooPlot*> rPlot;
    //~ 
    //~ TCanvas *cFrame = new TCanvas("cFrame", "cFrame", 600, 600);
    //~ Int_t bincounter = 0;
    //~ for(int i=0; i<Nts; i++){
        //~ // i = 0 den
        //~ // i = 1 num
        //~ 
        //~ for(int binTrk=0; binTrk<(Ntrk-1); binTrk++){
            //~ for(int binPt=0; binPt<(Npt-1); binPt++){
                //~ bincounter++;
                //~ 
                //~ sLowTrk = NumberToString(bins_ntrk[binTrk]);
                //~ sUpTrk  = NumberToString(bins_ntrk[binTrk+1]);
                //~ sLowPt  = NumberToString(bins_pt2[binPt]);
                //~ sUpPt   = NumberToString(bins_pt2[binPt+1]);
                //~ 
                //~ sBin    = "ntrk"+sLowTrk+"to"+sUpTrk+"_pt"+sLowPt+"to"+sUpPt;
                //~ sName   = ts[i]+"_"+sBin;
                //~ 
                //~ path = subfolder+"/"+sName+"/model";
                //~ 
                //~ 
                //~ // rFit =  (RooFitResult*)    fRes->Get((subfolder+"/"+sName+"/fitresult").c_str());
                //~ // rPars = (RooArgSet*) &(rFit->floatParsFinal());
                //~ // value = ((RooRealVar*)rPars->find(("Nsig_"+sName).c_str()))->getVal();
                //~ // err =   ((RooRealVar*)rPars->find(("Nsig_"+sName).c_str()))->getError();
                //~ rModel = (RooAbsPdf*) fRes2->Get((path).c_str());
                //~ rPars  = (RooArgSet*) rModel->getVariables();
                //~ value  = ((RooRealVar*)rPars->find(("Nsig_"+sName).c_str()))->getVal();
                //~ err    = ((RooRealVar*)rPars->find(("Nsig_"+sName).c_str()))->getError();
                //~ 
                //~ rPlot[sName] = (RooPlot*) fRes2->Get((subfolder+"/"+sName+"/frame").c_str());
                //~ cFrame->cd();
                //~ rPlot[sName]->Draw();
                //~ cFrame->SaveAs(("/user/rmeyer/Dropbox/data_uni/thesis/ma_SusyMeeting/2016_06_16/plots_pdf/"+sName+".pdf").c_str());
                //~ cFrame->SaveAs(("/user/rmeyer/Dropbox/data_uni/thesis/ma_SusyMeeting/2016_06_16/plots/"+sName+".png").c_str());
                //~ 
                //~ 
                //~ //cout << "sName: " << sName << "\tval:   " << (value) << "\terr:   " << (err) << endl;
                //~ //getchar();
                //~ 
                //~ // SetBinContent (Int_t binx, Int_t biny, Double_t content)
                //~ h2[ts[i]]->SetBinContent(binPt+1, binTrk+1, (value));
                //~ h2[ts[i]]->SetBinError(binPt+1, binTrk+1, (err));
                //~ 
            //~ }
        //~ }
        //~ 
    //~ }
    //~ 
    //~ cFrame->Close();
    //~ 
    //~ h2["eff"] = (TH2F*) h2["num"]->Clone();
    //~ h2["eff"]->Divide(h2["den"]);
    //~ h2["eff"]->SetTitle("fakerate");
    //~ h2["eff"]->GetXaxis()->SetTitle("p_{T} (GeV)");
    //~ 
    //~ h2["eff"]->GetYaxis()->SetTitle("N_{trk}");
    //~ //h2["eff"]->GetYaxis()->SetTitleSize(0.05);
    //~ //h2["eff"]->GetYaxis()->SetTitleOffset(1.2);
    //~ 
    //~ h2["eff"]->GetZaxis()->SetTitle("fakerate");
    //~ 
    //~ 
    //~ 
    //~ 
    //~ h["pt"]   = (TH1F*) h2["num"]->ProjectionX();
    //~ h["pt"]->Divide((TH1F*)h2["den"]->ProjectionX());
    //~ h["pt"]->SetTitle("Pt projection");
    //~ 
    //~ 
    //~ h["nvtx"] = (TH1F*)h2["num"]->ProjectionY();
    //~ h["nvtx"]->Divide((TH1F*)h2["den"]->ProjectionY());
    //~ h["nvtx"]->SetTitle("Nvtx projection");
    //~ 
    //~ h["pt"]->SetMarkerStyle(kFullCircle);
    //~ h["pt"]->SetMarkerColor(kBlack);
    //~ h["nvtx"]->SetMarkerStyle(kFullCircle);
    //~ h["nvtx"]->SetMarkerColor(kBlack);
    //~ 
    //~ string hname, h2name;
    //~ 
    //~ h2name="num";
    //~ h2[h2name]->GetYaxis()->SetTitleOffset(1.2);
    //~ h2[h2name]->GetZaxis()->SetTitleOffset(1.6);
    //~ 
    //~ h2name="den";
    //~ h2[h2name]->GetYaxis()->SetTitleOffset(1.2);
    //~ h2[h2name]->GetZaxis()->SetTitleOffset(1.6);
    
    //~ h2name="eff";
    //~ h2[h2name]->GetYaxis()->SetTitleOffset(1.2);
    //~ h2[h2name]->GetZaxis()->SetTitleOffset(1.6);
    //~ 
    //~ h2["eff"]->SetStats(0);
    //~ h2["num"]->SetStats(0);
    //~ h2["den"]->SetStats(0);
    //~ 
    //~ TCanvas *c2 = new TCanvas("c2", "c2", 1600, 950);
    //~ c2->Divide(3, 2);
    //~ 
    //~ c2->cd(1);
    //~ TPad *p1 = new TPad("p1", "p1", 0., 0., 1., 1.);
    //~ p1->SetRightMargin(0.2);
    //~ p1->Draw();
    //~ p1->cd();
    //~ h2["den"]->Draw("colz");
    //~ 
    //~ c2->cd(2);
    //~ TPad *p2 = new TPad("p2", "p2", 0., 0., 1., 1.);
    //~ p2->SetRightMargin(0.2);
    //~ p2->Draw();
    //~ p2->cd();
    //~ h2["num"]->Draw("colz");
    //~ 
    //~ c2->cd(3);
    //~ TPad *p3 = new TPad("p3", "p3", 0., 0., 1., 1.);
    //~ p3->SetRightMargin(0.2);
    //~ p3->Draw();
    //~ p3->cd();
    //~ h2["eff"]->Draw("colz");
    //~ 
    //~ c2->cd(5);
    //~ h["pt"]->Draw("ep");
    //~ 
    //~ c2->cd(6);
    //~ h["nvtx"]->Draw("ep");
    
    
    jumperEnd:
    fPlots->Close();
    
    return ;
}




void closurePlot(map<string, string> set){
    
    cout << "rooFitHeader.closurePlot()" << endl;
    
    //gROOT->SetBatch(kTRUE); // dont show canvases
    
    string closurefile = set["closureMC_stack"];
    
    cout << "closure file: " << endl;
    cout << closurefile << endl;
    
    TAuxiliary* aux = new TAuxiliary();
    
    
    string ts[] = {"den", "num"};
    Int_t Nts = arraysize(ts);
    
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //~ // open closurefile 
    TFile *fRes = new TFile(closurefile.c_str(), "read");
    
    
    Double_t f = 17260./1660811.; // mc fake rate 
    
    // read out histograms
    TH1F* h_e = (TH1F*) fRes->Get("clos_gen_e")->Clone();
    TH1F* h_g = (TH1F*) fRes->Get("clos_gen_g")->Clone();
    
    h_e->SetLineColor(kRed);
    h_e->SetLineWidth(2);
    h_g->SetLineColor(kBlue);
    h_g->SetLineWidth(2);
    
    h_e->Sumw2();
    h_g->Sumw2();
    
    h_e->Rebin(10);
    h_g->Rebin(10);
    
    //~ TCanvas* c1 = new TCanvas("c1", "c1", 1200, 600);
    //~ 
    //~ c1->Divide(2, 1);
    //~ 
    //~ c1->cd(1);
    //~ TPad* p11 = new TPad("p11", "p11", 0., 0., 1., 1.);
    //~ p11->SetLogy();
    //~ p11->Draw();
    //~ p11->cd();
    //~ 
    //~ h_e->DrawClone("hist e");
    //~ 
    //~ 
    //~ c1->cd(2);
    //~ TPad* p12 = new TPad("p12", "p12", 0., 0., 1., 1.);
    //~ p12->SetLogy();
    //~ p12->Draw();
    //~ p12->cd();
    //~ h_g->DrawClone("hist e");
    
    
    
    TCanvas* c2 = new TCanvas("c2", "c2", 1200, 600);
    
    c2->Divide(2, 1);
    c2->cd(1);
    
    TPad* p21 = new TPad("p21", "p21", 0., 0., 1., 1.);
    
    p21->SetLogy();
    p21->Draw();
    p21->cd();
    
    h_e->DrawClone("hist e");
    
    h_e->Scale(f/(1-f));
    
    TH1F* h_r = (TH1F*) h_e->Clone();
    
    c2->cd(2);
    TPad* p22 = new TPad("p22", "p22", 0., 0.3, 1., 1.);
    aux->setPadStyle(*p22);
    p22->SetLogy();
    p22->Draw();
    p22->cd();
    
    
    aux->setTH1PlotOptions(*h_e);
    aux->setTH1PlotOptions(*h_g);
    h_e->Draw("hist e");
    h_g->DrawClone("hist e same");
    
    //aux->drawCMS(*p22);
    
    c2->cd(2);
    TPad* p23 = new TPad("p23", "p23", 0., 0., 1., 0.3);
    aux->setPadStyle(*p23);
    p23->SetGridy();
    p23->Draw();
    p23->cd();
    
    aux->setTH1RatioPlotOptions(*h_r);
    h_r->Divide(h_g);
    h_r->Draw("ep");
    
    
    
    
    //~ rFit =  (RooFitResult*)    fRes->Get((path).c_str());
    //~ rPars = (RooArgSet*) &(rFit->floatParsFinal());
    //~ value = ((RooRealVar*)rPars->find(("Nsig_"+sName).c_str()))->getVal();
    //~ err =   ((RooRealVar*)rPars->find(("Nsig_"+sName).c_str()))->getError();
    //~ 
    
    
    //fRes->Close();
    
    return;
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



