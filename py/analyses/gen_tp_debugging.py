# debugging

import sys
sys.path.insert(0,"../../py")

import ROOT as rt
from modules import lib_plot as lp
from modules import auxiliary as aux
from modules import filemanager as fm
from modules import analysis as an
from modules import plotcanvas as pc
from modules import h2fakerate as h2f

from array import array

import numpy as np

import time
import datetime as dt

# Define the kinematic variables dependencies used for the plots.
# These have to be written in the histograms!

def diphoton():
    print "gen_tp_debugging.diphoton()"
    
    rt.gROOT.SetBatch(rt.kTRUE) # dont show the canvases
    
    plotfilename = "diphoton_reco.root"
    
    pdf_str = "diphoton_"
    
    _fm = fm.FileManager()
    tstr = str(dt.datetime.now()).replace(" ", "_").replace(":","-").split(".")[0]
    fs = fm.FileSaver(plotfilename,tstr,_fm)
    
    filename = "myTestSelector_SingleE_diphoton_check_1458295538.root"
    h, h2, h3 = _fm.loadfile(filename)
    
    #print "clos_e xaxis:", clos_e.GetXaxis().GetTitle()
    #print "clos_g xaxis:", clos_g.GetXaxis().GetTitle()
    
    
    tnp_ee = h2["tnp_ee"].Clone()
    tnp_eg = h2["tnp_eg"].Clone()
    tnp_gen_ee = h2["tnp_gen_ee"].Clone()
    tnp_gen_eg = h2["tnp_gen_eg"].Clone()
    
    tnp_gen_ee_ntracks = h2["tnp_gen_ee_ntracks"].Clone()
    tnp_gen_eg_ntracks = h2["tnp_gen_eg_ntracks"].Clone()
    tnp_ee_ntracks = h2["tnp_ee_ntracks"].Clone()
    tnp_eg_ntracks = h2["tnp_eg_ntracks"].Clone()
    
    tnp_gen_ee_nvtx = h2["tnp_gen_ee_nvtx"].Clone()
    tnp_gen_eg_nvtx = h2["tnp_gen_eg_nvtx"].Clone()
    tnp_ee_nvtx = h2["tnp_ee_nvtx"].Clone()
    tnp_eg_nvtx = h2["tnp_eg_nvtx"].Clone()
    
    tnp_gen_ee_eta = h2["tnp_gen_ee_eta"].Clone()
    tnp_gen_eg_eta = h2["tnp_gen_eg_eta"].Clone()
    tnp_ee_eta = h2["tnp_ee_eta"].Clone()
    tnp_eg_eta = h2["tnp_eg_eta"].Clone()
    
    pt_binning = range(40, 60+1, 5)+range(60, 100+1, 10)+range(100, 200+1, 20)
    
    
    
    tnp_ee.Add(tnp_eg)
    tpf = h2f.h2Fakerate("pt", tnp_ee, tnp_eg) #, 60, 120)
    tpf.rebin(pt_binning)
    tpf.addFitModel(rt.TF1("tpf", "[0]+[1]*x+[2]*(x*x)", 40, 200))
    tpf.fit()
    tpcan = h2f.h2FakerateCanvas(tpf)
    tpcan.createCanvas(fs, name=pdf_str+"f_tnp_pt")
    
    
    #fs.savePdf(tpcan.c, pdf_str+"f_tnp_pt")
    # '''
    
    #del tpf
    #del tpcan
    
    
    
    '''
    tnp_gen_ee.Add(tnp_gen_eg)
    tpgenf = h2f.h2Fakerate("pt_gen", tnp_gen_ee, tnp_gen_eg)
    tpgenf.rebin(pt_binning)
    tpgenf.addFitModel(rt.TF1(  "tpgenf", "[0]+[1]/x+[2]/(x*x)", 40, 200))
    tpgenf.fit()
    tpgencan = h2f.h2FakerateCanvas(tpgenf)
    tpgencan.createCanvas()
    fs.savePdf(tpgencan.c, "f_tnpgen_pt")
    
    clos_e = aux.rebin(clos_e, pt_binning)
    clos_g = aux.rebin(clos_g, pt_binning)
    
    tpclos = h2f.closure("tagnprobe"+strClosure, clos_e, clos_g)
    tpclos.addFakerate(tpgenf, "Tag n Probe gen info"+strClosure)
    tpclos.addFakerate(tpf, "Tag n Probe"+strClosure)
    tpclos.createFitPrediction()
    tpclos.createHistoPrediction()
    
    tpclos.histoCanvas("tnpgen_pt_closure"+strClosure, 0)
    tpclos.histoCanvas("tnp_pt_closure"+strClosure, 1)
    fs.savePdf(tpclos.ch["Tag n Probe gen info"+strClosure], "tnpgen_pt_closure_perbin"+strClosure)
    fs.savePdf(tpclos.ch["Tag n Probe"+strClosure], "tnp_pt_closure_perbin"+strClosure)
    # '''
    ntracks_binning = range(0,200+1,10)
    
    '''
    tnp_gen_ee_ntracks.Add(tnp_gen_eg_ntracks)
    tpgenf_ntracks = h2f.h2Fakerate("ntracks_gen", tnp_gen_ee_ntracks, tnp_gen_eg_ntracks)
    tpgenf_ntracks.rebin(ntracks_binning)
    tpgenf_ntracks.addFitModel(rt.TF1("tpgenf_ntracks", "[0]+[1]*x+[2]*x*x", 0, 200))
    tpgenf_ntracks.fit()
    tpgencan_ntracks = h2f.h2FakerateCanvas(tpgenf_ntracks)
    tpgencan_ntracks.createCanvas(fs, yrng=[0., .03], name=pdf_str+"f_tnpgen_ntracks")
    #fs.savePdf(tpgencan_ntracks.c, pdf_str+"f_tnpgen_ntracks")
    #'''
    
    
    
    
    
    
    tnp_ee_ntracks.Add(tnp_eg_ntracks)
    tpf_ntracks = h2f.h2Fakerate("ntracks", tnp_ee_ntracks, tnp_eg_ntracks)
    tpf_ntracks.rebin(ntracks_binning)
    tpf_ntracks.addFitModel(rt.TF1("tpf_ntracks", "[0]+[1]*x+[2]*x*x", 0, 200))
    tpf_ntracks.fit()
    tpcan_ntracks = h2f.h2FakerateCanvas(tpf_ntracks)
    #tpcan_ntracks.createCanvas([0., .03])
    tpcan_ntracks.createCanvas(fs, yrng=[0., .03], name=pdf_str+"f_tnp_ntracks")
    #fs.savePdf(tpcan_ntracks.c, pdf_str+"f_tnp_ntracks")
    # '''
    
    
    
    
    '''
    clos_e_ntracks = aux.rebin(clos_e_ntracks, ntracks_binning)
    clos_g_ntracks = aux.rebin(clos_g_ntracks, ntracks_binning)
    
    tpclos_ntracks = h2f.closure("tnp_ntracks"+strClosure, clos_e_ntracks, clos_g_ntracks)
    tpclos_ntracks.addFakerate(tpgenf_ntracks, "Tag n Probe gen info"+strClosure)
    tpclos_ntracks.addFakerate(tpf_ntracks, "Tag n Probe"+strClosure)
    tpclos_ntracks.createFitPrediction()
    tpclos_ntracks.createHistoPrediction()
    
    tpclos_ntracks.histoCanvas("tnpgen_ntracks_closure"+strClosure, 0)
    tpclos_ntracks.histoCanvas("tnp_ntracks_closure"+strClosure, 1)
    fs.savePdf(tpclos_ntracks.ch["Tag n Probe gen info"+strClosure], "tnpgen_ntracks_closure_perbin"+strClosure)
    fs.savePdf(tpclos_ntracks.ch["Tag n Probe"+strClosure], "tnp_ntracks_closure_perbin"+strClosure)
    #'''
    nvtx_binning = range(30)
    
    '''
    tnp_gen_ee_nvtx.Add(tnp_gen_eg_nvtx)
    tpgenf_nvtx = h2f.h2Fakerate("nvtx_gen", tnp_gen_ee_nvtx, tnp_gen_eg_nvtx)
    
    tpgenf_nvtx.addFitModel(rt.TF1("tpgenf_nvtx", "[0]+[1]*x", 0, 30))
    tpgenf_nvtx.fit()
    tpgencan_nvtx = h2f.h2FakerateCanvas(tpgenf_nvtx)
    tpgencan_nvtx.createCanvas(fs, name=pdf_str+"f_tnpgen_nvtx")
    
    #fs.savePdf(tpgencan_nvtx.c, "f_tnpgen_nvtx")
    #'''
    
    
    
    
    
    tnp_ee_nvtx.Add(tnp_eg_nvtx)
    tpf_nvtx = h2f.h2Fakerate("nvtx", tnp_ee_nvtx, tnp_eg_nvtx)
    
    tpf_nvtx.addFitModel(rt.TF1("tpf_nvtx", "[0]+[1]*x", 0, 30))
    tpf_nvtx.fit()
    tpcan_nvtx = h2f.h2FakerateCanvas(tpf_nvtx)
    tpcan_nvtx.createCanvas(fs, yrng=[0., 0.04], name=pdf_str+"f_tnp_nvtx")
    
    #fs.savePdf(tpcan_nvtx.c, pdf_str+"f_tnp_nvtx")
    #'''
    
    
    
    
    '''
    clos_e_nvtx = aux.rebin(clos_e_nvtx, nvtx_binning)
    clos_g_nvtx = aux.rebin(clos_g_nvtx, nvtx_binning)
    
    tpclos_nvtx = h2f.closure("tnp_nvtx"+strClosure, clos_e_nvtx, clos_g_nvtx)
    
    tpclos_nvtx.addFakerate(tpgenf_nvtx, "Tag n Probe gen info"+strClosure)
    tpclos_nvtx.addFakerate(tpf_nvtx, "Tag n Probe"+strClosure)
    
    tpclos_nvtx.createFitPrediction()
    tpclos_nvtx.createHistoPrediction()
    
    tpclos_nvtx.histoCanvas("tnpgen_nvtx_closure"+strClosure, 0)
    tpclos_nvtx.histoCanvas("tnp_nvtx_closure"+strClosure, 1)
    fs.savePdf(tpclos_nvtx.ch["Tag n Probe gen info"+strClosure], "tnpgen_nvtx_closure_perbin"+strClosure)
    fs.savePdf(tpclos_nvtx.ch["Tag n Probe"+strClosure], "tnp_nvtx_closure_perbin"+strClosure)
    #'''
    eta_binning = range(0, 150+1, 10)
    for i in range(len(eta_binning)):
        eta_binning[i] = eta_binning[i]*0.01
        
    '''
    
    
    tnp_gen_ee_eta.Add(tnp_gen_eg_eta)
    tpgenf_eta = h2f.h2Fakerate("eta_gen", tnp_gen_ee_eta, tnp_gen_eg_eta)
    tpgenf_eta.rebin(eta_binning)
    tpgenf_eta.addFitModel(rt.TF1("tpgenf_eta", "[0]+[1]*x", 0.1, 1.4))
    tpgenf_eta.fit()
    tpgencan_eta = h2f.h2FakerateCanvas(tpgenf_eta)
    tpgencan_eta.createCanvas()
    fs.savePdf(tpgencan_eta.c, "f_tnpgen_eta")
    #'''
    
    
    
    tnp_ee_eta.Add(tnp_eg_eta)
    tpf_eta = h2f.h2Fakerate("eta", tnp_ee_eta, tnp_eg_eta)
    tpf_eta.rebin(eta_binning)
    tpf_eta.addFitModel(rt.TF1("tpf_eta", "[0]+[1]*x", 0.2, 1.4))
    tpf_eta.fit()
    tpcan_eta = h2f.h2FakerateCanvas(tpf_eta)
    tpcan_eta.createCanvas(fs, name=pdf_str+"f_tnp_eta")
    #fs.savePdf(tpcan_eta.c, pdf_str+"f_tnp_eta")
    #'''
    
    
    
    
    '''
    clos_e_eta = aux.rebin(clos_e_eta, eta_binning)
    clos_g_eta = aux.rebin(clos_g_eta, eta_binning)
    
    tpclos_eta = h2f.closure("tnp_eta"+strClosure, clos_e_eta, clos_g_eta)
    tpclos_eta.addFakerate(tpgenf_eta, "Tag n Probe gen info"+strClosure)
    tpclos_eta.addFakerate(tpf_eta, "Tag n Probe"+strClosure)
    tpclos_eta.createFitPrediction()
    tpclos_eta.createHistoPrediction()
    
    tpclos_eta.histoCanvas("tnpgen_eta_closure"+strClosure, 0)
    tpclos_eta.histoCanvas("tnp_eta_closure"+strClosure, 1)
    fs.savePdf(tpclos_eta.ch["Tag n Probe gen info"+strClosure], "tnpgen_eta_closure_perbin"+strClosure)
    fs.savePdf(tpclos_eta.ch["Tag n Probe"+strClosure], "tnp_eta_closure_perbin"+strClosure)
    # '''
    
    
    return 0

