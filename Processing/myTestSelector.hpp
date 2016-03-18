
#include <ctime>
#include <sstream>
#include <iostream>
#include <fstream>
#include <iomanip>

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

#include <map>
#include <sys/utsname.h>

//#include "myTestSelector.hpp"
// */

using namespace tree;
using namespace std;


/**********************************************************************************************
 * setup functions
 * 
 * 
 */

// convert numer to string
template <typename T> string NumberToString ( T Number ) {
		ostringstream ss;
		ss << Number;
		return ss.str();
	}

// check operating system and define the file paths
void SystemPath(string & filepath, string & dropbox){
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
	string ubuntu = "Ubuntu";	// if the version string contains ubuntu, 
								// its the laptop
	cout << "System Version: " << version << endl;
	
	// search for ubuntu, otherwise its scientific
	if (version.find(ubuntu) != string::npos) {
		//	cout << "ubuntu found!" << '\n';
		path = "/home/jack/";
		filepath = path + "data_uni/mergedntuples/";
	}
	else {
		//	cout << "ubuntu not found! choose scientific" << '\n';
		path = "/user/rmeyer/";
		filepath = path + "mergedNTuples/";
	}
	
	dropbox = path + "Dropbox/data_uni/";
	
} // SystemPath()

// little 
void writeToFile(map<string,TH1F> &h, map<string,TH2F> &h2, string dataset){
	
	string outputName, filepath, dropbox;
	SystemPath(filepath, dropbox);
	
	auto PosStart = dataset.rfind("/");
	auto PosEnd = dataset.find(".root");
	  
	if( PosEnd != string::npos ) {
		outputName = 	dropbox +
						"root_selectorFiles/myTestSelector_" + 
						dataset.substr( PosStart+1, PosEnd-PosStart-1 ) + 
						"_" + 
						NumberToString(time(0)) +
						".root";
	}
	
	cout << endl;
	cout << "Writing to output file " << outputName << endl;
	
	
	
	// save all defined histograms to file
	TFile file( outputName.c_str(), "RECREATE");
	for( auto& mapIt : h )
		mapIt.second.Write( mapIt.first.c_str(), TObject::kWriteDelete );
	for( auto& mapIt : h2 )
		mapIt.second.Write( mapIt.first.c_str(), TObject::kWriteDelete );
	// */
	cout << endl;
	cout << "tb " << outputName << endl;
	cout << endl;
}

/**********************************************************************************************
 * analysis functions
 *
 */

void f_distribution_creator(map<string,TH1F> &h, map<string,TH2F> &h2, string dataset){
	cout << "f_distribution_creator()" << endl;
	cout << dataset << endl;
	
	// INITIALIZATION
	string s = "v1";
	string hname, h2name;
	
	h2name = "gen_e_gamma_PtdelR";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(), 
				";|p^{gen e}_{T} - p^{#gamma}_{T}|/p^{gen e}_{T}; #DeltaR(gen e, #gamma)",
				200, 0, 2,
				400, 0, 4);
	h2name = "gen_e_gamma_PtdelR_cut";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(), 
				";|p^{gen e}_{T} - p^{#gamma}_{T}|/p^{gen e}_{T}; #DeltaR(gen e, #gamma)",
				200, 0, 2,
				400, 0, 4);
	
	// **********************************************************************
	
	// Attention: isData is defined inverse!
	// -->	if mc	 set true
	// 		if data	 set false
	// >>> renamded to isSimu
	
	bool isSimu;
	isSimu = 	dataset.find("DYJetsToLL") != string::npos
			||	dataset.find("TTGJets") != string::npos
			||	dataset.find("WGToLNuG") != string::npos
			||	dataset.find("ZGTo2LG") != string::npos;
	// */
	
	// DATA FILE
	TFile *f = new TFile(dataset.c_str());
	TTreeReader reader("TreeWriter/eventTree", f);
	
	// initialize branches
	TTreeReaderArray<tree::Photon> photons(reader, "photons");
	TTreeReaderArray<tree::Jet> jets(reader, "jets");
	TTreeReaderArray<tree::Electron> electrons(reader, "electrons");
	TTreeReaderArray<tree::Muon> muons(reader, "muons");
	TTreeReaderArray<tree::Particle> genJets(reader, "genJets");
	TTreeReaderArray<tree::GenParticle> genParticles(reader, "genParticles");
	TTreeReaderValue<Float_t> pu_weight(reader, "pu_weight");
	TTreeReaderValue<Char_t> mc_weight(reader, "mc_weight");
	TTreeReaderValue<Int_t> nGoodVertices(reader, "nGoodVertices");
	
	
	Float_t selW = 1.;
	
	// initialize variables
	int		Nnum = 0;
	
	int		nCounts[100] = {};
	
	TVector3 	v;			// array of vectors
	TLorentzVector	lv;	// array of lorentz vectors
	float E = 0.;
	v.SetXYZ(0.,0.,0.);
	lv.SetPxPyPzE(0.,0.,0.,0.);
	
	
	bool gotoNextPhoton = false;
	
	bool matched = false;
	
	float 	delR = 0., 
			delPt = 0., 
			delRPt = 0.,
			mindelR = 0.,
			mindelPt = 0.;
	
	Photon selPhoton;
	GenParticle selGenParticle; 
	
	///////////////////////////////////////////////////////////////////////////////////////////////
	// loop the events
	while(reader.Next()){
		
		//if(Nnum > 100000) continue;
		
		// counter output
		if((Nnum % 100000) == 0){
			cout << "\revent " << Nnum << " done... \t" << flush;
		}
		
		E = 0.;
		v.SetXYZ(0.,0.,0.);
		lv.SetPxPyPzE(0.,0.,0.,0.);
		
		
		for(auto& genp: genParticles){
			if(abs(genp.pdgId) == 11){
				
				for(auto& p: photons){
					
					if(	p.p.Pt() > 40 &&
						fabs(p.p.Eta()) < 1.4442){
							
							h2name = "gen_e_gamma_PtdelR";
							h2[h2name].Fill(	fabs(genp.p.Pt()-p.p.Pt())/genp.p.Pt(),
												genp.p.DeltaR(p.p)
											);
							
						}
					
				}
				
			}
			
		}
		
		
		// 
		for(auto& genp: genParticles){
			if(abs(genp.pdgId) == 11){
				delR = 0.;
				delPt = 0.;
				delRPt = 0.;
				matched = false;
				for(auto& p: photons){
					
					if(	p.p.Pt() > 40 &&
						fabs(p.p.Eta()) < 1.4442){
							matched = true;
							delR = genp.p.DeltaR(p.p);
							delPt = fabs(genp.p.Pt()-p.p.Pt())/genp.p.Pt();
							// if delRPt is smaller than for the last catch, take the new one
							if(	delRPt == 0. || 
								sqrt(delR*delR/(0.2*0.2)+delPt*delPt/(0.1*0.1)) < delRPt) {
									delRPt = sqrt(delR*delR/(0.2*0.2)+delPt*delPt/(0.1*0.1));
									
									mindelR = delR;
									mindelPt = delPt;
									
									v = p.p;
									
									selPhoton = p;
									selGenParticle = genp;
							}
					}
					
				}
				if(	matched &&
					sqrt(delR*delR/(0.2*0.2)+delPt*delPt/(0.1*0.1)) < 1.){
						h2name = "gen_e_gamma_PtdelR_cut";
						h2[h2name].Fill(	mindelPt,
											mindelR
											);
						
				}
			}
			
		}
		Nnum++;
	} // //////////////////////////////////////////////////////////////////////////////////////////
	
	cout << endl;
	
	cout << "Looped events: " << Nnum << endl;
	cout << "isSimu: " << isSimu << endl;
	for(int ll=0;ll<100;ll++){
		cout << "nCounts[" << ll << "]: " << nCounts[ll] << "\t";
	}
	
}

