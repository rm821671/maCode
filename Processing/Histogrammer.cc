/*******************************************************************************************
 * Histogrammer.cc
 *
 * creates a root file containing histograms
 *
 *
 *******************************************************************************************
**/

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


#include "TreeParticles.hpp"
//#include "../../cms_sw/CMSSW_7_4_14/src/TreeWriter/TreeWriter/plugins/TreeParticles.hpp"
//#include "/home/home4/institut_1b/rmeyer/cms_sw/CMSSW_7_4_14/src/TreeWriter/TreeWriter/plugins/TreeParticles.hpp"
//#include "TreeParticles.hpp"

// comment catcher */
// namespace stuff
using namespace tree;
using namespace std;

/*******************************************************************************************
 * 
 * own functions and stuff
 *
 *******************************************************************************************/

// number of test count variables
const int sizeNcounts = 100;

//TCanvas* results;

// template to calculate the sign
template <typename T> int sign(T val) {
      return (T(0) < val) - (val < T(0));
}

// convert numer to string
template <typename T> string NumberToString ( T Number ) {
      ostringstream ss;
      ss << Number;
      return ss.str();
  }

// 
string getRawFileName( string strIn){
	// converts "/path/to/ntuple/XYZ.root"
	// to "XYZ"
	
	auto PosStart = strIn.rfind("/");
	auto PosEnd = strIn.find(".root");
	
	string outputName;
	
	if( PosEnd != string::npos ) {
		outputName = strIn.substr( PosStart+1, PosEnd-PosStart-1 );
	}
	return outputName;
	
}

// create output file name depending on input tree
string getOutputFilename( string strIn ) {

  // Converts "/path/to/ntuple/XYZ.root" 
  // to "../selector_rootFiles/my_selector_results_XYZ_(timestamp).root"



  auto PosStart = strIn.rfind("/");
  auto PosEnd = strIn.find(".root");
  string outputName;
  
  if( PosEnd != string::npos ) {
    outputName = "/user/rmeyer/Dropbox/data_uni/root_selectorFiles/my_selector_results_" + 
    				strIn.substr( PosStart+1, PosEnd-PosStart-1 ) + 
    				"_" + 
    				NumberToString(time(0)) +
    				".root";
  }
  return outputName;

}

void replaceInString(string &s) {
	std::replace( s.begin(), s.end(), '.', 'd'); // replace all '.' to 'd'
}

// calculate parameters for corrected iso gamma
void fIsoGammaCorr(float eta, float& alpha, float& A, float& kappa){
	
	alpha = 2.5;		// always
	A = 0.;				// default
	kappa = 4.5e-3;		// only change in specific cases, see below
	
	// AN Table 8:
	if(eta < 0.9) A = 0.17;
	else if(eta > 0.9 && eta < 1.4442) A = 0.14;
	else if(eta > 1.566 && eta < 2.0) A = 0.11;
	else if(eta > 2.0 && eta < 2.2){
		A = 0.14;
		kappa = 3.0e-3;
	}
	else if(eta > 2.2 && eta < 2.5){
		A = 0.22;
		kappa = 3.0e-3;
	}
	
}

// */

/*******************************************************************************************
 * 
 * Class Histogrammer
 *
 ************************************************/
class Histogrammer : public TSelector {
	public:
		
		Histogrammer(); // constructor defined below
		
		virtual ~Histogrammer() { }

		virtual void    Init(TTree *tree_);
		virtual void	Begin(TTree *tree_);
		virtual void    SlaveBegin(TTree *tree_);
		virtual Bool_t  Process(Long64_t entry);
		virtual void	SlaveTerminate();
		virtual void    Terminate();
		virtual Int_t   Version() const { return 2; }

		void resetSelection();
		void defaultSelection();

		void initSelection( string const& s );
		void fillSelection( string const& s );

		void initObjects( string const& s );
		void fillObjects( string const& s );

		void initBkgEst( string const& s );
		void fillBkgEst( string const& s );
		

		TTreeReader fReader;
		TTreeReaderArray<tree::Photon> photons;
		TTreeReaderArray<tree::Jet> jets;
		TTreeReaderArray<tree::Electron> electrons;
		TTreeReaderArray<tree::Muon> muons;
		TTreeReaderArray<tree::Particle> genJets;
		TTreeReaderArray<tree::GenParticle> genParticles;
		//TTreeReaderArray<tree::PFCandidate> pfCandidates;

