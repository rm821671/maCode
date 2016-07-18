


import sys
sys.path.insert(0,"../../py")

import ROOT as rt
from modules import lib_plot as lp
from modules import auxiliary as aux
from modules import filemanager as fm
from modules import analysis as an
from modules import plotcanvas as pc

from array import array

import numpy as np

import time
import datetime as dt

def closure():
    
    # genmatch for e->e and e->g, without Ht100To200 bin
    filename = "/user/rmeyer/Dropbox/data_uni/root_selectorFiles/myTestSelector_closureWJetsToLNu__1466503598.root"
    
    # no genmatch for e->e, only leading em object
    filename = "/user/rmeyer/Dropbox/data_uni/root_selectorFiles/myTestSelector_closureWJetsToLNu__1466514536.root"
    # nur barrell
    filename = "/user/rmeyer/Dropbox/data_uni/root_selectorFiles/myTestSelector_closureWJetsToLNu__1466514983.root"
    
    # drell yan
    #filename = "/user/rmeyer/Dropbox/data_uni/root_selectorFiles/myTestSelector_closureDY__1466515252.root"
    
    
    data = rt.TFile(filename)
    
    g = data.Get("clos_gen_g")
    e = data.Get("clos_gen_e")
    
    
    f = 0.0107
    f = 0.0077
    
    edges = [50., 60., 70., 70., 80., 90., 100., 120., 140., 160., 250.]
    #edges = [40., 250.]
    
    g = aux.rebin(g, edges)
    e = aux.rebin(e, edges)
    
    #g.Rebin(10)
    #e.Rebin(10)
    
    print "predicted: ", e.Integral()*f/(1-f)
    print "matched:   ", g.Integral()
    
    e.Scale(f/(1-f))
    
    e.SetMarkerColor(rt.kRed)
    g.SetMarkerColor(rt.kBlack)
    
    e.SetMarkerStyle(rt.kFullCircle)
    g.SetMarkerStyle(rt.kFullSquare)
    
    
    
    
    
    # empty histogram
    #h = rt.TH1F("h", "h", len(edges)-1, array( 'd', edges ))
    
    
    
    ### create ratio
    #def ratio_hishis(h1, h2):
    ##returns h1/h2
    ratio = aux.ratio_hishis(g, e)
    
    ratio.GetYaxis().SetLabelSize(0.065)
    ratio.GetYaxis().SetTitle("gen./pred.")
    ratio.GetYaxis().SetTitleSize(0.08)
    ratio.GetYaxis().SetTitleOffset(0.5)
    
    ratio.GetXaxis().SetLabelSize(0.)
    ratio.GetXaxis().SetTitleSize(0.)
    
    
    l = rt.TLegend(0.5,0.7,0.9,0.9);
    l.AddEntry(g, "N_{e#rightarrow#gamma}", "ep")
    l.AddEntry(e, "N_{e#rightarrowe} * f/(1-f)", "ep")
    
    
    c = rt.TCanvas("c", "c", 700, 700)
    c.cd()
    
    p1 = rt.TPad("p1", "p1", 0., 0.3, 1., 1.)
    p1.SetLeftMargin(0.12)
    p1.Draw()
    p1.cd()
    
    g.Draw("ep")
    e.Draw("ep same")
    l.Draw()
    
    c.cd()
    p2 = rt.TPad("p2", "p2", 0., 0.01, 1., 0.3)
    p2.SetLeftMargin(0.12)
    p2.SetGridy()
    p2.Draw()
    p2.cd()
    
    ratio.Draw("ep")
    
    
    
    raw_input()
    
    return 0

def plot():
    
    plotfilename = "plots.root"
    _fm = fm.FileManager()
    tstr = str(dt.datetime.now()).replace(" ", "_").replace(":","-").split(".")[0]
    _fs = fm.FileSaver(plotfilename,tstr,_fm)
    
    filename = sys.argv[1]
    
    data = rt.TFile(filename)
    
    tot = data.Get("zpeak_tot")
    pas = data.Get("zpeak_pas")
    
    print "pas", pas.Integral()
    print "tot", tot.Integral()
    
    tot.Scale(pas.Integral()/tot.Integral())
    #pas.Scale(1./pas.Integral())
    
    
    tot.Rebin(2)
    pas.Rebin(2)
    
    
    tot.SetLineWidth(2)
    tot.SetLineColor(rt.kRed)
    
    pas.SetLineWidth(2)
    pas.SetLineColor(rt.kBlue)
    
    c = rt.TCanvas("c", "c", 600, 600)
    c.cd()
    
    tot.Draw("hist")
    pas.Draw("hist same")
    
    c.Update()
    
    raw_input()
    
    return 0


def main():
    
    #rt.gROOT.SetBatch(rt.kTRUE)    # dont show the canvases
    
    #plot()
    closure()
    
    return 0

if __name__ == "__main__":
    main()
