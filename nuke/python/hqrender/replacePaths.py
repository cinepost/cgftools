### save this file as replaceWritePaths.py

import nuke
import os
import sys

def RecursiveFindNodes(nodeClass, startNode):
    if startNode.Class() == nodeClass:
        yield startNode
    elif isinstance(startNode, nuke.Group):
        for child in startNode.nodes():
            for foundNode in RecursiveFindNodes(nodeClass, child):
                yield foundNode



if len ( sys.argv ) != 5:
  print 'Usage: NUKE replaceWritePaths.py <nuke_script> <new_nuke_script> <in_file_pattern> <new_file_pattern>'
  sys.exit(-1)

inScript = sys.argv[1]
outScript = sys.argv[2]
inPattern = sys.argv[3]
outPattern = sys.argv[4]

nuke.scriptOpen( inScript )

allWriteNodes = [w for w in RecursiveFindNodes('Write', nuke.root())]

for write in allWriteNodes:
  path = write['file'].value()
  path = path.replace( inPattern, outPattern )
  write['file'].setValue( path )

nuke.scriptSave( outScript )
