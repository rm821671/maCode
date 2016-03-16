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
						
						//"DYJetsToLL_M-50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8_v01.root"
						"DYJetsToLL_M-50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8_v02.root",// 0
						//"TTGJets_TuneCUETP8M1_13TeV-amcatnloFXFX-madspin-pythia8_v01.root",
						"TTGJets_TuneCUETP8M1_13TeV-amcatnloFXFX-madspin-pythia8_v02.root",// 1
						"WGToLNuG_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8_v01.root",	// 2
						"ZGTo2LG_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8_v01.root", // 3
						"DoubleEG_Run2015D-05Oct2015-v1_v02.root",	// 4
						"DoubleEG_Run2015D-PromptReco-v4_v02.root",	// 5
						"DY_v1.root", // 6
						"DYJetsToLL_M-50_nTuple.root", // 7
					};
	
	double start_time = time(NULL); // measure running time
	
	// histograms
	map<string,TH1F> h;			// 1d histograms
	map<string,TH2F> h2;		// 2d histograms
	
	// ////////////////////////////////////////////////////////////////////////////////////////////
	//  short diphoton fakerate reconstruction
	//  (from Marc and Arka)
	/*
	for(int i=4; i<=5; i++){
		cout << endl;
		cout << "**************************************************************** " << i;
		cout << " *****************************************************************"<<endl;
		f_diphoton_fakerate(h, h2, filepath+datasets[i]);
	}
	writeToFile(h, h2, "/DoubleEG_diphoton_fakerate.root");
	// **/
	
	// ////////////////////////////////////////////////////////////////////////////////////////////
	//  simple gen matching fakerate from drell yan sample only
	/*
	int set = 0;
	string dataset = filepath + datasets[set];
	
	f_genmatch_fakerate_DY(h, h2, dataset);
	string selectorname = "/genmatch_fakerate_"; //DYJetsToLL_M-50_nTuple.root";
	writeToFile(h, h2, selectorname+datasets[set]);
	// */
	
	// ////////////////////////////////////////////////////////////////////////////////////////////
	//  fakerate using tag and probe (DPG)
	/*
	// monte carlo:
	// drell yan sample
	int set = 2;
	string dataset = filepath + datasets[set];
	f_tagnprobe_fakerate(h, h2, dataset);
	string selectorname;
	switch(set){
		case 0: selectorname = "/DY_tagnprobe_fakerate.root"; break;
		case 1: selectorname = "/TT_tagnprobe_fakerate.root"; break;
		case 2: selectorname = "/WG_tagnprobe_fakerate.root"; break;
		case 3: selectorname = "/ZG_tagnprobe_fakerate.root"; break;
	}
	writeToFile(h, h2, selectorname);
	// */
	
	// ////////////////////////////////////////////////////////////////////////////////////////////
	//  closure test on ttbar (DPG)
	
	// monte carlo:
	// ttbar sample
	/*
	int set = 0;
	string dataset = filepath + datasets[set];
	f__closure(h, h2, dataset);
	string selectorname = "/DY_fakerate_closure.root";
	writeToFile(h, h2, selectorname);
	
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
	// debugging genmatch and tagnprobe
	int set=0;
	string dataset = filepath + datasets[set];
	f_genmatch_tagnprobe(h, h2, dataset);
	string selectorname = "debug_genMatchTagnProbe.root";
	string ss = "/data_";
	switch(set){
		case 0: ss = "/DY_"; break;
		case 1: ss = "/TT_"; break;
		case 2: ss = "/WG_"; break;
		case 3: ss = "/ZG_"; break;
	}
	//cout << ss+selectorname << endl;
	//writeToFile(h, h2, ss+selectorname);
	// */
	
	
	double end_time = 1.*( time(NULL));
	cout << "Runtime ~" << (end_time - start_time)/1 << " sec." << endl;
	
} // myTestSelector() */

