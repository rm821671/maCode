# helper functions for specific calculations
# needed in the analyses


# ***********************************************************************************************
# plotter for a short graph plot
def drawer(h, fs, *opt):
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

