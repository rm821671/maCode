#include <stdio.h>
#include <algorithm>
#include <string>
#include <ctime>

//using namespace std;

#include <sys/utsname.h>

#include "test_bib.hpp"

#include "TMinuit.h"

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


void stringRef(string &s){
	
	s = "ciao";
}


// Example displaying two histograms and their ratio.
// Author: Olivier Couet
void ratioplot() {
   // Define two gaussian histograms. Note the X and Y title are defined
   // at booking time using the convention "Hist_title ; X_title ; Y_title"
   TH1F *h1 = new TH1F("h1", "Two gaussian plots and their ratio;x title; h1 and h2 gaussian histograms", 100, -5, 5);
   TH1F *h2 = new TH1F("h2", "h2", 100, -5, 5);
   h1->FillRandom("gaus");
   h2->FillRandom("gaus");

   // Define the Canvas
   TCanvas *c = new TCanvas("c", "canvas", 800, 800);

   // Upper plot will be in pad1
   TPad *pad1 = new TPad("pad1", "pad1", 0, 0.3, 1, 1.0);
   pad1->SetBottomMargin(0); // Upper and lower plot are joined
   pad1->SetGridx();         // Vertical grid
   pad1->Draw();             // Draw the upper pad: pad1
   pad1->cd();               // pad1 becomes the current pad
   h1->SetStats(0);          // No statistics on upper plot
   h1->Draw();               // Draw h1
   h2->Draw("same");         // Draw h2 on top of h1

   // Do not draw the Y axis label on the upper plot and redraw a small
   // axis instead, in order to avoid the first label (0) to be clipped.
   h1->GetYaxis()->SetLabelSize(0.);
   TGaxis *axis = new TGaxis( -5, 20, -5, 220, 20,220,510,"");
   axis->SetLabelFont(43); // Absolute font size in pixel (precision 3)
   axis->SetLabelSize(15);
   axis->Draw();

   // lower plot will be in pad
   c->cd();          // Go back to the main canvas before defining pad2
   TPad *pad2 = new TPad("pad2", "pad2", 0, 0.05, 1, 0.3);
   pad2->SetTopMargin(0);
   pad2->SetBottomMargin(0.2);
   pad2->SetGridx(); // vertical grid
   pad2->Draw();
   pad2->cd();       // pad2 becomes the current pad

   // Define the ratio plot
   TH1F *h3 = (TH1F*)h1->Clone("h3");
   h3->SetLineColor(kBlack);
   h3->SetMinimum(0.8);  // Define Y ..
   h3->SetMaximum(1.35); // .. range
   h3->Sumw2();
   h3->SetStats(0);      // No statistics on lower plot
   h3->Divide(h2);
   h3->SetMarkerStyle(21);
   h3->Draw("ep");       // Draw the ratio plot

   // h1 settings
   h1->SetLineColor(kBlue+1);
   h1->SetLineWidth(2);

   // Y axis h1 plot settings
   h1->GetYaxis()->SetTitleSize(20);
   h1->GetYaxis()->SetTitleFont(43);
   h1->GetYaxis()->SetTitleOffset(1.55);

   // h2 settings
   h2->SetLineColor(kRed);
   h2->SetLineWidth(2);

   // Ratio plot (h3) settings
   h3->SetTitle(""); // Remove the ratio title

   // Y axis ratio plot settings
   h3->GetYaxis()->SetTitle("ratio h1/h2 ");
   h3->GetYaxis()->SetNdivisions(505);
   h3->GetYaxis()->SetTitleSize(20);
   h3->GetYaxis()->SetTitleFont(43);
   h3->GetYaxis()->SetTitleOffset(1.55);
   h3->GetYaxis()->SetLabelFont(43); // Absolute font size in pixel (precision 3)
   h3->GetYaxis()->SetLabelSize(15);

   // X axis ratio plot settings
   h3->GetXaxis()->SetTitleSize(20);
   h3->GetXaxis()->SetTitleFont(43);
   h3->GetXaxis()->SetTitleOffset(4.);
   h3->GetXaxis()->SetLabelFont(43); // Absolute font size in pixel (precision 3)
   h3->GetXaxis()->SetLabelSize(15);
}


