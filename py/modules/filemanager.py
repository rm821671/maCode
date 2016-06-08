# 


# file and data management methods
import sys
sys.path.insert(0,"../../py")


import ROOT as rt
import numpy as np
import datetime
import calendar
import os
from array import array




class FileSaver():
    '''
    
    '''
    def __init__(self, filename, treepath, _fm):
        # 
        # fm has to be a FileManager object 
        
        
        self.path = _fm.GivePlotfolder()
        print "plot folder: ",self.path
        #print "self.path: " , self.path
        self.filename = self.path+filename # 
        self.treepath = treepath + '/' # e.g. 'HtBins'
        
        self.dropbox = _fm.dropbox
        
        # self.simulation = simulation
        # self.folder = 
        
    
    def save(self, obj, name):
        #print "name: ",name
        #name = self.treepath+name
        
        if isinstance(obj, rt.TCanvas):
            obj.Update()
            name = obj.GetName()
        
        #print "save.name: ", name
        #print "save.filename: ", self.filename
        #print "save.treepath: ", self.treepath
                
        f = rt.TFile(self.filename, "update")
        f.cd()
        #print "file created"
        
        #print "cd to treepath: ",f.cd(self.treepath)
        
        if not f.cd(self.treepath):
            print "dir in tree created ..."
            f.mkdir(self.treepath)
            f.cd(self.treepath)
        #print "done"
        
        obj.Write(name, rt.TObject.kOverwrite)
        f.Close()
        return 0
    
    def savePdf(self, obj, name):
        
        if isinstance(obj, rt.TCanvas):
            obj.Update()
            name = name + ".pdf"
        
        #f = rt.TFile(self.filename, "recreate")
        #f.cd()
        #obj.Write(name, rt.TObject.kOverwrite)
        #f.Close()
        obj.SaveAs(self.path+name)
        return 0

'''
class FileReader():
    #   Meathods to read the data.
    
    def __init__(self, fm):
        # fm has to be a filemanager object
        
        self.fm = fm
        
    
        
        
    
    
# '''

