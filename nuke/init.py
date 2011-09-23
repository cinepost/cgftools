import os, sys
cgftools_home = "/home/max/dev/cgftools"
pymodules_path = "/usr/lib/pymodules/python2.6"

os.environ['CGFTOOLS_HOME'] = cgftools_home
sys.path.append(cgftools_home + '/common/tools/python')
sys.path.append(cgftools_home + '/nuke/tools/python')
sys.path.append(pymodules_path)

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
