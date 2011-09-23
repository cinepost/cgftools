import sys, os
sys.path.append( os.environ.get("CGFTOOLS_HOME") + '/maya/tools/python' )
sys.path.append( os.environ.get("PYMODULES_PATH") )
print "Importing CGF python modules..."
from hqrender import *