void f_diphoton_fakerate(map<string,TH1F> &h, map<string,TH2F> &h2, string dataset){
	cout << "f_diphoton_fakerate()" << endl;
	cout << dataset << endl;
	
	// INITIALIZATION
	string s = "v1";
	string hname, h2name;
	
	hname = "genElectron_pt";
	h[hname] = TH1F((hname + "_" + s).c_str(), ";p_T (GeV); Events / bin",250,0,250);
	
	hname = "objPhotonPixel_pt";
	h[hname] = TH1F((hname + "_" + s).c_str(), ";p_T (GeV); Events / bin",250,0,250);
	
	hname = "trigger0";
	h[hname] = TH1F((hname + "_" + s).c_str(), ";m (GeV); Events / bin",250,0,250);
	hname = "trigger0_ee";
	h[hname] = TH1F((hname + "_" + s).c_str(), ";m (GeV); Events / bin",250,0,250);	
	hname = "trigger0_eg";
	h[hname] = TH1F((hname + "_" + s).c_str(), ";m (GeV); Events / bin",250,0,250);	
	
	hname = "trigger1";
	h[hname] = TH1F((hname + "_" + s).c_str(), ";m (GeV); Events / bin",250,0,250);
	hname = "trigger1_ee";
	h[hname] = TH1F((hname + "_" + s).c_str(), ";m (GeV); Events / bin",250,0,250);
	hname = "trigger1_eg";
	h[hname] = TH1F((hname + "_" + s).c_str(), ";m (GeV); Events / bin",250,0,250);
	
	hname = "trigger2";
	h[hname] = TH1F((hname + "_" + s).c_str(), ";m (GeV); Events / bin",250,0,250);
	hname = "trigger2_ee";
	h[hname] = TH1F((hname + "_" + s).c_str(), ";m (GeV); Events / bin",250,0,250);
	hname = "trigger2_eg";
	h[hname] = TH1F((hname + "_" + s).c_str(), ";m (GeV); Events / bin",250,0,250);
	
	hname = "trigger3";
	h[hname] = TH1F((hname + "_" + s).c_str(), ";m (GeV); Events / bin",250,0,250);
	hname = "trigger3_ee";
	h[hname] = TH1F((hname + "_" + s).c_str(), ";m (GeV); Events / bin",250,0,250);
	hname = "trigger3_eg";
	h[hname] = TH1F((hname + "_" + s).c_str(), ";m (GeV); Events / bin",250,0,250);
	
	hname = "triggers_eg_sum";
	h[hname] = TH1F((hname + "_" + s).c_str(), ";m (GeV); Events / bin",250,0,250);
	
	
	// **********************************************************************
	// fakerate_genmatch()
	//hname = "f_genmatch_tot";
	//h[hname] = "
	
	// */
	
	/*
	h2name = "f_zpeak_tot_eta";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(),";m [GeV];|#eta|",
						200,0,200,
						60,0,6);
	// */
	
	// DATA FILE
	TFile *f = new TFile(dataset.c_str());
	TTreeReader reader("TreeWriter/eventTree", f);
	
	// initialize branches
	TTreeReaderArray<tree::Photon> photons(reader, "photons");
	TTreeReaderArray<tree::Jet> jets(reader, "jets");
	TTreeReaderArray<tree::Electron> electrons(reader, "electrons");
	TTreeReaderArray<tree::Muon> muons(reader, "muons");
	TTreeReaderArray<tree::Particle> genJets(reader, "genJets");
	TTreeReaderArray<tree::GenParticle> genParticles(reader, "genParticles");
	TTreeReaderValue<Float_t> pu_weight(reader, "pu_weight");
	TTreeReaderValue<Char_t> mc_weight(reader, "mc_weight");
	
	TTreeReaderValue<Bool_t> Trigger( reader, 
		"HLT_Photon36_R9Id85_OR_CaloId24b40e_Iso50T80L_Photon22_AND_HE10_R9Id65_Eta2_Mass15_v");
	
	TTreeReaderValue<Bool_t> TriggerDiphoton_Primary( reader, 
		"HLT_Diphoton30_18_R9Id_OR_IsoCaloId_AND_HE_R9Id_Mass95_v");
	TTreeReaderValue<Bool_t> TriggerDiphoton_Contr01( reader,
		"HLT_Diphoton30_18_R9Id_OR_IsoCaloId_AND_HE_R9Id_DoublePixelSeedMatch_Mass70_v");
	TTreeReaderValue<Bool_t> TriggerDiphoton_Contr02( reader,
		"HLT_Diphoton30EB_18EB_R9Id_OR_IsoCaloId_AND_HE_R9Id_DoublePixelVeto_Mass55_v");
	TTreeReaderValue<Bool_t> TriggerDiphoton_Contr03( reader,
		"HLT_Diphoton30PV_18PV_R9Id_AND_IsoCaloId_AND_HE_R9Id_DoublePixelVeto_Mass55_v");
	
	TTreeReaderValue<Int_t> nGoodVertices(reader, "nGoodVertices");
	
	
	Float_t selW = 1.;
	
	// initialize variables
	int		Nnum = 0,
			nTest = 0,
			nPhotons = 0;
	
	int		nCounts[100] = {};
	
	float	fTest = 0.0;
	
	int ph=0, ph2=0;
	
	TVector3 	v;			// array of vectors
	TLorentzVector	lv;	// array of lorentz vectors
	float E = 0.;
	v.SetXYZ(0.,0.,0.);
	lv.SetPxPyPzE(0.,0.,0.,0.);
	
	vector<tree::Photon*> vp;
	//tp.hOverE = 3.5;
	
	///////////////////////////////////////////////////////////////////////////////////////////////
	// loop the events
	while(reader.Next()){
		
		//if(Nnum > 100000) continue;
		
		E = 0.;
		v.SetXYZ(0.,0.,0.);
		lv.SetPxPyPzE(0.,0.,0.,0.);
		
		if(*TriggerDiphoton_Primary) nCounts[0]++;
		if(*TriggerDiphoton_Contr01) nCounts[1]++;
		if(*TriggerDiphoton_Contr02) nCounts[2]++;
		if(*TriggerDiphoton_Contr03) nCounts[3]++;
		
		
		// ******************* tag and probe test:
		
		//hname = "trigger0"
		if(photons.GetSize() == 2){
			nCounts[5]++;
			for(auto& p: photons){
				v = v + p.p;
				E += p.p.Mag();
			}
			lv.SetVect(v);
			lv.SetE(E);
			
			
			if(*TriggerDiphoton_Primary){
				hname="trigger0";
				h[hname].Fill(lv.M());
			}
			if(*TriggerDiphoton_Contr01){
				hname="trigger1";
				h[hname].Fill(lv.M());
			}
			if(*TriggerDiphoton_Contr02){
				hname="trigger2";
				h[hname].Fill(lv.M());
			}
			if(*TriggerDiphoton_Contr03){
				hname="trigger3";
				h[hname].Fill(lv.M());
			}
			
			//tag 1
			if(	photons[ph].isMedium &&
				photons[ph].r9 < 1.0 &&
				photons[ph].sigmaIetaIeta < 0.0102 &&
				photons[ph].hOverE < 0.05 &&
				photons[ph].p.Pt() > 40 && 
				photons[ph].hasPixelSeed && 
				fabs(photons[ph].p.Eta()) < 1.4442){
					// probe
					ph = 1;
					
					if(ph==0) ph2 = 1;
					if(ph==1) ph2 = 0;
					
					if(	photons[ph].isMedium &&
						photons[ph].r9 < 1.0 &&
						photons[ph].sigmaIetaIeta < 0.0102 &&
						photons[ph].hOverE < 0.05 &&
						photons[ph].p.Pt() > 40 && 
						fabs(photons[ph].p.Eta()) < 1.4442){
							
							if(photons[ph].hasPixelSeed){
								// ee
								if(*TriggerDiphoton_Primary){
									nCounts[10]++;
									hname="trigger0_ee";
									h[hname].Fill(lv.M());
								}
								if(*TriggerDiphoton_Contr01){
									nCounts[11]++;
									hname="trigger1_ee";
									h[hname].Fill(lv.M());
								}
								if(*TriggerDiphoton_Contr02){
									nCounts[12]++;
									hname="trigger2_ee";
									h[hname].Fill(lv.M());
								}
								if(*TriggerDiphoton_Contr03){
									nCounts[13]++;
									hname="trigger3_ee";
									h[hname].Fill(lv.M());
								}
							}
							
							if(!photons[ph].hasPixelSeed){
								// eg
								if(*TriggerDiphoton_Primary){
									nCounts[15]++;
									hname="trigger0_eg";
									h[hname].Fill(lv.M());
								}
								if(*TriggerDiphoton_Contr01){
									nCounts[16]++;
									hname="trigger1_eg";
									h[hname].Fill(lv.M());
									hname = "triggers_eg_sum";
									h[hname].Fill(lv.M());
								}
								if(*TriggerDiphoton_Contr02){
									nCounts[17]++;
									hname="trigger2_eg";
									h[hname].Fill(lv.M());
									hname = "triggers_eg_sum";
									h[hname].Fill(lv.M());
								}
								if(*TriggerDiphoton_Contr03){
									nCounts[18]++;
									hname="trigger3_eg";
									h[hname].Fill(lv.M());
									hname = "triggers_eg_sum";
									h[hname].Fill(lv.M());
								}
								if(	*TriggerDiphoton_Contr01 ||
									*TriggerDiphoton_Contr02 ||
									*TriggerDiphoton_Contr03){
										//hname = "triggers_eg_sum";
										//h[hname].Fill(lv.M());
									}
							}
							
					}
			} // tag and probe 1
			ph = 1;
			// tag 2
			if(	photons[ph].isMedium &&
				photons[ph].r9 < 1.0 &&
				photons[ph].sigmaIetaIeta < 0.0102 &&
				photons[ph].hOverE < 0.05 &&
				photons[ph].p.Pt() > 40 && 
				photons[ph].hasPixelSeed && 
				fabs(photons[ph].p.Eta()) < 1.4442){
					// probe
					ph = 0;
					
					if(ph==0) ph2 = 1;
					if(ph==1) ph2 = 0;
					
					if(	photons[ph].isMedium &&
						photons[ph].r9 < 1.0 &&
						photons[ph].sigmaIetaIeta < 0.0102 &&
						photons[ph].hOverE < 0.05 &&
						photons[ph].p.Pt() > 40 && 
						fabs(photons[ph].p.Eta()) < 1.4442){
							
							if(photons[ph].hasPixelSeed){
								// ee
								if(*TriggerDiphoton_Primary){
									nCounts[10]++;
									hname="trigger0_ee";
									h[hname].Fill(lv.M());
								}
								if(*TriggerDiphoton_Contr01){
									nCounts[11]++;
									hname="trigger1_ee";
									h[hname].Fill(lv.M());
								}
								if(*TriggerDiphoton_Contr02){
									nCounts[12]++;
									hname="trigger2_ee";
									h[hname].Fill(lv.M());
								}
								if(*TriggerDiphoton_Contr03){
									nCounts[13]++;
									hname="trigger3_ee";
									h[hname].Fill(lv.M());
								}
							}
							
							if(!photons[ph].hasPixelSeed){
								// eg
								if(*TriggerDiphoton_Primary){
									nCounts[15]++;
									hname="trigger0_eg";
									h[hname].Fill(lv.M());
								}
								if(*TriggerDiphoton_Contr01){
									nCounts[16]++;
									hname="trigger1_eg";
									h[hname].Fill(lv.M());
									hname = "triggers_eg_sum";
									h[hname].Fill(lv.M());
								}
								if(*TriggerDiphoton_Contr02){
									nCounts[17]++;
									hname="trigger2_eg";
									h[hname].Fill(lv.M());
									hname = "triggers_eg_sum";
									h[hname].Fill(lv.M());
								}
								if(*TriggerDiphoton_Contr03){
									nCounts[18]++;
									hname="trigger3_eg";
									h[hname].Fill(lv.M());
									hname = "triggers_eg_sum";
									h[hname].Fill(lv.M());
								}
								if(	*TriggerDiphoton_Contr01 ||
									*TriggerDiphoton_Contr02 ||
									*TriggerDiphoton_Contr03){
										//hname = "triggers_eg_sum";
									//	h[hname].Fill(lv.M());
									}
							}
							
					}
			} // tag and probe 2
			
			
			
		}
		
		
		
		
		
		
		/*
		for(auto & gp: genParticles){
			
			if(abs(gp.pdgId) == 11){
				hname = hname = "genElectron_pt";
				h[hname].Fill(gp.p.Pt());
				
			}
		}
		
		for(auto & p: photons){
			nPhotons++;
			
			if(p.hasPixelSeed){
				hname = "objPhotonPixel_pt";
				h[hname].Fill(p.p.Pt());
			}
		}
		*/
		
		Nnum++;
	} // //////////////////////////////////////////////////////////////////////////////////////////
	
	
	cout << "Looped events: " << Nnum << endl;
	cout << "nTest: " << nTest << endl;
	cout << "fTest: " << fTest << endl;
	
	cout << "total Photons: " << nPhotons << endl;
	
	for(int ll=0;ll<100;ll++){
		cout << "nCounts[" << ll << "]: " << nCounts[ll] << "\t";
	}
	
	
	//cout << vp.size() << endl;
	
}

