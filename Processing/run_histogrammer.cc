//
//


#include <sys/utsname.h>

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
	string ubuntu = "Ubuntu";	// if the version string contains ubuntu, its the laptop
								// 
	
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
	
	//return path + "Dropbox/data_uni/";
} // SystemPath()

void run_histogrammer(bool allData = false, string comment = "no comment"){
	
	bool run = true;
	
	//bool allData = false;
	cout << "allData: " << allData << endl;
	cout << "comment: " << comment << endl;
	string filepath, dropbox;
	
	SystemPath(filepath, dropbox);
	
	cout << "filepath: " << filepath << endl;
	cout << "dropbox: " << dropbox << endl;
	
	string protocolfile = dropbox + "root_selectorFiles/protocol_hist.txt";
	
	cout << "protocol: " << protocolfile << endl;
	
	int kk = 4;
	int Ndatasets = 6;
	
	string datasets[] = {	"DYJetsToLL_M-50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8_v01.root",// 0
							"TTGJets_TuneCUETP8M1_13TeV-amcatnloFXFX-madspin-pythia8_v01.root",// 1
							"WGToLNuG_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8_v01.root",	// 2
							"ZGTo2LG_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8_v01.root", // 3
							"DoubleEG_Run2015D-05Oct2015-v1_v02.root",	// data  4
							"DoubleEG_Run2015D-PromptReco-v4_v02.root"	// data  5
						};
	
	//string file= path+"DY_v1.root"; // mc sample with particle flow candidates
	string file= filepath + datasets[kk] ;
	
	//cout << "printed all" << endl;
	//cout << *(datasets+5) << endl;
	
	cout << "file: " << file << endl;
	//string file="../../cms_sw/NTupel/DYJetsToLL_M-50_nTuple.root";
	//string file="../../NTupel/DY_FullSim.root";
	//string file="../../cms_sw/NTupel/myTestTree_unpacked_6_pt095charged.root";
	//string file="../../cms_sw/myTrees/mcDY_1446802738.root";
	
	gSystem->Load("pluginTreeWriterTreeWriterAuto.so");
	
	if(run){		// if boolean is true
		
		const char* selector="Histogrammer.cc+";
		
		if(allData){		// analyse all data sets
		
			cout << "allData" << endl;
			for(kk = 0; kk < Ndatasets; kk++){		//loop datasets
				cout << "++++++++++++++++++++++++++++++++";
				cout << "++++++++++++++++++++++++++++++++" << endl;
				cout << "+++++++++++++++++++++++++++++ "<<kk;
				cout <<" ++++++++++++++++++++++++++++++++" << endl;
				cout << datasets[kk] << endl;
				
				file = filepath+datasets[kk];
				
				ifstream f(file.c_str());
				if(!f.good()) {
					cout << "No file found, just compile." << endl;
					TSelector::GetSelector(selector);
					return;
				}
				
				cout << file.c_str() << endl;
			
				//gSystem->Load("pluginTreeWriterTreeWriterAuto.so");
				cout << "successfull" << endl;
		
				// create chain
				TChain ch("TreeWriter/eventTree");
		
				// 
				ch.AddFile( file.c_str() );
		
				double start_time = time(NULL); // measure running time
		
				cout << "processing with file: " << file << endl;
				cout << "... processing .." << endl;
	
				ch.Process(selector);
	
				double end_time = 1.*( time(NULL));
				cout << "Runtime ~" << (end_time - start_time)/1 << " sec." << endl;
	
				ofstream prot(protocolfile, ios::app); //append
				if(prot.is_open()){
					prot << "comment (call): " << comment << endl;
					prot << "Runtime ~" << (end_time - start_time)/1 << " sec." << endl;
					prot.close();
				}
				
			}
		} // if allData
		
		if(!allData){		// only one data set
			cout << "++++++++++++++++++++++++ not allData ";
			cout <<" ++++++++++++++++++++++++++" << endl;
			cout << "+++++++++++++++++++++++++++++ "<<kk;
			cout <<" ++++++++++++++++++++++++++++++++" << endl;
			cout << datasets[kk] << endl;
			ifstream f(file.c_str());
			if(!f.good()) {
				cout << "No file found, just compile." << endl;
				TSelector::GetSelector(selector);
				return;
			}
			
			cout << file.c_str() << endl;
			
			//gSystem->Load("pluginTreeWriterTreeWriterAuto.so");
			
			cout << "successfull" << endl;
		
			// create chain
			TChain ch("TreeWriter/eventTree");
		
			// 
			ch.AddFile( file.c_str() );
		
			double start_time = time(NULL); // measure running time
		
			cout << "processing with file: " << file << endl;
			cout << "... processing .." << endl;
	
			ch.Process(selector);
	
			double end_time = 1.*( time(NULL));
			cout << "Runtime ~" << (end_time - start_time)/1 << " sec." << endl;
	
			ofstream prot(protocolfile, ios::app); //append
			if(prot.is_open()){
				prot << "comment (call): " << comment << endl;
				prot << "Runtime ~" << (end_time - start_time)/1 << " sec." << endl;
				prot.close();
			}
		
		} //  if not allData
		
		
	}
	
	
}
