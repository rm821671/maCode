

from os.path import dirname, basename, isfile
import glob
modules = glob.glob(dirname(__file__)+"/*.py")
# add all python files in directory to module
__all__ = [ basename(f)[:-3] for f in modules if isfile(f)]