def starter():
    print "gen_tp_debugging.starter()"
    
    #rt.gROOT.Reset()
    rt.gROOT.SetBatch(rt.kTRUE) # dont show the canvases
    
    plotfilename = "debugging.root"
    
    _fm = fm.FileManager()
    tstr = str(dt.datetime.now()).replace(" ", "_").replace(":","-").split(".")[0]
    fs = fm.FileSaver(plotfilename,tstr,_fm)
    
    #filename = "myTestSelector_DY_debug_genMatchTagnProbe_1458133145.root"
    #filename = "myTestSelector_DY_debug_genMatchTagnProbe_1458257495.root"
    #filename = "myTestSelector_DY_debug_genMatchTagnProbe_1458263936.root"
    #filename = "myTestSelector_DY_debug_genMatchTagnProbe_1458270353.root"
    #filename = "myTestSelector_DY_debug_genMatchTagnProbe_1458271176.root"
    filename = "myTestSelector_DY_debug_genMatchTagnProbe_1458273497.root"
    h, h2, h3 = _fm.loadfile(filename)
    
    tnp_ee = h2["tnp_ee"].Clone() # pt dependency
    tnp_eg = h2["tnp_eg"].Clone()
    tnp_gen_ee = h2["tnp_gen_ee"].Clone()
    tnp_gen_eg = h2["tnp_gen_eg"].Clone()
    
    tnp_gen_ee_ntracks = h2["tnp_gen_ee_ntracks"].Clone()
    tnp_gen_eg_ntracks = h2["tnp_gen_eg_ntracks"].Clone()
    tnp_ee_ntracks = h2["tnp_ee_ntracks"].Clone()
    tnp_eg_ntracks = h2["tnp_eg_ntracks"].Clone()
    
    tnp_gen_ee_nvtx = h2["tnp_gen_ee_nvtx"].Clone()
    tnp_gen_eg_nvtx = h2["tnp_gen_eg_nvtx"].Clone()
    tnp_ee_nvtx = h2["tnp_ee_nvtx"].Clone()
    tnp_eg_nvtx = h2["tnp_eg_nvtx"].Clone()
    
    tnp_gen_ee_eta = h2["tnp_gen_ee_eta"].Clone()
    tnp_gen_eg_eta = h2["tnp_gen_eg_eta"].Clone()
    tnp_ee_eta = h2["tnp_ee_eta"].Clone()
    tnp_eg_eta = h2["tnp_eg_eta"].Clone()
    
    
    samplename = "DYJetsToLL"
    strClosure = "_perbin"
    # closure on wgamma sample
    #filename = "myTestSelector_WG_debug_genMatchTagnProbe_1458286818.root"
    
    if "WG" in filename:
        strClosure = "_perbin_wg"
        samplename = "WGToLNuG"
        del h
        del h2
        del h3
        h, h2, h3 = _fm.loadfile(filename)
    
    clos_e = h["clos_gen_e"]
    clos_g = h["clos_gen_g"]
    clos_e_ntracks = h["clos_gen_e_ntracks"]
    clos_g_ntracks = h["clos_gen_g_ntracks"]
    clos_e_nvtx = h["clos_gen_e_nvtx"]
    clos_g_nvtx = h["clos_gen_g_nvtx"]
    clos_e_eta = h["clos_gen_e_eta"]
    clos_g_eta = h["clos_gen_g_eta"]
    
    # '''
    
    
    # pt:
    pt_binning = range(40, 60, 5)+range(60, 100, 10)+range(100, 200+1, 20)
    #pt_binning = range(40, 60, 5)+range(60, 160+1, 10)#+range(100, 200+1, 20)
    print "pt_binning: ", pt_binning
    
    tnp_ee.Add(tnp_eg)
    tpf = h2f.h2Fakerate("pt", tnp_ee, tnp_eg)
    tpf.rebin(pt_binning)
    tpf.addFitModel(rt.TF1("tpf", "[0]+[1]/x+[2]/(x*x)", 40, 200))
    tpf.fit()
    tpcan = h2f.h2FakerateCanvas(tpf)
    tpcan.setLabel("Tag n probe")
    
    tnp_gen_ee.Add(tnp_gen_eg)
    tpgenf = h2f.h2Fakerate("pt_gen", tnp_gen_ee, tnp_gen_eg)
    tpgenf.rebin(pt_binning)
    tpgenf.addFitModel(rt.TF1(  "tpgenf", "[0]+[1]/x+[2]/(x*x)", 40, 200))
    tpgenf.fit()
    tpgencan = h2f.h2FakerateCanvas(tpgenf)
    tpgencan.setLabel("Tag n probe + gen match")
    
    clos_e = aux.rebin(clos_e, pt_binning)
    clos_g = aux.rebin(clos_g, pt_binning)
    
    tpclos = h2f.closure("tagnprobe"+strClosure, clos_e, clos_g)
    tpclos.setLabel("Closure on "+samplename)
    tpclos.addFakerate(tpgenf, "Tag n Probe gen info"+strClosure)
    tpclos.addFakerate(tpf, "Tag n Probe"+strClosure)
    tpclos.createFitPrediction()
    tpclos.createHistoPrediction()
    
    tpgencan.createCanvas(fs, name="f_tnpgen_pt")#, yrng=[0.002, 0.015])
    tpcan.createCanvas(fs, name="f_tnp_pt")#, yrng=[0.002, 0.015])
    
    # def histoCanvas(self, fs, name, clabel, logy=True, *opt):
    tpclos.histoCanvas( fs,
                        "tnpgen_pt_closure_ownclas"+strClosure,
                        "Closure on "+samplename)
    tpclos.histoCanvas( fs,
                        "tnp_pt_closure_ownclass"+strClosure,
                        "Closure on "+samplename,
                        opt=[1])
    h2f.histoCanvas(    fs,
                        "tnpgen_pt_closure"+strClosure, 
                        tpclos.target, 
                        tpclos.predictionh[0],
                        "Closure on "+samplename)
    h2f.histoCanvas(    fs,
                        "tnp_pt_closure"+strClosure, 
                        tpclos.target, 
                        tpclos.predictionh[1],
                        "Closure on "+samplename)
    
    
    # Ntracks:
    ntracks_binning = range(0, 200+1, 10)
    print "ntracks_binning: ", ntracks_binning
    tnp_gen_ee_ntracks.Add(tnp_gen_eg_ntracks)
    tpgenf_ntracks = h2f.h2Fakerate("ntracks_gen", tnp_gen_ee_ntracks, tnp_gen_eg_ntracks)
    tpgenf_ntracks.rebin(ntracks_binning)
    tpgenf_ntracks.addFitModel(rt.TF1("tpgenf_ntracks", "[0]+[1]*x+[2]*x*x", 0, 200))
    tpgenf_ntracks.fit()
    tpgencan_ntracks = h2f.h2FakerateCanvas(tpgenf_ntracks)
    tpgencan_ntracks.setLabel("Tag n probe + gen match")
    
    tnp_ee_ntracks.Add(tnp_eg_ntracks)
    tpf_ntracks = h2f.h2Fakerate("ntracks", tnp_ee_ntracks, tnp_eg_ntracks)
    tpf_ntracks.rebin(ntracks_binning)
    tpf_ntracks.addFitModel(rt.TF1("tpf_ntracks", "[0]+[1]*x+[2]*x*x", 0, 200))
    tpf_ntracks.fit()
    tpcan_ntracks = h2f.h2FakerateCanvas(tpf_ntracks)
    tpcan_ntracks.setLabel("Tag n probe")
    
    clos_e_ntracks = aux.rebin(clos_e_ntracks, ntracks_binning)
    clos_g_ntracks = aux.rebin(clos_g_ntracks, ntracks_binning)
    
    tpclos_ntracks = h2f.closure("tnp_ntracks"+strClosure, clos_e_ntracks, clos_g_ntracks)
    tpclos_ntracks.addFakerate(tpgenf_ntracks, "Tag n Probe gen info"+strClosure)
    tpclos_ntracks.addFakerate(tpf_ntracks, "Tag n Probe"+strClosure)
    tpclos_ntracks.createFitPrediction()
    tpclos_ntracks.createHistoPrediction()
    
    tpgencan_ntracks.createCanvas(fs, yrng=[0., .03], name="f_tnpgen_ntracks")
    tpcan_ntracks.createCanvas(fs, yrng=[0., .03], name="f_tnp_ntracks")
    h2f.histoCanvas(    fs,
                        "tnpgen_ntracks_closure"+strClosure,
                        tpclos_ntracks.target,
                        tpclos_ntracks.predictionh[0],
                        "Closure on "+samplename)
    h2f.histoCanvas(    fs,
                        "tnp_ntracks_closure"+strClosure,
                        tpclos_ntracks.target,
                        tpclos_ntracks.predictionh[1],
                        "Closure on "+samplename)
    
    
    # Nvtx:
    nvtx_binning = range(0, 30+1, 1)
    print "nvtx_binning: ", nvtx_binning
    tnp_gen_ee_nvtx.Add(tnp_gen_eg_nvtx)
    tpgenf_nvtx = h2f.h2Fakerate("nvtx_gen", tnp_gen_ee_nvtx, tnp_gen_eg_nvtx)
    
    tpgenf_nvtx.addFitModel(rt.TF1("tpgenf_nvtx", "[0]+[1]*x", 0, 30))
    tpgenf_nvtx.fit()
    tpgencan_nvtx = h2f.h2FakerateCanvas(tpgenf_nvtx)
    tpgencan_nvtx.setLabel("Tag n probe + gen match")
    
    tnp_ee_nvtx.Add(tnp_eg_nvtx)
    tpf_nvtx = h2f.h2Fakerate("nvtx", tnp_ee_nvtx, tnp_eg_nvtx)
    
    tpf_nvtx.addFitModel(rt.TF1("tpf_nvtx", "[0]+[1]*x", 0, 30))
    tpf_nvtx.fit()
    tpcan_nvtx = h2f.h2FakerateCanvas(tpf_nvtx)
    tpcan_nvtx.setLabel("Tag n probe")
    
    clos_e_nvtx = aux.rebin(clos_e_nvtx, nvtx_binning)
    clos_g_nvtx = aux.rebin(clos_g_nvtx, nvtx_binning)
    
    tpclos_nvtx = h2f.closure("tnp_nvtx"+strClosure, clos_e_nvtx, clos_g_nvtx)
    
    tpclos_nvtx.addFakerate(tpgenf_nvtx, "Tag n Probe gen info"+strClosure)
    tpclos_nvtx.addFakerate(tpf_nvtx, "Tag n Probe"+strClosure)
    
    tpclos_nvtx.createFitPrediction()
    tpclos_nvtx.createHistoPrediction()
    
    
    tpgencan_nvtx.createCanvas(fs, name="f_tnpgen_nvtx")
    tpcan_nvtx.createCanvas(fs, name="f_tnp_nvtx")
    h2f.histoCanvas(    fs,
                        "tnpgen_nvtx_closure"+strClosure,
                        tpclos_nvtx.target,
                        tpclos_nvtx.predictionh[0],
                        "Closure on "+samplename)
    h2f.histoCanvas(    fs,
                        "tnp_nvtx_closure"+strClosure,
                        tpclos_nvtx.target,
                        tpclos_nvtx.predictionh[1],
                        "Closure on "+samplename)
    
    # Eta:
    eta_binning = range(0, 150+1, 10)
    for i in range(len(eta_binning)):
        eta_binning[i] = round(eta_binning[i]*0.01, 2)
        
    print "eta_binning: ", eta_binning
    tnp_gen_ee_eta.Add(tnp_gen_eg_eta)
    tpgenf_eta = h2f.h2Fakerate("eta_gen", tnp_gen_ee_eta, tnp_gen_eg_eta)
    tpgenf_eta.rebin(eta_binning)
    tpgenf_eta.addFitModel(rt.TF1("tpgenf_eta", "[0]+[1]*x", 0.1, 1.4))
    tpgenf_eta.fit()
    tpgencan_eta = h2f.h2FakerateCanvas(tpgenf_eta)
    tpgencan_eta.setLabel("Tag n probe + gen match")
    
    tnp_ee_eta.Add(tnp_eg_eta)
    tpf_eta = h2f.h2Fakerate("eta", tnp_ee_eta, tnp_eg_eta)
    tpf_eta.rebin(eta_binning)
    tpf_eta.addFitModel(rt.TF1("tpf_eta", "[0]+[1]*x", 0.1, 1.4))
    tpf_eta.fit()
    tpcan_eta = h2f.h2FakerateCanvas(tpf_eta)
    tpcan_eta.setLabel("Tag n probe")
    
    #clos_e_eta = aux.rebin(clos_e_eta, eta_binning)
    #clos_g_eta = aux.rebin(clos_g_eta, eta_binning)
    clos_e_eta.Rebin(10)
    clos_g_eta.Rebin(10)
    
    tpclos_eta = h2f.closure("tnp_eta"+strClosure, clos_e_eta, clos_g_eta)
    tpclos_eta.addFakerate(tpgenf_eta, "Tag n Probe gen info"+strClosure)
    tpclos_eta.addFakerate(tpf_eta, "Tag n Probe"+strClosure)
    tpclos_eta.createFitPrediction()
    tpclos_eta.createHistoPrediction()
    
    tpgencan_eta.createCanvas(fs, name="f_tnpgen_eta")
    tpcan_eta.createCanvas(fs, name="f_tnp_eta")
    h2f.histoCanvas(    fs,
                        "tnpgen_eta_closure"+strClosure,
                        tpclos_eta.target,
                        tpclos_eta.predictionh[0],
                        "Closure on "+samplename,
                        logy=False)
    h2f.histoCanvas(    fs,
                        "tnp_eta_closure"+strClosure,
                        tpclos_eta.target,
                        tpclos_eta.predictionh[0],
                        "Closure on "+samplename,
                        logy=False)
    
    
    
    # '''
    
    #c = rt.TCanvas("c", "c", 600, 600)
    #c.cd()
    #tpf.egraph.Draw("ap")
    #raw_input()
    #raw_input()
    #x = 50.
    #err = [rt.Double(0)]
    #xerr = tpf.fitres.GetConfidenceIntervals(1, 1, 1, x, err, 0.683, False);
    #print tpf.fitres.GetConfidenceIntervals(10, 1, 2, x, err, 0.94999, True);
    
    
    #raw_input()
    
    return 0