void f_diphoton_singleelectron(map<string,TH1F> &h, map<string,TH2F> &h2, string dataset){
	cout << "f_diphoton_fakerate()" << endl;
	cout << dataset << endl;
	
	// INITIALIZATION
	float m_z = 91.1876; // Z mass in GeV
	
	string s = "v1";
	string hname, h2name;
	
	hname = "genElectron_pt";
	h[hname] = TH1F((hname + "_" + s).c_str(), ";p_T (GeV); Events / bin",250,0,250);
	
	hname = "objPhotonPixel_pt";
	h[hname] = TH1F((hname + "_" + s).c_str(), ";p_T (GeV); Events / bin",250,0,250);
	
	hname = "trigger0";
	h[hname] = TH1F((hname + "_" + s).c_str(), ";m (GeV); Events / bin",250,0,250);
	hname = "trigger0_ee";
	h[hname] = TH1F((hname + "_" + s).c_str(), ";m (GeV); Events / bin",250,0,250);	
	hname = "trigger0_eg";
	h[hname] = TH1F((hname + "_" + s).c_str(), ";m (GeV); Events / bin",250,0,250);	
	
	hname = "trigger1";
	h[hname] = TH1F((hname + "_" + s).c_str(), ";m (GeV); Events / bin",250,0,250);
	hname = "trigger1_ee";
	h[hname] = TH1F((hname + "_" + s).c_str(), ";m (GeV); Events / bin",250,0,250);
	hname = "trigger1_eg";
	h[hname] = TH1F((hname + "_" + s).c_str(), ";m (GeV); Events / bin",250,0,250);
	
	hname = "trigger2";
	h[hname] = TH1F((hname + "_" + s).c_str(), ";m (GeV); Events / bin",250,0,250);
	hname = "trigger2_ee";
	h[hname] = TH1F((hname + "_" + s).c_str(), ";m (GeV); Events / bin",250,0,250);
	hname = "trigger2_eg";
	h[hname] = TH1F((hname + "_" + s).c_str(), ";m (GeV); Events / bin",250,0,250);
	
	hname = "trigger3";
	h[hname] = TH1F((hname + "_" + s).c_str(), ";m (GeV); Events / bin",250,0,250);
	hname = "trigger3_ee";
	h[hname] = TH1F((hname + "_" + s).c_str(), ";m (GeV); Events / bin",250,0,250);
	hname = "trigger3_eg";
	h[hname] = TH1F((hname + "_" + s).c_str(), ";m (GeV); Events / bin",250,0,250);
	
	hname = "triggers_eg_sum";
	h[hname] = TH1F((hname + "_" + s).c_str(), ";m (GeV); Events / bin",250,0,250);
	
	
	
	
	
	h2name = "tnp_gen_ee";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(),
				";m (GeV);p_{T} (GeV)",
				40, 81, 101, // 0.5 GeV bins
				250, 0, 250);
	h2name = "tnp_gen_eg";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(),
				";m (GeV);p_{T} (GeV)",
				40, 81, 101, // 0.5 GeV bins
				250, 0, 250);
	h2name = "tnp_ee";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(),
				";m (GeV);p_{T} (GeV)",
				40, 81, 101, // 0.5 GeV bins
				250, 0, 250);
	h2name = "tnp_eg";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(),
				";m (GeV);p_{T} (GeV)",
				40, 81, 101, // 0.5 GeV bins
				250, 0, 250);
	
	
	h2name = "tnp_gen_ee_ntracks";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(),
				";m (GeV);N_{track}",
				40, 81, 101, // 0.5 GeV bins
				200, 0, 200);
	h2name = "tnp_gen_eg_ntracks";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(),
				";m (GeV);N_{track}",
				40, 81, 101, // 0.5 GeV bins
				200, 0, 200);
	h2name = "tnp_ee_ntracks";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(),
				";m (GeV);N_{track}",
				40, 81, 101, // 0.5 GeV bins
				200, 0, 200);
	h2name = "tnp_eg_ntracks";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(),
				";m (GeV);N_{track}",
				40, 81, 101, // 0.5 GeV bins
				200, 0, 200);
	
	h2name = "tnp_gen_ee_nvtx";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(),
				";m (GeV);N_{vertex}",
				40, 81, 101, // 0.5 GeV bins
				30, 0, 30);
	h2name = "tnp_gen_eg_nvtx";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(),
				";m (GeV);N_{vertex}",
				40, 81, 101, // 0.5 GeV bins
				30, 0, 30);
	h2name = "tnp_ee_nvtx";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(),
				";m (GeV);N_{vertex}",
				40, 81, 101, // 0.5 GeV bins
				30, 0, 30);
	h2name = "tnp_eg_nvtx";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(),
				";m (GeV);N_{vertex}",
				40, 81, 101, // 0.5 GeV bins
				30, 0, 30);
	
	h2name = "tnp_gen_ee_eta";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(),
				";m (GeV);|#eta|",
				40, 81, 101, // 0.5 GeV bins
				150, 0, 1.5);
	h2name = "tnp_gen_eg_eta";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(),
				";m (GeV);|#eta|",
				40, 81, 101, // 0.5 GeV bins
				150, 0, 1.5);
	h2name = "tnp_ee_eta";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(),
				";m (GeV);|#eta|",
				40, 81, 101, // 0.5 GeV bins
				150, 0, 1.5);
	h2name = "tnp_eg_eta";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(),
				";m (GeV);|#eta|",
				40, 81, 101, // 0.5 GeV bins
				150, 0, 1.5);
	
	
	// **********************************************************************
	// fakerate_genmatch()
	//hname = "f_genmatch_tot";
	//h[hname] = "
	
	// */
	
	/*
	h2name = "f_zpeak_tot_eta";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(),";m [GeV];|#eta|",
						200,0,200,
						60,0,6);
	// */
	
	// DATA FILE
	TFile *f = new TFile(dataset.c_str());
	TTreeReader reader("TreeWriter/eventTree", f);
	
	// initialize branches
	TTreeReaderArray<tree::Photon> photons(reader, "photons");
	TTreeReaderArray<tree::Jet> jets(reader, "jets");
	TTreeReaderArray<tree::Electron> electrons(reader, "electrons");
	TTreeReaderArray<tree::Muon> muons(reader, "muons");
	TTreeReaderArray<tree::Particle> genJets(reader, "genJets");
	TTreeReaderArray<tree::GenParticle> genParticles(reader, "genParticles");
	TTreeReaderValue<Float_t> pu_weight(reader, "pu_weight");
	TTreeReaderValue<Char_t> mc_weight(reader, "mc_weight");
	
	TTreeReaderValue<Bool_t> Trigger( reader, 
		"HLT_Photon36_R9Id85_OR_CaloId24b40e_Iso50T80L_Photon22_AND_HE10_R9Id65_Eta2_Mass15_v");
	
	TTreeReaderValue<Bool_t> SingleElectron_1 ( reader, 
		"HLT_Ele23_WP75_Gsf_v");
	TTreeReaderValue<Bool_t> SingleElectron_2 ( reader,
		"HLT_Ele22_eta2p1_WP75_Gsf_v");
	TTreeReaderValue<Bool_t> SingleElectron_3 ( reader,
		"HLT_Ele22_eta2p1_WPLoose_Gsf_v");
	TTreeReaderValue<Bool_t> SingleElectron_4 ( reader,
		"HLT_Ele27_eta2p1_WPLoose_Gsf_v");
	
	TTreeReaderValue<Int_t> nGoodVertices(reader, "nGoodVertices");
	TTreeReaderValue<Int_t> nTracksPV(reader, "nTracksPV");
	
	
	
	Float_t selW = 1.;
	
	// initialize variables
	int		Nnum = 0,
			nTest = 0,
			nPhotons = 0;
	
	int		nCounts[100] = {};
	
	float	fTest = 0.0;
	
	int ph=0, ph2=0;
	
	TVector3 	v;			// array of vectors
	TLorentzVector	lv;	// array of lorentz vectors
	float E = 0.;
	v.SetXYZ(0.,0.,0.);
	lv.SetPxPyPzE(0.,0.,0.,0.);
	
	vector<tree::Photon*> vp;
	//tp.hOverE = 3.5;
	
	///////////////////////////////////////////////////////////////////////////////////////////////
	// loop the events
	while(reader.Next()){
		
		// counter output
		if((Nnum % 100000) == 0){
			cout << "\revent " << Nnum << " done... \t" << flush;
		}
		
		E = 0.;
		v.SetXYZ(0.,0.,0.);
		lv.SetPxPyPzE(0.,0.,0.,0.);
		
		if(*SingleElectron_1) nCounts[0]++;
		if(*SingleElectron_2) nCounts[1]++;
		if(*SingleElectron_3) nCounts[2]++;
		if(*SingleElectron_4) nCounts[3]++;
		
		
		// ******************* tag and probe test:
		
		//hname = "trigger0"
		if(photons.GetSize() == 2){
			nCounts[5]++;
			for(auto& p: photons){
				v = v + p.p;
				E += p.p.Mag();
			}
			lv.SetVect(v);
			lv.SetE(E);
			
			
			if(*SingleElectron_1){
				hname="trigger0";
				h[hname].Fill(lv.M());
			}
			if(*SingleElectron_2){
				hname="trigger1";
				h[hname].Fill(lv.M());
			}
			if(*SingleElectron_3){
				hname="trigger2";
				h[hname].Fill(lv.M());
			}
			if(*SingleElectron_4){
				hname="trigger3";
				h[hname].Fill(lv.M());
			}
			
			//tag 1
			if(	photons[ph].isMedium &&
				photons[ph].r9 < 1.0 &&
				photons[ph].sigmaIetaIeta < 0.0102 &&
				photons[ph].hOverE < 0.05 &&
				photons[ph].p.Pt() > 40 && 
				photons[ph].hasPixelSeed && 
				fabs(photons[ph].p.Eta()) < 1.4442){
					// probe
					ph = 1;
					
					if(ph==0) ph2 = 1;
					if(ph==1) ph2 = 0;
					
					if(	photons[ph].isMedium &&
						photons[ph].r9 < 1.0 &&
						photons[ph].sigmaIetaIeta < 0.0102 &&
						photons[ph].hOverE < 0.05 &&
						photons[ph].p.Pt() > 40 && 
						fabs(photons[ph].p.Eta()) < 1.4442){
							
							if(photons[ph].hasPixelSeed){
								// ee
								if(*SingleElectron_1){
									nCounts[10]++;
									hname="trigger0_ee";
									h[hname].Fill(lv.M());
								}
								if(*SingleElectron_2){
									nCounts[11]++;
									hname="trigger1_ee";
									h[hname].Fill(lv.M());
								}
								if(*SingleElectron_3){
									nCounts[12]++;
									hname="trigger2_ee";
									h[hname].Fill(lv.M());
								}
								if(*SingleElectron_4){
									nCounts[13]++;
									hname="trigger3_ee";
									h[hname].Fill(lv.M());
									
									if(fabs(lv.M()-m_z)/m_z < 0.1) nCounts[40]++;
									if(fabs(lv.M()-m_z)/m_z < 0.3) nCounts[44]++;
									
									h2name = "tnp_ee";
									h2[h2name].Fill(lv.M(), photons[ph].p.Pt());
									
									h2name = "tnp_ee_ntracks";
									h2[h2name].Fill(lv.M(), *nTracksPV);
									h2name = "tnp_ee_nvtx";
									h2[h2name].Fill(lv.M(), *nGoodVertices);
									h2name = "tnp_ee_eta";
									h2[h2name].Fill(lv.M(), fabs(photons[ph].p.Eta()));
									
								}
							}
							
							if(!photons[ph].hasPixelSeed){
								// eg
								if(*SingleElectron_1){
									nCounts[15]++;
									hname="trigger0_eg";
									h[hname].Fill(lv.M());
								}
								if(*SingleElectron_2){
									nCounts[16]++;
									hname="trigger1_eg";
									h[hname].Fill(lv.M());
									hname = "triggers_eg_sum";
									h[hname].Fill(lv.M());
								}
								if(*SingleElectron_3){
									nCounts[17]++;
									hname="trigger2_eg";
									h[hname].Fill(lv.M());
									hname = "triggers_eg_sum";
									h[hname].Fill(lv.M());
								}
								if(*SingleElectron_4){
									nCounts[18]++;
									hname="trigger3_eg";
									h[hname].Fill(lv.M());
									hname = "triggers_eg_sum";
									h[hname].Fill(lv.M());
									
									if(fabs(lv.M()-m_z)/m_z < 0.1) nCounts[41]++;
									if(fabs(lv.M()-m_z)/m_z < 0.3) nCounts[45]++;
									
									h2name = "tnp_eg";
									h2[h2name].Fill(lv.M(), photons[ph].p.Pt());
									
									h2name = "tnp_eg_ntracks";
									h2[h2name].Fill(lv.M(), *nTracksPV);
									h2name = "tnp_eg_nvtx";
									h2[h2name].Fill(lv.M(), *nGoodVertices);
									h2name = "tnp_eg_eta";
									h2[h2name].Fill(lv.M(), fabs(photons[ph].p.Eta()));
									
								}
							}
							
							
							
					}
			} // tag and probe 1
			ph = 1;
			// tag 2
			if(	photons[ph].isMedium &&
				photons[ph].r9 < 1.0 &&
				photons[ph].sigmaIetaIeta < 0.0102 &&
				photons[ph].hOverE < 0.05 &&
				photons[ph].p.Pt() > 40 && 
				photons[ph].hasPixelSeed && 
				fabs(photons[ph].p.Eta()) < 1.4442){
					// probe
					ph = 0;
					
					if(ph==0) ph2 = 1;
					if(ph==1) ph2 = 0;
					
					if(	photons[ph].isMedium &&
						photons[ph].r9 < 1.0 &&
						photons[ph].sigmaIetaIeta < 0.0102 &&
						photons[ph].hOverE < 0.05 &&
						photons[ph].p.Pt() > 40 && 
						fabs(photons[ph].p.Eta()) < 1.4442){
							
							if(photons[ph].hasPixelSeed){
								// ee
								if(*SingleElectron_1){
									nCounts[10]++;
									hname="trigger0_ee";
									h[hname].Fill(lv.M());
								}
								if(*SingleElectron_2){
									nCounts[11]++;
									hname="trigger1_ee";
									h[hname].Fill(lv.M());
								}
								if(*SingleElectron_3){
									nCounts[12]++;
									hname="trigger2_ee";
									h[hname].Fill(lv.M());
								}
								if(*SingleElectron_4){
									nCounts[13]++;
									hname="trigger3_ee";
									h[hname].Fill(lv.M());
									
									if(fabs(lv.M()-m_z)/m_z < 0.1) nCounts[40]++;
									if(fabs(lv.M()-m_z)/m_z < 0.3) nCounts[44]++;
									
									h2name = "tnp_ee";
									h2[h2name].Fill(lv.M(), photons[ph].p.Pt());
									
									h2name = "tnp_ee_ntracks";
									h2[h2name].Fill(lv.M(), *nTracksPV);
									h2name = "tnp_ee_nvtx";
									h2[h2name].Fill(lv.M(), *nGoodVertices);
									h2name = "tnp_ee_eta";
									h2[h2name].Fill(lv.M(), fabs(photons[ph].p.Eta()));
									
								}
							}
							
							if(!photons[ph].hasPixelSeed){
								// eg
								if(*SingleElectron_1){
									nCounts[15]++;
									hname="trigger0_eg";
									h[hname].Fill(lv.M());
								}
								if(*SingleElectron_2){
									nCounts[16]++;
									hname="trigger1_eg";
									h[hname].Fill(lv.M());
									hname = "triggers_eg_sum";
									h[hname].Fill(lv.M());
								}
								if(*SingleElectron_3){
									nCounts[17]++;
									hname="trigger2_eg";
									h[hname].Fill(lv.M());
									hname = "triggers_eg_sum";
									h[hname].Fill(lv.M());
								}
								if(*SingleElectron_4){
									nCounts[18]++;
									hname="trigger3_eg";
									h[hname].Fill(lv.M());
									hname = "triggers_eg_sum";
									h[hname].Fill(lv.M());
									
									if(fabs(lv.M()-m_z)/m_z < 0.1) nCounts[41]++;
									if(fabs(lv.M()-m_z)/m_z < 0.3) nCounts[45]++;
									
									h2name = "tnp_eg";
									h2[h2name].Fill(lv.M(), photons[ph].p.Pt());
									
									h2name = "tnp_eg_ntracks";
									h2[h2name].Fill(lv.M(), *nTracksPV);
									h2name = "tnp_eg_nvtx";
									h2[h2name].Fill(lv.M(), *nGoodVertices);
									h2name = "tnp_eg_eta";
									h2[h2name].Fill(lv.M(), fabs(photons[ph].p.Eta()));
								}
							}
							
					}
			} // tag and probe 2
			
			
			
		} // photons.GetSize() == 2
		
		
		
		Nnum++;
	} // //////////////////////////////////////////////////////////////////////////////////////////
	
	cout << endl;
	cout << "Looped events: " << Nnum << endl;
	cout << "nTest: " << nTest << endl;
	cout << "fTest: " << fTest << endl;
	
	cout << "total Photons: " << nPhotons << endl;
	
	int breaker=0;
	string sc="";
	for(int ll=0;ll<100;ll++){
		cout << "nCounts[" << ll << "]: " << setfill('0') << setw(7) << nCounts[ll] << "\t";
		breaker++;
		if(breaker==4){ breaker = 0; cout << endl;}
	}
	
	
	//cout << vp.size() << endl;
	
}

