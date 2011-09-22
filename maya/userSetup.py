import sys, os
cgftools_home = os.environ.get("CGFTOOLS_HOME")#"/home/max/dev/cgftools"
sys.path.append( cgftools_home + '/maya/tools/python' )

print "Importing CGF python modules..."
from hqrender import *
