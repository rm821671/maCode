# 
#
# dpg plots

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

import copy

class h2Fakerate(object):
	# A class to calculate the fakerate from two given 2d histograms.
	# 
	def __init__(self, name, tot, pas, *opt):
		# tot and pas are 2d histograms for the total and the passed
		# content to determine the efficiency.
		self.name = name
		
		self.ybins = tot.GetNbinsY()
		self.ymin = tot.GetYaxis().GetXmin()
		self.ymax = tot.GetYaxis().GetXmax()
		self.ytitle = tot.GetYaxis().GetTitle()
		
		self._calc_proj(tot, pas)
		
		if not opt:
			self._calc_histo()
		else:
			self._calc_histo(opt[0], opt[1])
		
		self.f = aux.randomName()
		self.egraph = aux.randomName()
		self.ehis = aux.randomName()
	
	def _calc_proj(self, tot, pas):
		# calculate the projected histograms
		#tot = t.Clone(aux.randomName())
		#pas = p.Clone(aux.randomName())
		
		self.tot_proj = []
		self.pas_proj = []
		
		option = "e" # compute errors
		#print "ybins: ", self.ybins
		for yb in range(self.ybins):
			#print yb
			htemp1 = tot.ProjectionX("tot_"+str(yb), yb, yb, option)#.Clone(aux.randomName())
			rt.SetOwnership( htemp1, False )
			#htemp.SetDirectory(0)
			self.tot_proj.append(htemp1)
			#del htemp
			htemp2 = pas.ProjectionX("pas_"+str(yb), yb, yb, option)#.Clone(aux.randomName())
			rt.SetOwnership( htemp2, False )
			#htemp.SetDirectory(0)
			self.pas_proj.append(htemp2)
			#del htemp
		#print self.name
		#raw_input()
		return 0
	
	def _calc_histo(self, *opt):
		self.tot = rt.TH1F(	self.name+"_tot_"+aux.randomName(),
							";"+self.ytitle+"; Events / bin",
							self.ybins,
							self.ymin,
							self.ymax)
		self.pas = rt.TH1F(	self.name+"_pas_"+aux.randomName(),
							";"+self.ytitle+"; Events / bin",
							self.ybins,
							self.ymin,
							self.ymax)
		
		if not opt:
			rng = [0, self.ybins] # TODO: set this to a different range
		else:
			rng = [opt[0], opt[1]]
		
		for yb in range(self.ybins):
			val = self.tot_proj[yb].Integral(rng[0], rng[1])
			self.tot.SetBinContent(yb, val)
			
			val = self.pas_proj[yb].Integral(rng[0], rng[1])
			self.pas.SetBinContent(yb, val)
		self.tot.Sumw2()
		self.pas.Sumw2()
		
		#del self.tot_proj
		#del self.pas_proj
		return 0
	
	def addFitModel(self, f):
		# a root TF1 to be fitted to the fake rate dependency.
		self.f = f
		return 0
	
	def fit(self, opt="r"):
		if not isinstance(self.f, rt.TF1):
			print "no fit model added, no fit possible"
			return False
		if not isinstance(self.egraph, rt.TGraph):
			self.calc_eff()
		self.egraph_nofit = self.egraph.Clone()
		self.fitres = self.egraph.Fit(self.f, opt)
		
	
	def rebin(self, binEdges):
		self.pas = aux.rebin(self.pas, binEdges)
		self.tot = aux.rebin(self.tot, binEdges)
		return 0
	
	def calc_eff(self):
		
		if rt.TEfficiency.CheckConsistency(self.pas, self.tot):
			print "consistency checked!"
			pass
		else:
			print "not consistent"
			return False
		
		self.eff = rt.TEfficiency(self.pas, self.tot)
		
		self.egraph = self.eff.CreateGraph()
		self.egraph.GetXaxis().SetTitle(self.ytitle)
		
		self.ehis = self.pas.Clone()
		self.ehis.Divide(self.tot)
		self.ehis.SetName(self.name+"_ehis")
		
		return 0

