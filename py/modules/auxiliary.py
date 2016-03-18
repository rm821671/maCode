# 
import sys
sys.path.insert(0,"../../py")

import ROOT as rt

import numpy as np
import datetime
import calendar
import os
from array import array

import json

import copy

from modules import filemanager as fm


def checkRebinningConsistence( axis, newBinning ):
	# gives a warning if the bin widths are not consistent with the bin range
	# i.e. range 100-200, width 30
	oldBinning = []
	for i in range(axis.GetNbins()+1):
		oldBinning.append( axis.GetBinUpEdge(i) )
	for i in newBinning:
		if i not in oldBinning: print "New bin edge is not compatible with old binning", i

def rebin( h, binEdges ):
	# usage: h = rebin( hold, range(340,2001,20) )
	checkRebinningConsistence(h.GetXaxis(), binEdges)
	binEdgesArr = array( 'd', binEdges )
	hname = h.GetName() + "_rb"
	hnew = h.Rebin( len(binEdges)-1, hname, binEdgesArr )
	return hnew

def tagfilter(obj, tag):
	# Needs a dictionary of TObjects from the form:
	# {	"dataset1": {"name1:", obj1, "name2": obj2, ...}, 
	#	"dataset2": { ... etc }
	#	}
	# and filter them in their name for the given tag.
	# 
	# All objects without this tag are taken out,
	# only objects with this tag are returned.
	htemp = obj.copy() # TODO: maybe not necessary
	
	for key in obj.keys():
		for o in obj[key].keys():
			if tag in o:
				continue
			htemp[key].pop(o, None)
	return htemp

def ratio_hishis(h1, h2):
	# returns h1/h2
	#
	h = h1.Clone()
	h.Divide(h2)
	yax = h.GetYaxis()
	if yax:
		yax.SetNdivisions(5)
	mx = h.GetBinContent(h.GetMaximumBin())
	mn = h.GetBinContent(h.GetMinimumBin())
	rg = mx - mn
	h.SetMaximum(mx+rg*.1)
	h.SetMinimum(mn-rg*.1)
	h.SetLineColor(rt.kBlack)
	h.SetMarkerStyle(rt.kFullCircle)
	h.GetYaxis().SetTitle("ratio")
	return h

def ratio_fhis(h1, f1):
	# returns a histogram where in each bin
	# h1/f1(bin)
	#
	h = h1.Clone()
	h.Sumw2()
	binmin = h.GetMinimumBin()
	binmax = h.GetMaximumBin()
	for i in range(binmin, binmax+1):
		val = h.GetBinContent(i)
		x = h.GetXaxis().GetBinCenter(i)
		fx = f.Eval(x)
		h.SetBinContent(i, val/fx)
	h.SetMarkerStyle(rt.kOpenCircle)
	h.GetYaxis().SetTitle("data/fit")
	return h

def ratio_fgraph(g1, f1):
	# TODO
	# r->GetConfidenceIntervals(1, 1, 1, x, err, 0.683, false);
	g = g1.Clone()
	#g.GetFunction(f1.GetName()).SetBit(rt.kNotDraw)
	gdim = g1.GetN()
	x, y = rt.Double(0), rt.Double(0)
	ex, ey = 0., 0.
	# SetPointError (Int_t i, Double_t ex, Double_t ey)
	# GetErrorX 	( 	Int_t  	i	) 	
	#xset = rt.Double(0)
	for i in range(0, gdim):
		#print "point", g1.GetPoint(i, x, y), x, y
		g1.GetPoint(i, x, y)
		ytemp = f1.Eval(x)
		exh = g1.GetErrorXhigh(i)
		exl = g1.GetErrorXlow(i)
		eyh = g1.GetErrorYhigh(i)
		eyl = g1.GetErrorYlow(i)
		g.SetPoint(i, x, y/ytemp)
		g.SetPointError(i, exl, exh, eyl, eyh)
	#print xarr
	#print yarr
	g.SetMarkerStyle(rt.kOpenCircle)
	g.GetYaxis().SetTitle("data/fit")
	return g


def read_dict(filename):
	# reads a textfile with a dictionary
	with open(filename, "r") as f:
		s = f.read()
	json_str = s.replace("'", "\"")
	d = json.loads(json_str)
	return d

def write_dict(filename, d):
	# writes a dictionary to a textfile
	json_str = json.dumps(d)
	with open(filename, "w") as f:
		f.write(json_str)
	return 0



def calc_f_hist(he, hg, c=1):
	# given he and hg, hf is calculated by
	# hf = hg/(c*he+hg)
	# Use c=1 or c=2.
	h = he.Clone()
	h2 = hg.Clone()
	if c==2:
		h2.Add(hg)
	h2.Add(he)
	h.Divide(h2)
	return h





def randomName():
	"""
	Generate a random string. This function is useful to give ROOT objects
	different names to avoid overwriting.
	"""
	from random import randint
	from sys import maxint
	return "%x"%(randint(0, maxint))













def give_hCutFlow(path):
	'''
	gives back the first bin of hCutFlow
	'''
	data = rt.TFile(path)
	hcutflow = data.Get("TreeWriter/hCutFlow")
	return hcutflow.GetBinContent(2)



def giveDataPath(key):
	'''
	
	'''
	_f = fm.FileManager()
	datasets = [	"DYJetsToLL_M-50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8_v01.root",
					"TTGJets_TuneCUETP8M1_13TeV-amcatnloFXFX-madspin-pythia8_v01.root",
					"WGToLNuG_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8_v01.root",
					"ZGTo2LG_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8_v01.root",
					"DoubleEG_Run2015D-05Oct2015-v1.root",
					"DoubleEG_Run2015D-PromptReco-v4.root"
				]
	
	
	path = _f.filepath
	for k in datasets:
		if key in k:
			return path+k
	return False





def giveMClumiScalefactor(key):
	'''
	returns the lumi scale factor for a monte carlo dataset
	'''
	datapath = giveDataPath(key)
	N = give_hCutFlow(datapath)
	sigma = giveCrossSectionMC(key)
	L = giveLumiData(key)
	return float(L)/float(N)*float(sigma)
	


def giveCrossSectionMC(key):
	'''
	returns cross section in pb
	'''
	cs = {		"DYJetsToLL": 6025.2, # NLO
				# "DYJetsToLL": 6104., # LO
				"TTGJets": 3.697, # NLO
				"WGToLNuG": 489., # NLO
				"ZGTo2LG": 117.864 # NLO
			}
	for k in cs.keys():
		if key in k:
			return cs[k]
		
	return False


def giveLumiData(key):
	'''
	returns lumi in pb-1
	'''
	lumi = {	"Run2015-05Oct2015": 2.11e3,		# originally in fb-1
				"Run2015-PromptReco": 2.11e3		# originally in fb-1
			}
	#or k in lumi.keys():
	#	if key in k:
	#		return lumi[k]
	val = 2.11e3
	return val