		TTreeReaderValue<tree::MET> met;
		
		TTreeReaderValue<Float_t> rho;
		
		TTreeReaderValue<Float_t> pu_weight;
		TTreeReaderValue<Char_t> mc_weight;
		
		TTreeReaderValue<Float_t> genHt;
		
		
		TTreeReaderValue<Int_t> nGoodVertices;
		//TTreeReaderValue<Int_t> nChargedPfCandidates;
		TTreeReaderValue<Int_t> nPV;
		TTreeReaderValue<Int_t> genLeptonsFromW;

		TTreeReaderValue<ULong64_t> eventNo;
		TTreeReaderValue<UInt_t> runNo;
		TTreeReaderValue<UInt_t> lumNo;

		//TTreeReaderValue<UInt_t> size;

		TTreeReaderValue<Bool_t> isRealData;
		
		TTreeReaderValue<Bool_t> signalTrigger; /* this is the used trigger */
		TTreeReaderValue<Bool_t> signalTriggerFired;
		TTreeReaderValue<Bool_t> crossTriggerPhotonFired;
		TTreeReaderValue<Bool_t> crossTriggerHtFired;
		
		TTreeReaderValue<Bool_t> TriggerR9Id90;
		TTreeReaderValue<Bool_t> TriggerR9Id85;
		
		//TTreeReaderValue<Bool_t> TriggerDiphoton_Primary;
		//TTreeReaderValue<Bool_t> TriggerDiphoton_Contr01;
		//TTreeReaderValue<Bool_t> TriggerDiphoton_Contr02;
		//TTreeReaderValue<Bool_t> TriggerDiphoton_Contr03;
		
		
		vector<tree::Photon*> selPhotons;
		vector<tree::Jet*> selJets;
		vector<tree::Jet*> selBJets;
		vector<tree::Electron*> selElectrons;
		vector<tree::Muon*> selMuons;
		//vector<tree::PFCandidate*> selPFCandidates;
		
		// genParticles
		
		vector<tree::Photon*> selPhotons2;
		vector<tree::Jet*> selJets2;
		vector<tree::Jet*> selBJets2;
		vector<tree::Electron*> selElectrons2;
		vector<tree::Muon*> selMuons2;
		
		int nTotalEvents = 0;		// weighted number of monte carlo events
		
		
		int nTotalEvents_bin = 1;	// bin of the cutflow diagram

		Float_t selW = 1.;			// weight

		int Nnum = 0; // total counter, added up in every event
		
		int Ncounts[sizeNcounts] = {};			// some test counter int
		
		TVector3 	vTemp[sizeNcounts];			// array of vectors
		TLorentzVector	lvTemp[sizeNcounts];	// array of lorentz vectors
		
		float Etemp[sizeNcounts];
		
		float 	Ng_1 =0,
				Ne_1 =0;
		float	Ne_pt[10],
				Ng_pt[10];
		
		bool isData;
		
		// some name strings
		string 	hname,
				h2name,
				//h3name,
				numstr;
		
		map<string,TEfficiency> eff;	// efficiencies
		map<string,TH1F> h;				// 1d histograms
		map<string,TGraph> g;			// graphs , NOT USED
		map<string,TH2F> h2;			// 2d histograms
		//map<string,TH3F> h3;			// 3d histograms
		
		// constants:
		float	m_z = 91.1876; // z mass in GeV
		
};

/*******************************************************************************************
 * 
 * constructor: link the branches to the access variables
 *
 ************************************************/
