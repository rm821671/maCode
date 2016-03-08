
import sys
sys.path.insert(0,"../../py")

import ROOT as rt
from modules import lib_plot as lp
from modules import auxiliary as aux
from modules import filemanager as fm
from modules import analysis as an

import time
import datetime as dt


# Define the kinematic variables dependencies used for the plots.
# These have to be written in the histograms!
'''
Gvariables = {	"pt": 	None,
				"nvtx": None,
				"njet":	None,
				"eta":	None
					}
# '''

def main():
	rt.gROOT.SetBatch(rt.kTRUE)	# dont show the canvases
	#raw_input("start..")
	
	specific = "none"
	
	today = str(dt.date.today())
	#plotfilename = "fakerate_"+specific+"_"+today+".root"
	#plotfilename = "fakerate"+specific+"_"+today+".root"
	plotfilename = "fakerate_test.root"
	#print plotfilename
	
	
	# parameter object
	# 
	pars = fm.Parameters()
	
	# kinematic variables
	# the ranges are the bins
	pars.addvar("pt",[0,250])
	pars.addvar("nvtx",[0,60])
	pars.addvar("njet",[0,15])
	pars.addvar("eta",[-150,150])
	
	
	# file object
	_fm = fm.FileManager()
	print "dropbox path:", _fm.dropbox
	
	# h and h3 are not used here
	h, h2, h3 = _fm.readHisto(lines=6) # reads all data; no filtering
	
	# tag filter:
	h2 = aux.tagfilter(h2, "f_zpeak")
	
	# distinguish between data and montecarlo
	pars.stacks, pars.data, pars.legends = an.hstack(h2) 
	
	# create a file saver
	tstr = str(dt.datetime.now()).replace(" ", "_").replace(":","-").split(".")[0]
	#print "tstr: ", tstr
	# FileSaver(filename, treepath, _fm)
	_fs = fm.FileSaver(plotfilename,tstr,_fm)
	
	pars.fs = _fs
	
	# projections() uses the tags: "pas" and "tot" !!!
	pars.hproj = an.projections(pars)
	
	#for key in hproj.keys():
	#	print key
	#	for thing in hproj[key].keys():
	#		if "eta" in thing:
	#			for mod in hproj[key][thing].keys():
	#				for elem in hproj[key][thing][mod]:
	#					print elem
	#					print elem.Integral()
	
	
	dat, mc = an.calc_histos(pars)
	
	
	
	
	for key in dat["pt"].keys():
		dat["pt"][key].Rebin(10)
		mc["pt"][key].Rebin(10)
	
	pars.addfit("pt", rt.TF1("pt","[0]+[1]/x+[2]/(x*x)",40,200))
	pars.addfit("nvtx", rt.TF1("nvtx","[0]+[1]*x",0,20))
	pars.addfit("njet", rt.TF1("njet","[0]+[1]*x",1,10))
	pars.addfit("eta", rt.TF1("eta","[0]+[1]*x",0,600))
	
	#pars.addeffrng("pt", [0, 200])
	pars.addeffrng("nvtx", [0, 25])
	#pars.addeffrng("njet", [0, 10])
	#pars.addeffrng("eta", [0, 600])
	
	#for key in pars.variables.keys():
	#	for elem in pars.variables[key].keys():
	#		print key, elem, pars.variables[key][elem]
	#	#print key #pars.variables
	
	#return 0
	
	# create the efficiency plots
	for var in pars.variables.keys():
		print "kinematic variable: ", var
		an.eff(dat, var, pars)
		an.eff(mc, var, pars)
	
	
	# '''
	print "Plots written in " + _fs.path + plotfilename + ", Branch " + "/" + tstr
	return 0

if __name__ == "__main__":
	main()
