import nuke, os, sys
import getopt 
import cPickle as pickle
OSX = False
WIN = False
LIN = False

if nuke.env["LINUX"]:
	LIN = True
elif nuke.env["WIN32"]:
	WIN = True
else:
	OSX = True	

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

sys.stdout.write('ARGV       : %s \n' % sys.argv[1:])
options, reminder = getopt.getopt(sys.argv[1:], 's:e:i:', ['--f1', '--f2', '--f3'])
sys.stdout.write('OPTIONS    : %s \n' % options)
for opt, arg in options:
	if opt in ('-s', '--f1'):
		f1 = int(arg)
	elif opt in ('-e', '--f2'):
		f2 = int(arg)
	elif opt in ('-i', '--f3'):
		f3 = int(arg)		

inScript = sys.argv[-1]

# Open .nk script
nuke.scriptOpen( inScript )
sys.stdout.write('Using script: %s \n' % inScript)

# Read hidden conf node
try:
	conf = pickle.loads(nuke.knob('root.hqcfg'))
except:
	raise BaseException('Unable to load HQrender configuration from script !!!')

mapping = conf.get('cross_path')
mode = os.environ.get('HQ_NK_MODE')
out_nodes = [w for w in RecursiveFindNodes(['Write'], nuke.root())]
if mode == 'selected':
	nk_nodes = os.environ.get('HQ_NK_NODES')
	if nk_nodes: # Convert string fo form "word1;word2;word3" to list ["word1", "word2", "word3"]
		node_names_to_process = filter( lambda a: a!="", nk_nodes.split(';'))
		out_nodes = [nuke.toNode(n) for n in node_names_to_process]

sys.stdout.write('Rendering %s nodes: %s \n' % (mode, [node.name() for node in out_nodes]))
	

allReadWriteNodes = [w for w in RecursiveFindNodes(['Write','Read'], nuke.root())]
##fixOSPaths( allReadWriteNodes, inPattern, outPattern )

# Render needed nodes
if out_nodes:
	for node in out_nodes:
		nuke.execute( node, f1, f2, f3 )
		
