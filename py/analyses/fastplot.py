


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
	
	#rt.gROOT.SetBatch(rt.kTRUE)	# dont show the canvases
	
	plot()
	
	return 0

if __name__ == "__main__":
	main()