Histogrammer::Histogrammer(): 
	photons( fReader, "photons" ),
	jets( fReader, "jets" ),
	electrons( fReader, "electrons" ),
	muons( fReader, "muons" ),
	genJets( fReader, "genJets" ),
	genParticles( fReader, "genParticles" ),
	//pfCandidates( fReader, "PFCandidates" ),
	met( fReader, "met" ),
	nGoodVertices( fReader, "nGoodVertices" ),
	//nChargedPfCandidates( fReader, "nChargedPfCandidates" ),
	rho ( fReader, "rho" ),
	mc_weight( fReader, "mc_weight" ),
	pu_weight( fReader, "pu_weight" ),
	
	genHt( fReader, "genHt"),
	eventNo( fReader, "evtNo"),
	runNo( fReader, "runNo"),
	lumNo( fReader, "lumNo"),
	
	signalTriggerFired( fReader, "HLT_Photon90_CaloIdL_PFHT500_v" ),
	TriggerR9Id90( fReader, "HLT_Photon36_R9Id90_HE10_Iso40_EBOnly_PFMET40_v" ),
	TriggerR9Id85( fReader, 
		"HLT_Photon36_R9Id85_OR_CaloId24b40e_Iso50T80L_Photon22_AND_HE10_R9Id65_Eta2_Mass15_v" )
	
	//isRealData( fReader, "isRealData" ),
	//signalTriggerFired( fReader, "HLT_Photon90_CaloIdL_PFHT500_v" ),
	//crossTriggerPhotonFired( fReader, "HLT_Photon90_v" ),
	//crossTriggerHtFired( fReader, "HLT_PFHT600_v" )
{
	cout << "constructor()" << endl;
} // constructor


/*******************************************************************************************
 * 
 * initialize Selection
 *
 ************************************************/
void Histogrammer::initSelection( string const& s ) {
	
	cout << "initSelection()" << endl;
	
	// TH2F (const char *name, const char *title, 
	//	Int_t nbinsx, Double_t xlow, Double_t xup, 
	//	Int_t nbinsy, Double_t ylow, Double_t yup)
	
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// fake rate, photon tag
	
	h2name = "f_zpeak_tot_eta";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(),";m [GeV];|#eta|",
						200,0,200,
						300,-1.5,1.5);
	
	h2name = "f_zpeak_pas_eta";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(),";m [GeV];|#eta|",
						200,0,200,
						300,-1.5,1.5);
	
	h2name = "f_zpeak_tot_pt";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(),";m [GeV];pt [GeV]",
						200,0,200,
						250,0,250);
	
	h2name = "f_zpeak_pas_pt";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(),";m [GeV];pt [GeV]",
						200,0,200,
						250,0,250);
	
	h2name = "f_zpeak_tot_nvtx";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(),";m [GeV];N_{vtx}",
						200,0,200,
						60,0,60);

	h2name = "f_zpeak_pas_nvtx";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(),";m [GeV];N_{vtx}",
						200,0,200,
						60,0,60);

	h2name = "f_zpeak_tot_njet";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(),";m [GeV];N_{jet}",
						200,0,200,
						15,0,15);

	h2name = "f_zpeak_pas_njet";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(),";m [GeV];N_{jet}",
						200,0,200,
						15,0,15);
	
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// transfer factor, photon tag
	
	h2name = "r_zpeak_tot_eta";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(),";m [GeV];|#eta|",
						200,0,200,
						300,-1.5,1.5);
	
	h2name = "r_zpeak_pas_eta";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(),";m [GeV];|#eta|",
						200,0,200,
						300,-1.5,1.5);
	
	h2name = "r_zpeak_tot_pt";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(),";m [GeV];pt [GeV]",
						200,0,200,
						250,0,250);
	
	h2name = "r_zpeak_pas_pt";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(),";m [GeV];pt [GeV]",
						200,0,200,
						250,0,250);
	
	h2name = "r_zpeak_tot_nvtx";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(),";m [GeV];N_{vtx}",
						200,0,200,
						60,0,60);

	h2name = "r_zpeak_pas_nvtx";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(),";m [GeV];N_{vtx}",
						200,0,200,
						60,0,60);

	h2name = "r_zpeak_tot_njet";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(),";m [GeV];N_{jet}",
						200,0,200,
						15,0,15);

	h2name = "r_zpeak_pas_njet";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(),";m [GeV];N_{jet}",
						200,0,200,
						15,0,15);
	
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// transfer factor, electron tag
	h2name = "r_etag_delr_plane";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(),";#Delta #phi;#Delta #eta",
						330,-3.3,3.3, 	// 0.02 steps
						240,-6,6);		// 0.05 steps

	h2name = "r_etag_zpeak_tot_eta";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(),";m [GeV];|#eta|",
						200,0,200,
						150,0,1.5);
	
	h2name = "r_etag_zpeak_pas_eta";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(),";m [GeV];|#eta|",
						200,0,200,
						150,0,1.5);

	h2name = "r_etag_zpeak_tot_pt";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(),";m [GeV];pt [GeV]",
						200,0,200,
						250,0,250);
	
	h2name = "r_etag_zpeak_pas_pt";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(),";m [GeV];pt [GeV]",
						200,0,200,
						250,0,250);
	
	h2name = "r_etag_zpeak_tot_nvtx";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(),";m [GeV];N_{vtx}",
						200,0,200,
						60,0,60);
	
	h2name = "r_etag_zpeak_pas_nvtx";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(),";m [GeV];N_{vtx}",
						200,0,200,
						60,0,60);

	h2name = "r_etag_zpeak_tot_njet";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(),";m [GeV];N_{jet}",
						200,0,200,
						15,0,15);
	
	h2name = "r_etag_zpeak_pas_njet";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(),";m [GeV];N_{jet}",
						200,0,200,
						15,0,15);
	
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// fake rate, photon tag
	// medium id
	
	h2name = "f_medium_zpeak_tot_eta";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(),";m [GeV];|#eta|",
						200,0,200,
						300,-1.5,1.5);
	
	h2name = "f_medium_zpeak_pas_eta";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(),";m [GeV];|#eta|",
						200,0,200,
						300,-1.5,1.5);
	
	h2name = "f_medium_zpeak_tot_pt";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(),";m [GeV];pt [GeV]",
						200,0,200,
						250,0,250);
	
	h2name = "f_medium_zpeak_pas_pt";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(),";m [GeV];pt [GeV]",
						200,0,200,
						250,0,250);
	
	h2name = "f_medium_zpeak_tot_nvtx";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(),";m [GeV];N_{vtx}",
						200,0,200,
						60,0,60);

	h2name = "f_medium_zpeak_pas_nvtx";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(),";m [GeV];N_{vtx}",
						200,0,200,
						60,0,60);

	h2name = "f_medium_zpeak_tot_njet";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(),";m [GeV];N_{jet}",
						200,0,200,
						15,0,15);

	h2name = "f_medium_zpeak_pas_njet";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(),";m [GeV];N_{jet}",
						200,0,200,
						15,0,15);
	
	
	fOutput->AddAll(gDirectory->GetList());
} // init selection

