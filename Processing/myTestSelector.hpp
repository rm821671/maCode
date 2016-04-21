
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

struct Event{
        int Nnum;
        int eventNo;
        int runNo;
        int lumNo;
        string dataset; // 
    };

/**********************************************************************************************
 * setup functions
 * 
 * 
 */
void test(int *nCounts, vector<Event> &events){
    cout << "test()" << endl;
    for(int i=0;i<20;i++){
        nCounts[i] = i*i;
    }
    
    Event e;
    e.lumNo = 3;
    e.runNo = 5;
    e.eventNo = 430;
    events.push_back(e);
    
}

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

void writeToFile(map<string,TH1F> &h, map<string,TH2F> &h2, string dataset){
    // save objects in h and h2 to file
    
    string outputName, filepath, dropbox;
    SystemPath(filepath, dropbox);
    
    auto PosStart = dataset.rfind("/");
    auto PosEnd = dataset.find(".root");
      
    if( PosEnd != string::npos ) {
        outputName =    dropbox +
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

void eventwriter(vector<Event> &events){
    // write written events to file
    ofstream f("Trigger4Events.txt");//, ios::app); //append
    if(f.is_open()){
        for(auto& e: events){
            f << e.runNo << ":" << e.lumNo << ":" << e.eventNo << endl;
        }
        f.close();
    }
    ofstream f2("Trigger4Events_full.txt");//, ios::app); //append
    if(f2.is_open()){
        for(auto& e: events){
            f2 << e.runNo << ":" << e.lumNo << ":" << e.eventNo;
            f2 << "\t" << e.Nnum << "\t"<< e.dataset << endl;
        }
        f2.close();
    }
}

void init(map<string,TH1F> &h, map<string,TH2F> &h2){
    
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
    
    
    hname = "trigger1_size";
    h[hname] = TH1F((hname + "_" + s).c_str(), ";trigger4.size(); Events / bin",10,0,10);
    hname = "trigger2_size";
    h[hname] = TH1F((hname + "_" + s).c_str(), ";trigger4.size(); Events / bin",10,0,10);
    hname = "trigger3_size";
    h[hname] = TH1F((hname + "_" + s).c_str(), ";trigger4.size(); Events / bin",10,0,10);
    hname = "trigger4_size";
    h[hname] = TH1F((hname + "_" + s).c_str(), ";trigger4.size(); Events / bin",10,0,10);
    
    
    
    hname = "massDist_interGen_daughters";
    h[hname] = TH1F((hname + "_" + s).c_str(),
                ";m (GeV);Events / bin)",
                2000, 0, 200); // 0.1 GeV bins
    
    
    hname = "delR_selTag_selProbe";
    h[hname] = TH1F((hname + "_" + s).c_str(), ";#Delta R(tag, probe);Events / bin",500,0,5);
    
    
    
    
    
    h2name = "plane_genEl_emObj";
    h2[h2name] = TH2F((h2name + "_" + s).c_str(),
                ";p_{T}^{emObj} / p_{T}^{genEl};#Delta R(emObj, genEl)",
                200, 0, 2, // 0.5 GeV bins
                400, 0, 4);
    
    h2name = "plane_trObj_emObj";
    h2[h2name] = TH2F((h2name + "_" + s).c_str(),
                ";p_{T}^{emObj} / p_{T}^{trObj};#Delta R(emObj, trObj)",
                2000, 0, 2, // 0.5 GeV bins
                4000, 0, 4);
    
    h2name = "plane_PhiEta_selTag_selProbe";
    h2[h2name] = TH2F((h2name + "_" + s).c_str(),
                ";#Delta #Phi;#Delta #eta",
                700, 0, 7, // 0.5 GeV bins
                500, 0, 5);
    
    
    int     zbins = 2500,
            zmin = 0,
            zmax = 250;
    
    
    h2name = "tnp_gen_ee";
    h2[h2name] = TH2F((h2name + "_" + s).c_str(),
                ";m (GeV);p_{T} (GeV)",
                zbins, zmin, zmax,
                250, 0, 250);
    h2name = "tnp_gen_eg";
    h2[h2name] = TH2F((h2name + "_" + s).c_str(),
                ";m (GeV);p_{T} (GeV)",
                zbins, zmin, zmax,
                250, 0, 250);
    h2name = "tnp_ee";
    h2[h2name] = TH2F((h2name + "_" + s).c_str(),
                ";m (GeV);p_{T} (GeV)",
                zbins, zmin, zmax,
                250, 0, 250);
    h2name = "tnp_eg";
    h2[h2name] = TH2F((h2name + "_" + s).c_str(),
                ";m (GeV);p_{T} (GeV)",
                zbins, zmin, zmax,
                250, 0, 250);
    h2name = "tnp_gen_ee_ntracks";
    h2[h2name] = TH2F((h2name + "_" + s).c_str(),
                ";m (GeV);N_{track}",
                zbins, zmin, zmax,
                200, 0, 200);
    h2name = "tnp_gen_eg_ntracks";
    h2[h2name] = TH2F((h2name + "_" + s).c_str(),
                ";m (GeV);N_{track}",
                zbins, zmin, zmax,
                200, 0, 200);
    h2name = "tnp_ee_ntracks";
    h2[h2name] = TH2F((h2name + "_" + s).c_str(),
                ";m (GeV);N_{track}",
                zbins, zmin, zmax,
                200, 0, 200);
    h2name = "tnp_eg_ntracks";
    h2[h2name] = TH2F((h2name + "_" + s).c_str(),
                ";m (GeV);N_{track}",
                zbins, zmin, zmax,
                200, 0, 200);
    
    h2name = "tnp_gen_ee_nvtx";
    h2[h2name] = TH2F((h2name + "_" + s).c_str(),
                ";m (GeV);N_{vertex}",
                zbins, zmin, zmax,
                30, 0, 30);
    h2name = "tnp_gen_eg_nvtx";
    h2[h2name] = TH2F((h2name + "_" + s).c_str(),
                ";m (GeV);N_{vertex}",
                zbins, zmin, zmax,
                30, 0, 30);
    h2name = "tnp_ee_nvtx";
    h2[h2name] = TH2F((h2name + "_" + s).c_str(),
                ";m (GeV);N_{vertex}",
                zbins, zmin, zmax,
                30, 0, 30);
    h2name = "tnp_eg_nvtx";
    h2[h2name] = TH2F((h2name + "_" + s).c_str(),
                ";m (GeV);N_{vertex}",
                zbins, zmin, zmax,
                30, 0, 30);
    
    h2name = "tnp_gen_ee_eta";
    h2[h2name] = TH2F((h2name + "_" + s).c_str(),
                ";m (GeV);|#eta|",
                zbins, zmin, zmax,
                150, 0, 1.5);
    h2name = "tnp_gen_eg_eta";
    h2[h2name] = TH2F((h2name + "_" + s).c_str(),
                ";m (GeV);|#eta|",
                zbins, zmin, zmax,
                150, 0, 1.5);
    h2name = "tnp_ee_eta";
    h2[h2name] = TH2F((h2name + "_" + s).c_str(),
                ";m (GeV);|#eta|",
                zbins, zmin, zmax,
                150, 0, 1.5);
    h2name = "tnp_eg_eta";
    h2[h2name] = TH2F((h2name + "_" + s).c_str(),
                ";m (GeV);|#eta|",
                zbins, zmin, zmax,
                150, 0, 1.5);
    
    
    // z peak background constructed with muons as tag
    h2name = "tnp_muon_bkg_mu_e"; //
    h2[h2name] = TH2F((h2name + "_" + s).c_str(),
                ";m (GeV);p_{T} (GeV)",
                zbins, zmin, zmax,
                250, 0, 250);
    h2name = "tnp_muon_bkg_mu_g";
    h2[h2name] = TH2F((h2name + "_" + s).c_str(),
                ";m (GeV);p_{T} (GeV)",
                zbins, zmin, zmax,
                250, 0, 250);
    
    
    
}

void terminate(int *nCounts){
    // output:
    cout << "************************************************************";
    cout << "************************************************************" << endl;
    int breaker=0;
    string sc="";
    for(int ll=0;ll<100;ll++){
        cout << "nCounts[" << ll << "]: " << setfill('0') << setw(7) << nCounts[ll] << "\t";
        breaker++;
        if(breaker==4){ breaker = 0; cout << endl;}
    }
    
}

/**********************************************************************************************
 * analysis functions
 *
 */

void f__test(map<string,TH1F> &h, map<string,TH2F> &h2, string dataset){
    
    
    
    cout << "f__test()" << endl;
    cout << dataset << endl;
    
    // comment out:
    
    
    
    // INITIALIZATION
    string s = "v1";
    string hname, h2name;
    
    
    //hname = "e_gen";
    //h[hname] = TH1F((hname + "_" + s).c_str(), ";p_T (GeV); Events / bin",200,0,200);
    
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
    TTreeReaderArray<tree::TriggerObject> triggerObjects(reader, "triggerObjects");
    TTreeReaderValue<Float_t> pu_weight(reader, "pu_weight");
    TTreeReaderValue<Char_t> mc_weight(reader, "mc_weight");
    TTreeReaderValue<Int_t> nGoodVertices(reader, "nGoodVertices");
    
    TTreeReaderValue<Bool_t> SingleElectron_1 ( reader, 
        "HLT_Ele23_WP75_Gsf_v");
    TTreeReaderValue<Bool_t> SingleElectron_2 ( reader,
        "HLT_Ele22_eta2p1_WP75_Gsf_v");
    TTreeReaderValue<Bool_t> SingleElectron_3 ( reader,
        "HLT_Ele22_eta2p1_WPLoose_Gsf_v");
    TTreeReaderValue<Bool_t> SingleElectron_4 ( reader,
        "HLT_Ele27_eta2p1_WPLoose_Gsf_v");
    
    Float_t selW = 1.;
    
    // initialize variables
    int     Nnum = 0;
    
    int     nCounts[100] = {};
    
    int     ph = 0;
    
    bool    match = false;
    
    TVector3    v;          // array of vectors
    TLorentzVector  lv; // array of lorentz vectors
    float E = 0.;
    v.SetXYZ(0.,0.,0.);
    lv.SetPxPyPzE(0.,0.,0.,0.);
    
    //tree::Photon ph
    vector<tree::TriggerObject*> vtr;
    string triggerLabel = "hltEle27WPLooseGsfTrackIsoFilter";
    
    bool gotoNextPhoton = false;
    
    vector<string> sv;
    
    float   delR,
            relPt;
    
    
    cout << endl;
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // loop the events
    while(reader.Next()){
        
        if(Nnum > 3000000) break;
        
        // counter output
        if((Nnum % 100000) == 0){
            cout << "\revent " << Nnum << " done... \t" << flush;
        }
        
        //cout << "Nnum: ";
        
        
        sv.clear();
        vtr.clear();
        nCounts[0] = 0;
        
        
/*
        if( Nnum == 150 ||
            Nnum == 197 ||
            Nnum == 223 ||
            Nnum == 271 ||
            Nnum == 364493){
                cout << "Nnum: " << Nnum;
                cout << "\t photons.size() = " << photons.GetSize();
                cout << "\t triggerObjects.size() = " << triggerObjects.GetSize() << endl;
                for(auto& p: photons){
                    cout << "\t p.pt=" << p.p.Pt();
                    cout << "\t p.eta=" << p.p.Eta();
                    cout << "\t p.pixelSeed=" << p.hasPixelSeed << endl;
                    for(auto& tr: triggerObjects){
                        delR = p.p.DeltaR(tr.p);
                        relPt = p.p.Pt()/tr.p.Pt();
                        cout << "\t\t" << delR << "\t";
                        cout << relPt << endl;
                    }
                }
                
                
            }
// */
        
        // if (std::find(filters.begin(),filters.end(),"hltEle23WPLooseGsfTrackIsoFilter") != filters.end()) {
        // find the trigger object(s) with the triggerLabel in the label

        if(*SingleElectron_4){
            //cout << "Nnum: " << Nnum << endl;
            for(auto& tr: triggerObjects){
                //for(auto s: tr.filterLabels){
                //    cout << s << endl;
                //}
                if (std::find(  tr.filterLabels.begin(),
                                tr.filterLabels.end(),
                                triggerLabel) != tr.filterLabels.end()){
              //                      cout << "\t found" << endl;
                                    vtr.push_back(&tr);
                                }
            }
            //cout << "Number of triggers: " << vtr.size() << endl;
            
            if(vtr.size() > 2){
                cout << Nnum << "\t " << vtr.size() << endl;
                getchar();
                for(auto& v: vtr){
                    cout << v->filterLabels.size() << endl;
                }
            }
            
        }
// */
        
        
/*
        for(auto& p: photons){
            //cout << "\t em object: " << p.p.Pt() << endl;
            for(auto& tr: triggerObjects){
                delR = p.p.DeltaR(tr.p);
                relPt = p.p.Pt()/tr.p.Pt();
                //cout << "\t\t" << delR << "\t";
                //cout << relPt << endl;
                h2name = "relPtdelR";
                h2[h2name].Fill(relPt, delR);
            }
            
        }
// */
        //getchar();
        
        
/*
        for(auto& tr: triggerObjects){
            nCounts[0]++;
            //cout << tr.filterLabels[0] << endl;
            cout << "trObj: " << nCounts[0] << "\t Pt = " << tr.p.Pt() << endl; 
            for(auto s: tr.filterLabels) cout << s << endl;
            //getchar();
            
        } 
*/
        
        
        Nnum++;
    } // //////////////////////////////////////////////////////////////////////////////////////////
    
    
    cout << endl;
    cout << "Looped events: " << Nnum << endl;
    
    int breaker=0;
    string sc="";
    for(int ll=0;ll<100;ll++){
        cout << "nCounts[" << ll << "]: " << setfill('0') << setw(7) << nCounts[ll] << "\t";
        breaker++;
        if(breaker==4){ breaker = 0; cout << endl;}
    }
    
    // */
    
}

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
    // -->  if mc    set true
    //      if data  set false
    // >>> renamded to isSimu
    
    bool isSimu;
    isSimu =    dataset.find("DYJetsToLL") != string::npos
            ||  dataset.find("TTGJets") != string::npos
            ||  dataset.find("WGToLNuG") != string::npos
            ||  dataset.find("ZGTo2LG") != string::npos;
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
    int     Nnum = 0;
    
    int     nCounts[100] = {};
    
    TVector3    v;          // array of vectors
    TLorentzVector  lv; // array of lorentz vectors
    float E = 0.;
    v.SetXYZ(0.,0.,0.);
    lv.SetPxPyPzE(0.,0.,0.,0.);
    
    
    bool gotoNextPhoton = false;
    
    bool matched = false;
    
    float   delR = 0., 
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
                    
                    if( p.p.Pt() > 40 &&
                        fabs(p.p.Eta()) < 1.4442){
                            
                            h2name = "gen_e_gamma_PtdelR";
                            h2[h2name].Fill(    fabs(genp.p.Pt()-p.p.Pt())/genp.p.Pt(),
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
                    
                    if( p.p.Pt() > 40 &&
                        fabs(p.p.Eta()) < 1.4442){
                            matched = true;
                            delR = genp.p.DeltaR(p.p);
                            delPt = fabs(genp.p.Pt()-p.p.Pt())/genp.p.Pt();
                            // if delRPt is smaller than for the last catch, take the new one
                            if( delRPt == 0. || 
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
                if( matched &&
                    sqrt(delR*delR/(0.2*0.2)+delPt*delPt/(0.1*0.1)) < 1.){
                        h2name = "gen_e_gamma_PtdelR_cut";
                        h2[h2name].Fill(    mindelPt,
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


int f_diphoton_singleelectron_triggermatching( map<string,TH1F> &h, 
                                                map<string,TH2F> &h2, 
                                                string dataset,
                                                int *nCounts,
                                                vector<Event> &events){
    
    
    cout << "f_diphoton_singleelectron_triggermatching()" << endl;
    cout << dataset << endl;
    
    // INITIALIZATION
    float m_z = 91.1876; // Z mass in GeV
    
    string s = "v1";
    string hname, h2name;
    
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
    
    // get tree and read out branches
    TTree *t = reader.GetTree();
    TObjArray *tobj = t->GetListOfBranches();
    
    //tobj->Print();
    
    
    return 0;
    
    // initialize branches
    TTreeReaderArray<tree::Photon> photons(reader, "photons");
    TTreeReaderArray<tree::Jet> jets(reader, "jets");
    TTreeReaderArray<tree::Electron> electrons(reader, "electrons");
    TTreeReaderArray<tree::Muon> muons(reader, "muons");
    TTreeReaderArray<tree::Particle> genJets(reader, "genJets");
    TTreeReaderArray<tree::GenParticle> genParticles(reader, "genParticles");
    TTreeReaderArray<tree::TriggerObject> triggerObjects(reader, "triggerObjects");
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
    
    TTreeReaderValue<ULong64_t> eventNo ( reader, "evtNo");
    TTreeReaderValue<UInt_t> runNo ( reader, "runNo");
    TTreeReaderValue<UInt_t> lumNo ( reader, "lumNo");
    
    
    Float_t selW = 1.;
    
    // initialize variables
    int     Nnum = 0,
            nTest = 0,
            nPhotons = 0;
    
    //int     nCounts[100] = {};
    
    float   fTest = 0.0;
    
    int ph=0, ph2=0;
    
    TVector3    v;          // vector
    TLorentzVector  lv;     // lorentz vector
    float E = 0.;
    v.SetXYZ(0.,0.,0.);
    lv.SetPxPyPzE(0.,0.,0.,0.);
    
    float   delR,
            relPt;
    
    string triggerLabel;
    
    Event evt;
    
    vector<tree::Photon*>   selPhotons,
                            vp_tag,
                            vp_probe;
    
    vector<tree::TriggerObject*>    vtr1,
                                    vtr2,
                                    vtr3,
                                    vtr4;
    string triggerLabels[] = {  "", //0
                                "empty", //1
                                "empty", //2
                                "hltSingleEle22WPLooseGsfTrackIsoFilter", //3, 
                                                            //probably this is the label!
                                "hltEle27WPLooseGsfTrackIsoFilter" // 4
                            };
    
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
        vtr1.clear();
        vtr2.clear();
        vtr3.clear();
        vtr4.clear();
        //vp_probe.clear();
        vp_tag.clear();
        
        if(*SingleElectron_1) nCounts[0]++;
        if(*SingleElectron_2) nCounts[1]++;
        if(*SingleElectron_3) nCounts[2]++;
        if(*SingleElectron_4) nCounts[3]++;
        
        
        if(triggerObjects.GetSize() > 2){
            nCounts[7]++;
        }
        
        // find trigger objects
        for(auto& tr: triggerObjects){
            
            /*
            if(*SingleElectron_3){
                cout << Nnum << endl;
                for(auto& tl: tr.filterLabels){
                            cout << "\t" << tl << endl;
                }
                getchar();
            }// */
            
            if( std::find(  tr.filterLabels.begin(), 
                            tr.filterLabels.end(), 
                            triggerLabels[1]) != tr.filterLabels.end()){
                        vtr1.push_back(&tr);
                    }
            if( std::find(  tr.filterLabels.begin(), 
                            tr.filterLabels.end(), 
                            triggerLabels[2]) != tr.filterLabels.end()){
                        vtr2.push_back(&tr);
                    }
            if( std::find(  tr.filterLabels.begin(), 
                            tr.filterLabels.end(), 
                            triggerLabels[3]) != tr.filterLabels.end()){
                        vtr3.push_back(&tr);
                    }
            if( std::find(  tr.filterLabels.begin(), 
                            tr.filterLabels.end(), 
                            triggerLabels[4]) != tr.filterLabels.end()){
                        vtr4.push_back(&tr);
                    }
        }
        
        if(vtr4.size() > 2){
            nCounts[8]++;
            
            evt.Nnum = Nnum;
            evt.eventNo = *eventNo;
            evt.runNo = *runNo;
            evt.lumNo = *lumNo;
            evt.dataset = dataset;
            events.push_back(evt);
        }
        
        hname = "trigger1_size";
        h[hname].Fill(vtr1.size());
        hname = "trigger2_size";
        h[hname].Fill(vtr2.size());
        hname = "trigger3_size";
        h[hname].Fill(vtr3.size());
        hname = "trigger4_size";
        h[hname].Fill(vtr4.size());
        
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
                
                nCounts[20]++;
                
                nCounts[80] = 0;
                nCounts[81] = 0;
                // Preselection of objects that have fired this trigger
                // and objects that dont.
                // These will be used to assign the objects as the tag
                // or probe object, respectively.
                //
                bool    tagged = false,
                        probed = false;
                
                //cout << Nnum << "\t photons.size = " << photons.GetSize();
                //cout << "\t trObj.size = " << vtr4.size() << endl;
                
                
                
                selPhotons.clear();
                for(auto& p: photons){
                    selPhotons.push_back(&p);
                }
                
                nCounts[27]=0;
                for(auto& t: vtr4){
                    
                    vp_tag.clear();
                    vp_probe.clear();
                    
                    
                    /*cout << "\t trigger: " << t->p.Pt() << endl;
                    for(auto& tl: t->filterLabels){
                        cout << "\t\t " << tl << endl;
                    } // */
                    tagged = false;
                    probed = false;
                    for(auto& p: photons){
                        //cout << "\t\t photon: " << p.p.Pt() << "\t" << p.p.Eta() << endl;
                        if( p.p.Pt() > 40. &&
                            fabs(p.p.Eta()) < 1.4442){
                                
                                delR = t->p.DeltaR(p.p);
                                relPt = p.p.Pt()/t->p.Pt();
                                //cout << "\t " << delR << "\t " << relPt << endl;
                                
                                // the probe has to be delR>0.1 
                                // from the triggered object:
                                if( delR > 0.1 &&
                                    !probed){
                                    probed = true;
                                    //cout << "\t\t\t probe match!" << endl;
                                    vp_probe.push_back(&p);
                                    nCounts[81]++;
                                }
                                // require the tag object to
                                // really fired the trigger by
                                // matching it in delR and 
                                // relative Pt to a trigger object
                                if( delR < 0.1 &&
                                    relPt > 0.8 &&
                                    relPt < 1.4 &&
                                    p.hasPixelSeed &&
                                    !tagged){
                                        tagged = true;
                                        //cout << "\t\t\t tag match!" << endl;
                                        vp_tag.push_back(&p);
                                        nCounts[80]++;
                                    }
                            }
                    }
                    
                    if(vp_tag.size()==1 && vp_probe.size()==1){
                        
                        
                        if(vtr4.size()>2){
                            nCounts[12]++;
                            cout << endl;
                            cout << Nnum << "\t" << vtr4.size() << endl;
                            //getchar();
                        }
                        
                        // if there is one trigger matching tag
                        // and one probe not matching the trigger:
                        
                        nCounts[28]++;
                        nCounts[27]++;
                        
                        E = 0.;
                        v.SetXYZ(0.,0.,0.);
                        lv.SetPxPyPzE(0.,0.,0.,0.);
                        
                        v = vp_tag[0]->p       + vp_probe[0]->p;
                        E = vp_tag[0]->p.Mag() + vp_probe[0]->p.Mag();
                        lv.SetVect(v);
                        lv.SetE(E);
                        
                        if( fabs(lv.M()-m_z)/m_z < 0.3){
                            nCounts[29]++;
                            if(vp_probe[0]->hasPixelSeed) nCounts[30]++;
                            if(!vp_probe[0]->hasPixelSeed) nCounts[31]++;
                            
                        }
                        
                        /*
                        cout << Nnum << "\t invariant mass = " << lv.M();
                        cout << "\t probe.pixelseed = " << vp_probe[0]->hasPixelSeed;
                        cout << "\t tag.pixelseed = " << vp_tag[0]->hasPixelSeed << endl;
                        cout << vp_tag[0]->p.Mag() << "\t" << vp_probe[0]->p.Mag()<<endl;
                        cout << vp_tag[0]->p.Pt() << "\t" << vp_probe[0]->p.Pt()<<endl;
                         getchar();
                        // */
                    }
                    
                    
                }
                
                //cout << "Nnum: " << Nnum;
                //cout << "\t tag.size=" << vp_tag.size();
                //cout << "\t probe.size=" << vp_probe.size() << endl;
                //if(vp_tag.size() != vp_probe.size()) nCounts[99]++;
                //getchar();
                if(nCounts[27] > 1) nCounts[67]++;
                
                if(nCounts[80] == 0) nCounts[60]++;
                if(nCounts[80] == 1) nCounts[61]++;
                if(nCounts[80] > 1) nCounts[62]++;
                if(nCounts[81] == 0) nCounts[64]++;
                if(nCounts[81] == 1) nCounts[65]++;
                if(nCounts[81] > 1) nCounts[66]++;
                if(nCounts[80] != nCounts[81]) nCounts[68]++;
                
            }
            
            
        } // photons.GetSize() == 2
        
        
        
        Nnum++;
    } // //////////////////////////////////////////////////////////////////////////////////////////
    
    cout << endl;
    cout << "Looped events: " << Nnum << endl;
    cout << "nTest: " << nTest << endl;
    cout << "fTest: " << fTest << endl;
    
    cout << "total Photons: " << nPhotons << endl;

/*    
    int breaker=0;
    string sc="";
    for(int ll=0;ll<100;ll++){
        cout << "nCounts[" << ll << "]: " << setfill('0') << setw(7) << nCounts[ll] << "\t";
        breaker++;
        if(breaker==4){ breaker = 0; cout << endl;}
    }
*/
    
    
    
    
    //cout << vp.size() << endl;
    
}

int f_my_singleelectron_MC_triggermatching(     map<string,TH1F> &h, 
                                                map<string,TH2F> &h2, 
                                                string dataset,
                                                int *nCounts,
                                                vector<Event> &events){
    
    cout << "f_my_singleelectron_MC_triggermatching()" << endl;
    cout << dataset << endl;
    
    // INITIALIZATION
    float m_z = 91.1876;            // Z mass in GeV
    float _pi = 3.141592653589793;  // 
    
    string s = "v1";
    string hname, h2name;
    
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
    
    // get tree and read out branches
    //TTree *t = reader.GetTree();
    //TObjArray *tobj = t->GetListOfBranches();
    
    //tobj->Print();
    
    //return 0;
    
    // initialize branches
    TTreeReaderArray<tree::Photon> photons(reader, "photons");
    TTreeReaderArray<tree::Jet> jets(reader, "jets");
    TTreeReaderArray<tree::Electron> electrons(reader, "electrons");
    TTreeReaderArray<tree::Muon> muons(reader, "muons");
    TTreeReaderArray<tree::Particle> genJets(reader, "genJets");
    TTreeReaderArray<tree::GenParticle> genParticles(reader, "genParticles");
    TTreeReaderArray<tree::IntermediateGenParticle> intermediateGenParticles(
                                                    reader, "intermediateGenParticles");
    TTreeReaderArray<tree::TriggerObject> triggerObjects(reader, "triggerObjects");
    TTreeReaderValue<Float_t> pu_weight(reader, "pu_weight");
    TTreeReaderValue<Char_t> mc_weight(reader, "mc_weight");
    TTreeReaderValue<Int_t> nGoodVertices(reader, "nGoodVertices");
    TTreeReaderValue<Int_t> nTracksPV(reader, "nTracksPV");
    TTreeReaderValue<ULong64_t> eventNo ( reader, "evtNo");
    TTreeReaderValue<UInt_t> runNo ( reader, "runNo");
    TTreeReaderValue<UInt_t> lumNo ( reader, "lumNo");
    
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
    
    string triggerLabels[] = {  "", //0
                                "empty", //1
                                "hltSingleEle22WP75GsfTrackIsoFilter", //2
                                "hltSingleEle22WPLooseGsfTrackIsoFilter", //3, 
                                                            //probably this is the label!
                                "hltEle27WPLooseGsfTrackIsoFilter" // 4
                            };
    
    Float_t selW = 1.;
    
    // initialize variables
    int     Nnum = 0,
            nTest = 0,
            nPhotons = 0;
    
    //int     nCounts[100] = {};
    
    float   fTest = 0.0;
    
    int ph=0, ph2=0;
    
    TVector3    v;          // vector
    TLorentzVector  lv;     // lorentz vector
    float E = 0.;
    v.SetXYZ(0.,0.,0.);
    lv.SetPxPyPzE(0.,0.,0.,0.);
    
    float   delR,
            delPt,
            relPt;
    
    string triggerLabel;
    
    Event evt;
    
    tree::Photon    selTag,
                    selProbe; // after all selections, save tag and probe here
    
    int tc, pc;
    
    vector<tree::Photon*>   selPhotons,
                            vp_tag,
                            vp_probe;
    
    vector<tree::TriggerObject*>    vtr1,
                                    vtr2,
                                    vtr3,
                                    vtr4;
    
    
    // random number generator to randomly choose the tag and probe objects
    // if there are more than one each
    TRandom3 *rg = new TRandom3(time(NULL));
    
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
        vtr1.clear();
        vtr2.clear();
        vtr3.clear();
        vtr4.clear();
        vp_probe.clear();
        vp_tag.clear();
        
        
        // GENERATOR MATCHING for all events
        // here the distributions for closure tests are created!
        for(auto& interGen: intermediateGenParticles){
            if(fabs(interGen.pdgId) == 23){ // Z boson
                
                for(auto& d: interGen.daughters){
                    if(abs(d.pdgId) == 11){ // electrons/positrons
                        v += d.p;
                        E += d.p.Mag();
                        
                        for(auto& p: photons){
                            if( p.p.Pt() > 40. &&
                                fabs(p.p.Eta()) < 1.4442){
                                
                                delR = d.p.DeltaR(p.p);
                                delPt = fabs(d.p.Pt()-p.p.Pt())/d.p.Pt();
                                relPt = p.p.Pt() / d.p.Pt();
                                
                                h2name = "plane_genEl_emObj";
                                h2[h2name].Fill(relPt, delR);
                                
                                //delRPt =    sqrt(delR*delR/(sdelR*sdelR)+
                                //            delPt*delPt/(sdelPt*sdelPt));
                                if( delR < 0.15 &&
                                    relPt > 0.8 &&
                                    relPt < 1.2){
                                    //delRPt < 1.){
                                    //cout << p.hasPixelSeed << "\t";
                                    //cout << "delR: " << delR << "\t delPt: " << delPt;
                                    //cout << "\t delRPt: " << delRPt << endl;
                                    
                                    nCounts[96]++;
                                    
                                    // DENOMINATOR
                                    hname = "gen_all_tot";
                                    h[hname].Fill(p.p.Pt());
                                    
                                    // NUMERATOR
                                    if(!p.hasPixelSeed){
                                        nCounts[97]++;
                                        
                                        hname = "gen_all_pas";
                                        h[hname].Fill(p.p.Pt());
                                        
                                        // N_{e->g}
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
                                        nCounts[98]++;
                                        
                                        
                                        // N_{e->e}
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
                    
                    //
                    lv.SetVect(v);
                    lv.SetE(E);
                    
                    hname = "massDist_interGen_daughters";
                    h[hname].Fill(lv.M());
                }
            }
        }
        
        E = 0.;
        v.SetXYZ(0.,0.,0.);
        lv.SetPxPyPzE(0.,0.,0.,0.);
        
        
        if(*SingleElectron_1) nCounts[0]++;
        if(*SingleElectron_2) nCounts[1]++;
        if(*SingleElectron_3) nCounts[2]++;
        if(*SingleElectron_4) nCounts[3]++;
        
        if(triggerObjects.GetSize() > 2){
            nCounts[7]++;
        }
        
/*        
        if(*SingleElectron_2){
            cout << "Event: " << Nnum << endl;
            for(auto& tr: triggerObjects){
            
                if( std::find(  tr.filterLabels.begin(), 
                                tr.filterLabels.end(), 
                                triggerLabels[2]) != tr.filterLabels.end()){
                            cout << "\t trigger found" << endl;
                        }
            }
            getchar();
        }
*/        
        // fill trigger objects
        for(auto& tr: triggerObjects){
            
            if( std::find(  tr.filterLabels.begin(), 
                            tr.filterLabels.end(), 
                            triggerLabels[1]) != tr.filterLabels.end()){
                        vtr1.push_back(&tr);
                    }
            if( std::find(  tr.filterLabels.begin(), 
                            tr.filterLabels.end(), 
                            triggerLabels[2]) != tr.filterLabels.end()){
                        vtr4.push_back(&tr); // ATTENTION: on mc, trigger2 is used; 
                                            // so here labeled as vtr4. might be confusing
                    }
            if( std::find(  tr.filterLabels.begin(), 
                            tr.filterLabels.end(), 
                            triggerLabels[3]) != tr.filterLabels.end()){
                        vtr3.push_back(&tr);
                    }
            if( std::find(  tr.filterLabels.begin(), 
                            tr.filterLabels.end(), 
                            triggerLabels[4]) != tr.filterLabels.end()){
                        vtr2.push_back(&tr); // see above, now tr4 is labeled as tr2
                    }
        }
        
        if(vtr4.size() > 2){
            nCounts[8]++;
            
            //cout << Nnum << "\t" << vtr4.size() << endl;
            //getchar();
            evt.Nnum = Nnum;
            evt.eventNo = *eventNo;
            evt.runNo = *runNo;
            evt.lumNo = *lumNo;
            evt.dataset = dataset;
            events.push_back(evt);
        }
        
        hname = "trigger1_size";
        h[hname].Fill(vtr1.size());
        hname = "trigger2_size";
        h[hname].Fill(vtr2.size());
        hname = "trigger3_size";
        h[hname].Fill(vtr3.size());
        hname = "trigger4_size";
        h[hname].Fill(vtr4.size());
        
        // ************************************************************************ tag and probe:
        
        //
        //if(photons.GetSize() == 2){
            nCounts[5]++;
            
            //for(auto& p: photons){
            //    v = v + p.p;
            //    E += p.p.Mag();
            //}
            //lv.SetVect(v);
            //lv.SetE(E);
            
            if(*SingleElectron_1){
                hname="trigger0";
                h[hname].Fill(lv.M());
            }
            if(*SingleElectron_4){
                hname="trigger3";
                h[hname].Fill(lv.M());
            }
            if(*SingleElectron_3){
                hname="trigger2";
                h[hname].Fill(lv.M());
            }
            if(*SingleElectron_2){
                hname="trigger1";
                h[hname].Fill(lv.M());
                
                nCounts[20]++;
                
                nCounts[80] = 0;
                nCounts[81] = 0;
                // Preselection of objects that have fired this trigger
                // and objects that dont.
                // These will be used to assign the objects as the tag
                // or probe object, respectively.
                //
                bool    tagged = false,
                        probed = false;
                
                
                for(auto& tr: vtr4){
                    for(auto& p: photons){
                        h2name = "plane_trObj_emObj";
                        h2[h2name].Fill(p.p.Pt()/tr->p.Pt(), tr->p.DeltaR(p.p));
                    }
                }
                
                // preselect all photon objects
                // with Pt and Eta criteria
                selPhotons.clear();
                for(auto& p: photons){
                    if( p.p.Pt() > 40 &&
                        fabs(p.p.Eta()) < 1.4442){
                            selPhotons.push_back(&p);
                    }
                }
                
                // there have to be at least two photon objects fulfilling the criteria,
                // otherwise there would be no chance to find a tag and a probe object.
                if(selPhotons.size() >= 2){
                    nCounts[90]++;
                    nCounts[27]=0;
                    
                    vp_tag.clear();
                    vp_probe.clear();
                    
                    // loop the trigger objects:
                    //for(auto& t: vtr4){
                    
                    /// cout << "vtr4.size()=" << vtr4.size() << endl;
                    for(std::vector<tree::TriggerObject*>::iterator t = vtr4.begin();
                                                                    t!= vtr4.end();
                                                                    /* t++ */
                                                                    ){
                        //cout << "\t\t trigger obj.Pt: " << t->p.Pt() << endl;
                        
                        /*cout << "\t trigger: " << t->p.Pt() << endl;
                        for(auto& tl: t->filterLabels){
                            cout << "\t\t " << tl << endl;
                        } // */
                        ///cout << "\t tr.Pt= " << (*t)->p.Pt() << endl;
                        
                        tagged = false;
                        // loop the em candidates for each trigger object:
                        
                        nCounts[64]=0;
                        for(std::vector<tree::Photon*>::iterator p  = selPhotons.begin();
                                                                 p != selPhotons.end();
                                /* p++ */  ){
                            
                            delR = (*t)->p.DeltaR((*p)->p);
                            relPt = (*p)->p.Pt()/(*t)->p.Pt();
                            //cout << "\t " << delR << "\t " << relPt << endl;
                            
                            // require the tag object to
                            // really fired the trigger by
                            // matching it in delR and 
                            // relative Pt to a trigger object
                            if( delR < 0.1 &&
                                relPt > 0.8 &&
                                relPt < 1.4 &&
                                (*p)->hasPixelSeed){
                                    
                                    //cout << "\t\t\t tag match!" << endl;
                                    
                                    // if tag criteria fulfilled, push object to tag vector
                                    vp_tag.push_back((*p));
                                    
                                    // then erase it from selected em objects vector
                                    p = selPhotons.erase(p);
                                    
                                    // also erase the trigger object from the trigger
                                    // vector; in case the probe object is checked later
                                    // if it also could be a tag, it should never
                                    // be possible to match the same trigger object
                                    // as the initial tag object.
                                    //t = vtr4.erase(t);
                                    //cout << "\t\t erased! next pt " << (*t)->p.Pt() << endl;
                                    //tagged = true;
                                    
                                    //cout << "\t\t\t\t after del: " << (*p)->p.Pt() << endl;
                                    //p++;
                                    nCounts[80]++;
                                    
                                    nCounts[64]++; // counter to check if trigger
                                                    // objects match to more than one
                                                    // photon object (should not!)
                                }
                            else{
                                    //vp_probe.push_back((*p));
                                    //p = selPhotons.erase(p);
                                    
                                    p++;
                            }
                        }
                        
                        if(nCounts[64]==1){
                            nCounts[66]++;
                            ///cout << "\t\t now erase! pt " << (*t)->p.Pt() << endl;
                            ///t = vtr4.erase(t);
                            ///cout << "\t\t erased! next pt " << (*t)->p.Pt() << endl;
                            
                            }
                        if(nCounts[64]>1){ nCounts[65]++; }
                        if(nCounts[64]==0){ nCounts[67]++; } 
                        ///getchar();
                         t++; // next trigger object
                    } // triggerObj loop
                    
                    
                    // if more than 1 tag candidate, choose randomly
                    
                    if(vp_tag.size() > 0){ // only do the calculations if at least one
                                            // tag has been found
                        
                        nCounts[12]++;
                        
                        tc = rg->Integer(vp_tag.size());// random integer in the vector length range
                        selTag = *(vp_tag[tc]);         // copy this random chosen tag candidate to selTag
                        vp_tag.erase(vp_tag.begin()+tc);// erase the chosen one
                        //cout << "tagchoice: " << tc << "\t" << selTag.p.Pt() << endl;
                        
                        
                        // push the remaining tags that have not 
                        // been chosen back to selPhotons
                        for(auto& t: vp_tag){
                            selPhotons.push_back(t);
                        }
                        
                        if(selPhotons.size() > 0){ // == 0 never happens here anymore!
                                                // the above selection exclude this case
                                                // counts[12] = counts[14] always
                            nCounts[14]++;
                            
                            pc = rg->Integer(selPhotons.size());
                            selProbe = *(selPhotons[pc]);
                            //selPhotons.erase(selPhotons.begin()+tc); // not nescessary
                            //cout << "probechoice: " << pc << "\t" << selProbe.p.Pt() << endl;
                            
                            // here, one tag and one probe have been selected;
                            hname = "delR_selTag_selProbe";
                            h[hname].Fill(selTag.p.DeltaR(selProbe.p));
                            h2name = "plane_PhiEta_selTag_selProbe";
                            h2[h2name].Fill(    fabs(selTag.p.Phi()-selProbe.p.Phi()),
                                                fabs(selTag.p.Eta()-selProbe.p.Eta())
                                                );
                            
                            // On DY, the two electrons should be back to back.
                            // Require this for tag and probe:
                            if( fabs(selTag.p.Phi()-selProbe.p.Phi()) > 0.9*_pi &&
                                fabs(selTag.p.Phi()-selProbe.p.Phi()) < 1.1*_pi){
                                    
                                    
                                    // calculate lorentzvector from both:
                                    v = selTag.p + selProbe.p;
                                    E = selTag.p.Mag() + selProbe.p.Mag();
                                    lv.SetVect(v);
                                    lv.SetE(E);
                                    
                                    // now check probe for pixelseed and then assign sevent to
                                    // ee or egamma sample:
                                    if(!selProbe.hasPixelSeed){
                                        nCounts[44]++;
                                        
                                        // egamma sample
                                        h2name = "tnp_eg";
                                        h2[h2name].Fill(lv.M(), selProbe.p.Pt());
                                        
                                        h2name = "tnp_eg_nvtx";
                                        h2[h2name].Fill(lv.M(), *nGoodVertices);
                                        h2name = "tnp_eg_eta";
                                        h2[h2name].Fill(lv.M(), fabs(selProbe.p.Eta()));
                                        h2name = "tnp_eg_ntracks";
                                        h2[h2name].Fill(lv.M(), *nTracksPV);
                                        
                                    }
                                    
                                    if(selProbe.hasPixelSeed){
                                        nCounts[45]++;
                                        
                                        // ee sample
                                        h2name = "tnp_ee";
                                        h2[h2name].Fill(lv.M(), selProbe.p.Pt());
                                        h2name = "tnp_ee_nvtx";
                                        h2[h2name].Fill(lv.M(), *nGoodVertices);
                                        h2name = "tnp_ee_eta";
                                        h2[h2name].Fill(lv.M(), fabs(selProbe.p.Eta()));
                                        h2name = "tnp_ee_ntracks";
                                        h2[h2name].Fill(lv.M(), *nTracksPV);
                                        
                                        // add 2 times
                                        /*
                                        h2name = "tnp_ee";
                                        h2[h2name].Fill(lv.M(), selProbe.p.Pt());
                                        h2name = "tnp_ee_nvtx";
                                        h2[h2name].Fill(lv.M(), *nGoodVertices);
                                        h2name = "tnp_ee_eta";
                                        h2[h2name].Fill(lv.M(), fabs(selProbe.p.Eta()));
                                        h2name = "tnp_ee_ntracks";
                                        h2[h2name].Fill(lv.M(), *nTracksPV);
                                        */
                                    }
                                    
                                    // some counter variables
                                    if( fabs(lv.M()-m_z)/m_z<0.05){
                                        if(selProbe.hasPixelSeed) nCounts[32]++;
                                        if(!selProbe.hasPixelSeed) nCounts[33]++;
                                        nCounts[26]++;
                                    }
                                    if( fabs(lv.M()-m_z)/m_z<0.1){
                                        if(selProbe.hasPixelSeed) nCounts[24]++;
                                        if(!selProbe.hasPixelSeed) nCounts[25]++;
                                        nCounts[26]++;
                                    }
                                    if( fabs(lv.M()-m_z)/m_z<0.3){
                                        if(selProbe.hasPixelSeed) nCounts[28]++;
                                        if(!selProbe.hasPixelSeed) nCounts[29]++;
                                        nCounts[30]++;
                                    }
                                    
                                    // Check if probe object fullfills tag criteria,
                                    // which are:   does it match to a trigger object
                                    //              and possesses a pixel seed?
                                    // If yes, count this again to the electron electron
                                    // sample, due to cominatorical reasons.
                                    // The tag object always fulfills the probe criteria.
                                    if(selProbe.hasPixelSeed){
                                        nCounts[68]=0;
                                        for(auto& t: vtr4){ // loop trigger objects
                                            delR = t->p.DeltaR(selProbe.p);
                                            relPt = selProbe.p.Pt()/t->p.Pt();
                                            if( delR < 0.1 &&
                                                relPt > 0.8 &&
                                                relPt < 1.4){
                                                    
                                                    nCounts[68]++; //matching count per event, 
                                                                    // should be = 1.
                                                    
                                                    if(fabs(lv.M()-m_z)/m_z<0.05) nCounts[32]++;
                                                    if(fabs(lv.M()-m_z)/m_z<0.1)  nCounts[24]++;
                                                    if(fabs(lv.M()-m_z)/m_z<0.3)  nCounts[28]++;
                                                    
                                                    nCounts[46]++;
                                                    
                                                    /// is probe could also be tag
                                                    /// add again to ee sample
                                                    
                                                    h2name = "tnp_ee";
                                                    h2[h2name].Fill(lv.M(), selProbe.p.Pt());
                                                    h2name = "tnp_ee_nvtx";
                                                    h2[h2name].Fill(lv.M(), *nGoodVertices);
                                                    h2name = "tnp_ee_eta";
                                                    h2[h2name].Fill(lv.M(), fabs(selProbe.p.Eta()));
                                                    h2name = "tnp_ee_ntracks";
                                                    h2[h2name].Fill(lv.M(), *nTracksPV);
                                                    
                                                }
                                        }
                                        // this should never be the case!
                                        if(nCounts[68]>1) nCounts[69]++;
                                    }
                            } // back to back
                            
                        }
                    }
                    else{
                        //cout << endl;
                        //cout << Nnum << endl;
                        nCounts[13]++;
                    }
                    
/*            
                    cout << endl;
                    cout << Nnum << endl;
                    cout << "\t tag:\t" << selTag.p.Pt() << "\t" << selTag.p.Eta() << endl;
                    cout << "\t probe:\t" << selProbe.p.Pt() << "\t" << selProbe.p.Eta() << endl;
                    cout << "\t delR= " << selTag.p.DeltaR(selProbe.p) << "\t relPt=tag/probe= " << selTag.p.Pt()/selProbe.p.Pt()<<endl;
                    cout << endl;
                    //cout << "time(NULL)=" << time(NULL) << endl;
                    getchar();
*/
                
                } // selPhotons.size() == 2
                
            }
            
        //} // photons.GetSize() == 2
        
        Nnum++;
    } // //////////////////////////////////////////////////////////////////////////////////////////
    
    cout << endl;
    cout << "Looped events: " << Nnum << endl;
    cout << "nTest: " << nTest << endl;
    cout << "fTest: " << fTest << endl;
    
    cout << "total Photons: " << nPhotons << endl;
    
    
    
    //cout << vp.size() << endl;
    return 0;
}

int f_my_singleelectron_triggermatching(        map<string,TH1F> &h, 
                                                map<string,TH2F> &h2, 
                                                string dataset,
                                                int *nCounts,
                                                vector<Event> &events){
    
    cout << "f_my_singleelectron_triggermatching()" << endl;
    cout << dataset << endl;
    
    // INITIALIZATION
    float m_z = 91.1876; // Z mass in GeV
    
    string s = "v1";
    string hname, h2name;
    
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
    
    // get tree and read out branches
    //TTree *t = reader.GetTree();
    //TObjArray *tobj = t->GetListOfBranches();
    
    //tobj->Print();
    
    //return 0;
    
    // initialize branches
    TTreeReaderArray<tree::Photon> photons(reader, "photons");
    TTreeReaderArray<tree::Jet> jets(reader, "jets");
    TTreeReaderArray<tree::Electron> electrons(reader, "electrons");
    TTreeReaderArray<tree::Muon> muons(reader, "muons");
    TTreeReaderArray<tree::Particle> genJets(reader, "genJets");
    TTreeReaderArray<tree::GenParticle> genParticles(reader, "genParticles");
    TTreeReaderArray<tree::TriggerObject> triggerObjects(reader, "triggerObjects");
    TTreeReaderValue<Float_t> pu_weight(reader, "pu_weight");
    TTreeReaderValue<Char_t> mc_weight(reader, "mc_weight");
    TTreeReaderValue<Int_t> nGoodVertices(reader, "nGoodVertices");
    TTreeReaderValue<Int_t> nTracksPV(reader, "nTracksPV");
    TTreeReaderValue<ULong64_t> eventNo ( reader, "evtNo");
    TTreeReaderValue<UInt_t> runNo ( reader, "runNo");
    TTreeReaderValue<UInt_t> lumNo ( reader, "lumNo");
    
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
    
    
    
    Float_t selW = 1.;
    
    // initialize variables
    int     Nnum = 0,
            nTest = 0,
            nPhotons = 0;
    
    //int     nCounts[100] = {};
    
    float   fTest = 0.0;
    
    int ph=0, ph2=0;
    
    
    
    TVector3    v;          // vector
    TLorentzVector  lv;     // lorentz vector
    float E = 0.;
    v.SetXYZ(0.,0.,0.);
    lv.SetPxPyPzE(0.,0.,0.,0.);
    
    float   delR,
            relPt;
    
    string triggerLabel;
    
    Event evt;
    
    tree::Photon    selTag,
                    selProbe; // after all selections, save tag and probe here
    
    int tc, pc;
    
    vector<tree::Photon*>   selPhotons,
                            vp_tag,
                            vp_probe;
    
    vector<tree::TriggerObject*>    vtr1,
                                    vtr2,
                                    vtr3,
                                    vtr4;
    string triggerLabels[] = {  "", //0
                                "empty", //1
                                "empty", //2
                                "hltSingleEle22WPLooseGsfTrackIsoFilter", //3, 
                                                            //probably this is the label!
                                "hltEle27WPLooseGsfTrackIsoFilter" // 4
                            };
    
    // random number generator to randomly choose the tag and probe objects
    // if there are more than one each
    TRandom3 *rg = new TRandom3(time(NULL));
    
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
        vtr1.clear();
        vtr2.clear();
        vtr3.clear();
        vtr4.clear();
        vp_probe.clear();
        vp_tag.clear();
        
        if(*SingleElectron_1) nCounts[0]++;
        if(*SingleElectron_2) nCounts[1]++;
        if(*SingleElectron_3) nCounts[2]++;
        if(*SingleElectron_4) nCounts[3]++;
        
        if(triggerObjects.GetSize() > 2){
            nCounts[7]++;
        }
        
        // fill trigger objects
        for(auto& tr: triggerObjects){
            
            if( std::find(  tr.filterLabels.begin(), 
                            tr.filterLabels.end(), 
                            triggerLabels[1]) != tr.filterLabels.end()){
                        vtr1.push_back(&tr);
                    }
            if( std::find(  tr.filterLabels.begin(), 
                            tr.filterLabels.end(), 
                            triggerLabels[2]) != tr.filterLabels.end()){
                        vtr2.push_back(&tr);
                    }
            if( std::find(  tr.filterLabels.begin(), 
                            tr.filterLabels.end(), 
                            triggerLabels[3]) != tr.filterLabels.end()){
                        vtr3.push_back(&tr);
                    }
            if( std::find(  tr.filterLabels.begin(), 
                            tr.filterLabels.end(), 
                            triggerLabels[4]) != tr.filterLabels.end()){
                        vtr4.push_back(&tr);
                    }
        }
        
        if(vtr4.size() > 2){
            nCounts[8]++;
            
            //cout << Nnum << "\t" << vtr4.size() << endl;
            //getchar();
            evt.Nnum = Nnum;
            evt.eventNo = *eventNo;
            evt.runNo = *runNo;
            evt.lumNo = *lumNo;
            evt.dataset = dataset;
            events.push_back(evt);
        }
        
        hname = "trigger1_size";
        h[hname].Fill(vtr1.size());
        hname = "trigger2_size";
        h[hname].Fill(vtr2.size());
        hname = "trigger3_size";
        h[hname].Fill(vtr3.size());
        hname = "trigger4_size";
        h[hname].Fill(vtr4.size());
        
        // ************************************************************************ tag and probe:
        
        //
        //if(photons.GetSize() == 2){
            nCounts[5]++;
            
            //for(auto& p: photons){
            //    v = v + p.p;
            //    E += p.p.Mag();
            //}
            //lv.SetVect(v);
            //lv.SetE(E);
            
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
                
                nCounts[20]++;
                
                nCounts[80] = 0;
                nCounts[81] = 0;
                // Preselection of objects that have fired this trigger
                // and objects that dont.
                // These will be used to assign the objects as the tag
                // or probe object, respectively.
                //
                bool    tagged = false,
                        probed = false;
                
                
                for(auto& tr: vtr4){
                    for(auto& p: photons){
                        h2name = "plane_trObj_emObj";
                        h2[h2name].Fill(p.p.Pt()/tr->p.Pt(), tr->p.DeltaR(p.p));
                    }
                }
                
                // preselect all photon objects
                // with Pt and Eta criteria
                selPhotons.clear();
                for(auto& p: photons){
                    if( p.p.Pt() > 40 &&
                        fabs(p.p.Eta()) < 1.4442){
                            selPhotons.push_back(&p);
                    }
                }
                
                // there have to be at least two photon objects fulfilling the criteria,
                // otherwise there would be no chance to find a tag and a probe object.
                if(selPhotons.size() >= 2){
                    nCounts[90]++;
                    nCounts[27]=0;
                    
                    vp_tag.clear();
                    vp_probe.clear();
                    
                    // loop the trigger objects:
                    //for(auto& t: vtr4){
                    
                    /// cout << "vtr4.size()=" << vtr4.size() << endl;
                    for(std::vector<tree::TriggerObject*>::iterator t = vtr4.begin();
                                                                    t!= vtr4.end();
                                                                    /* t++ */
                                                                    ){
                        //cout << "\t\t trigger obj.Pt: " << t->p.Pt() << endl;
                        
                        /*cout << "\t trigger: " << t->p.Pt() << endl;
                        for(auto& tl: t->filterLabels){
                            cout << "\t\t " << tl << endl;
                        } // */
                        ///cout << "\t tr.Pt= " << (*t)->p.Pt() << endl;
                        
                        tagged = false;
                        // loop the em candidates for each trigger object:
                        
                        nCounts[64]=0;
                        for(std::vector<tree::Photon*>::iterator p  = selPhotons.begin();
                                                                 p != selPhotons.end();
                                /* p++ */  ){
                            
                            delR = (*t)->p.DeltaR((*p)->p);
                            relPt = (*p)->p.Pt()/(*t)->p.Pt();
                            //cout << "\t " << delR << "\t " << relPt << endl;
                            
                            // require the tag object to
                            // really fired the trigger by
                            // matching it in delR and 
                            // relative Pt to a trigger object
                            if( delR < 0.1 &&
                                relPt > 0.8 &&
                                relPt < 1.4 &&
                                (*p)->hasPixelSeed){
                                    
                                    //cout << "\t\t\t tag match!" << endl;
                                    
                                    // if tag criteria fulfilled, push object to tag vector
                                    vp_tag.push_back((*p));
                                    
                                    // then erase it from selected em objects vector
                                    p = selPhotons.erase(p);
                                    
                                    // also erase the trigger object from the trigger
                                    // vector; in case the probe object is checked later
                                    // if it also could be a tag, it should never
                                    // be possible to match the same trigger object
                                    // as the initial tag object.
                                    //t = vtr4.erase(t);
                                    //cout << "\t\t erased! next pt " << (*t)->p.Pt() << endl;
                                    //tagged = true;
                                    
                                    //cout << "\t\t\t\t after del: " << (*p)->p.Pt() << endl;
                                    //p++;
                                    nCounts[80]++;
                                    
                                    nCounts[64]++; // counter to check if trigger
                                                    // objects match to more than one
                                                    // photon object (should not!)
                                }
                            else{
                                    //vp_probe.push_back((*p));
                                    //p = selPhotons.erase(p);
                                    
                                    p++;
                            }
                        }
                        
                        if(nCounts[64]==1){
                            nCounts[66]++;
                            ///cout << "\t\t now erase! pt " << (*t)->p.Pt() << endl;
                            ///t = vtr4.erase(t);
                            ///cout << "\t\t erased! next pt " << (*t)->p.Pt() << endl;
                            
                            }
                        if(nCounts[64]>1){ nCounts[65]++; }
                        if(nCounts[64]==0){ nCounts[67]++; } 
                        ///getchar();
                         t++; // next trigger object
                    } // triggerObj loop
                    
                    
                    // if more than 1 tag candidate, choose randomly
                    
                    if(vp_tag.size() > 0){ // only do the calculations if at least one
                                            // tag has been found
                        
                        nCounts[12]++;
                        
                        tc = rg->Integer(vp_tag.size());// random integer in the vector length range
                        selTag = *(vp_tag[tc]);         // copy this to selTag
                        vp_tag.erase(vp_tag.begin()+tc);// erase the chosen one
                        //cout << "tagchoice: " << tc << "\t" << selTag.p.Pt() << endl;
                        
                        
                        // push the remaining tags that have not 
                        // been chosen back to selPhotons
                        for(auto& t: vp_tag){
                            selPhotons.push_back(t);
                        }
                        
                        if(selPhotons.size() > 0){ // == 0 never happens here anymore!
                                                // the above selection exclude this case
                                                // counts[12] = counts[14] always
                            nCounts[14]++;
                            
                            pc = rg->Integer(selPhotons.size());
                            selProbe = *(selPhotons[pc]);
                            //selPhotons.erase(selPhotons.begin()+tc); // not nescessary
                            //cout << "probechoice: " << pc << "\t" << selProbe.p.Pt() << endl;
                            
                            // here, one tag and one probe have been selected;
                            hname = "delR_selTag_selProbe";
                            h[hname].Fill(selTag.p.DeltaR(selProbe.p));
                            h2name = "plane_PhiEta_selTag_selProbe";
                            h2[h2name].Fill(    fabs(selTag.p.Phi()-selProbe.p.Phi()),
                                                fabs(selTag.p.Eta()-selProbe.p.Eta())
                                                );
                            
                            // calculate lorentzvector from both:
                            v = selTag.p + selProbe.p;
                            E = selTag.p.Mag() + selProbe.p.Mag();
                            lv.SetVect(v);
                            lv.SetE(E);
                            // now check probe for pixelseed and then assign sevent to
                            // ee or egamma sample:
                            if(!selProbe.hasPixelSeed){
                                nCounts[44]++;
                                
                                // egamma sample
                                h2name = "tnp_eg";
                                h2[h2name].Fill(lv.M(), selProbe.p.Pt());
                                
                                h2name = "tnp_eg_nvtx";
                                h2[h2name].Fill(lv.M(), *nGoodVertices);
                                h2name = "tnp_eg_eta";
                                h2[h2name].Fill(lv.M(), fabs(selProbe.p.Eta()));
                                h2name = "tnp_eg_ntracks";
                                h2[h2name].Fill(lv.M(), *nTracksPV);
                                
                            }
                            
                            if(selProbe.hasPixelSeed){
                                nCounts[45]++;
                                
                                // ee sample
                                h2name = "tnp_ee";
                                h2[h2name].Fill(lv.M(), selProbe.p.Pt());
                                h2name = "tnp_ee_nvtx";
                                h2[h2name].Fill(lv.M(), *nGoodVertices);
                                h2name = "tnp_ee_eta";
                                h2[h2name].Fill(lv.M(), fabs(selProbe.p.Eta()));
                                h2name = "tnp_ee_ntracks";
                                h2[h2name].Fill(lv.M(), *nTracksPV);
                                
                                // add 2 times
                                /*
                                h2name = "tnp_ee";
                                h2[h2name].Fill(lv.M(), selProbe.p.Pt());
                                h2name = "tnp_ee_nvtx";
                                h2[h2name].Fill(lv.M(), *nGoodVertices);
                                h2name = "tnp_ee_eta";
                                h2[h2name].Fill(lv.M(), fabs(selProbe.p.Eta()));
                                h2name = "tnp_ee_ntracks";
                                h2[h2name].Fill(lv.M(), *nTracksPV);
                                */
                            }
                            
                            // some counter variables
                            if( fabs(lv.M()-m_z)/m_z<0.05){
                                if(selProbe.hasPixelSeed) nCounts[32]++;
                                if(!selProbe.hasPixelSeed) nCounts[33]++;
                                nCounts[26]++;
                            }
                            if( fabs(lv.M()-m_z)/m_z<0.1){
                                if(selProbe.hasPixelSeed) nCounts[24]++;
                                if(!selProbe.hasPixelSeed) nCounts[25]++;
                                nCounts[26]++;
                            }
                            if( fabs(lv.M()-m_z)/m_z<0.3){
                                if(selProbe.hasPixelSeed) nCounts[28]++;
                                if(!selProbe.hasPixelSeed) nCounts[29]++;
                                nCounts[30]++;
                            }
                            
                            
                            
                            // Check if probe object fullfills tag criteria,
                            // which are:   does it match to a trigger object
                            //              and possesses a pixel seed?
                            // If yes, count this again to the electron electron
                            // sample, due to cominatorical reasons.
                            // The tag object always fulfills the probe criteria.
                            if(selProbe.hasPixelSeed){
                                nCounts[68]=0;
                                for(auto& t: vtr4){
                                    delR = t->p.DeltaR(selProbe.p);
                                    relPt = selProbe.p.Pt()/t->p.Pt();
                                    if( delR < 0.1 &&
                                        relPt > 0.8 &&
                                        relPt < 1.4){
                                            
                                            nCounts[68]++; //matching count per event, 
                                                            // should be = 1.
                                            
                                            if(fabs(lv.M()-m_z)/m_z<0.05) nCounts[32]++;
                                            if(fabs(lv.M()-m_z)/m_z<0.1) nCounts[24]++;
                                            if(fabs(lv.M()-m_z)/m_z<0.3) nCounts[28]++;
                                            
                                            nCounts[46]++;
                                                    
                                            /// is probe could also be tag
                                            /// add again to ee sample
                                            
                                            h2name = "tnp_ee";
                                            h2[h2name].Fill(lv.M(), selProbe.p.Pt());
                                            h2name = "tnp_ee_nvtx";
                                            h2[h2name].Fill(lv.M(), *nGoodVertices);
                                            h2name = "tnp_ee_eta";
                                            h2[h2name].Fill(lv.M(), fabs(selProbe.p.Eta()));
                                            h2name = "tnp_ee_ntracks";
                                            h2[h2name].Fill(lv.M(), *nTracksPV);
                                    
                                            
                                        }
                                }
                                // this should never be the case!
                                if(nCounts[68]>1) nCounts[69]++;
                            }
                            
                        }
                    }
                    else{
                        //cout << endl;
                        //cout << Nnum << endl;
                        nCounts[13]++;
                    }
                    
                    
                    
/*            
                    cout << endl;
                    cout << Nnum << endl;
                    cout << "\t tag:\t" << selTag.p.Pt() << "\t" << selTag.p.Eta() << endl;
                    cout << "\t probe:\t" << selProbe.p.Pt() << "\t" << selProbe.p.Eta() << endl;
                    cout << "\t delR= " << selTag.p.DeltaR(selProbe.p) << "\t relPt=tag/probe= " << selTag.p.Pt()/selProbe.p.Pt()<<endl;
                    cout << endl;
                    //cout << "time(NULL)=" << time(NULL) << endl;
                    getchar();
*/
                
                } // selPhotons.size() == 2
                
                
                
            }
            
            
        //} // photons.GetSize() == 2
        
        
        
        Nnum++;
    } // //////////////////////////////////////////////////////////////////////////////////////////
    
    cout << endl;
    cout << "Looped events: " << Nnum << endl;
    cout << "nTest: " << nTest << endl;
    cout << "fTest: " << fTest << endl;
    
    cout << "total Photons: " << nPhotons << endl;
    
    
    
    //cout << vp.size() << endl;
    return 0;
}

int f_my_muon_bkg(                              map<string,TH1F> &h, 
                                                map<string,TH2F> &h2, 
                                                string dataset,
                                                int *nCounts,
                                                vector<Event> &events){
    
    cout << "f_my_muon_bkg()" << endl;
    cout << dataset << endl;
    
    // INITIALIZATION
    float m_z = 91.1876; // Z mass in GeV
    
    string s = "v1";
    string hname, h2name;
    
    // **********************************************************************
    // fakerate_genmatch()
    //hname = "f_genmatch_tot";
    //h[hname] = "
    
    // 
    
    
    //h2name = "f_zpeak_tot_eta";
    //h2[h2name] = TH2F((h2name + "_" + s).c_str(),";m [GeV];|#eta|",
    //                    200,0,200,
    //                    60,0,6);
    // 
    
    // DATA FILE
    TFile *f = new TFile(dataset.c_str());
    TTreeReader reader("TreeWriter/eventTree", f);
    
    // get tree and read out branches
    //TTree *t = reader.GetTree();
    //TObjArray *tobj = t->GetListOfBranches();
    
    //tobj->Print();
    
    //return 0;
    
    // initialize branches
    TTreeReaderArray<tree::Photon> photons(reader, "photons");
    TTreeReaderArray<tree::Jet> jets(reader, "jets");
    TTreeReaderArray<tree::Electron> electrons(reader, "electrons");
    TTreeReaderArray<tree::Muon> muons(reader, "muons");
    TTreeReaderArray<tree::Particle> genJets(reader, "genJets");
    TTreeReaderArray<tree::GenParticle> genParticles(reader, "genParticles");
    TTreeReaderArray<tree::TriggerObject> triggerObjects(reader, "triggerObjects");
    TTreeReaderValue<Float_t> pu_weight(reader, "pu_weight");
    TTreeReaderValue<Char_t> mc_weight(reader, "mc_weight");
    TTreeReaderValue<Int_t> nGoodVertices(reader, "nGoodVertices");
    TTreeReaderValue<Int_t> nTracksPV(reader, "nTracksPV");
    TTreeReaderValue<ULong64_t> eventNo ( reader, "evtNo");
    TTreeReaderValue<UInt_t> runNo ( reader, "runNo");
    TTreeReaderValue<UInt_t> lumNo ( reader, "lumNo");
    
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
    
    // muonEG trigger
    TTreeReaderValue<Bool_t> muEG_t1 ( reader,
        "HLT_Mu12_Photon25_CaloIdL_v");
    // formerly HLT Mu22 Photon22 CaloIdL v (used by Yutaro), see 
    // https://indico.cern.ch/event/336052/contributions/1729780/attachments/656791/903017/DisplacedSUSY_Trigger_Proposal.pdf
    // slide 9:
    TTreeReaderValue<Bool_t> muEG_t2 ( reader,
        "HLT_Mu38NoFiltersNoVtx_Photon38_CaloIdL_v");
    TTreeReaderValue<Bool_t> muEG_t3 ( reader,
        "HLT_Mu23NoFiltersNoVtx_Photon23_CaloIdL_v");
    TTreeReaderValue<Bool_t> muEG_t4 ( reader,
        "HLT_Mu42NoFiltersNoVtx_Photon42_CaloIdL_v");
    
    // single muon trigger
    // Trigger with a single Isolated Muon identical to a muon leg for mu+tau triggers; seeded by prescaled L1 seed:
    TTreeReaderValue<Bool_t> singleMu_t1 ( reader,
        "HLT_IsoMu17_eta2p1_v");
    // prescaled non-iso trigger for iso and fake studies:
    TTreeReaderValue<Bool_t> singleMu_t2 ( reader,
        "HLT_Mu24_eta2p1_v");
    TTreeReaderValue<Bool_t> singleMu_t3 ( reader,
        "HLT_IsoMu20_v");
    // prescaled non-iso trigger for iso and fake studies:
    TTreeReaderValue<Bool_t> singleMu_t4 ( reader,
        "HLT_TkMu24_eta2p1_v");
    
    
    Float_t selW = 1.;
    
    // initialize variables
    int     Nnum = 0,
            nTest = 0,
            nPhotons = 0;
    
    //int     nCounts[100] = {};
    
    float   fTest = 0.0;
    
    int ph=0, ph2=0;
    
    
    
    TVector3    v;          // vector
    TLorentzVector  lv;     // lorentz vector
    float E = 0.;
    v.SetXYZ(0.,0.,0.);
    lv.SetPxPyPzE(0.,0.,0.,0.);
    
    float   delR,
            relPt;
    
    string triggerLabel;
    
    Event evt;
    
    tree::Photon    selTag,
                    selProbe; // after all selections, save tag and probe here
    
    int tc, pc;
    
    vector<tree::Photon*>   selPhotons,
                            selMuons,
                            vp_tag,
                            vp_probe;
    
    vector<tree::TriggerObject*>    vtr1,
                                    vtr2,
                                    vtr3,
                                    vtr4;
    string triggerLabels[] = {  "", //0
                                "empty", //1
                                "empty", //2
                                "hltSingleEle22WPLooseGsfTrackIsoFilter", //3, 
                                                            //probably this is the label!
                                "hltEle27WPLooseGsfTrackIsoFilter" // 4
                            };
    
    // random number generator to randomly choose the tag and probe objects
    // if there are more than one each
    TRandom3 *rg = new TRandom3(time(NULL));
    
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // loop the events
    while(reader.Next()){
        
        // counter output
        if((Nnum % 100000) == 0){
        //    cout << "\revent " << Nnum << " done... \t" << flush;
        }
        
        E = 0.;
        v.SetXYZ(0.,0.,0.);
        lv.SetPxPyPzE(0.,0.,0.,0.);
        vtr1.clear();
        vtr2.clear();
        vtr3.clear();
        vtr4.clear();
        vp_probe.clear();
        vp_tag.clear();
        
        // number of trigger fired events for each trigger
        if(*muEG_t1) nCounts[0]++;
        if(*muEG_t2) nCounts[1]++;
        if(*muEG_t3) nCounts[2]++;
        if(*muEG_t4) nCounts[3]++;
        
        if(triggerObjects.GetSize() > 2){
            nCounts[7]++;
        }
        
        // fill trigger objects
        for(auto& tr: triggerObjects){
            
            if( std::find(  tr.filterLabels.begin(), 
                            tr.filterLabels.end(), 
                            triggerLabels[1]) != tr.filterLabels.end()){
                        vtr1.push_back(&tr);
                    }
            if( std::find(  tr.filterLabels.begin(), 
                            tr.filterLabels.end(), 
                            triggerLabels[2]) != tr.filterLabels.end()){
                        vtr2.push_back(&tr);
                    }
            if( std::find(  tr.filterLabels.begin(), 
                            tr.filterLabels.end(), 
                            triggerLabels[3]) != tr.filterLabels.end()){
                        vtr3.push_back(&tr);
                    }
            if( std::find(  tr.filterLabels.begin(), 
                            tr.filterLabels.end(), 
                            triggerLabels[4]) != tr.filterLabels.end()){
                        vtr4.push_back(&tr);
                    }
        }
        
        
        // print trigger labels
        if(*singleMu_t2){
            cout << "event " << Nnum << "\t trObj.size = " << triggerObjects.GetSize() << endl;
            for(auto &tr: triggerObjects){
                cout << "\ttrObj:" << endl;
                for(auto& l: tr.filterLabels){
                    cout << "\t\t" << l << endl;
                }
                
                
            }
            getchar();
        }
        

/*

        if(vtr4.size() > 2){
            nCounts[8]++;
            
            //cout << Nnum << "\t" << vtr4.size() << endl;
            //getchar();
            evt.Nnum = Nnum;
            evt.eventNo = *eventNo;
            evt.runNo = *runNo;
            evt.lumNo = *lumNo;
            evt.dataset = dataset;
            events.push_back(evt);
        }
        
        hname = "trigger1_size";
        h[hname].Fill(vtr1.size());
        hname = "trigger2_size";
        h[hname].Fill(vtr2.size());
        hname = "trigger3_size";
        h[hname].Fill(vtr3.size());
        hname = "trigger4_size";
        h[hname].Fill(vtr4.size());
        
        // ************************************************************************ tag and probe:
        
        //
        //if(photons.GetSize() == 2){
            nCounts[5]++;
            
            //for(auto& p: photons){
            //    v = v + p.p;
            //    E += p.p.Mag();
            //}
            //lv.SetVect(v);
            //lv.SetE(E);
            
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
                
                nCounts[20]++;
                
                nCounts[80] = 0;
                nCounts[81] = 0;
                // Preselection of objects that have fired this trigger
                // and objects that dont.
                // These will be used to assign the objects as the tag
                // or probe object, respectively.
                //
                bool    tagged = false,
                        probed = false;
                
                
                for(auto& tr: vtr4){
                    for(auto& p: photons){
                        h2name = "plane_trObj_emObj";
                        h2[h2name].Fill(p.p.Pt()/tr->p.Pt(), tr->p.DeltaR(p.p));
                    }
                }
                
                // preselect all photon objects
                // with Pt and Eta criteria
                selPhotons.clear();
                for(auto& p: photons){
                    if( p.p.Pt() > 40 &&
                        fabs(p.p.Eta()) < 1.4442){
                            selPhotons.push_back(&p);
                    }
                }
                
                // there have to be at least two photon objects fulfilling the criteria,
                // otherwise there would be no chance to find a tag and a probe object.
                if(selPhotons.size() >= 2){
                    nCounts[90]++;
                    nCounts[27]=0;
                    
                    vp_tag.clear();
                    vp_probe.clear();
                    
                    // loop the trigger objects:
                    //for(auto& t: vtr4){
                    
                    /// cout << "vtr4.size()=" << vtr4.size() << endl;
                    for(std::vector<tree::TriggerObject*>::iterator t = vtr4.begin();
                                                                    t!= vtr4.end();
                                                                    // t++
                                                                    ){
                        //cout << "\t\t trigger obj.Pt: " << t->p.Pt() << endl;
                        
                        //cout << "\t trigger: " << t->p.Pt() << endl;
                        //for(auto& tl: t->filterLabels){
                        //    cout << "\t\t " << tl << endl;
                        //} // 
                        ///cout << "\t tr.Pt= " << (*t)->p.Pt() << endl;
                        
                        tagged = false;
                        // loop the em candidates for each trigger object:
                        
                        nCounts[64]=0;
                        for(std::vector<tree::Photon*>::iterator p  = selPhotons.begin();
                                                                 p != selPhotons.end();
                                                                // p++ 
                                                                ){
                            
                            delR = (*t)->p.DeltaR((*p)->p);
                            relPt = (*p)->p.Pt()/(*t)->p.Pt();
                            //cout << "\t " << delR << "\t " << relPt << endl;
                            
                            // require the tag object to
                            // really fired the trigger by
                            // matching it in delR and 
                            // relative Pt to a trigger object
                            if( delR < 0.1 &&
                                relPt > 0.8 &&
                                relPt < 1.4 &&
                                (*p)->hasPixelSeed){
                                    
                                    //cout << "\t\t\t tag match!" << endl;
                                    
                                    // if tag criteria fulfilled, push object to tag vector
                                    vp_tag.push_back((*p));
                                    
                                    // then erase it from selected em objects vector
                                    p = selPhotons.erase(p);
                                    
                                    // also erase the trigger object from the trigger
                                    // vector; in case the probe object is checked later
                                    // if it also could be a tag, it should never
                                    // be possible to match the same trigger object
                                    // as the initial tag object.
                                    //t = vtr4.erase(t);
                                    //cout << "\t\t erased! next pt " << (*t)->p.Pt() << endl;
                                    //tagged = true;
                                    
                                    //cout << "\t\t\t\t after del: " << (*p)->p.Pt() << endl;
                                    //p++;
                                    nCounts[80]++;
                                    
                                    nCounts[64]++; // counter to check if trigger
                                                    // objects match to more than one
                                                    // photon object (should not!)
                                }
                            else{
                                    //vp_probe.push_back((*p));
                                    //p = selPhotons.erase(p);
                                    
                                    p++;
                            }
                        }
                        
                        if(nCounts[64]==1){
                            nCounts[66]++;
                            ///cout << "\t\t now erase! pt " << (*t)->p.Pt() << endl;
                            ///t = vtr4.erase(t);
                            ///cout << "\t\t erased! next pt " << (*t)->p.Pt() << endl;
                            
                            }
                        if(nCounts[64]>1){ nCounts[65]++; }
                        if(nCounts[64]==0){ nCounts[67]++; } 
                        ///getchar();
                         t++; // next trigger object
                    } // triggerObj loop
                    
                    
                    // if more than 1 tag candidate, choose randomly
                    
                    if(vp_tag.size() > 0){ // only do the calculations if at least one
                                            // tag has been found
                        
                        nCounts[12]++;
                        
                        tc = rg->Integer(vp_tag.size());// random integer in the vector length range
                        selTag = *(vp_tag[tc]);         // copy this to selTag
                        vp_tag.erase(vp_tag.begin()+tc);// erase the chosen one
                        //cout << "tagchoice: " << tc << "\t" << selTag.p.Pt() << endl;
                        
                        
                        // push the remaining tags that have not 
                        // been chosen back to selPhotons
                        for(auto& t: vp_tag){
                            selPhotons.push_back(t);
                        }
                        
                        if(selPhotons.size() > 0){ // == 0 never happens here anymore!
                                                // the above selection exclude this case
                                                // counts[12] = counts[14] always
                            nCounts[14]++;
                            
                            pc = rg->Integer(selPhotons.size());
                            selProbe = *(selPhotons[pc]);
                            //selPhotons.erase(selPhotons.begin()+tc); // not nescessary
                            //cout << "probechoice: " << pc << "\t" << selProbe.p.Pt() << endl;
                            
                            // here, one tag and one probe have been selected;
                            hname = "delR_selTag_selProbe";
                            h[hname].Fill(selTag.p.DeltaR(selProbe.p));
                            h2name = "plane_PhiEta_selTag_selProbe";
                            h2[h2name].Fill(    fabs(selTag.p.Phi()-selProbe.p.Phi()),
                                                fabs(selTag.p.Eta()-selProbe.p.Eta())
                                                );
                            
                            // calculate lorentzvector from both:
                            v = selTag.p + selProbe.p;
                            E = selTag.p.Mag() + selProbe.p.Mag();
                            lv.SetVect(v);
                            lv.SetE(E);
                            // now check probe for pixelseed and then assign sevent to
                            // ee or egamma sample:
                            if(!selProbe.hasPixelSeed){
                                nCounts[44]++;
                                
                                // egamma sample
                                h2name = "tnp_eg";
                                h2[h2name].Fill(lv.M(), selProbe.p.Pt());
                                
                                h2name = "tnp_eg_nvtx";
                                h2[h2name].Fill(lv.M(), *nGoodVertices);
                                h2name = "tnp_eg_eta";
                                h2[h2name].Fill(lv.M(), fabs(selProbe.p.Eta()));
                                h2name = "tnp_eg_ntracks";
                                h2[h2name].Fill(lv.M(), *nTracksPV);
                                
                            }
                            
                            if(selProbe.hasPixelSeed){
                                nCounts[45]++;
                                
                                // ee sample
                                h2name = "tnp_ee";
                                h2[h2name].Fill(lv.M(), selProbe.p.Pt());
                                h2name = "tnp_ee_nvtx";
                                h2[h2name].Fill(lv.M(), *nGoodVertices);
                                h2name = "tnp_ee_eta";
                                h2[h2name].Fill(lv.M(), fabs(selProbe.p.Eta()));
                                h2name = "tnp_ee_ntracks";
                                h2[h2name].Fill(lv.M(), *nTracksPV);
                                
                                
                            }
                            
                            // some counter variables
                            if( fabs(lv.M()-m_z)/m_z<0.05){
                                if(selProbe.hasPixelSeed) nCounts[32]++;
                                if(!selProbe.hasPixelSeed) nCounts[33]++;
                                nCounts[26]++;
                            }
                            if( fabs(lv.M()-m_z)/m_z<0.1){
                                if(selProbe.hasPixelSeed) nCounts[24]++;
                                if(!selProbe.hasPixelSeed) nCounts[25]++;
                                nCounts[26]++;
                            }
                            if( fabs(lv.M()-m_z)/m_z<0.3){
                                if(selProbe.hasPixelSeed) nCounts[28]++;
                                if(!selProbe.hasPixelSeed) nCounts[29]++;
                                nCounts[30]++;
                            }
                            
                            
                            
                            // Check if probe object fullfills tag criteria,
                            // which are:   does it match to a trigger object
                            //              and possesses a pixel seed?
                            // If yes, count this again to the electron electron
                            // sample, due to cominatorical reasons.
                            // The tag object always fulfills the probe criteria.
                            if(selProbe.hasPixelSeed){
                                nCounts[68]=0;
                                for(auto& t: vtr4){
                                    delR = t->p.DeltaR(selProbe.p);
                                    relPt = selProbe.p.Pt()/t->p.Pt();
                                    if( delR < 0.1 &&
                                        relPt > 0.8 &&
                                        relPt < 1.4){
                                            
                                            nCounts[68]++; //matching count per event, 
                                                            // should be = 1.
                                            
                                            if(fabs(lv.M()-m_z)/m_z<0.05) nCounts[32]++;
                                            if(fabs(lv.M()-m_z)/m_z<0.1) nCounts[24]++;
                                            if(fabs(lv.M()-m_z)/m_z<0.3) nCounts[28]++;
                                            
                                            nCounts[46]++;
                                                    
                                            /// is probe could also be tag
                                            /// add again to ee sample
                                            
                                            h2name = "tnp_ee";
                                            h2[h2name].Fill(lv.M(), selProbe.p.Pt());
                                            h2name = "tnp_ee_nvtx";
                                            h2[h2name].Fill(lv.M(), *nGoodVertices);
                                            h2name = "tnp_ee_eta";
                                            h2[h2name].Fill(lv.M(), fabs(selProbe.p.Eta()));
                                            h2name = "tnp_ee_ntracks";
                                            h2[h2name].Fill(lv.M(), *nTracksPV);
                                    
                                            
                                        }
                                }
                                // this should never be the case!
                                if(nCounts[68]>1) nCounts[69]++;
                            }
                            
                        }
                    }
                    else{
                        //cout << endl;
                        //cout << Nnum << endl;
                        nCounts[13]++;
                    }
                    
                
                } // selPhotons.size() == 2
                
                
                
            }
            
            
        //} // photons.GetSize() == 2
        
*/        
        
        Nnum++;
    } // //////////////////////////////////////////////////////////////////////////////////////////
    
    cout << endl;
    cout << "Looped events: " << Nnum << endl;
    cout << "nTest: " << nTest << endl;
    cout << "fTest: " << fTest << endl;
    
    cout << "total Photons: " << nPhotons << endl;
    
    
    
    //cout << vp.size() << endl;
    return 0;
}




