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
	
	rt.gROOT.SetBatch(rt.kTRUE)	# dont show the canvases
	
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
	
	
	
	
	
	# '''
	
	'''
	tnp_num = tnp_eg.Clone()
	tnp_den = tnp_ee.Clone()
	tnp_den.Add(tnp_eg)
	proj1 = tnp_num.ProjectionX()
	proj2 = tnp_den.ProjectionX()
	
	tnpgen_num = tnp_gen_eg.Clone()
	tnpgen_den = tnp_gen_ee.Clone()
	tnpgen_den.Add(tnp_gen_eg)
	proj3 = tnpgen_num.ProjectionX()
	proj4 = tnpgen_den.ProjectionX()
	print "f (tnp)    =", float(proj1.Integral())/(proj2.Integral())
	print "f (tnpgen) =", float(proj3.Integral())/(proj4.Integral())
	#'''
	
	
	pt_binning = range(40, 60+1, 5)+range(60, 100+1, 10)+range(100, 200+1, 20)
	
	
	
	
	
	tnp_ee.Add(tnp_eg)
	tpf = h2f.h2Fakerate("pt", tnp_ee, tnp_eg) #, 60, 120)
	tpf.rebin(pt_binning)
	tpf.addFitModel(rt.TF1("tpf", "[0]+[1]*x+[2]*(x*x)", 40, 200))
	tpf.fit()
	tpcan = h2f.h2FakerateCanvas(tpf)
	tpcan.createCanvas()
	fs.savePdf(tpcan.c, pdf_str+"f_tnp_pt")
	# '''
	
	#del tpf
	#del tpcan
	
	
	
	'''
	tnp_gen_ee.Add(tnp_gen_eg)
	tpgenf = h2f.h2Fakerate("pt_gen", tnp_gen_ee, tnp_gen_eg)
	tpgenf.rebin(pt_binning)
	tpgenf.addFitModel(rt.TF1(	"tpgenf", "[0]+[1]/x+[2]/(x*x)", 40, 200))
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
	tpgencan_ntracks.createCanvas([0., .03])
	fs.savePdf(tpgencan_ntracks.c, pdf_str+"f_tnpgen_ntracks")
	#'''
	
	
	
	
	
	'''
	tnp_ee_ntracks.Add(tnp_eg_ntracks)
	tpf_ntracks = h2f.h2Fakerate("ntracks", tnp_ee_ntracks, tnp_eg_ntracks)
	tpf_ntracks.rebin(ntracks_binning)
	tpf_ntracks.addFitModel(rt.TF1("tpf_ntracks", "[0]+[1]*x+[2]*x*x", 0, 200))
	tpf_ntracks.fit()
	tpcan_ntracks = h2f.h2FakerateCanvas(tpf_ntracks)
	tpcan_ntracks.createCanvas([0., .03])
	fs.savePdf(tpcan_ntracks.c, pdf_str+"f_tnp_ntracks")
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
	tpgencan_nvtx.createCanvas()
	fs.savePdf(tpgencan_nvtx.c, "f_tnpgen_nvtx")
	#'''
	
	
	
	
	'''
	tnp_ee_nvtx.Add(tnp_eg_nvtx)
	tpf_nvtx = h2f.h2Fakerate("nvtx", tnp_ee_nvtx, tnp_eg_nvtx)
	
	tpf_nvtx.addFitModel(rt.TF1("tpf_nvtx", "[0]+[1]*x", 0, 30))
	tpf_nvtx.fit()
	tpcan_nvtx = h2f.h2FakerateCanvas(tpf_nvtx)
	tpcan_nvtx.createCanvas([0., 0.04])
	fs.savePdf(tpcan_nvtx.c, pdf_str+"f_tnp_nvtx")
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
	
	
	'''
	tnp_ee_eta.Add(tnp_eg_eta)
	tpf_eta = h2f.h2Fakerate("eta", tnp_ee_eta, tnp_eg_eta)
	tpf_eta.rebin(eta_binning)
	tpf_eta.addFitModel(rt.TF1("tpf_eta", "[0]+[1]*x", 0.2, 1.4))
	tpf_eta.fit()
	tpcan_eta = h2f.h2FakerateCanvas(tpf_eta)
	tpcan_eta.createCanvas()
	fs.savePdf(tpcan_eta.c, pdf_str+"f_tnp_eta")
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
	
	rt.gROOT.SetBatch(rt.kTRUE)	# dont show the canvases
	
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
	
	
	# closure on wgamma sample
	filename = "myTestSelector_WG_debug_genMatchTagnProbe_1458286818.root"
	
	
	strClosure = ""
	if "WG" in filename:
		strClosure = "_wg"
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
	
	'''
	tnp_num = tnp_eg.Clone()
	tnp_den = tnp_ee.Clone()
	tnp_den.Add(tnp_eg)
	proj1 = tnp_num.ProjectionX()
	proj2 = tnp_den.ProjectionX()
	
	tnpgen_num = tnp_gen_eg.Clone()
	tnpgen_den = tnp_gen_ee.Clone()
	tnpgen_den.Add(tnp_gen_eg)
	proj3 = tnpgen_num.ProjectionX()
	proj4 = tnpgen_den.ProjectionX()
	print "f (tnp)    =", float(proj1.Integral())/(proj2.Integral())
	print "f (tnpgen) =", float(proj3.Integral())/(proj4.Integral())
	#'''
	
	
	pt_binning = range(40, 60+1, 5)+range(60, 100+1, 10)+range(100, 200+1, 20)
	#pt_binning = range(40, 61, 5)+range(60, 101, 10)
	
	
	
	tnp_ee.Add(tnp_eg)
	tpf = h2f.h2Fakerate("pt", tnp_ee, tnp_eg)
	tpf.rebin(pt_binning)
	tpf.addFitModel(rt.TF1("tpf", "[0]+[1]/x+[2]/(x*x)", 40, 200))
	tpf.fit()
	tpcan = h2f.h2FakerateCanvas(tpf)
	tpcan.createCanvas()
	fs.savePdf(tpcan.c, "f_tnp_pt")
	
	tnp_gen_ee.Add(tnp_gen_eg)
	tpgenf = h2f.h2Fakerate("pt_gen", tnp_gen_ee, tnp_gen_eg)
	tpgenf.rebin(pt_binning)
	tpgenf.addFitModel(rt.TF1(	"tpgenf", "[0]+[1]/x+[2]/(x*x)", 40, 200))
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
	
	#del tpf, tpgenf, tpclos, tpcan, tpgencan
	
	
	'''
	ntracks_binning = range(0,200+1,10)
	tnp_gen_ee_ntracks.Add(tnp_gen_eg_ntracks)
	tpgenf_ntracks = h2f.h2Fakerate("ntracks_gen", tnp_gen_ee_ntracks, tnp_gen_eg_ntracks)
	tpgenf_ntracks.rebin(ntracks_binning)
	tpgenf_ntracks.addFitModel(rt.TF1("tpgenf_ntracks", "[0]+[1]*x+[2]*x*x", 0, 200))
	tpgenf_ntracks.fit()
	tpgencan_ntracks = h2f.h2FakerateCanvas(tpgenf_ntracks)
	tpgencan_ntracks.createCanvas([0., .03])
	fs.savePdf(tpgencan_ntracks.c, "f_tnpgen_ntracks")
	
	tnp_ee_ntracks.Add(tnp_eg_ntracks)
	tpf_ntracks = h2f.h2Fakerate("ntracks", tnp_ee_ntracks, tnp_eg_ntracks)
	tpf_ntracks.rebin(ntracks_binning)
	tpf_ntracks.addFitModel(rt.TF1("tpf_ntracks", "[0]+[1]*x+[2]*x*x", 0, 200))
	tpf_ntracks.fit()
	tpcan_ntracks = h2f.h2FakerateCanvas(tpf_ntracks)
	tpcan_ntracks.createCanvas([0., .03])
	fs.savePdf(tpcan_ntracks.c, "f_tnp_ntracks")
	
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
	
	#del tpf_ntracks, tpgenf_ntracks, tpclos_ntracks
	#'''
	
	
	'''
	nvtx_binning = range(30)
	tnp_gen_ee_nvtx.Add(tnp_gen_eg_nvtx)
	tpgenf_nvtx = h2f.h2Fakerate("nvtx_gen", tnp_gen_ee_nvtx, tnp_gen_eg_nvtx)
	
	tpgenf_nvtx.addFitModel(rt.TF1("tpgenf_nvtx", "[0]+[1]*x", 0, 30))
	tpgenf_nvtx.fit()
	tpgencan_nvtx = h2f.h2FakerateCanvas(tpgenf_nvtx)
	tpgencan_nvtx.createCanvas()
	fs.savePdf(tpgencan_nvtx.c, "f_tnpgen_nvtx")
	
	tnp_ee_nvtx.Add(tnp_eg_nvtx)
	tpf_nvtx = h2f.h2Fakerate("nvtx", tnp_ee_nvtx, tnp_eg_nvtx)
	
	tpf_nvtx.addFitModel(rt.TF1("tpf_nvtx", "[0]+[1]*x", 0, 30))
	tpf_nvtx.fit()
	tpcan_nvtx = h2f.h2FakerateCanvas(tpf_nvtx)
	tpcan_nvtx.createCanvas()
	fs.savePdf(tpcan_nvtx.c, "f_tnp_nvtx")
	
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
	
	
	'''
	eta_binning = range(0, 150+1, 10)
	for i in range(len(eta_binning)):
		eta_binning[i] = eta_binning[i]*0.01
	tnp_gen_ee_eta.Add(tnp_gen_eg_eta)
	tpgenf_eta = h2f.h2Fakerate("eta_gen", tnp_gen_ee_eta, tnp_gen_eg_eta)
	tpgenf_eta.rebin(eta_binning)
	tpgenf_eta.addFitModel(rt.TF1("tpgenf_eta", "[0]+[1]*x", 0.1, 1.4))
	tpgenf_eta.fit()
	tpgencan_eta = h2f.h2FakerateCanvas(tpgenf_eta)
	tpgencan_eta.createCanvas()
	fs.savePdf(tpgencan_eta.c, "f_tnpgen_eta")
	
	tnp_ee_eta.Add(tnp_eg_eta)
	tpf_eta = h2f.h2Fakerate("eta", tnp_ee_eta, tnp_eg_eta)
	tpf_eta.rebin(eta_binning)
	tpf_eta.addFitModel(rt.TF1("tpf_eta", "[0]+[1]*x", 0.1, 1.4))
	tpf_eta.fit()
	tpcan_eta = h2f.h2FakerateCanvas(tpf_eta)
	tpcan_eta.createCanvas()
	fs.savePdf(tpcan_eta.c, "f_tnp_eta")
	
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
	
	
	
	
	
	
	
	
	
	#pc.histoplot("some1", tpclos.h[0], fs)
	
	
	
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

def main():
	
	print "gen_tp_debugging.main()"
	
	#rt.gROOT.Reset()
	starter()
	#diphoton()
	
	return 0

if __name__ == "__main__":
	main()
