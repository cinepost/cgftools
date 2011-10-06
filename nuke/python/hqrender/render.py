import nuke, os, sys
import getopt 
import cPickle as pickle
OS_ENV = ""		
		
if nuke.env["LINUX"]:
	OS_ENV = "lin"
elif nuke.env["WIN32"]:
	OS_ENV = "win"
else:
	OS_ENV = "osx"

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

source_os = os.environ.get('HQ_NK_SRC_OS')	
cross_paths = os.environ.get('HQ_NK_PATHS')
if cross_paths and source_os:
	try:
		cross_paths = pickle.loads(cross_paths)
	except:
		sys.stderr.write('Unable to load pickled cross paths %s !' % cross_paths)
		exit(1)
	else:
		for key in cross_paths.keys():
			sys.stdout.write('Replacing script path form %s to %s \n' % (source_os, OS_ENV))
			triple = cross_paths[key]	
			inScript = inScript.replace( triple[source_os], triple[OS_ENV] )
				
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
	
if cross_paths and source_os:
	allReadWriteNodes = [w for w in RecursiveFindNodes(['Write','Read'], nuke.root())]
	for key in cross_paths.keys():
		sys.stdout.write('Replacing file paths form %s to %s \n' % (source_os, OS_ENV))
		triple = cross_paths[key]	
		fixOSPaths( allReadWriteNodes, triple[source_os], triple[OS_ENV] )

# Render needed nodes
if out_nodes:
	for node in out_nodes:
		nuke.execute( node, f1, f2, f3 )
		
