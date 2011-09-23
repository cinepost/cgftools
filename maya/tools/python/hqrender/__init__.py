import sys, traceback

try:
	import hqrender
	import hqdialog
except:
	print "Warning: Unable to load HQrender module !!!"
	traceback.print_exc(file=sys.stdout) 	
else:
	print "HQrender module loaded"
