# helper functions for specific calculations
# needed in the analyses

import sys
sys.path.insert(0,"../../py")

import ROOT as rt
from modules import lib_plot as lp
from modules import auxiliary as aux
from modules import filemanager as fm

import time
import datetime as dt

Gvariables = {	"pt": 	None,
				"nvtx": None,
				"njet":	None,
				"eta":	None
					}

# ***********************************************************************************************
# plotter for a short graph plot
def drawer(h, fs, *opt):
	print "analysis.drawer()"
	# histogram and filesaver object
	
	name = h.GetName()
	drawmode = ""
	
	csize = 800
	
	if not opt:
		c = rt.TCanvas("c_"+name,"c_"+name,csize,csize)
	else:
		c = rt.TCanvas("c_"+str(opt[0])+"_"+name,"c_"+str(opt[0])+"_"+name,csize,csize)
	
	c.SetRightMargin(0.15)
	c.cd()	
	
	if isinstance(h, rt.TH1):
		#print "TH1#"
		ht = h.Clone()
		drawmode = "hist"
		ht.Draw(drawmode)
	
	if isinstance(h, rt.TH2):
		#print "TH2#"
		ht = h.Clone()
		drawmode = "colz"
		ht.Draw(drawmode)
		
		
	
	if isinstance(h, rt.THStack):
		#print "is stack!"
		#ht = h.GetLast()
		h.Draw()
	
	c.Update()
	
	fs.save(c, name)

# ***********************************************************************************************
# 
def calc_histos(pars):
	print "analysis.calc_histos()"
	'''
	Needs a dictionary of projected histograms;
	from each histogram the integral is calculated in the range rng
	'''
	rng = pars.zrng # integral range
	
	hproj = pars.hproj
	
	#variables = ["pt","nvtx","njet"]
	variables = []
	for key in pars.variables.keys():
		variables.append(key)
	
	bins = dict.fromkeys(pars.variables.keys())
	for key in bins.keys():
		bins[key] = pars.variables[key]["bins"]
	
	#bins["pt"] = [0, 250]
	#bins["nvtx"] = [0, 60]
	#bins["njet"] = [0, 15]
	#bins["eta"] = [0, 600]
	
	data = dict.fromkeys(Gvariables.keys())
	for key in data.keys():
		data[key] = {	"tot": 0, 	"pas": 0}
	
	mc = dict.fromkeys(Gvariables.keys())
	for key in mc.keys():
		mc[key] = {	"tot": 0, "pas": 0}
	
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

# ***********************************************************************************************
#
def projections(pars):
	print "analysis.projections()"
	import copy
	
	stacks = pars.stacks
	data = pars.data
	legends = pars.legends
	fs = pars.fs
		
	hvariables = dict.fromkeys(pars.variables.keys())
	for key in hvariables.keys():
		hvariables[key] = {	"tot": [], "pas": []}
	
	ranges = dict.fromkeys(pars.variables.keys())
	for key in ranges.keys():
		ranges[key] = pars.variables[key]["range"]
	
	# initialize histogram dictionary
	hproj = {	"data":	copy.deepcopy(hvariables), 
				"mc":	copy.deepcopy(hvariables)
			}
	
	#off0 = 0
	#off1 = 0
	
	# range in bins! not x value!
	#ranges = {	"pt": range(off0, 250+off1),
	#			"nvtx": range(off0,60+off1),
	#			"njet": range(off0,15+off1),
	#			"eta": range(off0, 600+off1)
	#		}
	
	variables = []
	for key in pars.variables.keys():
		variables.append(key)
	
	#print "variables: ", variables
	
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
	
	option = "e"
	
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
											variable+"_"+tag+"_"+str(b), b, b, option)
							htemp.SetDirectory(0)
							h[isdata][variable][tag].append(htemp)
					tag = "pas"
					if tag in key:
						for b in ranges[variable]:
							htemp = data[key].ProjectionX(
											variable+"_"+tag+"_"+str(b), b, b, option)
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
											variable+"_"+tag+"_"+str(b), b, b, option)
							htemp.SetDirectory(0)
							h[isdata][variable][tag].append(htemp)
					tag = "pas"
					if tag in key:
						for b in ranges[variable]:
							htemp = mc[key].ProjectionX(
											variable+"_"+tag+"_"+str(b), b, b, option)
							htemp.SetDirectory(0)
							h[isdata][variable][tag].append(htemp)
		return h
	
	hproj = projdata(hproj)
	hproj = projmc(hproj)
	
	return hproj

# ***********************************************************************************************
#
def hstack(h):
	print "analysis.hstack()"
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

# ***********************************************************************************************
# some short efficiency plots
# with different settings like rebinning, etc...
def eff_stuff(h, fs, *xrng):
	print "analysis.eff_stuff()"
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

