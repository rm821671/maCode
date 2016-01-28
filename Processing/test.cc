#include <stdio.h>
#include <algorithm>
#include <string>
#include <ctime>

//using namespace std;

#include <sys/utsname.h>

template <typename T> int sign(T val) {
   	return (T(0) < val) - (val < T(0));
	}
	
template <typename T> string NumberToString ( T Number )
  {
     ostringstream ss;
     ss << Number;
     return ss.str();
  }
	

// create output file name depending on input tree
string getOutputFilename( string strIn ) {

  // Converts "/path/to/ntuple/XYZ.root" 
  // to "../selector_rootFiles/my_selector_results_XYZ_(timestamp).root"



  auto PosStart = strIn.rfind("/");
  auto PosEnd = strIn.find(".root");
  string outputName;
  
  if( PosEnd != string::npos ) {
    outputName = "../selector_rootFiles/my_selector_results_" + 
    				strIn.substr( PosStart+1, PosEnd-PosStart-1 ) + 
    				"_" + 
    				NumberToString(time(0)) +
    				".root";
  }
  return outputName;

}



//#include <algorithm>
//#include <string>

void replaceInString(string &s) {
  //std::string s = "example string";
  std::replace( s.begin(), s.end(), '.', 'd'); // replace all 'x' to 'y'
  //std::cout << s << std::endl;
}

void removeCharsFromString( string &str, char* charsToRemove ) {
   for ( unsigned int i = 0; i < strlen(charsToRemove); ++i ) {
      str.erase( remove(str.begin(), str.end(), charsToRemove[i]), str.end() );
   }
}


void somestuff(){
	
	string s="../../cms_sw/myTrees/mcDY_1446802738.root";
	
	string erg = getOutputFilename(s);
	
	cout << erg << endl;
	string t = NumberToString(time(0));	
	
	cout << t << endl;
	
	
	int a = int(1e6);
	float b = float(10);
	
	cout << a << endl;
	
	string k = "hallo.hallo";
	replaceInString(k);
	cout << k << endl;
	
	
	time_t rawtime;
	struct tm * timeinfo;
	time ( &rawtime );
	timeinfo = localtime ( &rawtime );
	cout << "time: " << asctime (timeinfo) << endl;
	
	int n=0;
	for(int i=20;i<200;i+=5){
		cout << i << " ";
		n += 1;
	}
	cout << endl;
	cout << "n=" << n << endl;
	
	/*
	vector<string> s;
	
	s = {"../../NTupel/DYJetsToLL_M-50_nTuple.root", "../../NTupel/DY_FullSim.root"};
	
	for(auto it : s){
		cout << it << endl;
	}
	
	
	int k = -3;
	
	cout << sign(k) << endl;
	
	
	
	cout << sign(5) << endl;
	*/
	
	
	
}


void stringtest(){
	
	string path = "/user/rmeyer/mergedNTuples/";
	string datasets[] = {	"DYJetsToLL_M-50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8_v01.root",		// 0
							"TTGJets_TuneCUETP8M1_13TeV-amcatnloFXFX-madspin-pythia8_v01.root", // 1
							"WGToLNuG_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8_v01.root",	// 2
							"ZGTo2LG_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8_v01.root", // 3
							"DoubleEG_Run2015D-05Oct2015-v1.root",	// data  4
							"DoubleEG_Run2015D-PromptReco-v4.root"	// data  5
						};
	
	string back = getOutputFilename(path+datasets[0]);
	cout << back << endl;
	
	cout << "done" << endl;
	
}

string getOsName()
{
    #ifdef _WIN32
    return "Windows 32-bit";
    #elif _WIN64
    return "Windows 64-bit";
    #elif __unix
    return "__Unix";
    #elif __unix__
    return "__Unix__";
    #elif __APPLE__ || __MACH__
    return "Mac OSX";
    #elif __linux__
    return "Linux";
    #elif __FreeBSD__
    return "FreeBSD";
    #else
    return "Other";
    #endif
}




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
	string ThisSys(uts.nodename);
	
	cout << ThisSys << endl;
	
	return ThisSys;
}


