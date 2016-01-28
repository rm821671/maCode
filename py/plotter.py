# plotter


import sys
#sys.path.insert(0,"../py")

from modules import lib_plot as lp
from modules import auxiliary as aux
from modules import filemanager as fm
#from analyses import FakerateData
from analyses import *




def main():
	
	print sys.argv
	
	
	FakerateData.main()
	
	
	
	#
	


if __name__ == '__main__':
	print "... start"
	main()
	print "__main__ ... done!"
	