/*******************************************************************************************
 * 
 * initialize the tree reader
 *
 ************************************************/
void Histogrammer::Init(TTree *tree_)
{
	cout << "Init()" << endl;
	
	fReader.SetTree(tree_);
	string thisFileName = fReader.GetTree()->GetCurrentFile()->GetName();
	cout << "opened file: " << thisFileName << endl;
	
	// Attention: isData is defined inverse!
	// -->	if mc	 set true
	// 		if data	 set false
	isData = 	thisFileName.find("DYJetsToLL") != string::npos
			||	thisFileName.find("TTGJets") != string::npos
			||	thisFileName.find("WGToLNuG") != string::npos
			||	thisFileName.find("ZGTo2LG") != string::npos;
	
	cout << "isData: " << endl;
	
	TFile thisFile(thisFileName.c_str());
	if(!thisFile.IsZombie()){
		TH1F* hcut = (TH1F*)thisFile.Get("TreeWriter/hCutFlow");
		nTotalEvents = hcut->GetBinContent(nTotalEvents_bin);
	}
	
	cout << "total events: " << nTotalEvents << endl;
	cout << "rawfile: " << getRawFileName(fReader.GetTree()->GetCurrentFile()->GetName()) << endl;
	
	cout << "end of Init()" << endl;
	
}

/*******************************************************************************************
 * 
 * reset the selections
 *
 ************************************************/
void Histogrammer::resetSelection() {
		
		
	selPhotons.clear();
	selJets.clear();
	selBJets.clear();
	selElectrons.clear();
	selMuons.clear();
	//selPFCandidates.clear();
	selPhotons2.clear();
	selJets2.clear();
	selBJets2.clear();
	selElectrons2.clear();
	selMuons2.clear();	
	
	for(int i = 0; i < sizeNcounts; i++){
		vTemp[i].SetXYZ(0.,0.,0.);			// reset temp vectors
		lvTemp[i].SetPxPyPzE(0.,0.,0.,0.);	// reset temp lorentzvectors
		Etemp[i] = 0.;
	
	}
	
	// ************************************************************************
	// */		
	
	
} // reset selection