void f_genmatch_fakerate_DY(map<string,TH1F> &h, map<string,TH2F> &h2, string dataset){
	
	cout << "f_genmatch_fakerate_DY()" << endl;
	cout << dataset << endl;
	
	// INITIALIZATION
	string s = "v1";
	string hname, h2name;
	
	hname = "f_pt_tot_1bin";
	h[hname] = TH1F((hname + "_" + s).c_str(), ";p_T (GeV); Events / bin",1,0,200);
	hname = "f_pt_pas_1bin";
	h[hname] = TH1F((hname + "_" + s).c_str(), ";p_T (GeV); Events / bin",1,0,200);
	
	hname = "f_pt_tot";
	h[hname] = TH1F((hname + "_" + s).c_str(), ";p_T (GeV); Events / bin",200,0,200);
	hname = "f_pt_pas";
	h[hname] = TH1F((hname + "_" + s).c_str(), ";p_T (GeV); Events / bin",200,0,200);
	
	hname = "f_nvtx_tot";
	h[hname] = TH1F((hname + "_" + s).c_str(), ";N_{vtx}; Events / bin",40,0,40);
	hname = "f_nvtx_pas";
	h[hname] = TH1F((hname + "_" + s).c_str(), ";N_{vtx}; Events / bin",40,0,40);
	
	hname = "f_njet_tot";
	h[hname] = TH1F((hname + "_" + s).c_str(), ";N_{jet}; Events / bin",40,0,40);
	hname = "f_njet_pas";
	h[hname] = TH1F((hname + "_" + s).c_str(), ";N_{jet}; Events / bin",40,0,40);
	
	hname = "f_eta_tot";
	h[hname] = TH1F((hname + "_" + s).c_str(), ";|#eta|; Events / bin",200,0,2);
	hname = "f_eta_pas";
	h[hname] = TH1F((hname + "_" + s).c_str(), ";|#eta|; Events / bin",200,0,2);
	
	hname = "f_pt_two_tot";
	h[hname] = TH1F((hname + "_" + s).c_str(), ";p_T (GeV); Events / bin",200,0,200);
	hname = "f_pt_two_pas";
	h[hname] = TH1F((hname + "_" + s).c_str(), ";p_T (GeV); Events / bin",200,0,200);
	
	hname = "f_pt_two_tot_1bin";
	h[hname] = TH1F((hname + "_" + s).c_str(), ";p_T (GeV); Events / bin",1,0,200);
	hname = "f_pt_two_pas_1bin";
	h[hname] = TH1F((hname + "_" + s).c_str(), ";p_T (GeV); Events / bin",1,0,200);
	
	// **********************************************************************
	
	// DATA FILE
	TFile *f = new TFile(dataset.c_str());
	TTreeReader reader("TreeWriter/eventTree", f);
	
	// initialize branches
	TTreeReaderArray<tree::Photon> photons(reader, "photons");
	TTreeReaderArray<tree::Jet> jets(reader, "jets");
	TTreeReaderArray<tree::Electron> electrons(reader, "electrons");
	TTreeReaderArray<tree::Muon> muons(reader, "muons");
	TTreeReaderArray<tree::Particle> genJets(reader, "genJets");
	TTreeReaderArray<tree::GenParticle> genParticles(reader, "genParticles");
	TTreeReaderValue<Float_t> pu_weight(reader, "pu_weight");
	TTreeReaderValue<Char_t> mc_weight(reader, "mc_weight");
	TTreeReaderValue<Int_t> nGoodVertices(reader, "nGoodVertices");
	
	Float_t selW = 1.;
	
	// initialize variables
	int		Nnum = 0;
	
	int		nCounts[100] = {};
	
	TVector3 	v;			// array of vectors
	TLorentzVector	lv;	// array of lorentz vectors
	float E = 0.;
	v.SetXYZ(0.,0.,0.);
	lv.SetPxPyPzE(0.,0.,0.,0.);
	
	bool gotoNextPhoton = false;
	
	float delR=0., delPt=0., delRPt=0.;
	
	///////////////////////////////////////////////////////////////////////////////////////////////
	// loop the events
	while(reader.Next()){
		
		//if(Nnum > 100000) continue;
		
		
		E = 0.;
		v.SetXYZ(0.,0.,0.);
		lv.SetPxPyPzE(0.,0.,0.,0.);
		
		if(photons.GetSize()==2){
			for(auto& p: photons){
				gotoNextPhoton = false;
				
				
				for(auto& genp: genParticles){
					if(abs(genp.pdgId) == 11 && !gotoNextPhoton){
						if(	genp.p.DeltaR(p.p)<0.2 &&
							p.p.Pt() > 40. &&
							fabs(p.p.Eta())<1.4442){
								// fill total:
								hname = "f_pt_two_tot";
								h[hname].Fill(p.p.Pt());
								
								hname = "f_pt_two_tot_1bin";
								h[hname].Fill(p.p.Pt());
								
								if(!p.hasPixelSeed){
									// fill passed:
									hname = "f_pt_two_pas";
									h[hname].Fill(p.p.Pt());
									
									hname = "f_pt_two_pas_1bin";
									h[hname].Fill(p.p.Pt());
								}
							}
					}
					//goto nextPhoton;
					gotoNextPhoton = true;
				} // genParticles loop
			} //photons loop
		}	
				
		for(auto& p: photons){
			gotoNextPhoton = false;
			for(auto& genp: genParticles){
				if(abs(genp.pdgId) == 11 && !gotoNextPhoton){
					if(	
						genp.p.DeltaR(p.p)<0.2 &&
						p.p.Pt() > 40. &&
						fabs(p.p.Eta())<1.4442){
							// fill total:
								hname = "f_pt_tot";
								h[hname].Fill(p.p.Pt());
								hname = "f_nvtx_tot";
								h[hname].Fill(*nGoodVertices);
								hname = "f_njet_tot";
								h[hname].Fill(jets.GetSize());
								hname = "f_eta_tot";
								h[hname].Fill(fabs(p.p.Eta()));
							
								hname = "f_pt_tot_1bin";
								h[hname].Fill(p.p.Pt());
							
							if(!p.hasPixelSeed){
								// fill passed:
								hname = "f_pt_pas";
								h[hname].Fill(p.p.Pt());
								hname = "f_nvtx_pas";
								h[hname].Fill(*nGoodVertices);
								hname = "f_njet_pas";
								h[hname].Fill(jets.GetSize());
								hname = "f_eta_pas";
								h[hname].Fill(fabs(p.p.Eta()));
								
								hname = "f_pt_pas_1bin";
								h[hname].Fill(p.p.Pt());
							
							}
						
						}
				}
				//goto nextPhoton;
				gotoNextPhoton = true;
			} // genParticles loop
		} //photons loop
		
/*		
		for(auto& genp: genParticles){
			if(abs(genp.pdgId) == 11){
				delR = 0.;
				delPt = 0.;
				delRPt = 0.;
				for(auto& p: photons){
					delR = genp.p.DeltaR(p.p);
					delPt = fabs(genp.p.Pt()-p.p.Pt())/genp.p.Pt();
					// if delRPt is smaller than for the last catch, take the new one
					if(delRPt == 0. || sqrt(delR*delR+delPt*delPt) < delRPt) {
						delRPt = sqrt(delR*delR+delPt*delPt);
						
						
					}
					
					
				
				}
				
			}
			
		}
*/
		
		Nnum++;
	} // //////////////////////////////////////////////////////////////////////////////////////////
	
	
	cout << "Looped events: " << Nnum << endl;
	
	for(int ll=0;ll<100;ll++){
		cout << "nCounts[" << ll << "]: " << nCounts[ll] << "\t";
	}
	
}

void f_tagnprobe_fakerate(map<string,TH1F> &h, map<string,TH2F> &h2, string dataset){
	
	cout << "f_tagnprobe_fakerate()" << endl;
	cout << dataset << endl;
	
	// INITIALIZATION
	string s = "v1";
	string hname, h2name;
	
	h2name = "f_zpeak_tot_eta";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(),";m [GeV];|#eta|",
						200,0,200,
						150,0.,1.5);
	//h2[h2name].Setw2();
	
	h2name = "f_zpeak_pas_eta";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(),";m [GeV];|#eta|",
						200,0,200,
						150,0,1.5);
	//h2[h2name].Setw2();
	
	h2name = "f_zpeak_tot_pt";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(),";m [GeV];pt [GeV]",
						200,0,200,
						250,0,250);
	//h2[h2name].Setw2();
	
	h2name = "f_zpeak_pas_pt";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(),";m [GeV];pt [GeV]",
						200,0,200,
						250,0,250);
	//h2[h2name].Setw2();
	
	h2name = "f_zpeak_tot_nvtx";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(),";m [GeV];N_{vtx}",
						200,0,200,
						40,0,40);
	//h2[h2name].Setw2();
	
	h2name = "f_zpeak_pas_nvtx";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(),";m [GeV];N_{vtx}",
						200,0,200,
						40,0,40);
	//h2[h2name].Setw2();
	
	h2name = "f_zpeak_tot_njet";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(),";m [GeV];N_{jet}",
						200,0,200,
						15,0,15);
	//h2[h2name].Setw2();
	
	h2name = "f_zpeak_pas_njet";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(),";m [GeV];N_{jet}",
						200,0,200,
						15,0,15);
	//h2[h2name].Setw2();
	
	// **********************************************************************
	
	// DATA FILE
	TFile *f = new TFile(dataset.c_str());
	TTreeReader reader("TreeWriter/eventTree", f);
	
	// initialize branches
	TTreeReaderArray<tree::Photon> photons(reader, "photons");
	TTreeReaderArray<tree::Jet> jets(reader, "jets");
	TTreeReaderArray<tree::Electron> electrons(reader, "electrons");
	TTreeReaderArray<tree::Muon> muons(reader, "muons");
	TTreeReaderArray<tree::Particle> genJets(reader, "genJets");
	TTreeReaderArray<tree::GenParticle> genParticles(reader, "genParticles");
	TTreeReaderValue<Float_t> pu_weight(reader, "pu_weight");
	TTreeReaderValue<Char_t> mc_weight(reader, "mc_weight");
	TTreeReaderValue<Int_t> nGoodVertices(reader, "nGoodVertices");
	
	
	
	TTreeReaderValue<Bool_t> TriggerR9Id85( reader, 
		"HLT_Photon36_R9Id85_OR_CaloId24b40e_Iso50T80L_Photon22_AND_HE10_R9Id65_Eta2_Mass15_v" );
	
	// Attention: isData is defined inverse!
	// -->	if mc	 set true
	// 		if data	 set false
	/*
	bool isData;
	isData = 	thisFileName.find("DYJetsToLL") != string::npos
			||	thisFileName.find("TTGJets") != string::npos
			||	thisFileName.find("WGToLNuG") != string::npos
			||	thisFileName.find("ZGTo2LG") != string::npos;
	*/
	//bool useTrigger = false;
	//if(isData) useTrigger = true;
	
	Float_t selW = 1.;
	
	// initialize variables
	int		Nnum = 0;
	
	int		nCounts[100] = {};
	
	int		ph=0, 
			ph2=0;
	
	TVector3 	v;			// array of vectors
	TLorentzVector	lv;	// array of lorentz vectors
	float E = 0.;
	v.SetXYZ(0.,0.,0.);
	lv.SetPxPyPzE(0.,0.,0.,0.);
	
	bool gotoNextPhoton = false;
	
	///////////////////////////////////////////////////////////////////////////////////////////////
	// loop the events
	while(reader.Next()){
		
		//if(Nnum > 100000) continue;
		
		
		E = 0.;
		v.SetXYZ(0.,0.,0.);
		lv.SetPxPyPzE(0.,0.,0.,0.);
		
		//if(*TriggerR9Id85 && photons.GetSize() ==2){
		if(photons.GetSize() ==2){
			// calculate invariant mass
			for(auto& p: photons){
				v = v + p.p;
				E += p.p.Mag();
			}
			
			lv.SetVect(v);
			lv.SetE(E);
			
			// tag
			ph=0;
			if(	photons[ph].p.Pt() > 40 && 
				photons[ph].hasPixelSeed && 
				fabs(photons[ph].p.Eta()) < 1.4442){
					// probe
					ph = 1;
					if(	photons[ph].p.Pt() > 40 && 
						fabs(photons[ph].p.Eta()) < 1.4442){
							// fake rate: FILL TOTAL
							h2name = "f_zpeak_tot_pt";
							h2[h2name].Fill(lv.M(), photons[ph].p.Pt(), selW);
							h2name = "f_zpeak_tot_nvtx";
							h2[h2name].Fill(lv.M(), *nGoodVertices, selW);
							h2name = "f_zpeak_tot_njet";
							h2[h2name].Fill(lv.M(), jets.GetSize(), selW);
							h2name = "f_zpeak_tot_eta";
							h2[h2name].Fill(lv.M(), fabs(photons[ph].p.Eta()), selW);
							
							if(!photons[ph].hasPixelSeed){
								// fake rate: FILL PASSED
								h2name = "f_zpeak_pas_pt";
								h2[h2name].Fill(lv.M(), photons[ph].p.Pt(), selW);
								h2name = "f_zpeak_pas_nvtx";
								h2[h2name].Fill(lv.M(), *nGoodVertices, selW);
								h2name = "f_zpeak_pas_njet";
								h2[h2name].Fill(lv.M(), jets.GetSize(), selW);
								h2name = "f_zpeak_pas_eta";
								h2[h2name].Fill(lv.M(), fabs(photons[ph].p.Eta()), selW);
							}
					}
			}
			
			ph = 1; //change photons
			//tag
			if(	photons[ph].p.Pt() > 40 && 
				photons[ph].hasPixelSeed && 
				fabs(photons[ph].p.Eta()) < 1.4442){
					// probe
					ph = 0;
					if(	photons[ph].p.Pt() > 40 && 
						fabs(photons[ph].p.Eta()) < 1.4442){
							// fake rate: FILL TOTAL
							h2name = "f_zpeak_tot_pt";
							h2[h2name].Fill(lv.M(), photons[ph].p.Pt(), selW);
							h2name = "f_zpeak_tot_nvtx";
							h2[h2name].Fill(lv.M(), *nGoodVertices, selW);
							h2name = "f_zpeak_tot_njet";
							h2[h2name].Fill(lv.M(), jets.GetSize(), selW);
							h2name = "f_zpeak_tot_eta";
							h2[h2name].Fill(lv.M(), fabs(photons[ph].p.Eta()), selW);
							
							if(!photons[ph].hasPixelSeed){
								// fake rate: FILL PASSED
								h2name = "f_zpeak_pas_pt";
								h2[h2name].Fill(lv.M(), photons[ph].p.Pt(), selW);
								h2name = "f_zpeak_pas_nvtx";
								h2[h2name].Fill(lv.M(), *nGoodVertices, selW);
								h2name = "f_zpeak_pas_njet";
								h2[h2name].Fill(lv.M(), jets.GetSize(), selW);
								h2name = "f_zpeak_pas_eta";
								h2[h2name].Fill(lv.M(), fabs(photons[ph].p.Eta()), selW);
							}
					}
			}
			
			
			
		} // if trigger and 2 photons
		
		
		Nnum++;
	} // //////////////////////////////////////////////////////////////////////////////////////////
	
	
	cout << "Looped events: " << Nnum << endl;
	
	for(int ll=0;ll<100;ll++){
		cout << "nCounts[" << ll << "]: " << nCounts[ll] << "\t";
	}
	
}

