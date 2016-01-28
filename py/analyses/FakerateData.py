
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
Gvariables = {	"pt": 	None,
				"nvtx": None,
				"njet":	None,
				"eta":	None
					}


def calc_histos(hproj):
	'''
	Needs a dictionary of projected histograms;
	from each histogram the integral is calculated in the range rng
	'''
	rng = [80, 100] # integral range
	
	#variables = ["pt","nvtx","njet"]
	variables = []
	for key in Gvariables.keys():
		variables.append(key)
	
	bins = dict.fromkeys(Gvariables.keys())
	for key in bins.keys():
		bins[key] = []
	
	bins["pt"] = [0, 250]
	bins["nvtx"] = [0, 60]
	bins["njet"] = [0, 15]
	bins["eta"] = [0, 60]
	
	data = dict.fromkeys(Gvariables.keys())
	for key in data.keys():
		data[key] = {	"tot": 0, 	"pas": 0}
	
	mc = dict.fromkeys(Gvariables.keys())
	for key in mc.keys():
		mc[key] = {	"tot": 0, "pas": 0}
	
	'''
	data = {	"pt": 	{	"tot": 0, 
							"pas": 0},
				"nvtx": {	"tot": 0, 
							"pas": 0},
				"njet": {	"tot": 0, 
							"pas": 0}
					}
	mc = 	{	"pt": 	{	"tot": 0, 
							"pas": 0},
				"nvtx": {	"tot": 0, 
							"pas": 0},
				"njet": {	"tot": 0, 
							"pas": 0}
					}
	# '''
	
	for key in data.keys():
		for inner in data[key].keys():
			data[key][inner] = rt.TH1F("data"+str(key)+str(inner),str(key)+str(inner),
										bins[key][1], bins[key][0], bins[key][1])
			mc[key][inner] = rt.TH1F("mc"+str(key)+str(inner),str(key)+str(inner),
										bins[key][1], bins[key][0], bins[key][1])
	
	# data:
	tag = "data"
	for variable in variables:
		for inner in data[variable].keys():
			for histo in hproj[tag][variable][inner]:
				name = histo.GetName()
				splitname = name.split("_")
				nbin = int(splitname[len(splitname)-1]) # last element is the bin number
				if histo.GetEntries() > 0:
					zint = histo.Integral(rng[0], rng[1])
					if zint > 0:
						data[variable][inner].SetBinContent(nbin, zint)
					else:
						#print "negative value data! variable, bin", variable, nbin
						data[variable][inner].SetBinContent(nbin, 0.)
	
	tag = "mc"
	for variable in variables:
		for inner in mc[variable].keys():
			for histo in hproj[tag][variable][inner]:
				name = histo.GetName()
				splitname = name.split("_")
				nbin = int(splitname[len(splitname)-1]) # last element is the bin number
				if histo.GetEntries() > 0:
					zint = histo.Integral(rng[0], rng[1])
					if zint > 0:
						mc[variable][inner].SetBinContent(nbin, zint)
					else:
						#print "negative value mc! variable / bin", variable, nbin
						mc[variable][inner].SetBinContent(nbin, 0)
	
	return data, mc



def projections(stacks, data, legends, fs):
	
	import copy
	
	
	hvariables = dict.fromkeys(Gvariables.keys())
	for key in hvariables.keys():
		hvariables[key] = {	"tot": [], "pas": []}
	
	'''
	hvariables = {	"pt": 	{	"tot": [], 
								"pas": []},
					"nvtx": {	"tot": [], 
								"pas": []},
					"njet": {	"tot": [], 
								"pas": []},
					"eta": {	"tot": [], 
								"pas": []}
					}
	# '''
	
	# initialize histogram dictionary
	hproj = {	"data":	copy.deepcopy(hvariables), 
				"mc":	copy.deepcopy(hvariables)
			}
	
	off0 = 0
	off1 = 0
	
	# range in bins! not x value!
	ranges = {	"pt": range(off0, 250+off1),
				"nvtx": range(off0,60+off1),
				"njet": range(off0,15+off1),
				"eta": range(off0, 60+off1)
			}
	
	variables = []
	for key in Gvariables.keys():
		variables.append(key)
	
	print "variables: ", variables
	
	# get stacked histograms from the montecarlo samples
	# Float_t binContent = ((TH1*)(stack->GetStack()->Last()))->GetBinContent(ibin);
	mc = dict.fromkeys(stacks.keys())
	mclist = dict.fromkeys(stacks.keys())
	for key in mc.keys():
		mc[key] = stacks[key].GetStack().Last()
		mclist[key] = stacks[key].GetStack()
	
	# how to get the single histograms from stack:
	#for key in mclist.keys():
	#	for ob in range(mclist[key].GetEntries()):
	#		print ob, " histo: ", mclist[key].At(ob)
	
	
	
	# ProjectionX (const char *name="_px", Int_t firstybin=0,
	#				 Int_t lastybin=-1, Option_t *option="")
	def projdata(h, *index):
		# h should be the defined histogram dictionary
		# 
		isdata = "data"
		for key in data.keys():
			for variable in variables:
				if variable in key:
					tag = "tot"
					if tag in key:
						for b in ranges[variable]:
							htemp = data[key].ProjectionX(
													variable+"_"+tag+"_"+str(b), b, b)
							htemp.SetDirectory(0)
							h[isdata][variable][tag].append(htemp)
					tag = "pas"
					if tag in key:
						for b in ranges[variable]:
							htemp = data[key].ProjectionX(
													variable+"_"+tag+"_"+str(b), b, b)
							htemp.SetDirectory(0)
							h[isdata][variable][tag].append(htemp)
		return h
	
	def projmc(h):
		# h should be the defined histogram dictionary
		# 
		isdata = "mc"
		for key in mc.keys():
			for variable in variables:
				if variable in key:
					tag = "tot"
					if tag in key:
						for b in ranges[variable]:
							htemp = mc[key].ProjectionX(
													variable+"_"+tag+"_"+str(b), b, b)
							htemp.SetDirectory(0)
							h[isdata][variable][tag].append(htemp)
					tag = "pas"
					if tag in key:
						for b in ranges[variable]:
							htemp = mc[key].ProjectionX(
													variable+"_"+tag+"_"+str(b), b, b)
							htemp.SetDirectory(0)
							h[isdata][variable][tag].append(htemp)
		return h
	
	
	hproj = projdata(hproj)
	hproj = projmc(hproj)
	
	#print hproj
	
	#hnvtx = projdata(hnvtx)
	return hproj