class h2FakerateCanvas(object):
	# A class to plot a h2Fakerate
	def __init__(self, h2f):
		
		self.h2f = h2f
		self.egraph = h2f.egraph
		
		if(h2f.f is None):
			print "fakerate has no fitted function"
		else:
			self.ratio = aux.ratio_fgraph(h2f.egraph_nofit, h2f.f)
			self.ratio.GetXaxis().SetTitle(self.h2f.ytitle)
			self.ratio.SetMinimum(0.5)
			self.ratio.SetMaximum(1.5)
		
	def createCanvas(self, yrng=""):
		cwidth = 800
		cheight = 800
		
		rat = 0.3
		
		c = rt.TCanvas(self.h2f.name, self.h2f.name, cwidth, cheight)
		c.cd()
		
		#c.SetLeftMargin(0.16)
		#c.SetBottomMargin(0.15)
		
		self.egraph.GetXaxis().SetTitleSize(0.05)
		self.egraph.GetXaxis().SetLabelSize(0.0)
		self.egraph.GetYaxis().SetTitleSize(0.05)
		self.egraph.GetYaxis().SetLabelSize(0.03)
		self.egraph.GetYaxis().SetTitleOffset(1.3)
		#h1.SetMinimum(-10)
		
		self.ratio.GetXaxis().SetTitleSize(0.08)
		self.ratio.GetXaxis().SetLabelSize(0.08)
		self.ratio.GetYaxis().SetTitleSize(0.08)
		self.ratio.GetYaxis().SetLabelSize(0.08)
		self.ratio.GetYaxis().SetTitleOffset(0.7)
		
		pad1 = rt.TPad("pad1","pad1",0,rat,1,1)
		
		pad1.SetBottomMargin(0)
		pad1.SetLeftMargin(0.16)
		pad1.Draw()
		pad1.cd()
		pad1.Update()
		
		self.egraph.GetYaxis().SetTitle("Fakerate")
		self.egraph.GetYaxis().SetTitleOffset(1.3)
		self.egraph.GetYaxis().SetTitleSize(0.05)
		
		self.egraph.GetXaxis().SetLabelSize(0.0)
		self.egraph.GetXaxis().SetTitleSize(0.05)
		#.SetMaximum(0.015)
		
		#eff.GetXaxis().SetRangeUser(0.,210.)
		if yrng=="":
			self.egraph.SetMinimum(0.)
		else:
			self.egraph.SetMinimum(yrng[0])
			self.egraph.SetMaximum(yrng[1])
		
		#eff.Draw("ap")
		
		self.egraph.Draw("ap")
		
		c.cd()
		
		pad2 = rt.TPad("pad2","pad2",0,0,1,rat)
		pad2.SetTopMargin(0)
		pad2.SetBottomMargin(0.25)
		pad2.SetLeftMargin(0.16)
		pad2.Draw()
		pad2.cd()
		
		
		oneline = rt.TF1("oneline", "1", 	self.egraph.GetXaxis().GetXmin(),
											self.egraph.GetXaxis().GetXmax())
		oneline.SetLineWidth(1)
		oneline.SetLineColor(rt.kBlack)
		
		self.ratio.Draw("ap")
		oneline.Draw("same")
		pad2.Update()
		
		c.Update()
		
		#drawCMS(c)
		#c.Update()
		
		#raw_input()
		self.c = c.Clone()
		self.c.Update()
		return 0

def drawCMS(c):
	# TODO: doesnt work atm
	
	# Needs a canvas
	# TLatex(x, y, text)
	info = rt.TLatex(9.,9.,"13 TeV")
	info.SetNDC()
	info.SetTextAngle(0)
	info.SetTextFont(42)
	#info.SetTextAlign(31)
	info.SetTextSize(0.04)
	info.SetTextColor(rt.kBlack)
	info.Draw()
	c.Update()
	#raw_input()
	#return c