void f_genmatch_tagnprobe(map<string,TH1F> &h, map<string,TH2F> &h2, string dataset){
	cout << "f_genmatch_tagnprobe()" << endl;
	cout << dataset << endl;
	
	// INITIALIZATION
	float m_z = 91.1876; // Z mass in GeV
	
	string s = "v1";
	string hname, h2name;
	
	hname = "zpeak_tot";
	h[hname] = TH1F((hname+"_"+s).c_str(), ";m (GeV);Events / bin",400, 0, 200);
	hname = "zpeak_pas";
	h[hname] = TH1F((hname+"_"+s).c_str(), ";m (GeV);Events / bin",400, 0, 200);
	
	hname = "gen_all_tot";
	h[hname] = TH1F((hname+"_"+s).c_str(), ";p_{T} (GeV);Events / bin",500, 0, 250);
	hname = "gen_all_pas";
	h[hname] = TH1F((hname+"_"+s).c_str(), ";p_{T} (GeV);Events / bin",500, 0, 250);
	hname = "gen_two_tot";
	h[hname] = TH1F((hname+"_"+s).c_str(), ";p_{T} (GeV);Events / bin",500, 0, 250);
	hname = "gen_two_pas";
	h[hname] = TH1F((hname+"_"+s).c_str(), ";p_{T} (GeV);Events / bin",500, 0, 250);
	hname = "tnp_gen_tot";
	h[hname] = TH1F((hname+"_"+s).c_str(), ";p_{T} (GeV);Events / bin",500, 0, 250);
	hname = "tnp_gen_pas";
	h[hname] = TH1F((hname+"_"+s).c_str(), ";p_{T} (GeV);Events / bin",500, 0, 250);
	hname = "tnp_nogen_tot";
	h[hname] = TH1F((hname+"_"+s).c_str(), ";p_{T} (GeV);Events / bin",500, 0, 250);
	hname = "tnp_nogen_pas";
	h[hname] = TH1F((hname+"_"+s).c_str(), ";p_{T} (GeV);Events / bin",500, 0, 250);
	
	hname = "clos_gen_e";
	h[hname] = TH1F((hname+"_"+s).c_str(), ";p_{T} (GeV);Events / bin",250, 0, 250);
	hname = "clos_gen_g";
	h[hname] = TH1F((hname+"_"+s).c_str(), ";p_{T} (GeV);Events / bin",250, 0, 250);
	
	hname = "clos_gen_e_ntracks";
	h[hname] = TH1F((hname+"_"+s).c_str(), ";N_{track};Events / bin",200, 0, 200);
	hname = "clos_gen_g_ntracks";
	h[hname] = TH1F((hname+"_"+s).c_str(), ";N_{track};Events / bin",200, 0, 200);
	
	hname = "clos_gen_e_nvtx";
	h[hname] = TH1F((hname+"_"+s).c_str(), ";N_{vertex};Events / bin",30, 0, 30);
	hname = "clos_gen_g_nvtx";
	h[hname] = TH1F((hname+"_"+s).c_str(), ";N_{vertex};Events / bin",30, 0, 30);
	
	hname = "clos_gen_e_eta";
	h[hname] = TH1F((hname+"_"+s).c_str(), ";|#eta|;Events / bin",150, 0, 1.5);
	hname = "clos_gen_g_eta";
	h[hname] = TH1F((hname+"_"+s).c_str(), ";|#eta|;Events / bin",150, 0, 1.5);
	
	
	
	h2name = "tnp_gen_ee";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(),
				";m (GeV);p_{T} (GeV)",
				40, 81, 101, // 0.5 GeV bins
				250, 0, 250);
	h2name = "tnp_gen_eg";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(),
				";m (GeV);p_{T} (GeV)",
				40, 81, 101, // 0.5 GeV bins
				250, 0, 250);
	h2name = "tnp_ee";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(),
				";m (GeV);p_{T} (GeV)",
				40, 81, 101, // 0.5 GeV bins
				250, 0, 250);
	h2name = "tnp_eg";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(),
				";m (GeV);p_{T} (GeV)",
				40, 81, 101, // 0.5 GeV bins
				250, 0, 250);
	
	
	h2name = "tnp_gen_ee_ntracks";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(),
				";m (GeV);N_{track}",
				40, 81, 101, // 0.5 GeV bins
				200, 0, 200);
	h2name = "tnp_gen_eg_ntracks";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(),
				";m (GeV);N_{track}",
				40, 81, 101, // 0.5 GeV bins
				200, 0, 200);
	h2name = "tnp_ee_ntracks";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(),
				";m (GeV);N_{track}",
				40, 81, 101, // 0.5 GeV bins
				200, 0, 200);
	h2name = "tnp_eg_ntracks";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(),
				";m (GeV);N_{track}",
				40, 81, 101, // 0.5 GeV bins
				200, 0, 200);
	
	h2name = "tnp_gen_ee_nvtx";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(),
				";m (GeV);N_{vertex}",
				40, 81, 101, // 0.5 GeV bins
				30, 0, 30);
	h2name = "tnp_gen_eg_nvtx";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(),
				";m (GeV);N_{vertex}",
				40, 81, 101, // 0.5 GeV bins
				30, 0, 30);
	h2name = "tnp_ee_nvtx";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(),
				";m (GeV);N_{vertex}",
				40, 81, 101, // 0.5 GeV bins
				30, 0, 30);
	h2name = "tnp_eg_nvtx";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(),
				";m (GeV);N_{vertex}",
				40, 81, 101, // 0.5 GeV bins
				30, 0, 30);
	
	h2name = "tnp_gen_ee_eta";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(),
				";m (GeV);|#eta|",
				40, 81, 101, // 0.5 GeV bins
				150, 0, 1.5);
	h2name = "tnp_gen_eg_eta";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(),
				";m (GeV);|#eta|",
				40, 81, 101, // 0.5 GeV bins
				150, 0, 1.5);
	h2name = "tnp_ee_eta";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(),
				";m (GeV);|#eta|",
				40, 81, 101, // 0.5 GeV bins
				150, 0, 1.5);
	h2name = "tnp_eg_eta";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(),
				";m (GeV);|#eta|",
				40, 81, 101, // 0.5 GeV bins
				150, 0, 1.5);
	
