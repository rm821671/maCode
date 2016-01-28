
from ROOT import *
from lib_plot import *
from math import *
from array import array



# global objects
csize = 800


def bw(x, pars):
	# breit wigner
	return pars[0]*1./(pow((pars[1]-x[0]),2) + pow((x[0]*pars[2]),2))
	

def fit_tool(h, f, c):
	h.Scale(1./float(h.GetEntries()))
	h.Fit(f)	
	c.cd()
	f.Draw("same")
	
	return True


def fake_dep(fr, var):
	pass




# //////////////////////////////////////////////////////////////////////////////////////////////////
# read data
def getData(datatag, lines=6, lineoff = 0):
	'''
	parameter:
		lines, gives the number of files written per run
		lineoff, means the offset; by default zero, so it starts with the "latest" lines
	'''
	rebin = 1
	
	datakeys = {	"DYJetsToLL": 0,
					"TTGJets": 2,
					"WGToLNuG": 4,
					"ZGTo2LG": 6,
					"Run2015D-05Oct2015": 0,
					"Run2015D-PromptReco": 1
				}
	defcolor = {	0: kGreen}
	
	# get number of lines from file:
	total_lines = inputFileNumberOfLines()
	h = {} #histograms
	stacks = {} # thstacks
	c = {} # canvases
	
	# read the files:
	for ll in range(total_lines-lineoff-lines+1,total_lines-lineoff+1):
		# filename
		strInputFile = inputFile(ll)
		for key in datakeys:
			if(key in strInputFile):
				hname = key
		#print hname
		h[hname] = {}
		try:
			data = TFile(strInputFile)
		except:
			return False		
		# loop all histograms and search for raw ones
		for key in data.GetListOfKeys():
			kgn = key.GetName()
			if datatag in kgn:
				#print type(data.Get(kgn))
				h[hname][kgn] = data.Get(kgn)
				h[hname][kgn].SetDirectory(0)
			
		
		stacks = dict.fromkeys(h[hname].keys()) # THStack for mc
	
	l = dict.fromkeys(stacks.keys()) # legends
	dh = dict.fromkeys(stacks.keys()) # data histogram(s)
	added_data = dict.fromkeys(stacks.keys())
		
	#initialize stacks with THStacks:
	for key in stacks.keys():
		stacks[key] = THStack()
		l[key] = TLegend(0.9,0.7,0.65,0.9)
		dh[key] = {}
		
	
	
	cstyle = ""
	
	# scale montecarlo samples and add to stack
	for key in h.keys():
		if "Run2015" not in key:
			sf = giveMClumiScalefactor(key)
			for his in h[key].keys():
				#print key, his
				h[key][his].Scale(sf)
				h[key][his].Rebin(rebin)
				h[key][his].SetFillColor(defcolor[0]+datakeys[key])
				h[key][his].SetLineColor(defcolor[0]+datakeys[key])
				l[his].AddEntry(h[key][his], key)
				stacks[his].Add(h[key][his])
		
		if "Run2015D-05Oct2015" in key:
			for his in h[key].keys():
				h[key][his].Rebin(rebin)
				dh[his][key] = h[key][his]
				#dh[his][key].SetFillColor(kBlack)
				dh[his][key].SetMarkerStyle(20) # full square
				dh[his][key].SetMarkerColor(kBlack)
				l[his].AddEntry(h[key][his], key)
				#print "key, his: ", key, his
				#print "his, key: ", his, key
		
		
		if "Run2015D-PromptReco" in key:
			for his in h[key].keys():
				h[key][his].Rebin(rebin)
				dh[his][key] = h[key][his]
				
	
	# add reco and prompt reco
	for key in dh.keys():
		dh[key]["Run2015D-05Oct2015"].Add(dh[key]["Run2015D-PromptReco"])
	
	
	return h, stacks, dh, l




# //////////////////////////////////////////////////////////////////////////////////////////////////