class closure(object):
	# to do a closure plot with a given fakerate;
	# the fakerate might be a fitted function
	# or a histogram to weight each bin
	
	def __init__(self, name, initial, target):
		# Needs two TH1 histograms:
		# One initial where the fakerate weights are applied on,
		# one target which should be the result from the weighing.
		
		self.initial = initial
		self.target = target
		self.name = name
		
		self.f = []
		self.h = []
		self.names = []
		self.predictionf = [] # prediction from fit
		self.predictionh = [] # prediction from histo (per each bin)
		
		self.ch = {} #
		self.cf = {} #
		
		#self.cf = 1 # canvas for fit prediction
		#self.ch = 1 # canvas for histo prediction
		
		self.leg = 1 # legend
	
	def addFakerate(self, f, name=""):
		# fakerate should be of type h2Fakerate
		self.h.append(f.ehis)
		self.f.append(f.f)
		
		if name=="":
			self.names.append(aux.randomName())
		else:
			self.names.append(name)
		return 0
	
	def rebin(self, binEdges):
		self.initial = aux.rebin(self.initial, binEdges)
		self.target = aux.rebin(self.target, binEdges)
		#if self.h:
		#	for entry in self.h:
		#		entry= aux.rebin(entry, binEdges)
		return 0
	
	def evalFakerate(self, f, x):
		return f.Eval(x)/(1-f.Eval(x))
	
	def createFitPrediction(self, *opt):
		# here, the fitted fakerate function is used
		# to make a prediction for the closure test.
		bins = self.initial.GetNbinsX()
		xmin = self.initial.GetXaxis().GetXmin()
		xmax = self.initial.GetXaxis().GetXmax()
		for h in self.h:
			self.predictionf.append(self.initial.Clone(aux.randomName()))
		for i in range(bins):
			val = self.initial.GetBinContent(i)
			err = self.initial.GetBinError(i)
			x = self.initial.GetBinCenter(i)
			count = 0
			for f in self.f:
				R = self.evalFakerate(f, x)
				self.predictionf[count].SetBinContent(i, R*val)
				self.predictionf[count].SetBinError(i, R*err)
				count = count+1
		return 0
	
	def createHistoPrediction(self, *opt):
		# here, the fakerate histogram is used
		# to make a prediction for the closure test
		bins = self.initial.GetNbinsX()
		xmin = self.initial.GetXaxis().GetXmin()
		xmax = self.initial.GetXaxis().GetXmax()
		for h in self.h:
			self.predictionh.append(self.initial.Clone(aux.randomName()))
		for i in range(bins):
			val = self.initial.GetBinContent(i)
			err = self.initial.GetBinError(i)
			count = 0
			for h in self.h:
				fr = h.GetBinContent(i)
				R = float(fr)/(1-fr)
				self.predictionh[count].SetBinContent(i, R*val)
				self.predictionh[count].SetBinError(i, R*err)
				count = count +1
		return 0
	
	def histoCanvas(self, name, *opt):
		cwidth = 800
		cheight = 800
		rat = 0.3
		
		rt.gStyle.SetLegendBorderSize(0)
		
		if not opt:
			num = 0
			#print "num: ", num
		if opt:
			if opt[0] >= len(self.predictionh):
				print "index not available"
				return 0
			else:
				num = opt[0]
		
		
		if self.leg is None:
			#print "isnone: ", self.leg
			
			offset = -0.1
			self.leg = rt.TLegend(.6+offset,.65+offset,.96+offset,.87+offset)
			self.leg.SetFillColor(rt.kWhite)
			self.leg.SetFillStyle(0)
			self.leg.AddEntry(self.target, "N_{e#rightarrow#gamma}", "f")
			self.leg.AddEntry(self.predictionh[num], self.names[num], "ep")
			
		
		c = rt.TCanvas(name, name, cwidth, cheight)
		c.cd()
		
		self.target.GetXaxis().SetTitleSize(0.05)
		self.target.GetXaxis().SetLabelSize(0.0)
		self.target.GetYaxis().SetTitleSize(0.05)
		self.target.GetYaxis().SetLabelSize(0.03)
		self.target.GetYaxis().SetTitleOffset(1.3)
		self.target.GetYaxis().SetTitle("Events / bin")
		self.target.SetFillColor(rt.kGray+2)
		self.target.SetLineColor(rt.kGray+2)
		
		self.predictionh[num].SetMarkerStyle(rt.kFullCircle)
		self.predictionh[num].SetMarkerColor(rt.kRed)
		
		
		
		self.ratio = aux.ratio_hishis(self.target, self.predictionh[num])
		self.ratio.GetXaxis().SetTitleSize(0.08)
		self.ratio.GetXaxis().SetLabelSize(0.08)
		self.ratio.GetYaxis().SetTitleSize(0.08)
		self.ratio.GetYaxis().SetLabelSize(0.08)
		self.ratio.GetYaxis().SetTitleOffset(0.7)
		
		pad1 = rt.TPad("pad1","pad1",0,rat,1,1)
		
		pad1.SetBottomMargin(0)
		pad1.SetLeftMargin(0.16)
		pad1.Draw()
		pad1.cd()
		pad1.Update()
		
		#self.egraph.GetYaxis().SetTitle("Fakerate")
		#self.egraph.GetYaxis().SetTitleOffset(1.3)
		#self.egraph.GetYaxis().SetTitleSize(0.05)
		
		#self.egraph.GetXaxis().SetLabelSize(0.0)
		#self.egraph.GetXaxis().SetTitleSize(0.05)
		#.SetMaximum(0.015)
		
		#eff.GetXaxis().SetRangeUser(0.,210.)
		#self.egraph.SetMinimum(0.)
		
		#eff.Draw("ap")
		#print self.leg
		
		self.target.Draw("E2")
		self.predictionh[num].Draw("ep same")
		
		
		#if self.leg is not None:
		#	self.leg.Draw()
		
		c.cd()
		
		pad2 = rt.TPad("pad2","pad2",0,0,1,rat)
		pad2.SetTopMargin(0)
		pad2.SetBottomMargin(0.25)
		pad2.SetLeftMargin(0.16)
		pad2.Draw()
		pad2.cd()
		
		oneline = rt.TF1("oneline", "1", 	self.target.GetXaxis().GetXmin(),
											self.target.GetXaxis().GetXmax())
		oneline.SetLineWidth(1)
		oneline.SetLineColor(rt.kBlack)
		
		self.ratio.Draw("ep")
		oneline.Draw("same")
		pad2.Update()
		
		c.Update()
		
		#drawCMS(c)
		#c.Update()
		
		#raw_input()
		self.ch[self.names[num]] = c.Clone(self.names[num])
		#self.ch[self.names[num]].SetDirectory(0)
		self.ch[self.names[num]].Update()
		
		return 0
	
	def histoCanvasTest(self, name, *opt):
		cwidth = 800
		cheight = 800
		rat = 0.3
		
		if not opt:
			num = 0
		if opt:
			if opt[0] >= len(self.predictionh):
				print "index not available"
				return 0
		
		c = rt.TCanvas(name, name, cwidth, cheight)
		c.cd()
		
		self.target.GetXaxis().SetTitleSize(0.05)
		self.target.GetXaxis().SetLabelSize(0.0)
		self.target.GetYaxis().SetTitleSize(0.05)
		self.target.GetYaxis().SetLabelSize(0.03)
		self.target.GetYaxis().SetTitleOffset(1.3)
		self.target.GetYaxis().SetTitle("Events / bin")
		self.target.SetFillColor(rt.kGray+2)
		self.target.SetLineColor(rt.kGray+2)
		
		self.predictionh[num].SetMarkerStyle(rt.kFullCircle)
		self.predictionh[num].SetMarkerColor(rt.kRed)
		
		self.ratio = aux.ratio_hishis(self.target, self.predictionh[num])
		self.ratio.GetXaxis().SetTitleSize(0.08)
		self.ratio.GetXaxis().SetLabelSize(0.08)
		self.ratio.GetYaxis().SetTitleSize(0.08)
		self.ratio.GetYaxis().SetLabelSize(0.08)
		self.ratio.GetYaxis().SetTitleOffset(0.7)
		
		pad1 = rt.TPad("pad1","pad1",0,rat,1,1)
		
		pad1.SetBottomMargin(0)
		pad1.SetLeftMargin(0.16)
		pad1.Draw()
		pad1.cd()
		pad1.Update()
		
		#self.egraph.GetYaxis().SetTitle("Fakerate")
		#self.egraph.GetYaxis().SetTitleOffset(1.3)
		#self.egraph.GetYaxis().SetTitleSize(0.05)
		
		#self.egraph.GetXaxis().SetLabelSize(0.0)
		#self.egraph.GetXaxis().SetTitleSize(0.05)
		#.SetMaximum(0.015)
		
		#eff.GetXaxis().SetRangeUser(0.,210.)
		#self.egraph.SetMinimum(0.)
		
		#eff.Draw("ap")
		
		
		self.predictionh[num].Draw("ep")
		self.target.Draw("E2 same")
		
		c.cd()
		
		pad2 = rt.TPad("pad2","pad2",0,0,1,rat)
		pad2.SetTopMargin(0)
		pad2.SetBottomMargin(0.25)
		pad2.SetLeftMargin(0.16)
		pad2.Draw()
		pad2.cd()
		
		
		oneline = rt.TF1("oneline", "1", 	self.target.GetXaxis().GetXmin(),
											self.target.GetXaxis().GetXmax())
		oneline.SetLineWidth(1)
		oneline.SetLineColor(rt.kBlack)
		
		self.ratio.Draw("ep")
		oneline.Draw("same")
		pad2.Update()
		
		c.Update()
		
		#drawCMS(c)
		#c.Update()
		
		raw_input()
		self.c = c.Clone()
		self.c.Update()
		
		return 0
	
	
	def fitCanvas(self, *opt):
		
		
		return 0
		
	def canvas(self, upperpad, lowerpad):
		pass