/*
	h2name = "gen_e_gamma_PtdelR";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(), 
				";|p^{gen e}_{T} - p^{#gamma}_{T}|/p^{gen e}_{T}; #DeltaR(gen e, #gamma)",
				200, 0, 2,
				400, 0, 4);
	h2name = "gen_e_gamma_PtdelR_cut";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(), 
				";|p^{gen e}_{T} - p^{#gamma}_{T}|/p^{gen e}_{T}; #DeltaR(gen e, #gamma)",
				200, 0, 2,
				400, 0, 4);
*/
	
	
	// **********************************************************************
	
	// Attention: isData is defined inverse!
	// -->	if mc	 set true
	// 		if data	 set false
	// >>> renamded to isSimu
	
	bool isSimu;
	isSimu = 	dataset.find("DYJetsToLL") != string::npos
			||	dataset.find("TTGJets") != string::npos
			||	dataset.find("WGToLNuG") != string::npos
			||	dataset.find("ZGTo2LG") != string::npos;
	// */
	
	// DATA FILE
	TFile *f = new TFile(dataset.c_str());
	TTreeReader reader("TreeWriter/eventTree", f);
	
	// initialize branches
	TTreeReaderArray<tree::Photon> photons(reader, "photons");
	TTreeReaderArray<tree::Jet> jets(reader, "jets");
	TTreeReaderArray<tree::Electron> electrons(reader, "electrons");
	TTreeReaderArray<tree::Muon> muons(reader, "muons");
	TTreeReaderArray<tree::Particle> genJets(reader, "genJets");
	TTreeReaderArray<tree::GenParticle> genParticles(reader, "genParticles");
	TTreeReaderArray<tree::IntermediateGenParticle> intermediateGenParticles(
													reader, "intermediateGenParticles");
	TTreeReaderValue<Float_t> pu_weight(reader, "pu_weight");
	TTreeReaderValue<Char_t> mc_weight(reader, "mc_weight");
	TTreeReaderValue<Int_t> nGoodVertices(reader, "nGoodVertices");
	TTreeReaderValue<Int_t> nTracksPV(reader, "nTracksPV");
	
	
	Float_t selW = 1.;
	
	// initialize variables
	int		Nnum = 0;
	
	int		nCounts[100] = {};
	
	int		ph = 0;
	
	TVector3 	v;			// vector
	TLorentzVector	lv;		// lorent vectors
	float E = 0.;
	v.SetXYZ(0.,0.,0.);
	lv.SetPxPyPzE(0.,0.,0.,0.);
	
	
	
	bool gotoNextPhoton = false;
	
	bool matched = false;
	
	bool	p0 = false,
			p1 = false;
			
	bool	massMatch = false;
	
	float 	delR = 0., 
			delPt = 0., 
			delRPt = 0.,
			mindelR = 0.,
			mindelPt = 0.;
	
	float	sdelR = 0.2,
			sdelPt = 0.1;
	
	Photon selPhoton;
	GenParticle selGenParticle; 
	
	///////////////////////////////////////////////////////////////////////////////////////////////
	// loop the events
	while(reader.Next()){
		
		//if(Nnum > 100000) continue;
		
		// counter output
		if((Nnum % 100000) == 0){
			cout << "\revent " << Nnum << " done... \t" << flush;
		}
		
		E = 0.;
		v.SetXYZ(0.,0.,0.);
		lv.SetPxPyPzE(0.,0.,0.,0.);
		
		
		// GENERATOR MATCHING for all events
		for(auto& interGen: intermediateGenParticles){
			if(fabs(interGen.pdgId) == 23){ // Z boson
				nCounts[20]++;
				//cout << Nnum << "\t" << interGen.daughters.size() << endl;
				for(auto& d: interGen.daughters){
					if(abs(d.pdgId) == 11){ // electrons/positrons
						for(auto& p: photons){
							if(	p.p.Pt() > 40. &&
								fabs(p.p.Eta()) < 1.4442){
								
								delR = d.p.DeltaR(p.p);
								delPt = fabs(d.p.Pt()-p.p.Pt())/d.p.Pt();
								delRPt = 	sqrt(delR*delR/(sdelR*sdelR)+
											delPt*delPt/(sdelPt*sdelPt));
								if(	delR < 0.2 &&
									delPt < 0.1){
									//delRPt < 1.){
									//cout << p.hasPixelSeed << "\t";
									//cout << "delR: " << delR << "\t delPt: " << delPt;
									//cout << "\t delRPt: " << delRPt << endl;
									
									nCounts[21]++;
									
									
									
									// DENOMINATOR
									hname = "gen_all_tot";
									h[hname].Fill(p.p.Pt());
									
									// NUMERATOR
									if(!p.hasPixelSeed){
										nCounts[22]++;
										
										hname = "gen_all_pas";
										h[hname].Fill(p.p.Pt());
										
										hname = "clos_gen_g";
										h[hname].Fill(p.p.Pt());
										
										hname = "clos_gen_g_ntracks";
										h[hname].Fill(*nTracksPV);
										hname = "clos_gen_g_nvtx";
										h[hname].Fill(*nGoodVertices);
										hname = "clos_gen_g_eta";
										h[hname].Fill(fabs(p.p.Eta()));
									}
									
									if(p.hasPixelSeed){
										hname = "clos_gen_e";
										h[hname].Fill(p.p.Pt());
										
										hname = "clos_gen_e_ntracks";
										h[hname].Fill(*nTracksPV);
										hname = "clos_gen_e_nvtx";
										h[hname].Fill(*nGoodVertices);
										hname = "clos_gen_e_eta";
										h[hname].Fill(fabs(p.p.Eta()));
									}
									
								}
							}
						} //photon loop
					}
					//cout << "\t" << d.pdgId;
				}
				//getchar();
			}
			
		} // loop intermediate particles
		
		// DIPHOTON events:
		// compare genmatch and tagnprobe
		if(photons.GetSize() == 2){
			
			
			// calculate invariant mass
			nCounts[80] = 0;
			nCounts[81] = 0;
			for(auto& p: photons){
				v = v + p.p;
				E += p.p.Mag();
				
				
				if(	p.p.Pt() > 40 &&
					fabs(p.p.Eta()) < 1.4442) nCounts[80]++;
				if( p.hasPixelSeed) nCounts[81]++;
			}
			lv.SetVect(v);
			lv.SetE(E);
			
			nCounts[40] = 0; // match counter
			// GENERATOR MATCHING for diphoton events
			for(auto& interGen: intermediateGenParticles){
				if(fabs(interGen.pdgId) == 23){ // Z boson
					nCounts[0]++;
					//cout << Nnum << "\t" << interGen.daughters.size() << endl;
					for(auto& d: interGen.daughters){
						if(abs(d.pdgId) == 11){ // electrons/positrons
							for(auto& p: photons){
								
								if(	p.p.Pt() > 40. &&
									fabs(p.p.Eta()) < 1.4442){
								
									delR = d.p.DeltaR(p.p);
									delPt = fabs(d.p.Pt()-p.p.Pt())/d.p.Pt();
									delRPt = 	sqrt(delR*delR/(sdelR*sdelR)+
												delPt*delPt/(sdelPt*sdelPt));
									if(	delR < 0.2 &&
										delPt < 0.1){
										//delRPt < 1.){
										//cout << p.hasPixelSeed << "\t";
										//cout << "delR: " << delR << "\t delPt: " << delPt;
										//cout << "\t delRPt: " << delRPt << endl;
										
										nCounts[40]++; //match counter
										
										hname = "gen_two_tot";
										h[hname].Fill(p.p.Pt());
										
										// Ne:
										if(p.hasPixelSeed){
											nCounts[10]++;
											//cout << "gen e: " << nCounts[10];
										}
										
										// Ng:
										if(!p.hasPixelSeed){
											nCounts[11]++;
											//cout << "\t g: " << nCounts[11];
											
											hname = "gen_two_tot";
											h[hname].Fill(p.p.Pt());
										}
									}
								}
							}
						}
						//cout << "\t" << d.pdgId;
					}
					//getchar();
				}
			}
			if(nCounts[40]==2){
				//cout << "Nnum: " << Nnum << "\t" << fabs(lv.M()-m_z)/m_z;
				nCounts[42]++;
				if(fabs(lv.M()-m_z)/m_z < 0.1){
					nCounts[43]++;
					
					
					if(!photons[0].hasPixelSeed != !photons[1].hasPixelSeed){
						// Neg
						nCounts[46]++;
					}
					
					if(photons[0].hasPixelSeed && photons[1].hasPixelSeed){
						// Nee
						nCounts[47]++;
					}
				}
				//getchar();
			}
			
			// TAG N PROBE
			if(	//lv.M() > 80. &&
				//lv.M() < 100. &&
				fabs(lv.M()-m_z)/m_z < 0.1 &&
				nCounts[80] == 2 &&
				nCounts[81] >= 1 &&
				true){
				
				if(!photons[0].hasPixelSeed != !photons[1].hasPixelSeed){
					// Neg
					nCounts[70]++;
				}
				
				if(photons[0].hasPixelSeed && photons[1].hasPixelSeed){
					// Nee
					nCounts[71]++;
				}
				
				// generator matching cross check
				p0 = false;
				p1 = false;
				//cout << "Nnum: "<< Nnum << endl;
				for(auto& interGen: intermediateGenParticles){
					if(abs(interGen.pdgId) == 23){
						for(auto& genp: interGen.daughters){
							if(abs(genp.pdgId) == 11){
								
								if(	genp.p.DeltaR(photons[0].p) < 0.2 &&
									fabs(genp.p.Pt()-photons[0].p.Pt())/genp.p.Pt() < 0.1){
									
									p0 = true;
									
									/*
									cout << genp.p.DeltaR(photons[0].p);
									cout << "\t del[Pt()]=";
									cout << fabs(genp.p.Pt()-photons[0].p.Pt())/genp.p.Pt();
									cout << endl;
									*/
								}
								
								if(	genp.p.DeltaR(photons[1].p) < 0.2 &&
									fabs(genp.p.Pt()-photons[1].p.Pt())/genp.p.Pt() < 0.1){
									
									p1 = true;
									/*
									cout << genp.p.DeltaR(photons[1].p);
									cout << "\t del[Pt()]=";
									cout << fabs(genp.p.Pt()-photons[1].p.Pt())/genp.p.Pt();
									cout << endl;
									*/
								}
							
							}
							
						}
						
					}
				}
				if(p0 && p1){
					nCounts[59]++;
					if(!photons[0].hasPixelSeed != !photons[1].hasPixelSeed){
						// Neg
						nCounts[60]++;
						
						if(photons[0].hasPixelSeed){
							h2name = "tnp_gen_eg";
							h2[h2name].Fill(lv.M(), photons[1].p.Pt());
							
							h2name = "tnp_gen_eg_ntracks";
							h2[h2name].Fill(lv.M(), *nTracksPV);
							h2name = "tnp_gen_eg_nvtx";
							h2[h2name].Fill(lv.M(), *nGoodVertices);
							h2name = "tnp_gen_eg_eta";
							h2[h2name].Fill(lv.M(), fabs(photons[1].p.Eta()));
							
						}
						if(photons[1].hasPixelSeed){
							h2name = "tnp_gen_eg";
							h2[h2name].Fill(lv.M(), photons[0].p.Pt());
							
							h2name = "tnp_gen_eg_ntracks";
							h2[h2name].Fill(lv.M(), *nTracksPV);
							h2name = "tnp_gen_eg_nvtx";
							h2[h2name].Fill(lv.M(), *nGoodVertices);
							h2name = "tnp_gen_eg_eta";
							h2[h2name].Fill(lv.M(), fabs(photons[0].p.Eta()));
						}
						
					}
					
					if(photons[0].hasPixelSeed && photons[1].hasPixelSeed){
						// Nee
						nCounts[61]++;
						
						h2name = "tnp_gen_ee";
						h2[h2name].Fill(lv.M(), photons[0].p.Pt());
						h2[h2name].Fill(lv.M(), photons[1].p.Pt());
						
						h2name = "tnp_gen_ee_ntracks";
						h2[h2name].Fill(lv.M(), *nTracksPV);
						h2[h2name].Fill(lv.M(), *nTracksPV);
						h2name = "tnp_gen_ee_nvtx";
						h2[h2name].Fill(lv.M(), *nGoodVertices);
						h2[h2name].Fill(lv.M(), *nGoodVertices);
						h2name = "tnp_gen_ee_eta";
						h2[h2name].Fill(lv.M(), fabs(photons[0].p.Eta()));
						h2[h2name].Fill(lv.M(), fabs(photons[1].p.Eta()));
					}
					
					if(!photons[0].hasPixelSeed && !photons[1].hasPixelSeed){
						
						nCounts[62]++;
					}
					
				}
				//getchar();
			}
			
			
			if(	fabs(lv.M()-m_z)/m_z < 0.1 &&
				//lv.M() > 80. &&
				//lv.M() < 100. &&
				true){
				
				// tag
				ph = 0;
				if(	photons[ph].p.Pt() > 40 && 
					fabs(photons[ph].p.Eta()) < 1.4442 &&
					photons[ph].hasPixelSeed){
						// probe
						ph = 1;
						if(	photons[ph].p.Pt() > 40 && 
							fabs(photons[ph].p.Eta()) < 1.4442){
								
								hname = "zpeak_tot";
								h[hname].Fill(lv.M());
								
								hname = "tnp_nogen_tot";
								h[hname].Fill(photons[ph].p.Pt());
								
								// Nee
								if(photons[ph].hasPixelSeed){
									nCounts[14]++;
									
									h2name = "tnp_ee";
									h2[h2name].Fill(lv.M(), photons[ph].p.Pt());
									
									h2name = "tnp_ee_ntracks";
									h2[h2name].Fill(lv.M(), *nTracksPV);
									h2name = "tnp_ee_nvtx";
									h2[h2name].Fill(lv.M(), *nGoodVertices);
									h2name = "tnp_ee_eta";
									h2[h2name].Fill(lv.M(), fabs(photons[ph].p.Eta()));
								}
								
								if(!photons[ph].hasPixelSeed){
									// Neg
									nCounts[15]++;
									
									hname = "zpeak_pas";
									h[hname].Fill(lv.M());
									
									hname = "tnp_nogen_pas";
									h[hname].Fill(photons[ph].p.Pt());
									
									h2name = "tnp_eg";
									h2[h2name].Fill(lv.M(), photons[ph].p.Pt());
									
									h2name = "tnp_eg_ntracks";
									h2[h2name].Fill(lv.M(), *nTracksPV);
									h2name = "tnp_eg_nvtx";
									h2[h2name].Fill(lv.M(), *nGoodVertices);
									h2name = "tnp_eg_eta";
									h2[h2name].Fill(lv.M(), fabs(photons[ph].p.Eta()));
								
								}
						}
				}
				ph = 1; //change photons
				//tag
				if(	photons[ph].p.Pt() > 40 && 
					fabs(photons[ph].p.Eta()) < 1.4442 &&
					photons[ph].hasPixelSeed){
						// probe
						ph = 0;
						if(	photons[ph].p.Pt() > 40 && 
							fabs(photons[ph].p.Eta()) < 1.4442){
								
								hname = "zpeak_tot";
								h[hname].Fill(lv.M());
								
								hname = "tnp_nogen_tot";
								h[hname].Fill(photons[ph].p.Pt());
								
								// Nee
								if(photons[ph].hasPixelSeed){
									nCounts[14]++;
									
									h2name = "tnp_ee";
									h2[h2name].Fill(lv.M(), photons[ph].p.Pt());
									
									h2name = "tnp_ee_ntracks";
									h2[h2name].Fill(lv.M(), *nTracksPV);
									h2name = "tnp_ee_nvtx";
									h2[h2name].Fill(lv.M(), *nGoodVertices);
									h2name = "tnp_ee_eta";
									h2[h2name].Fill(lv.M(), fabs(photons[ph].p.Eta()));
								}
								
								if(!photons[ph].hasPixelSeed){
									// Neg
									nCounts[15]++;
									
									hname = "zpeak_pas";
									h[hname].Fill(lv.M());
									
									hname = "tnp_nogen_pas";
									h[hname].Fill(photons[ph].p.Pt());
									
									h2name = "tnp_eg";
									h2[h2name].Fill(lv.M(), photons[ph].p.Pt());
									
									h2name = "tnp_eg_ntracks";
									h2[h2name].Fill(lv.M(), *nTracksPV);
									h2name = "tnp_eg_nvtx";
									h2[h2name].Fill(lv.M(), *nGoodVertices);
									h2name = "tnp_eg_eta";
									h2[h2name].Fill(lv.M(), fabs(photons[ph].p.Eta()));
								
								}
						}
				}
			
			}
			
			
			/*
			cout << "Nnum: " << Nnum << endl;
			cout << "gen:\t Ne="<<nCounts[10]<< "\t Ng="<<nCounts[11];
			cout << "\t f=" << float(nCounts[11])/float(nCounts[10]+nCounts[11]) << endl;
			cout << "tnp:\t Ne="<<nCounts[14]<< "\t Ng="<<nCounts[15];
			cout << "\t f=" << float(nCounts[15])/float(2*nCounts[14]+nCounts[15]) << endl;
			
			getchar();
			
			// */
			
		} // photons.GetSize() == 2
		
		
		
		Nnum++;
	} // //////////////////////////////////////////////////////////////////////////////////////////
	
	cout << endl;
	
	cout << "Looped events: " << Nnum << endl;
	cout << "isSimu: " << isSimu << endl;
	
	cout << "genmatch:   f = " << float(nCounts[11])/float(nCounts[10]+nCounts[11]) << endl;
	cout << "gentotal:   f = " << float(nCounts[22])/float(nCounts[21]+nCounts[22]) << endl;
	cout << "tagprobe:   f = " << float(nCounts[15])/float(nCounts[14]+nCounts[15]) << endl;
	cout << "tagdiff:    f = " << float(nCounts[70])/float(nCounts[70]+2*nCounts[71]) << endl;
	cout << "tnp+gen:    f = " << float(nCounts[60])/float(nCounts[60]+2*nCounts[61]) << endl;
	cout << "tnp+gen+mz: f = " << float(nCounts[46])/float(nCounts[46]+2*nCounts[47]) << endl;
	
	int breaker=0;
	string sc="";
	for(int ll=0;ll<100;ll++){
		//sc = NumberToString(nCounts[ll]);
		//if(nCounts[ll]==0) sc="00000";
		//cout << "nCounts[" << ll << "]: " << nCounts[ll] << "\t";
		//cout << "nCounts[" << ll << "]: " +sc << "\t";
		cout << "nCounts[" << ll << "]: " << setfill('0') << setw(7) << nCounts[ll] << "\t";
		breaker++;
		if(breaker==4){ breaker = 0; cout << endl;}
	}
	
	
}