def simpleBackgroundFit():
    # try a simple background fit
    
    print "gen_tp_debugging.simpleBackgroundFit()"
    rt.gROOT.SetBatch(rt.kTRUE) # dont show the canvases
    
    plotfilename = "simpleBkgFit.root"
    
    _fm = fm.FileManager()
    tstr = str(dt.datetime.now()).replace(" ", "_").replace(":","-").split(".")[0]
    fs = fm.FileSaver(plotfilename,tstr,_fm)
    
    filename = "myTestSelector_NEW_tnp_forPlots_1460042765.root"
    
    h, h2, h3 = _fm.loadfile(filename)
    
    ee = h2["tnp_ee"].Clone()
    eg = h2["tnp_eg"].Clone()
    
    
    
    
    
    return 0

def tagnprobe_new():
    print "gen_tp_debugging.tagnprobe_new()"
    
    #rt.gROOT.Reset()
    rt.gROOT.SetBatch(rt.kTRUE) # dont show the canvases
    
    plotfilename = "debugging.root"
    
    _fm = fm.FileManager()
    tstr = str(dt.datetime.now()).replace(" ", "_").replace(":","-").split(".")[0]
    fs = fm.FileSaver(plotfilename,tstr,_fm)
    
    #filename = "myTestSelector_DY_debug_genMatchTagnProbe_1458133145.root"
    #filename = "myTestSelector_DY_debug_genMatchTagnProbe_1458257495.root"
    #filename = "myTestSelector_DY_debug_genMatchTagnProbe_1458263936.root"
    #filename = "myTestSelector_DY_debug_genMatchTagnProbe_1458270353.root"
    #filename = "myTestSelector_DY_debug_genMatchTagnProbe_1458271176.root"
    #filename = "myTestSelector_DY_debug_genMatchTagnProbe_1458273497.root"
    
    #filename = "myTestSelector_NEW_tnp_forPlots_1460026482.root"
    #filename = "myTestSelector_NEW_tnp_forPlots_1460033048.root"
    #filename = "myTestSelector_NEW_tnp_forPlots_1460036199.root"
    #filename = "myTestSelector_NEW_tnp_forPlots_1460036586.root"
    #filename = "myTestSelector_NEW_tnp_forPlots_1460040961.root"
    #filename = "myTestSelector_NEW_tnp_forPlots_1460042765.root"
    #filename = "myTestSelector_NEW_tnp_forPlots_1460048500.root"
    #filename = "myTestSelector_NEW_tnp_forPlots_1460109853.root"
    filename = "myTestSelector_NEW_tnp_forPlots_1460109853.root"
    
    
    h, h2, h3 = _fm.loadfile(filename)
    
    tnp_ee = h2["tnp_ee"].Clone() # pt dependency
    tnp_eg = h2["tnp_eg"].Clone()
    
    tnp_ee_ntracks = h2["tnp_ee_ntracks"].Clone()
    tnp_eg_ntracks = h2["tnp_eg_ntracks"].Clone()
    
    tnp_ee_nvtx = h2["tnp_ee_nvtx"].Clone()
    tnp_eg_nvtx = h2["tnp_eg_nvtx"].Clone()
    
    tnp_ee_eta = h2["tnp_ee_eta"].Clone()
    tnp_eg_eta = h2["tnp_eg_eta"].Clone()
    
    
    
    samplename = "DYJetsToLL"
    strClosure = "_perbin"
    
    
    
    '''
    # closure on wgamma sample
    #filename = "myTestSelector_WG_debug_genMatchTagnProbe_1458286818.root"
    
    
    if "WG" in filename:
        strClosure = "_perbin_wg"
        samplename = "WGToLNuG"
        del h
        del h2
        del h3
        h, h2, h3 = _fm.loadfile(filename)
    # '''
    
    clos_e = h["clos_gen_e"]
    clos_g = h["clos_gen_g"]
    clos_e_ntracks = h["clos_gen_e_ntracks"]
    clos_g_ntracks = h["clos_gen_g_ntracks"]
    clos_e_nvtx = h["clos_gen_e_nvtx"]
    clos_g_nvtx = h["clos_gen_g_nvtx"]
    clos_e_eta = h["clos_gen_e_eta"]
    clos_g_eta = h["clos_gen_g_eta"]
    
    # '''
    
    
    # BINNINGS
    
    #ss = "1bin_" # for the plots
    ss = "_1bin"
    
    #pt_binning = range(40, 60, 5)+range(60, 100, 10)+range(100, 200+1, 20)
    pt_binning = range(40, 201, 160)
    print "pt_binning: ", pt_binning
    #ntracks_binning = range(0, 200+1, 10)
    ntracks_binning = range(0, 200+1, 200)
    print "ntracks_binning: ", ntracks_binning
    #nvtx_binning = range(0, 30+1, 1)
    nvtx_binning = range(0, 30+1, 30)
    print "nvtx_binning: ", nvtx_binning
    #eta_binning = range(0, 150+1, 10)
    eta_binning = range(0, 150+1, 150)
    for i in range(len(eta_binning)):
        eta_binning[i] = eta_binning[i]*0.01
    #    #eta_binning[i] = round(eta_binning[i]*0.01, 2)
    
    
    # CALCULATIONS
    
    # pt:
    tnp_ee.Add(tnp_eg)
    tpf = h2f.h2Fakerate("pt", tnp_ee, tnp_eg)
    tpf.rebin(pt_binning)
    #tpf.addFitModel(rt.TF1("tpf", "[0]+[1]/x+[2]/(x*x)", 40, 200))
    tpf.addFitModel(rt.TF1("tpf", "[0]", 40, 200))
    tpf.fit()
    tpcan = h2f.h2FakerateCanvas(tpf)
    tpcan.setLabel("Tag n probe")
    
    clos_e = aux.rebin(clos_e, pt_binning)
    clos_g = aux.rebin(clos_g, pt_binning)
    
    tpclos = h2f.closure("tagnprobe"+strClosure, clos_e, clos_g)
    tpclos.setLabel("Closure on "+samplename)
    #tpclos.addFakerate(tpgenf, "Tag n Probe gen info"+strClosure)
    tpclos.addFakerate(tpf, "Tag n Probe"+strClosure)
    tpclos.createFitPrediction()
    tpclos.createHistoPrediction()
    
    
    # Ntracks:
    
    tnp_ee_ntracks.Add(tnp_eg_ntracks)
    tpf_ntracks = h2f.h2Fakerate("ntracks", tnp_ee_ntracks, tnp_eg_ntracks)
    tpf_ntracks.rebin(ntracks_binning)
    tpf_ntracks.addFitModel(rt.TF1("tpf_ntracks", "[0]+[1]*x+[2]*x*x", 0, 200))
    tpf_ntracks.fit()
    tpcan_ntracks = h2f.h2FakerateCanvas(tpf_ntracks)
    tpcan_ntracks.setLabel("Tag n probe")
    
    clos_e_ntracks = aux.rebin(clos_e_ntracks, ntracks_binning)
    clos_g_ntracks = aux.rebin(clos_g_ntracks, ntracks_binning)
    
    tpclos_ntracks = h2f.closure("tnp_ntracks"+strClosure, clos_e_ntracks, clos_g_ntracks)
    #tpclos_ntracks.addFakerate(tpgenf_ntracks, "Tag n Probe gen info"+strClosure)
    tpclos_ntracks.addFakerate(tpf_ntracks, "Tag n Probe"+strClosure)
    tpclos_ntracks.createFitPrediction()
    tpclos_ntracks.createHistoPrediction()
    
    
    
    
    # Nvtx:
    
    tnp_ee_nvtx.Add(tnp_eg_nvtx)
    tpf_nvtx = h2f.h2Fakerate("nvtx", tnp_ee_nvtx, tnp_eg_nvtx)
    tpf_nvtx.rebin(nvtx_binning)
    tpf_nvtx.addFitModel(rt.TF1("tpf_nvtx", "[0]+[1]*x", 0, 30))
    tpf_nvtx.fit()
    tpcan_nvtx = h2f.h2FakerateCanvas(tpf_nvtx)
    tpcan_nvtx.setLabel("Tag n probe")
    
    clos_e_nvtx = aux.rebin(clos_e_nvtx, nvtx_binning)
    clos_g_nvtx = aux.rebin(clos_g_nvtx, nvtx_binning)
    
    tpclos_nvtx = h2f.closure("tnp_nvtx"+strClosure, clos_e_nvtx, clos_g_nvtx)
    
    #tpclos_nvtx.addFakerate(tpgenf_nvtx, "Tag n Probe gen info"+strClosure)
    tpclos_nvtx.addFakerate(tpf_nvtx, "Tag n Probe"+strClosure)
    
    tpclos_nvtx.createFitPrediction()
    tpclos_nvtx.createHistoPrediction()
    
    
    
    
    # Eta:
        
    print "eta_binning: ", eta_binning
    
    tnp_ee_eta.Add(tnp_eg_eta)
    tpf_eta = h2f.h2Fakerate("eta", tnp_ee_eta, tnp_eg_eta)
    tpf_eta.rebin(eta_binning)
    tpf_eta.addFitModel(rt.TF1("tpf_eta", "[0]+[1]*x", 0.1, 1.4))
    tpf_eta.fit()
    tpcan_eta = h2f.h2FakerateCanvas(tpf_eta)
    tpcan_eta.setLabel("Tag n probe")
    
    clos_e_eta = aux.rebin(clos_e_eta, eta_binning)
    clos_g_eta = aux.rebin(clos_g_eta, eta_binning)
    #clos_e_eta.Rebin(10)
    #clos_g_eta.Rebin(10)
    
    tpclos_eta = h2f.closure("tnp_eta"+strClosure, clos_e_eta, clos_g_eta)
    #tpclos_eta.addFakerate(tpgenf_eta, "Tag n Probe gen info"+strClosure)
    tpclos_eta.addFakerate(tpf_eta, "Tag n Probe"+strClosure)
    tpclos_eta.createFitPrediction()
    tpclos_eta.createHistoPrediction()
        
    
    
    # PLOTS and CANVASES
    
    
    
    tpcan.createCanvas(fs, name="f_tnp_pt"+ss)#, yrng=[0.002, 0.015])
    tpcan_ntracks.createCanvas(fs, yrng=[0., .03], name="f_tnp_ntracks"+ss)
    tpcan_nvtx.createCanvas(fs, name="f_tnp_nvtx"+ss)
    tpcan_eta.createCanvas(fs, name="f_tnp_eta"+ss)
    
    # def histoCanvas(self, fs, name, clabel, logy=True, *opt):
    tpclos.histoCanvas( fs,
                        "tnp_pt_closure_ownclass"+ss+strClosure,
                        "Closure on "+samplename,
                        logy = True,
                        opt=[0])
    h2f.histoCanvas(    fs,
                        "tnp_pt_closure"+ss+strClosure, 
                        tpclos.target, 
                        tpclos.predictionh[0],
                        "Closure on "+samplename,
                        logy = True,
                        yrng = [1000., 10000000.])
    h2f.histoCanvas(    fs,
                        "tnp_ntracks_closure"+ss+strClosure,
                        tpclos_ntracks.target,
                        tpclos_ntracks.predictionh[0],
                        "Closure on "+samplename)
    h2f.histoCanvas(    fs,
                        "tnp_nvtx_closure"+ss+strClosure,
                        tpclos_nvtx.target,
                        tpclos_nvtx.predictionh[0],
                        "Closure on "+samplename,
                        logy = True,
                        yrng = [1, 10000000])
    h2f.histoCanvas(    fs,
                        "tnp_eta_closure"+ss+strClosure,
                        tpclos_eta.target,
                        tpclos_eta.predictionh[0],
                        "Closure on "+samplename,
                        logy=True)
    
    
    # '''
    
    #c = rt.TCanvas("c", "c", 600, 600)
    #c.cd()
    #tpf.egraph.Draw("ap")
    #raw_input()
    #raw_input()
    #x = 50.
    #err = [rt.Double(0)]
    #xerr = tpf.fitres.GetConfidenceIntervals(1, 1, 1, x, err, 0.683, False);
    #print tpf.fitres.GetConfidenceIntervals(10, 1, 2, x, err, 0.94999, True);
    
    
    #raw_input()
    
    return 0

def creator(name, ee, eg, f, fs, binning=range(3)):
    '''
    tpf = h2f.h2Fakerate("pt", tnp_ee, tnp_eg)
    tpf.rebin(pt_binning)
    tpf.addFitModel(rt.TF1("tpf", "[0]+[1]/x+[2]/(x*x)", 40, 200))
    tpf.fit()
    tpcan = h2f.h2FakerateCanvas(tpf)
    tpcan.setLabel("aaa")
    tpcan.createCanvas(fs, name="f_tnp_pt")
    #fs.savePdf(tpcan.c, "f_tnp_pt")
    # '''
    return 0

def main():
    
    print "gen_tp_debugging.main()"
    a = 5
    
    tagnprobe_new()
    #starter()
    #diphoton()
    
    return 0

if __name__ == "__main__":
    main()