def hstack(h):
	# build a stack of the monte carlo
	# add up both data
	
	datakeys = {	"DYJetsToLL": 0,
					"TTGJets": 2,
					"WGToLNuG": 4,
					"ZGTo2LG": 6,
					"Run2015D-05Oct2015": 0,
					"Run2015D-PromptReco": 1
			}
	
	defcolor = {	0: rt.kGreen}
	colorindex = 0
	
	# initialize stack, legend
	stacks = {}	# stack for mc
	l = {}		# legend for mc
	dh = {}		# data
	dtemp = {}
	
	for key in h.keys():
		if h[key]:
			#print h[key]
			stacks = dict.fromkeys(h[key].keys())
			l = dict.fromkeys(h[key].keys())
	
	for key in stacks.keys():
		stacks[key] = rt.THStack(key, "")
		l[key] = rt.TLegend(0.9,0.7,0.65,0.9)
		dh[key] = {}
	
	# scale montecarlo samples and add to stack,
	# if data, add up
	for key in h.keys():
		if "Run2015" not in key: # mc
			scaling = aux.giveMClumiScalefactor(key)
			#print "mc scaling: ", scaling
			for his in h[key].keys():
				#print key, his
				h[key][his].Scale(scaling)
				h[key][his].SetFillColor(defcolor[colorindex]+datakeys[key])
				h[key][his].SetLineColor(defcolor[colorindex]+datakeys[key])
				l[his].AddEntry(h[key][his], key)
				stacks[his].Add(h[key][his])
		
		if "Run2015D-05Oct2015" in key:
			for his in h[key].keys():
				dh[his][key] = h[key][his]
				dh[his][key].SetMarkerStyle(20) # full square
				dh[his][key].SetMarkerColor(rt.kBlack)
				l[his].AddEntry(h[key][his], key)
		
		if "Run2015D-PromptReco" in key:
			for his in h[key].keys():
				dh[his][key] = h[key][his]
	
	# add reco and prompt reco
	for key in dh.keys():
		dh[key]["Run2015D-05Oct2015"].Add(dh[key]["Run2015D-PromptReco"])
	
	for key in dh.keys():
		dtemp[key] = dh[key]["Run2015D-05Oct2015"].Clone()
	
	#print stacks
	#print dh
	#print dtemp
	
	return stacks, dtemp, l

def eff_stuff(h, fs, *xrng):
	
	# some other plots
	
	h1loc = h["pas"].Clone()
	h2loc = h["tot"].Clone()
	
	print h1loc, h2loc
	
	h1loc = aux.rebin(h1loc, range(100,201,100))
	h2loc = aux.rebin(h2loc, range(100,201,100))
	
	if rt.TEfficiency.CheckConsistency(h1loc, h2loc):
		print "consistency checked!"
		eff = rt.TEfficiency(h1loc, h2loc)
		egraph = eff.CreateGraph()
		
		#egraph.Fit(f, "r")
		
		#fmin = f.GetMinimumX()
		#fmax = f.GetMaximumX()
		
		#pmin = (f.Eval(fmin-fmin/2.))
		#pmax = (f.Eval(fmax+fmax/2.))
		pmin = 0.005
		pmax = 0.045
		
		#print "xrng: ", xrng[0][0], xrng[0][1]
		if xrng:
			egraph.GetXaxis().SetRangeUser(xrng[0][0],xrng[0][1]);
		
		if pmin < pmax:
			egraph.SetMinimum(pmin)
			egraph.SetMaximum(pmax)
		if pmin >= pmax:
			egraph.SetMinimum(pmax)
			egraph.SetMaximum(pmin)
		
		csize = 800
		name = h1loc.GetName() + "_" + h2loc.GetName()
		egraph.SetTitle(name)
		c = rt.TCanvas("c_"+name,"c_"+name,csize,csize)
		c.cd()
		
		egraph.Draw("ap")
		#f.Draw("same")
		
		c.Update()
		
		fs.save(c, name)
		
	else:
		print "not consistent"

