# 
import sys
sys.path.insert(0,"../../py")

import ROOT as rt

import numpy as np
import datetime
import calendar
import os
from array import array

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
	
	
	path = _f.path + "mergedntuples/";#
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