# ***********************************************************************************************
# create efficiency plots
def eff(h,var,pars):
	print "analysis.eff()"
	# 
	# (h, var, pars)
	print "fit function: ",pars.variables[var]["fit"]
	
	#return 0
	f = pars.variables[var]["fit"]
	fs = pars.fs
	
	xrng = pars.variables[var]["effrng"]
	#print "xrng: ", xrng
	
	hp = h[var]["pas"].Clone()
	ht = h[var]["tot"].Clone()
	
	print "total: ", ht
	print "pass: ", hp
	
	
	#if rt.TEfficiency.CheckConsistency(h["pas"], h["tot"]):
	if rt.TEfficiency.CheckConsistency(hp, ht):
		print "consistency checked!"
		pass
	else:
		print "not consistent"
		return 0
	
	#eff = rt.TEfficiency(h["pas"], h["tot"])
	eff = rt.TEfficiency(hp, ht)
	print "eff: ", eff
	
	#csize = 800
	#name = h["pas"].GetName() + "_" + h["tot"].GetName()
	#name = hp.GetName() + "_" + ht.GetName()
	#c = rt.TCanvas("c_"+name,"c_"+name,csize,csize)
	
	egraph = eff.CreateGraph()
	#egraph.Draw()
	#raw_input()
	#return 0
	
	
	
	#if "eta" not in hp.GetName():
	egraph.Fit(f, "r")
	
	fmin = f.GetMinimumX()
	fmax = f.GetMaximumX()
	'''
	#pmin = (f.Eval(fmin-fmin/2.))
	#pmax = (f.Eval(fmax+fmax/2.))
	pmin = 0.005
	pmax = 0.05
	
	#print "xrng: ", xrng[0][0], xrng[0][1]
	if xrng:
		egraph.GetXaxis().SetRangeUser(xrng[0],xrng[0]);
	
	if pmin < pmax:
		egraph.SetMinimum(pmin)
		egraph.SetMaximum(pmax)
	if pmin >= pmax:
		egraph.SetMinimum(pmax)
		egraph.SetMaximum(pmin)
	# '''
	
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
	
	#raw_input("press any key...")
	
	fs.save(c, name)

# ***********************************************************************************************
# simple efficiency function
def eff_simple(tot, pas, xtitle):
	print "analysis.eff()"
		
	hp = pas.Clone()
	ht = tot.Clone()
	
	print "total: ", ht
	print "pass: ", hp
	
	
	#if rt.TEfficiency.CheckConsistency(h["pas"], h["tot"]):
	if rt.TEfficiency.CheckConsistency(hp, ht):
		print "consistency checked!"
		pass
	else:
		print "not consistent"
		return 0
	
	#eff = rt.TEfficiency(h["pas"], h["tot"])
	eff = rt.TEfficiency(hp, ht)
	print "eff: ", eff
	
	
	egraph = eff.CreateGraph()
	egraph.GetXaxis().SetTitle(xtitle)
	
	return egraph
	'''
	csize = 800
	#name = h["pas"].GetName() + "_" + h["tot"].GetName()
	name = hp.GetName() + "_" + ht.GetName()
	egraph.SetTitle(name)
	c = rt.TCanvas("c_"+name,"c_"+name,csize,csize)
	c.cd()
	egraph.Draw("ap")
	#if "eta" not in hp.GetName():
	#f.Draw("same")
	c.Update()
	#raw_input("press any key...")
	fs.save(c, name)
	# '''



# ***********************************************************************************************
# create transfer factor plots
def transferfactor(h,var,pars):
	print "analysis.transferfactor()"
	# 
	# (h, var, pars)
	print "fit function: ",pars.variables[var]["fit"]
	
	#return 0
	f = pars.variables[var]["fit"]
	fs = pars.fs
	
	xrng = pars.variables[var]["effrng"]
	#print "xrng: ", xrng
	
	hp = h[var]["pas"].Clone()
	ht = h[var]["tot"].Clone()
	
	print "total: ", ht
	print "pass: ", hp
	
	ht.Sumw2()
	hp.Sumw2()
	
	# divide the histograms
	hp.Divide(ht)
	
	
	#hp.Divide(ht, 1)
	
	egraph = hp.Clone()
	egraph.SetName(hp.GetName()+"_"+ht.GetName())
	print "egraph: ", egraph
	#return 0
	
	#if "eta" not in hp.GetName():
	egraph.Fit(f, "r")
	
	fmin = f.GetMinimumX()
	fmax = f.GetMaximumX()
	
	#pmin = (f.Eval(fmin-fmin/2.))
	#max = (f.Eval(fmax+fmax/2.))
	pmin = 0.005
	pmax = 0.05
	
	#print "xrng: ", xrng[0][0], xrng[0][1]
	if xrng:
		egraph.GetXaxis().SetRangeUser(xrng[0],xrng[0]);
	
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
	
	
	
	egraph.Draw("hist")
	#if "eta" not in hp.GetName():
	f.Draw("same")
	
	c.Update()
	
	#raw_input("press any key...")
	
	fs.save(c, name)














