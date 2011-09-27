import nuke
import os
import sys

def RecursiveFindNodes(nodeClasses, startNode):
    if startNode.Class() in nodeClasses:
        yield startNode
    elif isinstance(startNode, nuke.Group):
        for child in startNode.nodes():
            for foundNode in RecursiveFindNodes(nodeClasses, child):
                yield foundNode

def fixOSPaths( nodes, inPattern, outPattern ):
	for node in nodes:
		path = node['file'].value()
		path = path.replace( inPattern, outPattern )
		node['file'].setValue( path )

if len ( sys.argv ) != 4:
  print 'Usage: NUKE render.py <nuke_script> <in_file_pattern> <new_file_pattern>'
  sys.exit(-1)

inScript = sys.argv[1]
inPattern = sys.argv[2]
outPattern = sys.argv[3]

# Open .nk script
nuke.scriptOpen( inScript )

allReadWriteNodes = [w for w in RecursiveFindNodes(['Write','Read'], nuke.root())]
fixOSPaths( allReadWriteNodes, inPattern, outPattern )

# Render needed nodes
frame_range = range(f1, f2, f3)

mode = 'all'
if mode == 'all':
	out_nodes = RecursiveFindNodes(['Write'], nuke.root())
else:
	out_nodes = None

if out_nodes:
	for node in out_nodes:
		for frame in frame_range:
			nuke.execute( nodeName, frame, frame )
		