void f_genmatch_tagnprobe_wg(map<string,TH1F> &h, map<string,TH2F> &h2, string dataset){
	cout << "f_genmatch_tagnprobe_wg()" << endl;
	cout << dataset << endl;
	
	// INITIALIZATION
	float m_z = 91.1876; // Z mass in GeV
	
	string s = "v1";
	string hname, h2name;
	
	hname = "zpeak_tot";
	h[hname] = TH1F((hname+"_"+s).c_str(), ";m (GeV);Events / bin",400, 0, 200);
	hname = "zpeak_pas";
	h[hname] = TH1F((hname+"_"+s).c_str(), ";m (GeV);Events / bin",400, 0, 200);
	
	hname = "gen_all_tot";
	h[hname] = TH1F((hname+"_"+s).c_str(), ";p_{T} (GeV);Events / bin",500, 0, 250);
	hname = "gen_all_pas";
	h[hname] = TH1F((hname+"_"+s).c_str(), ";p_{T} (GeV);Events / bin",500, 0, 250);
	hname = "gen_two_tot";
	h[hname] = TH1F((hname+"_"+s).c_str(), ";p_{T} (GeV);Events / bin",500, 0, 250);
	hname = "gen_two_pas";
	h[hname] = TH1F((hname+"_"+s).c_str(), ";p_{T} (GeV);Events / bin",500, 0, 250);
	hname = "tnp_gen_tot";
	h[hname] = TH1F((hname+"_"+s).c_str(), ";p_{T} (GeV);Events / bin",500, 0, 250);
	hname = "tnp_gen_pas";
	h[hname] = TH1F((hname+"_"+s).c_str(), ";p_{T} (GeV);Events / bin",500, 0, 250);
	hname = "tnp_nogen_tot";
	h[hname] = TH1F((hname+"_"+s).c_str(), ";p_{T} (GeV);Events / bin",500, 0, 250);
	hname = "tnp_nogen_pas";
	h[hname] = TH1F((hname+"_"+s).c_str(), ";p_{T} (GeV);Events / bin",500, 0, 250);
	
	hname = "clos_gen_e";
	h[hname] = TH1F((hname+"_"+s).c_str(), ";p_{T} (GeV);Events / bin",250, 0, 250);
	hname = "clos_gen_g";
	h[hname] = TH1F((hname+"_"+s).c_str(), ";p_{T} (GeV);Events / bin",250, 0, 250);
	
	hname = "clos_gen_e_ntracks";
	h[hname] = TH1F((hname+"_"+s).c_str(), ";N_{track};Events / bin",200, 0, 200);
	hname = "clos_gen_g_ntracks";
	h[hname] = TH1F((hname+"_"+s).c_str(), ";N_{track};Events / bin",200, 0, 200);
	
	hname = "clos_gen_e_nvtx";
	h[hname] = TH1F((hname+"_"+s).c_str(), ";N_{vertex};Events / bin",30, 0, 30);
	hname = "clos_gen_g_nvtx";
	h[hname] = TH1F((hname+"_"+s).c_str(), ";N_{vertex};Events / bin",30, 0, 30);
	
	hname = "clos_gen_e_eta";
	h[hname] = TH1F((hname+"_"+s).c_str(), ";|#eta|;Events / bin",150, 0, 1.5);
	hname = "clos_gen_g_eta";
	h[hname] = TH1F((hname+"_"+s).c_str(), ";|#eta|;Events / bin",150, 0, 1.5);
	
	
	h2name = "tnp_gen_ee";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(),
				";m (GeV);p_{T} (GeV)",
				40, 81, 101, // 0.5 GeV bins
				250, 0, 250);
	h2name = "tnp_gen_eg";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(),
				";m (GeV);p_{T} (GeV)",
				40, 81, 101, // 0.5 GeV bins
				250, 0, 250);
	h2name = "tnp_ee";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(),
				";m (GeV);p_{T} (GeV)",
				40, 81, 101, // 0.5 GeV bins
				250, 0, 250);
	h2name = "tnp_eg";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(),
				";m (GeV);p_{T} (GeV)",
				40, 81, 101, // 0.5 GeV bins
				250, 0, 250);
	
	
	h2name = "tnp_gen_ee_ntracks";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(),
				";m (GeV);N_{track}",
				40, 81, 101, // 0.5 GeV bins
				200, 0, 200);
	h2name = "tnp_gen_eg_ntracks";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(),
				";m (GeV);N_{track}",
				40, 81, 101, // 0.5 GeV bins
				200, 0, 200);
	h2name = "tnp_ee_ntracks";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(),
				";m (GeV);N_{track}",
				40, 81, 101, // 0.5 GeV bins
				200, 0, 200);
	h2name = "tnp_eg_ntracks";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(),
				";m (GeV);N_{track}",
				40, 81, 101, // 0.5 GeV bins
				200, 0, 200);
	
	h2name = "tnp_gen_ee_nvtx";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(),
				";m (GeV);N_{vertex}",
				40, 81, 101, // 0.5 GeV bins
				30, 0, 30);
	h2name = "tnp_gen_eg_nvtx";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(),
				";m (GeV);N_{vertex}",
				40, 81, 101, // 0.5 GeV bins
				30, 0, 30);
	h2name = "tnp_ee_nvtx";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(),
				";m (GeV);N_{vertex}",
				40, 81, 101, // 0.5 GeV bins
				30, 0, 30);
	h2name = "tnp_eg_nvtx";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(),
				";m (GeV);N_{vertex}",
				40, 81, 101, // 0.5 GeV bins
				30, 0, 30);
	
	h2name = "tnp_gen_ee_eta";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(),
				";m (GeV);|#eta|",
				40, 81, 101, // 0.5 GeV bins
				150, 0, 1.5);
	h2name = "tnp_gen_eg_eta";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(),
				";m (GeV);|#eta|",
				40, 81, 101, // 0.5 GeV bins
				150, 0, 1.5);
	h2name = "tnp_ee_eta";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(),
				";m (GeV);|#eta|",
				40, 81, 101, // 0.5 GeV bins
				150, 0, 1.5);
	h2name = "tnp_eg_eta";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(),
				";m (GeV);|#eta|",
				40, 81, 101, // 0.5 GeV bins
				150, 0, 1.5);
	
/*
	h2name = "gen_e_gamma_PtdelR";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(), 
				";|p^{gen e}_{T} - p^{#gamma}_{T}|/p^{gen e}_{T}; #DeltaR(gen e, #gamma)",
				200, 0, 2,
				400, 0, 4);
	h2name = "gen_e_gamma_PtdelR_cut";
	h2[h2name] = TH2F((h2name + "_" + s).c_str(), 
				";|p^{gen e}_{T} - p^{#gamma}_{T}|/p^{gen e}_{T}; #DeltaR(gen e, #gamma)",
				200, 0, 2,
				400, 0, 4);
*/
	
	
	// **********************************************************************
	
	// Attention: isData is defined inverse!
	// -->	if mc	 set true
	// 		if data	 set false
	// >>> renamded to isSimu
	
	bool isSimu;
	isSimu = 	dataset.find("DYJetsToLL") != string::npos
			||	dataset.find("TTGJets") != string::npos
			||	dataset.find("WGToLNuG") != string::npos
			||	dataset.find("ZGTo2LG") != string::npos;
	// */
	
	// DATA FILE
	TFile *f = new TFile(dataset.c_str());
	TTreeReader reader("TreeWriter/eventTree", f);
	
	// initialize branches
	TTreeReaderArray<tree::Photon> photons(reader, "photons");
	TTreeReaderArray<tree::Jet> jets(reader, "jets");
	TTreeReaderArray<tree::Electron> electrons(reader, "electrons");
	TTreeReaderArray<tree::Muon> muons(reader, "muons");
	TTreeReaderArray<tree::Particle> genJets(reader, "genJets");
	TTreeReaderArray<tree::GenParticle> genParticles(reader, "genParticles");
	TTreeReaderArray<tree::IntermediateGenParticle> intermediateGenParticles(
													reader, "intermediateGenParticles");
	TTreeReaderValue<Float_t> pu_weight(reader, "pu_weight");
	TTreeReaderValue<Char_t> mc_weight(reader, "mc_weight");
	TTreeReaderValue<Int_t> nGoodVertices(reader, "nGoodVertices");
	TTreeReaderValue<Int_t> nTracksPV(reader, "nTracksPV");
	
	
	Float_t selW = 1.;
	
	// initialize variables
	int		Nnum = 0;
	
	int		nCounts[100] = {};
	
	int		ph = 0;
	
	TVector3 	v;			// vector
	TLorentzVector	lv;		// lorent vectors
	float E = 0.;
	v.SetXYZ(0.,0.,0.);
	lv.SetPxPyPzE(0.,0.,0.,0.);
	
	
	
	bool gotoNextPhoton = false;
	
	bool matched = false;
	
	bool	p0 = false,
			p1 = false;
			
	bool	massMatch = false;
	
	float 	delR = 0., 
			delPt = 0., 
			delRPt = 0.,
			mindelR = 0.,
			mindelPt = 0.;
	
	float	sdelR = 0.2,
			sdelPt = 0.1;
	
	Photon selPhoton;
	GenParticle selGenParticle; 
	
	///////////////////////////////////////////////////////////////////////////////////////////////
	// loop the events
	while(reader.Next()){
		
		//if(Nnum > 100000) continue;
		
		// counter output
		if((Nnum % 100000) == 0){
			cout << "\revent " << Nnum << " done... \t" << flush;
		}
		
		E = 0.;
		v.SetXYZ(0.,0.,0.);
		lv.SetPxPyPzE(0.,0.,0.,0.);
		
		
		// GENERATOR MATCHING for all events
		for(auto& interGen: intermediateGenParticles){
			if(abs(interGen.pdgId) == 24){ // W boson
				nCounts[20]++;
				//cout << Nnum << "\t" << interGen.daughters.size() << endl;
				for(auto& d: interGen.daughters){
					if(abs(d.pdgId) == 11){ // electrons/positrons
						for(auto& p: photons){
							if(	p.p.Pt() > 40. &&
								fabs(p.p.Eta()) < 1.4442){
								
								delR = d.p.DeltaR(p.p);
								delPt = fabs(d.p.Pt()-p.p.Pt())/d.p.Pt();
								delRPt = 	sqrt(delR*delR/(sdelR*sdelR)+
											delPt*delPt/(sdelPt*sdelPt));
								if(	delR < 0.2 &&
									delPt < 0.1){
									//delRPt < 1.){
									//cout << p.hasPixelSeed << "\t";
									//cout << "delR: " << delR << "\t delPt: " << delPt;
									//cout << "\t delRPt: " << delRPt << endl;
									
									nCounts[21]++;
									
									
									
									// DENOMINATOR
									hname = "gen_all_tot";
									h[hname].Fill(p.p.Pt());
									
									// NUMERATOR
									if(!p.hasPixelSeed){
										nCounts[22]++;
										
										hname = "gen_all_pas";
										h[hname].Fill(p.p.Pt());
										
										hname = "clos_gen_g";
										h[hname].Fill(p.p.Pt());
										
										hname = "clos_gen_g_ntracks";
										h[hname].Fill(*nTracksPV);
										hname = "clos_gen_g_nvtx";
										h[hname].Fill(*nGoodVertices);
										hname = "clos_gen_g_eta";
										h[hname].Fill(fabs(p.p.Eta()));
									}
									
									if(p.hasPixelSeed){
										hname = "clos_gen_e";
										h[hname].Fill(p.p.Pt());
										
										hname = "clos_gen_e_ntracks";
										h[hname].Fill(*nTracksPV);
										hname = "clos_gen_e_nvtx";
										h[hname].Fill(*nGoodVertices);
										hname = "clos_gen_e_eta";
										h[hname].Fill(fabs(p.p.Eta()));
									}
									
								}
							}
						} //photon loop
					}
					//cout << "\t" << d.pdgId;
				}
				//getchar();
			}
			
		} // loop intermediate particles
		
		Nnum++;
	} // //////////////////////////////////////////////////////////////////////////////////////////
	
	cout << endl;
	
	cout << "Looped events: " << Nnum << endl;
	cout << "isSimu: " << isSimu << endl;
	
	cout << "genmatch:   f = " << float(nCounts[11])/float(nCounts[10]+nCounts[11]) << endl;
	cout << "gentotal:   f = " << float(nCounts[22])/float(nCounts[21]+nCounts[22]) << endl;
	cout << "tagprobe:   f = " << float(nCounts[15])/float(nCounts[14]+nCounts[15]) << endl;
	cout << "tagdiff:    f = " << float(nCounts[70])/float(nCounts[70]+2*nCounts[71]) << endl;
	cout << "tnp+gen:    f = " << float(nCounts[60])/float(nCounts[60]+2*nCounts[61]) << endl;
	cout << "tnp+gen+mz: f = " << float(nCounts[46])/float(nCounts[46]+2*nCounts[47]) << endl;
	
	int breaker=0;
	string sc="";
	for(int ll=0;ll<100;ll++){
		//sc = NumberToString(nCounts[ll]);
		//if(nCounts[ll]==0) sc="00000";
		//cout << "nCounts[" << ll << "]: " << nCounts[ll] << "\t";
		//cout << "nCounts[" << ll << "]: " +sc << "\t";
		cout << "nCounts[" << ll << "]: " << setfill('0') << setw(7) << nCounts[ll] << "\t";
		breaker++;
		if(breaker==4){ breaker = 0; cout << endl;}
	}
	
	
}