void confidenceintervals(){
//Illustrates TVirtualFitter::GetConfidenceIntervals
//This method computes confidence intervals for the fitted function
//Author: Anna Kreshuk

   TCanvas *myc = new TCanvas("myc",
      "Confidence intervals on the fitted function",1200, 500);
   myc->Divide(3,1);

/////1. A graph
   //Create and fill a graph
   Int_t ngr = 100;
   TGraph *gr = new TGraph(ngr);
   gr->SetName("GraphNoError");
   Double_t x, y;
   Int_t i;
   for (i=0; i<ngr; i++){
      x = gRandom->Uniform(-1, 1);
      y = -1 + 2*x + gRandom->Gaus(0, 1);
      gr->SetPoint(i, x, y);
   }
   //Create the fitting function
   TF1 *fpol = new TF1("fpol", "pol1", -1, 1);
   fpol->SetLineWidth(2);
   gr->Fit(fpol, "Q");

   //Create a TGraphErrors to hold the confidence intervals
   TGraphErrors *grint = new TGraphErrors(ngr);
   grint->SetTitle("Fitted line with .95 conf. band");
   for (i=0; i<ngr; i++)
      grint->SetPoint(i, gr->GetX()[i], 0);
   //Compute the confidence intervals at the x points of the created graph
   (TVirtualFitter::GetFitter())->GetConfidenceIntervals(grint);
   //Now the "grint" graph contains function values as its y-coordinates
   //and confidence intervals as the errors on these coordinates
   //Draw the graph, the function and the confidence intervals
   myc->cd(1);
   grint->SetLineColor(kRed);
   grint->Draw("ap");
   gr->SetMarkerStyle(5);
   gr->SetMarkerSize(0.7);
   gr->Draw("psame");

/////2. A histogram
   myc->cd(2);
   //Create, fill and fit a histogram
   Int_t nh=5000;
   TH1D *h = new TH1D("h",
      "Fitted gaussian with .95 conf.band", 100, -3, 3);
   h->FillRandom("gaus", nh);
   TF1 *f = new TF1("fgaus", "gaus", -3, 3);
   f->SetLineWidth(2);
   h->Fit(f, "Q");
   h->Draw();

   //Create a histogram to hold the confidence intervals
   TH1D *hint = new TH1D("hint",
      "Fitted gaussian with .95 conf.band", 100, -3, 3);
   (TVirtualFitter::GetFitter())->GetConfidenceIntervals(hint);
   //Now the "hint" histogram has the fitted function values as the
   //bin contents and the confidence intervals as bin errors
   hint->SetStats(kFALSE);
   hint->SetFillColor(2);
   hint->Draw("e3 same");

/////3. A 2d graph
   //Create and fill the graph
   Int_t ngr2 = 100;
   Double_t z, rnd, e=0.3;
   TGraph2D *gr2 = new TGraph2D(ngr2);
   gr2->SetName("Graph2DNoError");
   TF2  *f2 = new TF2("f2",
      "1000*(([0]*sin(x)/x)*([1]*sin(y)/y))+250",-6,6,-6,6);
   f2->SetParameters(1,1);
   for (i=0; i<ngr2; i++){
      f2->GetRandom2(x,y);
      // Generate a random number in [-e,e]
      rnd = 2*gRandom->Rndm()*e-e;
      z = f2->Eval(x,y)*(1+rnd);
      gr2->SetPoint(i,x,y,z);
   }
   //Create a graph with errors to store the intervals
   TGraph2DErrors *grint2 = new TGraph2DErrors(ngr2);
   for (i=0; i<ngr2; i++)
      grint2->SetPoint(i, gr2->GetX()[i], gr2->GetY()[i], 0);

   //Fit the graph
   f2->SetParameters(0.5,1.5);
   gr2->Fit(f2, "Q");
   //Compute the confidence intervals
   (TVirtualFitter::GetFitter())->GetConfidenceIntervals(grint2);
   //Now the "grint2" graph contains function values as z-coordinates
   //and confidence intervals as their errors
   //draw
   myc->cd(3);
   f2->SetNpx(30);
   f2->SetNpy(30);
   f2->SetFillColor(kBlue);
   f2->Draw("surf4");
   grint2->SetNpx(20);
   grint2->SetNpy(20);
   grint2->SetMarkerStyle(24);
   grint2->SetMarkerSize(0.7);
   grint2->SetMarkerColor(kRed);
   grint2->SetLineColor(kRed);
   grint2->Draw("E0 same");
   grint2->SetTitle("Fitted 2d function with .95 error bars");
   
   myc->cd();
   
   
}

void fitcont()
{
   // Example illustrating how to draw the n-sigma contour of a Minuit fit.
   // To get the n-sigma contour the ERRDEF parameter in Minuit has to set
   // to n^2. The fcn function has to be set before the routine is called.
   //
   // WARNING!!! This test works only with TMinuit
   //
   // The TGraph object is created via the interpreter. The user must cast it
   // to a TGraph*
   // Author;  Rene Brun
     
   TCanvas *c1 = new TCanvas("c1");
   TH1F *h = new TH1F("h","My histogram",100,-3,3);
   h->FillRandom("gaus",6000);
   h->Fit("gaus");
   
   
   
   c1->Update();
   
   
   
   TCanvas *c2 = new TCanvas("c2","contours",10,10,600,800);
   c2->Divide(1,2);
   c2->cd(1);
   //get first contour for parameter 1 versus parameter 2
   TGraph *gr12 = (TGraph*)gMinuit->Contour(40,1,2);
   gr12->Draw("alp");
   c2->cd(2);
   
   
   //Get contour for parameter 0 versus parameter 2  for ERRDEF=2
   gMinuit->SetErrorDef(4); //note 4 and not 2!
   TGraph *gr2 = (TGraph*)gMinuit->Contour(80,0,2);
   gr2->SetFillColor(42);
   gr2->Draw("alf");
   //Get contour for parameter 0 versus parameter 2 for ERRDEF=1 
   gMinuit->SetErrorDef(1);
   TGraph *gr1 = (TGraph*)gMinuit->Contour(80,0,2);
   gr1->SetFillColor(38);
   gr1->Draw("lf");
   // */
   
}