class FileManager():
    '''
    This class sets the paths for file management
    and provides some methods for the handling.
    '''
    def __init__(self, processingfolder = "code/Processing/"):
        
        
        
        self.processingfolder = processingfolder
        
        self.dropbox, self.filepath = self.SysInfo()
        
    
    def SysInfo(self):
        # find out operating system:
        # dis(tribution), ver(sion), vertitle(title of the version)
        import platform
        dist, ver, title = platform.linux_distribution()
        if "Ubuntu" in dist:
            dropbox = "/home/jack/Dropbox/data_uni/"
            filepath = "/home/jack/data_uni/mergedntuples"
        elif "Scientific" in dist:
            dropbox = "/user/rmeyer/Dropbox/data_uni/"
            filepath = "/user/rmeyer/mergedNTuples/"
        else:
            dropbox = "no dropbox"
            filepath = "no path"
        return dropbox, filepath
    
    def GiveOutputString(self,strIn):
        '''
        takes the timestamp from the given string to
        create a string for the matching plot output file.
        an additional timestamp 144XXXXXX is added.
        
        input string hast to be of the form:
            /xxx/root_selectorFiles/my_selector_results_DY_v1_1447935068.root
            
        return:
            
            /xxx/root_plotFiles/2015_48/my_plot_results_v1_1447935068_144XXXXXX.root
        '''
        ss = self.dropbox + "root_plotFiles/"
        foldername = self.GiveYearWeekFoldername()
        strTemp = strIn.split("_")  
        strPath = ss + foldername + "my_plot_results_"
        dd = datetime.datetime.utcnow()
        strTimestamp = str(calendar.timegm(dd.timetuple()))
        strFormerTitle = strTemp[len(strTemp)-2]
        strEnd = strTemp[len(strTemp)-1] # 14XXXXXXXXXXX.root
        strFormerTimestamp = strEnd.split(".")[0]
        strFile = strFormerTitle + "_" + strFormerTimestamp + "_" + strTimestamp + ".root"
        if self.ensure_Dir(ss+foldername):
            #print "dir ensured"
            return strPath + strFile
        else:
            #print "dir not ensured"
            return False
    
    def inputFile(self,n=""):
        '''
        returns the last line (default) or given line of the file outputfiles.txt,
        cleaned by the \n newline sign
        '''
        path = self.dropbox + "root_selectorFiles/"
        f = path + "outputfiles.txt"
        k = 0
        l = False
        if(n!=""):
            l = True
        with open(f,"r") as s:
            for line in s:
                k += 1
                if(l and k == n):
                    break
        return self.inputFilePathCorrection(line.replace("\n",""))
    
    def inputFileNumberOfLines(self):
        path = self.dropbox + "root_selectorFiles/"
        f = path + "outputfiles.txt"
        k = 0
        with open(f,"r") as s:
            for line in s:
                k += 1
        return k
    
    def inputFilePathCorrection(self, s):
        '''
        gets a file of the form /xxx/root_selectorFiles/fileXYZ.root,
        clean it up to fileXYZ.root and create 
        the correct link for the current operating machine,
        
        '''
        strTemp = s.split("/")
        strFile = strTemp[len(strTemp)-1]
        return self.dropbox + "root_selectorFiles/" + strFile
            
    
    def FilelistDatetime(self):
        '''
        reads the file outputfiles.txt used by plot_tool.py
        and writes a list with the filenames and the
        datetime given from the timestamp, also the file size in byte
        
        output: files_date.txt
        '''
        path = self.dropbox + "root_selectorFiles/"
        fi = path +"outputfiles.txt"
        fo = path +"files_date.txt"
        lines = []
        with open(fi,"r") as s:
            for line in s:
                path = line.replace("\n","")
                try:
                    fsize = str(os.path.getsize(path))
                except os.error:
                    fsize = " n/a "
                ss = line.split("_")
                d = int(ss[len(ss)-1].split(".")[0])        #date from filename
                ds = str(datetime.datetime.fromtimestamp(d)) #datestring
                lines.append(ds + "\t" + fsize + "\t" + line)
        with open(fo,"w") as s:
            for l in lines:
                s.write(l)
        return True
    
    def GivePlotfolder(self):
        '''
        returns the path of the current plot folder (year and week number)
        example: "../plots/2015_48/" (including the last / )
        '''
        ss = self.dropbox + "root_plotFiles/"
        foldername = self.GiveYearWeekFoldername()
        if self.ensure_Dir(ss+foldername):
            #print "dir ensured"
            return ss+foldername
        else:
            #print "dir not ensured"
            return False
    
    def GiveYearWeekFoldername(self):
        '''
        returns a string constructed by year and week, e.g.:
        "2015_48/"
        '''
        today = datetime.date.today()
        y,w,d = today.isocalendar()
        return str(y)+"_"+str(w)+"/"
    
    
    def GivePlotFilePath(self,strFile):
        '''
        returns the file
        '''
        path = self.GivePlotfolder()
        return path+strFile
    
    
    def Folder(self):
        '''
        
        '''
        pass
    
    
    def ensure_Dir(self,path):
        '''
        checks if a path exists, otherwise create it
        '''
        d = os.path.dirname(path)
        if not os.path.exists(d):
            # print "path doesnt exist"
            os.mkdir(d)
            return True
        elif os.path.exists(d):
            # print "path exists"
            return True
    
    def GiveTimeStamp(self, s):
        '''
        returns the timestamp of a given string,
        if the structure of the string is of the form:
            "... _xyz_timestamp.root"
        '''
        pass
    
    # //////////////////////////////////////////////////////////////////////////////////
    # 
    def readHisto(self, lines=6, lineoff=0):
        '''
        Read the data in
        '''
        datakeys = {    "DYJetsToLL": 0,
                "TTGJets": 2,
                "WGToLNuG": 4,
                "ZGTo2LG": 6,
                "Run2015D-05Oct2015": 0,
                "Run2015D-PromptReco": 1
            }
        total_lines = self.inputFileNumberOfLines()
        
        h = {} # histograms
        h2 = {} # 2d histograms
        h3 = {} # 3d histograms
        stacks = {}
        
        #print "read histo()"
        
        # read files
        for ll in range(total_lines-lineoff-lines+1,total_lines-lineoff+1):
            # filename
            strInputFile = self.inputFile(ll)
            print "file: ", strInputFile
            for key in datakeys:
                if(key in strInputFile):
                    hname = key
            #print hname
            h[hname] = {}
            h2[hname] = {}
            try:
                data = rt.TFile(strInputFile)
            except:
                return False
                
            # loop all objects and check for histograms
            for key in data.GetListOfKeys():
                kgn = key.GetName()
                if "globalEcal" in kgn: # skip this mysterious histogram...
                    continue
                dt = data.Get(kgn)
                if isinstance(dt, rt.TH1):
                    h[hname][kgn] = data.Get(kgn)
                    h[hname][kgn].SetDirectory(0)
                if isinstance(dt, rt.TH2):
                    h2[hname][kgn] = data.Get(kgn)
                    h2[hname][kgn].SetDirectory(0)
                if isinstance(dt, rt.TH3):
                    h3[hname][kgn] = data.Get(kgn)
                    h3[hname][kgn].SetDirectory(0)
            
            data.Close()
        
        return h, h2, h3
        
        
    # //////////////////////////////////////////////////////////////////////////////////
    # 
    def readSingleFile(self, filename):
        '''
        Read a single file in
        '''
        datakeys = {    "DYJetsToLL": 0,
                "TTGJets": 2,
                "WGToLNuG": 4,
                "ZGTo2LG": 6,
                "Run2015D-05Oct2015": 0,
                "Run2015D-PromptReco": 1
            }
        total_lines = self.inputFileNumberOfLines()
        
        h = {} # histograms
        h2 = {} # 2d histograms
        h3 = {} # 3d histograms
        stacks = {}
        
        dropbox, filepath = self.SysInfo()
        
        # filename
        strInputFile = dropbox + "root_selectorFiles/" + filename
        
        print "file: ", strInputFile
        for key in datakeys:
            if(key in strInputFile):
                hname = key
        #print hname
        h[hname] = {}
        h2[hname] = {}
        try:
            data = rt.TFile(strInputFile)
        except:
            return False
            
        # loop all objects and check for histograms
        for key in data.GetListOfKeys():
            kgn = key.GetName()
            if "globalEcal" in kgn: # skip this mysterious histogram...
                continue
            dt = data.Get(kgn)
            if isinstance(dt, rt.TH1):
                h[hname][kgn] = data.Get(kgn)
                h[hname][kgn].SetDirectory(0)
            if isinstance(dt, rt.TH2):
                h2[hname][kgn] = data.Get(kgn)
                h2[hname][kgn].SetDirectory(0)
            if isinstance(dt, rt.TH3):
                h3[hname][kgn] = data.Get(kgn)
                h3[hname][kgn].SetDirectory(0)
        
        data.Close()
        
        return h, h2, h3
    
    
    def loadfile(self, filename):
        # 
        h = {}
        h2 = {}
        h3 = {}
        
        dropbox, filepath = self.SysInfo()
        data = rt.TFile(dropbox+"root_selectorFiles/"+filename)
        
        for key in data.GetListOfKeys():
            kgn = key.GetName()
            dt = data.Get(kgn)
            if isinstance(dt, rt.TH1):
                h[kgn] = data.Get(kgn)
                h[kgn].SetDirectory(0)
            if isinstance(dt, rt.TH2):
                h2[kgn] = data.Get(kgn)
                h2[kgn].SetDirectory(0)
            if isinstance(dt, rt.TH3):
                h3[kgn] = data.Get(kgn)
                h3[kgn].SetDirectory(0)
        
        data.Close()
        return h, h2, h3