def eff(h,f,fs,*xrng):
	# f is a fit function
	#print f
	hp = h["pas"].Clone()
	ht = h["tot"].Clone()
	
	print "histo:", h
	print "function: ", f
	
	#if rt.TEfficiency.CheckConsistency(h["pas"], h["tot"]):
	if rt.TEfficiency.CheckConsistency(hp, ht):
		print "consistency checked!"
		#eff = rt.TEfficiency(h["pas"], h["tot"])
		eff = rt.TEfficiency(hp, ht)
		egraph = eff.CreateGraph()
		
		#if "eta" not in hp.GetName():
		egraph.Fit(f, "r")
		
		fmin = f.GetMinimumX()
		fmax = f.GetMaximumX()
		
		#pmin = (f.Eval(fmin-fmin/2.))
		#pmax = (f.Eval(fmax+fmax/2.))
		pmin = 0.005
		pmax = 0.05
		
		#print "xrng: ", xrng[0][0], xrng[0][1]
		if xrng:
			egraph.GetXaxis().SetRangeUser(xrng[0][0],xrng[0][1]);
		
		if pmin < pmax:
			egraph.SetMinimum(pmin)
			egraph.SetMaximum(pmax)
		if pmin >= pmax:
			egraph.SetMinimum(pmax)
			egraph.SetMaximum(pmin)
		
		csize = 800
		#name = h["pas"].GetName() + "_" + h["tot"].GetName()
		name = hp.GetName() + "_" + ht.GetName()
		egraph.SetTitle(name)
		c = rt.TCanvas("c_"+name,"c_"+name,csize,csize)
		c.cd()
		
		
		
		egraph.Draw("ap")
		#if "eta" not in hp.GetName():
		f.Draw("same")
		
		c.Update()
		
		fs.save(c, name)
		
	else:
		print "not consistent"

def main():
	
	rt.gROOT.SetBatch(rt.kTRUE)	# dont show the canvases
	
	# file object
	_fm = fm.FileManager()
	print "system path:", _fm.path
	
	# h and h3 are not used here
	h, h2, h3 = _fm.readHisto(lines=6) # reads all data; no filtering
	
	# tag filter:
	h2 = aux.tagfilter(h2, "f_zpeak")
	
	#for key in h2.keys():
	#	for histo in h2[key].keys():
	#		print histo
	#return 0
	
	stacks, data, legends = hstack(h2) # distinguish between data and montecarlo
	
	#print "data: ",   data
	#print "stacks: ", stacks
	
	# create a file saver
	tstr = str(dt.datetime.now()).replace(" ", "_").replace(":","-").split(".")[0]
	print "tstr: ", tstr
	# FileSaver(filename, treepath, _fm)
	_fs = fm.FileSaver("fakerate.root",tstr,_fm)
	
	# projections() uses the tags: "pas" and "tot" !!!
	hproj = projections(stacks, data, legends, _fs)
	
	for key in hproj.keys():
		print key
		for thing in hproj[key].keys():
			if "eta" in thing:
				for mod in hproj[key][thing].keys():
					for elem in hproj[key][thing][mod]:
						print elem
						print elem.Integral()
	
	
	#drawer(hproj["mc"]["eta"]["tot"][10], _fs)
	#drawer(hproj["mc"]["eta"]["pas"][10], _fs)
	
	
	# all data here
	dat, mc = calc_histos(hproj)
	# eta not here
	
	
	
	'''
	print "notagged: "
	for key in h.keys():
		for histo in h[key]:
			print histo
	# '''
	
	'''
	print "tagged: "
	for key in h.keys():
		for histo in h[key]:
			print histo
	# '''
	
	#'''
	print "dat:", dat
	drawer(dat["eta"]["tot"], _fs)
	
	
	#for key in dat["pt"].keys():
	#	dat["pt"][key].Rebin(10)
	#	mc["pt"][key].Rebin(10)
	
	
	
	funcs = {	"pt": rt.TF1("pt","[0]+[1]/x+[2]/(x*x)",20,200),
				"nvtx": rt.TF1("nvtx","[0]+[1]*x",0,20),
				"njet": rt.TF1("njet","[0]+[1]*x",1,10),
				"eta": rt.TF1("eta","[0]+[1]*x",0,6),
			}	
	
	xrng = {	"pt": [0, 200], 
				"nvtx": [0, 25], 
				"njet": [0, 10],
				"eta": [0, 60]
			}
	
	#print "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
	#print "\n dat[eta]:"
	#print dat["eta"]
	
	#print dat["eta"]["tot"]
	
	#drawer(dat["eta"]["pas"],_fs)
	
	for var in funcs.keys():
		print "kinematic variable: ", var
		eff(dat[var], funcs[var], _fs, xrng[var])
		eff(mc[var], funcs[var], _fs, xrng[var])
	
	# '''
	
	# '''
	return 0

if __name__ == "__main__":
	main()
