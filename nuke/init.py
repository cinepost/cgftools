import os, sys
pymodules_path = "/usr/lib/pymodules/python2.6"

home = os.environ.get('HOME')
sys.path.append(home + '/.nuke/python/')
sys.path.append(pymodules_path)

from hqrender import *

#nuke.pluginAddPath(searchPath)
#if nuke.env["LINUX"]:
#    pluginFile = "libNT_Guides"
    
#elif nuke.env["WIN32"]:
#    pluginFile = "NT_Guides"
    
#try:        
#    nuke.load(pluginFile)
#    print ("Loaded plugin: "+pluginFile)
#except:
#    print ("Failed to load: "+pluginFile)
