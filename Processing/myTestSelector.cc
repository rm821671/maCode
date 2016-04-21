/**
 *  little script for test studies
**/

// /*
#include <ctime>
#include <sstream>
#include <iostream>
#include <fstream>

#include <algorithm>
#include <string>

#include "TROOT.h"
#include "TChain.h"
#include "TFile.h"
#include "TSelector.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "TTreeReaderArray.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TProfile.h"
#include "TEfficiency.h"

#include "TRandom.h"
#include "TRandom3.h"

#include <TCanvas.h>
#include <TGraph.h>

#include <TLorentzVector.h>
#include <TVector3.h>

#include "TreeParticles.hpp"
//#include "/home/home4/institut_1b/rmeyer/cms_sw/CMSSW_7_4_14/src/TreeWriter/TreeWriter/plugins/TreeParticles.hpp"

#include <map>
#include <sys/utsname.h>

#include "myTestSelector.hpp"
// */

using namespace tree;
using namespace std;

void resetSelection(){
    // moved to myTestSelector.hpp
}

void initSelection(map<string,TH1F> &h, map<string,TH2F> &h2){
    // moved to myTestSelector.hpp
}

void myTestSelector(){
    
    //gROOT->Reset();
    //gSystem->Load("pluginTreeWriterTreeWriterAuto.so");
    
    string filepath, dropbox;
    
    // filepath and dropbox path changes, depending on system
    SystemPath(filepath, dropbox);
    
    string datasets[] = {   
                        
                        //"DYJetsToLL_M-50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8_v01.root" // 0
                        //"DYJetsToLL_M-50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8_v02.root",// 0
                        "DYJetsToLL_M-50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8_v03.root",// 0
                        //"TTGJets_TuneCUETP8M1_13TeV-amcatnloFXFX-madspin-pythia8_v01.root", //1
                        //"TTGJets_TuneCUETP8M1_13TeV-amcatnloFXFX-madspin-pythia8_v02.root",// 1
                        "TTGJets_TuneCUETP8M1_13TeV-amcatnloFXFX-madspin-pythia8_v03.root", //1
                        //"WGToLNuG_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8_v01.root", // 2
                        //"WGToLNuG_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8_v02.root", // 2
                        "WGToLNuG_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8_v03.root", //2
                        // "ZGTo2LG_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8_v01.root", // 3
                        //"ZGTo2LG_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8_v02.root", // 3
                        "ZGTo2LG_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8_v03.root" // 3
                        
                        "DoubleEG_Run2015D-05Oct2015-v1_v02.root",  // 4
                        "DoubleEG_Run2015D-PromptReco-v4_v02.root", // 5
                        
                        "DY_v1.root", // 6
                        "DYJetsToLL_M-50_nTuple.root", // 7
                        
                        //"SingleElectron_Run2015C_25ns-05Oct2015-v1.root", // 8
                        //"SingleElectron_Run2015D-05Oct2015-v1.root", // 9
                        //"SingleElectron_Run2015D-PromptReco-v4.root", // 10
                        "SingleElectron_Run2015C_25ns-05Oct2015-v1_v02.root", // 8
                        "SingleElectron_Run2015D-05Oct2015-v1_v02.root", // 9
                        "SingleElectron_Run2015D-PromptReco-v4_v02.root", // 10
                        
                        "MuonEG_Run2015C_25ns-05Oct2015-v1_v01.root", // 11
                        "MuonEG_Run2015D-05Oct2015-v2_v01.root", // 12
                        "MuonEG_Run2015D-PromptReco-v4_v01.root", //13
                        
                        "SingleMuon_Run2015C_25ns-05Oct2015-v1_v01.root", // 14
                        "SingleMuon_Run2015D-05Oct2015-v1_v01.root",   // 15
                        "SingleMuon_Run2015D-PromptReco-v4_v01.root"    // 16
                        
                    };
    
    
    
    int nCounts[100] = {};
    
    // histograms
    map<string,TH1F> h;         // 1d histograms
    map<string,TH2F> h2;        // 2d histograms
    string s = "v1";
    string hname, h2name;
    
    // event writer
    vector<Event> events;
    
    
    double start_time = time(NULL); // measure running time
    
    
    //test(nCounts, events);
    //cout << events.size() << endl;
    //cout << events[0].lumNo << endl;
    
    
    //terminate(nCounts);
    
    
    
    
    
    // ////////////////////////////////////////////////////////////////////////////////////////////
    // test
    // 
/*
    int set=0;
    //string dataset = filepath + datasets[set];
    //f__test(h, h2, dataset);
    h2name = "relPtdelR";
    h2[h2name] = TH2F((h2name + "_" + s).c_str(), 
                ";p^{#gamma}_{T}/p^{trObj}_{T}; #DeltaR(#gamma, trObj)",
                2000, 0., 2.,
                1000, 0, 0.1);
    
    for(int i=8; i<=8; i++){
        cout << endl;
        cout << "**************************************************************** " << i;
        cout << " *****************************************************************"<<endl;
        f__test(h, h2, filepath+datasets[i]);
    } // 
    
    f_diphoton_singleelectron_triggermatching(h, h2, dataset);
    
    //writeToFile(h, h2, "/distributions_SingleElectron.root");
    // */
    
    // ////////////////////////////////////////////////////////////////////////////////////////////
    // create distributions
/*
    int set=1;
    string dataset = filepath + datasets[set];
    f_distribution_creator(h, h2, dataset);
    string selectorname = "/data_distributions.root";
    switch(set){
        case 0: selectorname = "/DY_distributions.root"; break;
        case 1: selectorname = "/TT_distributions.root"; break;
        case 2: selectorname = "/WG_distributions.root"; break;
        case 3: selectorname = "/ZG_distributions.root"; break;
    }
    writeToFile(h, h2, selectorname);
    // */
    
    
    // ////////////////////////////////////////////////////////////////////////////////////////////
    // tag and probe NEUE METHODE
    // 
    // - mit trigger matching
    // - mit diesdas
    // - NUR FUER DATEN
/*
    init(h, h2);
    int set=8;
    int r;
    string dataset;
    for(set=8; set <=10; set++){
        dataset = filepath + datasets[set];
        r = f_my_singleelectron_triggermatching(h, h2, dataset, nCounts, events);
    }
    cout << "return: " << r << endl;
    terminate(nCounts); //output
    //eventwriter(events);
    //writeToFile(h, h2, "/triggermatching_stuff.root");
    // */
    
    
    // ////////////////////////////////////////////////////////////////////////////////////////////
    // tag and probe NEUE METHODE
    // 
    // - mit trigger matching
    // - mit diesdas
    // - NUR FUER SIMULATION
    /*
    init(h, h2);
    int set=8;
    int r;
    string dataset;
    for(set=8; set <=10; set++){
        dataset = filepath + datasets[set];
        //r = f_my_singleelectron_MC_triggermatching(h, h2, dataset, nCounts, events);
        r = f_my_singleelectron_triggermatching(h, h2, dataset, nCounts, events);
    }
    cout << "return: " << r << endl;
    terminate(nCounts); //output
    //eventwriter(events);
    writeToFile(h, h2, "/NEW_data_tnp_forPlots.root");
    // */
    
    
    
    
    // ////////////////////////////////////////////////////////////////////////////////////////////
    // muon background estimation
    init(h, h2);
    int set;
    int r;
    string dataset;
    for(set=14; set <=14; set++){
        dataset = filepath + datasets[set];
        r = f_my_muon_bkg(h, h2, dataset, nCounts, events);
    }
    cout << "return: " << r << endl;
    terminate(nCounts); //output
    //eventwriter(events);
    writeToFile(h, h2, "/muon_bkg_estimation.root");
    // */
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    double end_time = 1.*( time(NULL));
    cout << "Runtime ~" << (end_time - start_time)/1 << " sec." << endl;
    
} // myTestSelector() */