void fitslicesy() {
//
// Illustrates how to use the TH1::FitSlicesY function
// To see the output of this macro, click begin_html <a href="gif/fitslicesy.gif" >here</a> end_html
//    It uses the TH2F histogram generated in macro hsimple.C
//    It invokes FitSlicesY and draw the fitted "mean" and "sigma"
//    in 2 sepate pads.
//    This macro shows also how to annotate a picture, change
//    some pad parameters.
//Author: Rene Brun

// Change some default parameters in the current style
   gStyle->SetLabelSize(0.06,"x");
   gStyle->SetLabelSize(0.06,"y");
   gStyle->SetFrameFillColor(38);
   gStyle->SetTitleW(0.6);
   gStyle->SetTitleH(0.1);

// Connect the input file and get the 2-d histogram in memory
   TString dir = gSystem->UnixPathName(__FILE__);
   dir.ReplaceAll("fitslicesy.C","../hsimple.C");
   dir.ReplaceAll("/./","/");
   if (!gInterpreter->IsLoaded(dir.Data())) gInterpreter->LoadMacro(dir.Data());
   TFile *hsimple = (TFile*)gROOT->ProcessLineFast("hsimple(1)");
   if (!hsimple) return;
   TH2F *hpxpy = (TH2F*)hsimple->Get("hpxpy");

// Create a canvas and divide it
   TCanvas *c1 = new TCanvas("c1","c1",700,500);
   c1->SetFillColor(42);
   c1->Divide(2,1);
   TPad *left = (TPad*)c1->cd(1);;
   left->Divide(1,2);

// Draw 2-d original histogram
   left->cd(1);
   gPad->SetTopMargin(0.12);
   gPad->SetFillColor(33);
   hpxpy->Draw();
   hpxpy->GetXaxis()->SetLabelSize(0.06);
   hpxpy->GetYaxis()->SetLabelSize(0.06);
   hpxpy->SetMarkerColor(kYellow);

// Fit slices projected along Y fron bins in X [7,32] with more than 20 bins  in Y filled
   hpxpy->FitSlicesY(0,7,32,20);

// Show fitted "mean" for each slice
   left->cd(2);
   gPad->SetFillColor(33);
   TH2F *hpxpy_0 = (TH2F*)hsimple->Get("hpxpy_0");
   hpxpy_0->Draw();
   TPad *right = (TPad*)c1->cd(2);
   right->Divide(1,2);
   right->cd(1);
   gPad->SetTopMargin(0.12);
   gPad->SetLeftMargin(0.15);
   gPad->SetFillColor(33);
   TH2F *hpxpy_1 = (TH2F*)hsimple->Get("hpxpy_1");
   hpxpy_1->Draw();

// Show fitted "sigma" for each slice
   right->cd(2);
   gPad->SetTopMargin(0.12);
   gPad->SetLeftMargin(0.15);
   gPad->SetFillColor(33);
   TH2F *hpxpy_2 = (TH2F*)hsimple->Get("hpxpy_2");
   hpxpy_2->SetMinimum(0.8);
   hpxpy_2->Draw();

//attributes
   hpxpy_0->SetLineColor(5);
   hpxpy_1->SetLineColor(5);
   hpxpy_2->SetLineColor(5);
   hpxpy_0->SetMarkerColor(2);
   hpxpy_1->SetMarkerColor(2);
   hpxpy_2->SetMarkerColor(2);
   hpxpy_0->SetMarkerStyle(21);
   hpxpy_1->SetMarkerStyle(21);
   hpxpy_2->SetMarkerStyle(21);
   hpxpy_0->SetMarkerSize(0.6);
   hpxpy_1->SetMarkerSize(0.6);
   hpxpy_2->SetMarkerSize(0.6);
}


void test(){
	
	//ratioplot();
	
	//confidenceintervals();
	
	//fitcont();
	
	//fitslicesy();
	
	cout << sqrt(2) << endl;
	
	
	//stringtest();
	//sysinfo();
/*
	int a=3, b=5;
	cout << a << b << endl;
	
	string s = "hallo";
	cout << "before: " << s << endl;
	
	stringRef(s);
	
	cout << "after: " << s << endl;
*/
	
	
	//checkString();
	
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

