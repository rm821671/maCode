//
// Implementation of the TAuxiliary.
// 
// @author: ralf.meyer@rwth-aachen.de
// @version: 0.8
// 

#include "TAuxiliary.h"


ClassImp(TAuxiliary) ;

TAuxiliary::TAuxiliary(){};

void TAuxiliary::drawCMS(TPad& p)
{
    // draw labels in pad
    p.cd();
    
    //~ def drawCMS(c):
    //~ # TODO: doesnt work atm
    //~ c.cd()
    //~ # Needs a canvas
    //~ # TLatex(x, y, text)
    //~ info = rt.TLatex(9.,9.,"13 TeV")
    //~ info.SetNDC()
    //~ info.SetTextAngle(0)
    //~ info.SetTextFont(42)
    //~ #info.SetTextAlign(31)
    //~ info.SetTextSize(0.04)
    //~ info.SetTextColor(rt.kBlack)
    //~ info.Draw()
    //~ c.Update()
    //~ #raw_input()
    //~ #return c
    
    //~ TLatex info(0.5, 0.5, "hallo hallo hallo");
    //~ info.SetNDC();
    //~ info.SetTextAngle(0);
    //~ info.SetTextFont(42);
    //~ info.SetTextSize(0.5);
    //~ info.SetTextColor(kBlack);
    //~ 
    //~ info.Draw();
    
    cout << "latex drawing... " << endl;
    TLatex latex;
    latex.SetTextSize(0.025);
    latex.SetTextAlign(13);  //align at top
    latex.DrawLatex(.71,.91,"hallo hallo hallo");
    
    p.Update();
    return ;
}

void TAuxiliary::setTH1PlotOptions(TH1& h)
{
    h.SetStats(0);
    h.SetTitle("");
    
    //~ h.GetYaxis()->SetNdivisions(5);
    h.GetYaxis()->SetLabelSize(0.08);
    h.GetYaxis()->SetTitleSize(0.08);
    h.GetYaxis()->SetTitleOffset(0.9);
    //h.GetYaxis()->SetTitle("ratio");
    
    h.GetXaxis()->SetTitle("");
    h.GetXaxis()->SetLabelSize(0.08);
    
    
    h.SetLineWidth(2);
    
    
    return ;
}

void TAuxiliary::setTH1RatioPlotOptions(TH1& h)
{
    // standard plot options for a ratio TH1
    
    h.SetStats(0);
    
    h.SetTitle("");
    
    h.GetYaxis()->SetNdivisions(5);
    h.GetYaxis()->SetLabelSize(0.08);
    h.GetYaxis()->SetTitleSize(0.08);
    h.GetYaxis()->SetTitleOffset(0.9);
    //h.GetYaxis()->SetTitle("ratio");
    
    h.GetXaxis()->SetTitle("");
    h.GetXaxis()->SetLabelSize(0.08);
    
    
    // 
    h.SetLineWidth(2);
    
    
    return ;
}

void TAuxiliary::setPadStyle(TPad& p)
{
    p.SetLeftMargin(0.16);
    
    
    //TPad *p1 = new TPad("p1", "p1", 0., 0., 1., 1.);
    //p1->SetLeftMargin(0.16);
    
    return ;
}

void TAuxiliary::setCanvasStyle(TCanvas& c)
{
    
    
    
    
    return ;
}

map<Int_t, map<string, RooDataSet*>> TAuxiliary::SortRooDataSets(map<string, RooDataSet*> r)
{
    //map<string, RooDataSet*> rds;
    map<Int_t, map<string, RooDataSet*>> rr;
    
    map<Int_t, labeledRooDataSet> rs;
    labeledRooDataSet rss;
    
    // Sort RooDataSets by number of entries.
    // The returned map has a running integer index with ne number of entries.
    // If this map is iterated, the iterator starts from low index to high index.
    Int_t  n;
    
    // sort the roodatasets by number of entries.
    for(auto const& rIt: r){
        n = rIt.second->numEntries();
        
        rss.name = rIt.first;
        rss.r    = rIt.second;
        
        rs[n] = rss;
        
        //~ rr[n].first = rIt.first;
        //~ rr[n].second = rIt.second;
    }
    
    return rr;
}

THStack* TAuxiliary::RooTStack(string name, string title, map<Int_t, map<string, RooDataSet*>> r, RooRealVar& x, Int_t Nbins)
{
    // Returns a THStack from all given RooDataSets.
    // It is required that the datasets are unbinned or at least each has the same binning!
    // The reference variable x (with tespect to the binning) has to be given.
    THStack *hs = new THStack(name.c_str(), title.c_str());
    
    // a histogram object pointer; used to create binned histograms
    TH1F* h = NULL;
    string s;
    
    // The integer index (outer.first) iterates in the correct order.
    for(auto const& outer: r){
        for(auto const& inner: outer.second){
            s = inner.first;
            h = (TH1F*) inner.second->createHistogram(x.GetName(), Nbins);
            h->SetTitle(s.c_str());
            hs->Add(h);
        }
    }
    
    return hs;
}