def getData2d(datatag, lines=6, lineoff = 0):
	'''
	reads out 2d histograms
	'''
	
	
	datakeys = {	"DYJetsToLL": 0,
					"TTGJets": 2,
					"WGToLNuG": 4,
					"ZGTo2LG": 6,
					"Run2015D-05Oct2015": 0,
					"Run2015D-PromptReco": 1
				}
	defcolor = {	0: kGreen}
	
	# get number of lines from file:
	total_lines = inputFileNumberOfLines()
	h = {} #histograms
	stacks = {} # thstacks
	c = {} # canvases
	
		# read the files:
	for ll in range(total_lines-lineoff-lines+1,total_lines-lineoff+1):
		# filename
		strInputFile = inputFile(ll)
		print "dataset: ", strInputFile
		for key in datakeys:
			if(key in strInputFile):
				hname = key
		#print hname
		h[hname] = {}
		try:
			data = TFile(strInputFile)
		except:
			return False		
		# loop all histograms and search for raw ones
		for key in data.GetListOfKeys():
			kgn = key.GetName()
			if datatag in kgn:
				#print type(data.Get(kgn))
				h[hname][kgn] = data.Get(kgn)
				h[hname][kgn].SetDirectory(0)
			
		
		stacks = dict.fromkeys(h[hname].keys()) # THStack for mc
	
	l = dict.fromkeys(stacks.keys()) # legends
	dh = dict.fromkeys(stacks.keys()) # data histogram(s)
	
	
		#initialize stacks with THStacks:
	for key in stacks.keys():
		stacks[key] = THStack()
		l[key] = TLegend(0.9,0.7,0.65,0.9)
		dh[key] = {}
		
	
	
	cstyle = ""
	
	# scale montecarlo samples and add to stack
	for key in h.keys():
		if "Run2015" not in key:
			sf = giveMClumiScalefactor(key)
			for his in h[key].keys():
				#print key, his
				h[key][his].Scale(sf)
				#h[key][his].Rebin(rebin)
				h[key][his].SetFillColor(defcolor[0]+datakeys[key])
				h[key][his].SetLineColor(defcolor[0]+datakeys[key])
				l[his].AddEntry(h[key][his], key)
				stacks[his].Add(h[key][his])
		
		if "Run2015D-05Oct2015" in key:
			for his in h[key].keys():
				#h[key][his].Rebin(rebin)
				dh[his][key] = h[key][his]
				#dh[his][key].SetFillColor(kBlack)
				dh[his][key].SetMarkerStyle(20) # full square
				dh[his][key].SetMarkerColor(kBlack)
				l[his].AddEntry(h[key][his], key)
				#print "key, his: ", key, his
				#print "his, key: ", his, key
		
		
		if "Run2015D-PromptReco" in key:
			for his in h[key].keys():
				#h[key][his].Rebin(rebin)
				dh[his][key] = h[key][his]
				
	
	for key in dh.keys():
		dh[key]["Run2015D-05Oct2015"].Add(dh[key]["Run2015D-PromptReco"])
	
	
	return h, stacks, dh, l





# //////////////////////////////////////////////////////////////////////////////////////////////////
def fakerate(datatag, lines=6, lineoff = 0):
	
	h, stacks, dh, l = getData2d(datatag, lines, lineoff)
	
	cwidth = 800
	cheight = 800
	
	# integrate this range (around the z peak)
	rmin = 80
	rmax = 100
	
	datastr = "Run2015D-05Oct2015"
	#datastr = "DYJetsToLL"
	
	hdata = {}
	hproj = {} # projection histograms
	ct = {} # canvases
	
	htemp = 0
	hw = 0
	
	print h.keys()
	
	#print h["Run2015D-05Oct2015"]
	
	for key in h[datastr].keys():
		if "etag" not in key:
			hdata[key] = h[datastr][key]
			
	
	# ProjectionX (const char *name="_px", Int_t firstybin=0, Int_t lastybin=-1, Option_t *option="")
	for key in hdata.keys():
		#print key
		ymax = hdata[key].GetNbinsY()
		hproj[key] = TH1F(key,key+";;",ymax,0,ymax)
		for i in range(0,ymax):
			j = i+1
			htemp = hdata[key].ProjectionX(key+str(j),j,j)
			hw = htemp.Integral(rmin, rmax)
			for filler in range(1,int(hw+1)):
				hproj[key].Fill(i)
		
		#ct[key] = TCanvas(key, key, cwidth, cheight)
		#hproj[key].Draw("hist")
		
		#print hdata[key].ProjectionX(key,3,3)
	
	# rebin the pt histogram
	for key in hproj.keys():
		if "pt" in key:
			hproj[key].Rebin(5)
	
	
	
	#  efficiencies
	heff = {	"pt": 0,
				"nvtx": 0,
				"njet": 0
			}
	
	funcs = {	"pt": TF1("pt","[0]+[1]/x+[2]/x",20,200),
				"nvtx": TF1("nvtx","[0]+[1]*x",0,25),
				"njet": TF1("njet","[0]+[1]*x",2,7)
			}
	
	
	for key in heff.keys():
		#print "heff.key: ", key
		heff[key] = Eff(key)
		for pkey in hproj.keys():
			#print "pkey: ", pkey
			if key in pkey:
				#print "hproj.key: ",pkey
				if "tot" in pkey:
					heff[key].addTotal(hproj[pkey])
				if "pas" in pkey:
					heff[key].addPassed(hproj[pkey])
				else:
					print "nope"
		
		#if key == "pt":
		#	heff[key].
		
		heff[key].fit(funcs[key])
		heff[key].Canvas()
		#heff[key] = Eff(key)
		#heff[key].Canvas()
		#ct[key] = TCanvas(key, key, cwidth, cheight)
		#ct[key].cd()
		#heff[key].Graph().Draw("ap")
		#ct[key].cd()
	
	
	
	
	
	raw_input()
	
	
	