/*******************************************************************************************
 * 
 * begin
 *
 ************************************************/
void Histogrammer::Begin(TTree * /*tree*/)
{
	// The Begin() function is called at the start of the query.
	// When running with PROOF Begin() is only called on the client.
	// The tree argument is deprecated (on PROOF 0 is passed).

	cout << "Begin()" << endl;
	
	//TString option = GetOption();
	//cout << "Option: " << option << endl;
	//Info("Begin", "starting a simple exercise with process option: %s", option.Data());
	//printf("\n");
	
}

/*******************************************************************************************
 * 
 * slave begin
 *
 ************************************************/
void Histogrammer::SlaveBegin(TTree *tree_)
{
	cout << "SlaveBegin()" << endl;
	
	
	
	// initalize the selection histograms	
	//initSelection("nocut");
	
	initSelection("test");
	
} // slave begin

/*******************************************************************************************
 * 
 * process.
 *
 ************************************************/
Bool_t Histogrammer::Process(Long64_t entry)
{
	// comment this out to analyze the whole tree:
	//if(Nnum > 100000) return kTRUE;
	
	// counter output
	if((Nnum % 200000) == 0){
			
			cout << "event " << Nnum << " done... \t";
			cout << float(Nnum)/float(nTotalEvents)*100. << " %" << endl;
		} 
	
	resetSelection();
	
	fReader.SetEntry(entry);	// fReader points on current entry
	
	//string filename = getRawFileName(fReader.GetTree()->GetCurrentFile()->GetName());
	
	// set weight
	selW = *mc_weight * *pu_weight;
	
/*
	for(auto& p: photons){
			if(p.passElectronVeto) Ncounts[50]++;
			if(!p.passElectronVeto) Ncounts[51]++;
			
			if(p.passElectronVeto && !p.passElectronVeto) Ncounts[52]++;
	}
*/
	
	int 	ph = 0,
			ph2 = 0,
			_c = 0;
	
	
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//
	// Calculate fakerate with all events by gen match 
	// only on montecarlo! --> if(isData == True)
	// Ncounts 10-19
	// 
{	// method start
	if(isData){
		// TODO
		
	}
	
}	//method end
	
	resetSelection();
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//
	// Tag and Probe Method with a photon object as tag
	// Ncounts 20-29
	//
	// FAKERATE: f = Neg/(2*Nee + Neg)
	//
	// here, the "total" histograms contain: 2*Nee + Neg
	// while the "passed" histograms contain: Neg
	//
{	// method start
	_c = 0;
	ph = 0;
	
	// calculate four vector of all photon objects
	for(auto& p: photons){
		vTemp[_c] = vTemp[_c] + p.p;
		Etemp[_c] += p.p.Mag();
	}
	
	lvTemp[_c].SetVect(vTemp[_c]);
	lvTemp[_c].SetE(Etemp[_c]);
	
	// events with trigger and two photon objects
	if(*TriggerR9Id85 && photons.GetSize() == 2){
		Ncounts[0] ++;
		// tag:
		if(	photons[ph].p.Pt() > 40 && 
			photons[ph].hasPixelSeed && 
			fabs(photons[ph].p.Eta()) < 1.4442){
				// probe
				ph = 1;
				if(	photons[ph].p.Pt() > 40 && 
					fabs(photons[ph].p.Eta()) < 1.4442){
						// fake rate: FILL TOTAL
						h2name = "f_zpeak_tot_pt";
						h2[h2name].Fill(lvTemp[_c].M(), photons[ph].p.Pt(), selW);
						h2name = "f_zpeak_tot_nvtx";
						h2[h2name].Fill(lvTemp[_c].M(), *nGoodVertices, selW);
						h2name = "f_zpeak_tot_njet";
						h2[h2name].Fill(lvTemp[_c].M(), jets.GetSize(), selW);
						h2name = "f_zpeak_tot_eta";
						h2[h2name].Fill(lvTemp[_c].M(), photons[ph].p.Eta(), selW);
						
						if(!photons[ph].hasPixelSeed){
							// fake rate: FILL PASSED
							h2name = "f_zpeak_pas_pt";
							h2[h2name].Fill(lvTemp[_c].M(), photons[ph].p.Pt(), selW);
							h2name = "f_zpeak_pas_nvtx";
							h2[h2name].Fill(lvTemp[_c].M(), *nGoodVertices, selW);
							h2name = "f_zpeak_pas_njet";
							h2[h2name].Fill(lvTemp[_c].M(), jets.GetSize(), selW);
							h2name = "f_zpeak_pas_eta";
							h2[h2name].Fill(lvTemp[_c].M(), photons[ph].p.Eta(), selW);
						}
				}
		}
		ph = 1; // change photons
		// tag:
		if(	photons[ph].p.Pt() > 40 && 
			photons[ph].hasPixelSeed && 
			fabs(photons[ph].p.Eta()) < 1.4442){
				// probe
				ph = 0;
				if(	photons[ph].p.Pt() > 40 && 
					fabs(photons[ph].p.Eta()) < 1.4442){
						// fake rate: FILL TOTAL
						h2name = "f_zpeak_tot_pt";
						h2[h2name].Fill(lvTemp[_c].M(), photons[ph].p.Pt(), selW);
						h2name = "f_zpeak_tot_nvtx";
						h2[h2name].Fill(lvTemp[_c].M(), *nGoodVertices, selW);
						h2name = "f_zpeak_tot_njet";
						h2[h2name].Fill(lvTemp[_c].M(), jets.GetSize(), selW);
						h2name = "f_zpeak_tot_eta";
						h2[h2name].Fill(lvTemp[_c].M(), photons[ph].p.Eta(), selW);
						
						if(!photons[ph].hasPixelSeed){
							// fake rate: FILL PASSED
							h2name = "f_zpeak_pas_pt";
							h2[h2name].Fill(lvTemp[_c].M(), photons[ph].p.Pt(), selW);
							h2name = "f_zpeak_pas_nvtx";
							h2[h2name].Fill(lvTemp[_c].M(), *nGoodVertices, selW);
							h2name = "f_zpeak_pas_njet";
							h2[h2name].Fill(lvTemp[_c].M(), jets.GetSize(), selW);
							h2name = "f_zpeak_pas_eta";
							h2[h2name].Fill(lvTemp[_c].M(), photons[ph].p.Eta(), selW);
						}
				}
		}
	} // trigger and 2 electrons
	
}	// method end
	
	resetSelection();
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//
	// Tag and Probe Method with a photon object as tag
	// Ncounts 30-39
	//
	// TRANSFERFACTOR: R = Neg/2*Nee
	//
	// here, the "total" histograms contain: 2*Nee
	// while the "passed" histograms contain: Neg
	//
	//
{	// method start
	_c = 0;
	ph = 0;
	
	// calculate four vector of all photon objects
	for(auto& p: photons){
		vTemp[_c] = vTemp[_c] + p.p;
		Etemp[_c] += p.p.Mag();
	}
	
	lvTemp[_c].SetVect(vTemp[_c]);
	lvTemp[_c].SetE(Etemp[_c]);
	
	// events with trigger and two photon objects
	if(*TriggerR9Id85 && photons.GetSize() == 2){
		Ncounts[0] ++;
		// tag:
		if(	photons[ph].p.Pt() > 40 && 
			photons[ph].hasPixelSeed && 
			fabs(photons[ph].p.Eta()) < 1.4442){
				// probe
				ph = 1;
				if(	photons[ph].p.Pt() > 40 && 
					fabs(photons[ph].p.Eta()) < 1.4442){
						if(photons[ph].hasPixelSeed){
							// transfer factor: FILL TOTAL
							h2name = "r_zpeak_tot_pt";
							h2[h2name].Fill(lvTemp[_c].M(), photons[ph].p.Pt(), selW);
							h2name = "r_zpeak_tot_nvtx";
							h2[h2name].Fill(lvTemp[_c].M(), *nGoodVertices, selW);
							h2name = "r_zpeak_tot_njet";
							h2[h2name].Fill(lvTemp[_c].M(), jets.GetSize(), selW);
							h2name = "r_zpeak_tot_eta";
							h2[h2name].Fill(lvTemp[_c].M(), fabs(photons[ph].p.Eta()), selW);
						}
						
						if(!photons[ph].hasPixelSeed){
							// transfer factor: FILL PASSED
							h2name = "r_zpeak_pas_pt";
							h2[h2name].Fill(lvTemp[_c].M(), photons[ph].p.Pt(), selW);
							h2name = "r_zpeak_pas_nvtx";
							h2[h2name].Fill(lvTemp[_c].M(), *nGoodVertices, selW);
							h2name = "r_zpeak_pas_njet";
							h2[h2name].Fill(lvTemp[_c].M(), jets.GetSize(), selW);
							h2name = "r_zpeak_pas_eta";
							h2[h2name].Fill(lvTemp[_c].M(), fabs(photons[ph].p.Eta()), selW);
						}
				}
		}
		ph = 1; // change photons
		// tag:
		if(	photons[ph].p.Pt() > 40 && 
			photons[ph].hasPixelSeed && 
			fabs(photons[ph].p.Eta()) < 1.4442){
				// probe
				ph = 0;
				if(	photons[ph].p.Pt() > 40 && 
					fabs(photons[ph].p.Eta()) < 1.4442){
						if(photons[ph].hasPixelSeed){
							// transfer factor: FILL TOTAL
							h2name = "r_zpeak_tot_pt";
							h2[h2name].Fill(lvTemp[_c].M(), photons[ph].p.Pt(), selW);
							h2name = "r_zpeak_tot_nvtx";
							h2[h2name].Fill(lvTemp[_c].M(), *nGoodVertices, selW);
							h2name = "r_zpeak_tot_njet";
							h2[h2name].Fill(lvTemp[_c].M(), jets.GetSize(), selW);
							h2name = "r_zpeak_tot_eta";
							h2[h2name].Fill(lvTemp[_c].M(), fabs(photons[ph].p.Eta()), selW);
						}
						
						if(!photons[ph].hasPixelSeed){
							// transfer factor: FILL PASSED
							h2name = "r_zpeak_pas_pt";
							h2[h2name].Fill(lvTemp[_c].M(), photons[ph].p.Pt(), selW);
							h2name = "r_zpeak_pas_nvtx";
							h2[h2name].Fill(lvTemp[_c].M(), *nGoodVertices, selW);
							h2name = "r_zpeak_pas_njet";
							h2[h2name].Fill(lvTemp[_c].M(), jets.GetSize(), selW);
							h2name = "r_zpeak_pas_eta";
							h2[h2name].Fill(lvTemp[_c].M(), fabs(photons[ph].p.Eta()), selW);
						}
				}
		}
	} // trigger and 2 electrons
	
}	// method end
	
	resetSelection();
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//
	// Tag and Probe Method with an electron object as tag
	// Ncounts 50-59
	//
{	// method start
	_c = 0;
	ph = 0;
	
	//if((Nnum % 100000) == 0) {
	//	cout <<Nnum << "\tphoton objects: " << photons.GetSize() << endl;
	//	cout <<Nnum << "\telectr objects: " << electrons.GetSize() << endl;
	//}
	if(*TriggerR9Id85){
		for(auto& el: electrons){
			for(auto& p: photons) {
				
				// look for tag requirements in the electron object
				if(	el.p.Pt() > 40 &&
					fabs(el.p.Eta()) < 1.4442){
						
						// dR map between all electron and photon
						// objects for trigger and tag requirements
						h2name = "r_etag_delr_plane";
						h2[h2name].Fill(	el.p.DeltaPhi(p.p),
											el.p.Eta()-p.p.Eta(),
											selW);
						
						// check if the current photon object is
						// the same as the electron object
						// by dR criterium
						
						
						
				}
				
				
				
			}
			
		}
		
		
		
		
	} // if trigger
	
	
	
	
}	//method end
	
	resetSelection();
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//
	// Tag and Probe Method with a photon object as tag
	//
	// MEDIUM working point for the probe object
	//
	// Ncounts 60-69
	//
	// FAKERATE: f = Neg/(2*Nee + Neg)
	//
	// here, the "total" histograms contain: 2*Nee + Neg
	// while the "passed" histograms contain: Neg
	//
	// 60: Neg
	// 61: 2*Nee
	//
{	// method start
	_c = 0;
	ph = 0;
	ph2 = 0;
	
	// calculate four vector of all photon objects
	for(auto& p: photons){
		vTemp[_c] = vTemp[_c] + p.p;
		Etemp[_c] += p.p.Mag();
	}
	
	lvTemp[_c].SetVect(vTemp[_c]);
	lvTemp[_c].SetE(Etemp[_c]);
	

	/**
	 *
		TriggerDiphoton_Primary;
		TriggerDiphoton_Contr01;
		TriggerDiphoton_Contr02;
		TriggerDiphoton_Contr03;
	 * 
	 * */

}	// method end
	
	resetSelection();
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//
	// categorize events by Weinberg selection
	//
	// MEDIUM working point for the probe object
	//
	// Ncounts 70-79
	//
	// FAKERATE: f = Neg/(2*Nee + Neg)
	//
	// here, the "total" histograms contain: 2*Nee + Neg
	// while the "passed" histograms contain: Neg
	//
	// 60: Neg
	// 61: 2*Nee
	//
{	// method start
	
	/*
	if(photons.GetSize() >= 2){
		// loop all photon objects
		for(int ip=0, np=photons.GetSize(); ip<np; ip++){
			// construct all combinations:
			// second loop
			for(int iq=0; iq<np; iq++){
				
				
				
			}
			
			
			
		}
	}
	*/
	
}	// method end
	
	
// comment catcher */
	
	Nnum +=1 ;
	
	return kTRUE;
	
// comment catcher */
	
} // process