void f_check_mc(map<string,TH1F> &h, map<string,TH2F> &h2, string dataset){
	cout << "f_check_mc()" << endl;
	cout << dataset << endl;
	
	// INITIALIZATION
	string s = "v1";
	string hname, h2name;
	
	// **********************************************************************
	
	// Attention: isData is defined inverse!
	// -->	if mc	 set true
	// 		if data	 set false
	// >>> renamded to isSimu
	
	bool isSimu;
	isSimu = 	dataset.find("DYJetsToLL") != string::npos
			||	dataset.find("TTGJets") != string::npos
			||	dataset.find("WGToLNuG") != string::npos
			||	dataset.find("ZGTo2LG") != string::npos;
	// */
	
	// DATA FILE
	TFile *f = new TFile(dataset.c_str());
	TTreeReader reader("TreeWriter/eventTree", f);
	
	// initialize branches
	TTreeReaderArray<tree::Photon> photons(reader, "photons");
	TTreeReaderArray<tree::Jet> jets(reader, "jets");
	TTreeReaderArray<tree::Electron> electrons(reader, "electrons");
	TTreeReaderArray<tree::Muon> muons(reader, "muons");
	TTreeReaderArray<tree::Particle> genJets(reader, "genJets");
	TTreeReaderArray<tree::GenParticle> genParticles(reader, "genParticles");
	TTreeReaderArray<tree::IntermediateGenParticle> intermediateGenParticles(
													reader, "intermediateGenParticles");
	TTreeReaderValue<Float_t> pu_weight(reader, "pu_weight");
	TTreeReaderValue<Char_t> mc_weight(reader, "mc_weight");
	TTreeReaderValue<Int_t> nGoodVertices(reader, "nGoodVertices");
	
	
	Float_t selW = 1.;
	
	// initialize variables
	int		Nnum = 0;
	
	int		nCounts[100] = {};
	
	int		ph = 0;
	
	TVector3 	v;			// array of vectors
	TLorentzVector	lv;	// array of lorentz vectors
	float E = 0.;
	v.SetXYZ(0.,0.,0.);
	lv.SetPxPyPzE(0.,0.,0.,0.);
	
	float m_z = 91.1876; // GeV
	
	bool gotoNextPhoton = false;
	
	bool matched = false;
	
	bool	p0 = false,
			p1 = false;
			
	bool	massMatch = false;
	
	float 	delR = 0., 
			delPt = 0., 
			delRPt = 0.,
			mindelR = 0.,
			mindelPt = 0.;
	
	float	sdelR = 0.2,
			sdelPt = 0.1;
	
	Photon selPhoton;
	GenParticle selGenParticle; 
	
	///////////////////////////////////////////////////////////////////////////////////////////////
	// loop the events
	while(reader.Next()){
		
		//if(Nnum > 100000) continue;
		
		// counter output
		if((Nnum % 100000) == 0){
			cout << "\revent " << Nnum << " done... \t" << flush;
		}
		
		E = 0.;
		v.SetXYZ(0.,0.,0.);
		lv.SetPxPyPzE(0.,0.,0.,0.);
		
		
		
		
		
		
		
		Nnum++;
	} // //////////////////////////////////////////////////////////////////////////////////////////
	
	
	cout << endl;
	
	cout << "Looped events: " << Nnum << endl;
	cout << "isSimu: " << isSimu << endl;
	
	int breaker=0;
	for(int ll=0;ll<100;ll++){
		cout << "nCounts[" << ll << "]: " << setfill('0') << setw(7) << nCounts[ll] << "\t";
		breaker++;
		if(breaker==4){ breaker = 0; cout << endl;}
	}
	
	
}

void f__closure(map<string,TH1F> &h, map<string,TH2F> &h2, string dataset){
	
	cout << "f__closure()" << endl;
	cout << dataset << endl;
	
	// INITIALIZATION
	string s = "v1";
	string hname, h2name;
	
	hname = "e_gen";
	h[hname] = TH1F((hname + "_" + s).c_str(), ";p_T (GeV); Events / bin",200,0,200);
	
	hname = "eg_1";
	h[hname] = TH1F((hname + "_" + s).c_str(), ";p_T (GeV); Events / bin",200,0,200);
	hname = "eg_2";
	h[hname] = TH1F((hname + "_" + s).c_str(), ";p_T (GeV); Events / bin",200,0,200);
	
	hname = "eg_3";
	h[hname] = TH1F((hname + "_" + s).c_str(), ";p_T (GeV); Events / bin",200,0,200);
	hname = "eg_4";
	h[hname] = TH1F((hname + "_" + s).c_str(), ";p_T (GeV); Events / bin",200,0,200);
	
	hname = "eg_5";
	h[hname] = TH1F((hname + "_" + s).c_str(), ";p_T (GeV); Events / bin",200,0,200);
	hname = "eg_6";
	h[hname] = TH1F((hname + "_" + s).c_str(), ";p_T (GeV); Events / bin",200,0,200);
	
	hname = "eg_7";
	h[hname] = TH1F((hname + "_" + s).c_str(), ";p_T (GeV); Events / bin",200,0,200);
	hname = "eg_8";
	h[hname] = TH1F((hname + "_" + s).c_str(), ";p_T (GeV); Events / bin",200,0,200);
	
	hname = "eg_9";
	h[hname] = TH1F((hname + "_" + s).c_str(), ";p_T (GeV); Events / bin",200,0,200);
	hname = "eg_10";
	h[hname] = TH1F((hname + "_" + s).c_str(), ";p_T (GeV); Events / bin",200,0,200);
	
	
	hname = "eg_tot_1bin";
	h[hname] = TH1F((hname + "_" + s).c_str(), ";p_T (GeV); Events / bin",1,0,200);
	hname = "eg_pas_1bin";
	h[hname] = TH1F((hname + "_" + s).c_str(), ";p_T (GeV); Events / bin",1,0,200);
	
	
	// **********************************************************************
	
	// DATA FILE
	TFile *f = new TFile(dataset.c_str());
	TTreeReader reader("TreeWriter/eventTree", f);
	
	// initialize branches
	TTreeReaderArray<tree::Photon> photons(reader, "photons");
	TTreeReaderArray<tree::Jet> jets(reader, "jets");
	TTreeReaderArray<tree::Electron> electrons(reader, "electrons");
	TTreeReaderArray<tree::Muon> muons(reader, "muons");
	TTreeReaderArray<tree::Particle> genJets(reader, "genJets");
	TTreeReaderArray<tree::GenParticle> genParticles(reader, "genParticles");
	TTreeReaderValue<Float_t> pu_weight(reader, "pu_weight");
	TTreeReaderValue<Char_t> mc_weight(reader, "mc_weight");
	TTreeReaderValue<Int_t> nGoodVertices(reader, "nGoodVertices");
	
	Float_t selW = 1.;
	
	// initialize variables
	int		Nnum = 0;
	
	int		nCounts[100] = {};
	
	int		ph = 0;
	
	bool	match = false;
	
	TVector3 	v;			// array of vectors
	TLorentzVector	lv;	// array of lorentz vectors
	float E = 0.;
	v.SetXYZ(0.,0.,0.);
	lv.SetPxPyPzE(0.,0.,0.,0.);
	
	bool gotoNextPhoton = false;
	
	///////////////////////////////////////////////////////////////////////////////////////////////
	// loop the events
	while(reader.Next()){
		
		//if(Nnum > 100000) continue;
		
		/*
		E = 0.;
		v.SetXYZ(0.,0.,0.);
		lv.SetPxPyPzE(0.,0.,0.,0.);
		// */
		
		// generated, true electrons
		
		for(auto& genp: genParticles){
			if(	fabs(genp.p.Eta())<1.4442 &&
				abs(genp.pdgId) == 11) {
					hname = "e_gen";
					h[hname].Fill(genp.p.Pt());
				
			}
		}
		
		
		if(photons.GetSize() == 2){
			
			for(auto& genp: genParticles){
				if(abs(genp.pdgId) == 11){
					hname = "eg_1";
					h[hname].Fill(genp.p.Pt());
					
					ph = 0;
					if(photons[ph].p.DeltaR(genp.p)<0.2){
						if(photons[ph].hasPixelSeed){
							hname = "eg_2";
							h[hname].Fill(genp.p.Pt());
							
						}
						if(!photons[ph].hasPixelSeed){
							hname = "eg_3";
							h[hname].Fill(genp.p.Pt());
							
						}
					}
					ph = 1;
					if(photons[ph].p.DeltaR(genp.p)<0.2){
						if(photons[ph].hasPixelSeed){
							hname = "eg_2";
							h[hname].Fill(genp.p.Pt());
							
						}
						if(!photons[ph].hasPixelSeed){
							hname = "eg_3";
							h[hname].Fill(genp.p.Pt());
							
						}
					}
					
					
				}
			}
			
		}
		
		for(auto& p: photons){
			gotoNextPhoton = false;
			for(auto& genp: genParticles){
				if(abs(genp.pdgId) == 11 && !gotoNextPhoton){
					if(	genp.p.DeltaR(p.p)<0.2 &&
						fabs(p.p.Eta())<1.4442
						//p.p.Pt() > 30.
						){
							
							if(!p.hasPixelSeed){
								hname = "eg_4";
								h[hname].Fill(p.p.Pt());
								
								hname = "eg_pas_1bin";
								h[hname].Fill(p.p.Pt());
							
							}
							
							if(p.hasPixelSeed){
								hname = "eg_5";
								h[hname].Fill(p.p.Pt());
								
								
								hname = "eg_tot_1bin";
								h[hname].Fill(p.p.Pt());
							}
						
						}
				}
				//goto nextPhoton;
				gotoNextPhoton = true;
			} // genParticles loop
		} //photons loop
		// */
		
		if(	genParticles.GetSize() == 1	&&
			abs(genParticles[0].pdgId) == 11	&&
			fabs(genParticles[0].p.Eta()) < 1.4442){
				match = false;
				for(auto& e: electrons){
					if(e.p.DeltaR(genParticles[0].p) < 0.2){
						hname = "eg_6";
						h[hname].Fill(genParticles[0].p.Pt());
						match = true;
					}
				}
				if(!match){
					hname = "eg_7";
					h[hname].Fill(genParticles[0].p.Pt());
				}
				
				for(auto& p: photons){
					if(p.p.DeltaR(genParticles[0].p) < 0.2){
						if(p.hasPixelSeed){
							hname = "eg_8";
							h[hname].Fill(genParticles[0].p.Pt());
						}
						if(!p.hasPixelSeed){
							hname = "eg_9";
							h[hname].Fill(genParticles[0].p.Pt());
						}
						
					}
				}
				
		}
		// | Ralf | [[%ATTACHURLPATH%/Abstract_DPG_Meyer.pdf][pdf]] | [[%ATTACHURLPATH%/2016_03_02_DPG_RM_05.pdf][pdf]] |
		
		Nnum++;
	} // //////////////////////////////////////////////////////////////////////////////////////////
	
	
	cout << "Looped events: " << Nnum << endl;
	
	for(int ll=0;ll<100;ll++){
		cout << "nCounts[" << ll << "]: " << nCounts[ll] << "\t";
	}
	
}