# //////////////////////////////////////////////////////////////////////////////////////////////////
def rawplot(datatag,lines=6):
	
	h, stacks, dh, l = getData(datatag,lines)
	
	myc = {}
	
	for key in stacks:
		print "key: ", key
		#print "key in stacks: ", key
		myc[key] = myCanvas(key)
		myc[key].addData(dh[key]["Run2015D-05Oct2015"])
		myc[key].addMC(stacks[key])
		#myc[key].xmin = 70
		#myc[key].xmax = 110
		myc[key].createCanvas(l[key])
		
		#c[key] = TCanvas(key,key,csize,csize)
		#dh[key]["Run2015D-05Oct2015"].Draw("ep")
		#stacks[key].Draw("same hist")
		#dh[key]["Run2015D-05Oct2015"].Draw("same ep")
		#l[key].Draw()
	
	print myc.keys()
	
	#myList = TList()
	#for key in c.keys():
	#	myList.Add(myc[key].getCanvas())
	
	#strMyFile = GiveOutputString(strInputFile)
	#print 'output: ',strMyFile
	
	#myFile = TFile(strMyFile,"RECREATE")
	#gFile.WriteObject(myList,"list")	
	print "hallo"
	
	raw_input()





# //////////////////////////////////////////////////////////////////////////////////////////////////
def main():
	_start = datetime.datetime.now()
	
	# write timestamp file
	if(FilelistDatetime()):
		print("file_dates.txt successfully written")
	else:
		print("file_dates.txt failed")
	
	#gROOT.SetBatch(kTRUE)	# dont show the canvases
	
	c = {}		# canvases
	#csize = 700
	
	hnumber = 0
	hnames = {}	# names of the histograms
	
	hnameplot = {}	# 
	h = {}		# histograms 
	h2 = {}		# 2d histograms
	h2p = {}	# projected histograms
	
	#temph2 = 0
	
	#print sys.argv
	
	
	#rawplot("rawdata",6)
	
	#rawplot("diphotonraw",6)
	
	b = 0
	
	#rawplot("diphotonZpeak",6)
	
	fakerate("fZmass_h2_", 6)
	
	
	
	
	"""
	
	strInputFile = inputFile() # get latest created file
	
	print "input: ", strInputFile
	
	try:
		data = TFile(strInputFile)
		print "file successfully loaded"
	except:
		print "file load failed"
		sys.exit(0)
	
	for key in data.GetListOfKeys(): # loop all keys
		hnames[hnumber] = key.GetName()
		#print hnames[hnumber]
		c[hnumber] = TCanvas("c"+str(hnumber),hnames[hnumber],csize,csize)
		
		if "rawdata" in hnames[hnumber]:
			
		
		
		if "h2" in hnames[hnumber]:			
			print hnames[hnumber], hnumber
			h2[temph2] = data.Get(key.GetName())
			h2[temph2].Draw("colz")
			temph2 += 1
		
		hnumber += 1
	
	
	
	for k in range(0,temph2):
		
		
	
	
	myList = TList()
	for key in c.keys():
		myList.Add(c[key])
	
	strMyFile = GiveOutputString(strInputFile)
	print 'output: ',strMyFile
	
	myFile = TFile(strMyFile,"RECREATE")
	gFile.WriteObject(myList,"list")	
	
	end_ = datetime.datetime.now()
	print "runtime: ", end_ - _start
	
	#"""



	# sdf


if __name__ == "__main__":
	main()
	print "__main__ ... done!"