void sysinfo(){
	
	//sys = TSystem();
	
	//cout << sys.HostName() << endl;
	
	//cout << getOsName() << endl;
	
	//cout << getOsName() << endl;
	
	/*
	 * struct utsname 
   {
       char sysname[];    // Operating system name (e.g., "Linux") 
       char nodename[];   // Name within "some implementation-defined network" 
       char release[];    // OS release (e.g., "2.6.28") 
       char version[];    // OS version 
       char machine[];    // Hardware identifier 
       #ifdef _GNU_SOURCE
          char domainname[]; // NIS or YP domain name 
       #endif
   };
	
	*/
	struct utsname uts;
	uname(&uts);
	
	cout << "uts.sysname: " << uts.sysname << endl;
	cout << "uts.nodename: " << uts.nodename << endl;
	cout << "uts.release: " << uts.release << endl;
	cout << "uts.version: " << uts.version << endl;
	cout << "uts.machine: " << uts.machine << endl;
	
	string 	version,
			ubuntu;
	
	version = FilePath();
	ubuntu = "Ubuntu";
	//string scientific = "physik";
	
	// search for ubuntu, otherwise its scientific
	if (version.find(ubuntu) != string::npos) {
		cout << "laptop!" << '\n';
	}
	else {
		cout << "institute!" << '\n';
	}
	
}


void writedata(){
	
	string protocolfile = "diesdas.txt";
	
	ofstream prot(protocolfile, ios::app); //append
	if(prot.is_open()){
		prot << "hallo hallo" << endl;
		prot.close();
	}
}


void checkString(){
	
	string thisFileName = "bla";
	bool isData;
	
		// if mc, set true, if data set false
	isData = 	thisFileName.find("DYJetsToLL") != string::npos
			||	thisFileName.find("TTGJets") != string::npos
			||	thisFileName.find("WGToLNuG") != string::npos
			||	thisFileName.find("ZGTo2LG") != string::npos;
	
	if(isData) cout << "isData: True" << endl;
	if(!isData) cout << "isData: False" << endl;
	
	
}



void test(){
	
	
	//stringtest();
	sysinfo();
	
	checkString();
	
	//writedata();
	
	
// comment catcher */
}	
	
	
/*
	
	
	   TCanvas* c1 = new TCanvas("example","",600,400);
   c1->SetFillStyle(1001);
   c1->SetFillColor(kWhite);
   //create one-dimensional TEfficiency object with fixed bin size
   TEfficiency* pEff = new TEfficiency("eff","my efficiency;x;#epsilon",20,0,10);
   TRandom3 rand3;
   bool bPassed;
   double x;
   for(int i=0; i<10000; ++i)
   {
      //simulate events with variable under investigation
      x = rand3.Uniform(10);
      //check selection: bPassed = DoesEventPassSelection(x)
      bPassed = rand3.Rndm() < TMath::Gaus(x,5,4);
      pEff->Fill(bPassed,x);
   }
   pEff->Draw("AP");
   





*/


	/*
	struct fown{
		float x;
		float y;
	};
		
	
	vector<fown> v;
	
	fown u;
	u.x=0.5;
	u.y=0.4;
	
	v.push_back(u);
	
	for(int i=0;i<10;i++){
		u.x=u.x+i*0.1;
		u.y=u.y+i*0.2;
		v.push_back(u);
	}
	
	//cout << v.x.size() << endl;
	//cout << v[0].x << v.at(0).x << endl;
	float min = 0,t =0;
	for(int i=0;i<v.size();i++){
		
		min = v.at(i).x;
		if(i<1) t = v.at(0).y;
		else t = v.at(i-1).y;
		
		if(v.at(i).y < t) t=v.
	}
	
	//a = min_element(begin(v),end(v));
	
	// */
	
	/*
  		TCanvas *c1 = new TCanvas("c1","Profile histogram example",200,10,700,500);
  		TCanvas *c2 = new TCanvas("c2","Profile histogram example",200,10,700,500);
  		
  		// TProfile (const char *name, const char *title, Int_t nbinsx, Double_t xlow, Double_t xup, Option_t *option="")
  		hprof  = new TProfile("hprof","Profile of pz versus px",100,-4,4,0,20);
  		 		
  		// TH2F (const char *name, const char *title, Int_t nbinsx, Double_t xlow, Double_t xup, Int_t nbinsy, Double_t ylow, Double_t yup)
  		TH2F *h2  = new TH2F("h2","Profile of pz versus px",100,-4,4,100,-0,20);

  		
  		Float_t px, py, pz;
  		for ( Int_t i=0; i<25000; i++) {
    			gRandom->Rannor(px,py);
    			pz = px*px + py*py;
    			hprof->Fill(px,pz,1);
    			h2->Fill(px,pz,1);
  			}
  			
  		c1->cd();
  		hprof->Draw("hist");
  		
  		c2->cd();
  		h2->Draw();
	
	*/
	/*
	map<string,vector<float>> vc;
	string s = "ja";
	vc["jepp"];
	vc["nein"].push_back(7);
	vc["nein"].push_back(9);
	vc["ja"].push_back(3);
	
	cout << vc["ja"].at(0) << endl;
	cout << vc["nein"].at(1) << endl;
	
	vc[s].at(0) += 1;
	cout << vc["ja"].at(0) << endl;
	
	*/