class Parameters():
    '''
    a little class to handle parameters for methods
    '''
    def __init__(self):
        testfloat = 0.
        
        # define the attributes here
        
        self.variables = {} # all variables and dependencies
        
        self.stacks = None # stacks from mc
        self.data = None # data
        self.legends = None # legends
        
        self.fs = None #filesaver
        
        self.hproj = {} # projections
        
        self.zrng = [75, 105]
        
        
    
    def addvar(self, v, binedges, *pars):
        # v: string of the variable name
        # binrange: list with bin edges, like [0, 100]
        # valrange:
        if v not in self.variables.keys():
            self.variables[v] = {}
        self.variables[v]["bins"] = binedges
        self.variables[v]["range"] = range(binedges[0], binedges[1])
        self.variables[v]["effrng"] = range(binedges[0], binedges[1])
        
        
    def addfit(self, v, f):
        # f has do be a TF1 for the fit
        if v not in self.variables.keys():
            self.variables[v] = {}
        self.variables[v]["fit"] = f.Clone()
        
    def addeffrng(self, v, rng):
        # adds a plot x range for the efficiency
        if v not in self.variables.keys():
            self.variables[v] = {}
        self.variables[v]["effrng"] = rng

class ParVariables(dict):
    '''
    handle the variables 
    '''
    def add(self, var):
        self[var] = 0









