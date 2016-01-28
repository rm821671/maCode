// little script for test studies

#include "TreeParticles.hpp"

#include <map>
#include <sys/utsname.h>

using namespace tree;
using namespace std;


// check operating system and define the file path
string FilePath(){
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
	string ubuntu = "Ubuntu";	// if the version string contains ubuntu, its the laptop
								// 
	
	// search for ubuntu, otherwise its scientific
	if (version.find(ubuntu) != string::npos) {
		//	cout << "ubuntu found!" << '\n';
		path = "/home/jack/";
	}
	else {
		//	cout << "ubuntu not found!" << '\n';
		path = "/user/rmeyer/";
	}
	
	return path + "Dropbox/data_uni/";
} // FilePath()



void initSelection(map<string,TH1F> &h){
	
	
	string name = "111";
	h["1"] = TH1F(name.c_str(), "first; jo; joo", 10, 0, 10);
	
	//h["bye"] = TH1F(("bye").c_str(), "bye; no; ", 9, 0, 9);
	//h2["hallo"] = TH2F(("hallo").c_str(), "hallo; yea; ", 10, 0, 10);
	
	/*
	string s = "v1";
	string hname, h2name;
	
	h2name = "f_zpeak_tot_eta";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(),";m [GeV];|#eta|",
						200,0,200,
						60,0,6);
	
	hname = "pt_spectrum";
	h[hname] = TH1F("a1", ";bla;jo",100, 0, 100);
	
	// */
}


void selector(string file){
	cout << "selector()" << endl;
	//cout << file << endl;
	
	TFile *f = new TFile(file.c_str());
	TTreeReader reader("TreeWriter/eventTree", f);
	
	// initialize branches
	TTreeReaderArray<Photon> photons(reader, "photons");
	TTreeReaderArray<Jet> jets(reader, "jets");
	TTreeReaderArray<Electron> electrons(reader, "electrons");
	TTreeReaderArray<Muon> muons(reader, "muons");
	TTreeReaderArray<Particle> genJets(reader, "genJets");
	TTreeReaderArray<GenParticle> genParticles(reader, "genParticles");
	TTreeReaderValue<Float_t> pu_weight(reader, "pu_weight");
	TTreeReaderValue<Char_t> mc_weight(reader, "mc_weight");

	TTreeReaderValue<Bool_t> Trigger( reader, 
		"HLT_Photon36_R9Id85_OR_CaloId24b40e_Iso50T80L_Photon22_AND_HE10_R9Id65_Eta2_Mass15_v");
	
	Float_t selW = 1.;
	
	// histograms
	map<string,TH1F> h;				// 1d histograms
	map<string,TH2F> h2;			// 2d histograms
	
	//initSelection(h, h2);
	
	initSelection(h);
	
	
	
	cout << h["111"].GetName() << endl;
}








void myTestSelector(int dset){
	
	
	int set = 0;
	
	set = dset;
	
	string basicpath = FilePath();
	string path = basicpath + "mergedntuples/" ;
	
	string datasets[] = {	
						"DYJetsToLL_M-50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8_v01.root",// 0
						"TTGJets_TuneCUETP8M1_13TeV-amcatnloFXFX-madspin-pythia8_v01.root",// 1
						"WGToLNuG_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8_v01.root",	// 2
						"ZGTo2LG_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8_v01.root", // 3
						"DoubleEG_Run2015D-05Oct2015-v1.root",	// data  4
						"DoubleEG_Run2015D-PromptReco-v4.root"	// data  5
					};
	
	string file = path + datasets[set];
	
	cout << "Open file: " << file << endl;
	
	selector(file);
	
	
	
	
	
	
	
	
	
} // myTestSelector()










int main(){
	cout << "called!" << endl;
	
	return 0;
}
// */