/*******************************************************************************************
 * 
 * terminate slave function - 
 *
 ************************************************/
void Histogrammer::SlaveTerminate()
{
	// The SlaveTerminate() function is called after all entries or objects
	// have been processed. When running with PROOF SlaveTerminate() is called
	// on each slave server.
	
	cout << "\nSlaveTerminate()" << endl;
	
}

/*******************************************************************************************
 * 
 * terminate - last function to call
 *
 ************************************************/
void Histogrammer::Terminate()
{
	auto openedDataFile = fReader.GetTree()->GetCurrentFile()->GetName();

	cout << "Terminate()" << endl;
	
	cout << "opened data file: " << openedDataFile << endl;
	cout << isData << endl;
	cout << "total counted: " << Nnum << endl;
	cout << "total from cutflow: " << nTotalEvents << endl;
	
	
	for(int ll=0;ll<sizeNcounts;ll++){
		cout << "Ncounts[" << ll << "]: " << Ncounts[ll] << "\t";
	}
	cout << endl;
	
	string comment;
	
	
	comment = "";
	
	
	//fOutput->Add(&g["Pt"]);*/
	
	//fOutput->Add(eff["Pt"]);
	
	//eff["Pt"].SetDirectory(gDirectory);
	//fOutput->AddAll(gDirectory->GetList());
	
	auto outputName = getOutputFilename( fReader.GetTree()->GetCurrentFile()->GetName() );
	
	
	
	
	// some time stuff
	time_t rawtime;
	struct tm * timeinfo;
	time ( &rawtime );
	timeinfo = localtime ( &rawtime );
	
	
	cout << "results written in: " << outputName << endl;
	
	TFile file( outputName.c_str(), "RECREATE");
	
// */
	fOutput->Write(); // write all objects merged by fOutput (used by PROOF)
	
	// publish protocoll
	ofstream prot("protocol_hist.txt", ios::app); //append
	if(prot.is_open()){
		prot << "--------------------------------------------------------------------------------";
		prot << endl;
		prot << "opened data file: " << openedDataFile << endl;
		prot << "finished at: " << asctime (timeinfo) << endl; 	// readable timestamp
		prot << "total counted: " << Nnum << endl;
		prot << "total from cutflow: " << nTotalEvents << endl;
		for(int ll=0;ll<sizeNcounts;ll++){
			prot << "Ncounts[" << ll << "]: " << Ncounts[ll] << "\t";
		}
		prot << endl;
		prot << "results written in: " << outputName << endl;
		prot << "comment (in code): " << comment << endl;
		prot.close();
		cout << "protocol published!" << endl;
	}
	
	// publish filename
	ofstream myFile("/user/rmeyer/Dropbox/data_uni/root_selectorFiles/outputfiles.txt", 
					ios::app); // append
	if(myFile.is_open()){
		myFile << outputName << endl;
		myFile.close();
		cout << "filename published!" << endl;
		
		
	}
	
	
} // terminate































