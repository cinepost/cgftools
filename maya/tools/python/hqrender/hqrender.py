import maya
import maya.cmds as cmds
import pymel.core as pm
from common import *

def clientCback(item):
	print item

def getHQserver(parms={}):
	try:
		import xmlrpclib
	except:
		pm.error( "Unable to import xmlrpclib." )
		return None
	else:
		hq_server = xmlrpclib.ServerProxy( "http://" + parms.get('hqserver'))
		try:
			hq_server.ping()
		except:
			pm.error( "HQueue server is down." )
			return None
		else:
			return hq_server

def getRGAttr(rg, attr):
	try:
		return cmds.getAttr("%s.%s" % (rg, attr))
	except:
		return None

def render(parms={}):
	rg = "defaultRenderGlobals"
	hq_server = getHQserver(parms)		
	if hq_server is not None:
		rendering_proj	= maya.mel.eval('workspace -q -fullName')
		if parms.get('source') == 1:
			rendering_scene = maya.mel.eval('file -q -sn')
		else:
			rendering_scene = parms.get('scene_file')
			
		if not rendering_scene:
			rendering_scene = 'undefined'
		
		start_frame = getRGAttr(rg, 'startFrame')
		end_frame	= getRGAttr(rg, 'endFrame')
		frame_step  = getRGAttr(rg, 'byFrameStep')
		padding 	= getRGAttr(rg, 'extensionPadding')
		if end_frame > start_frame + 10:
			padding = 4
		
		job_frames = []
		for frame in range(start_frame, end_frame, frame_step):
			rcmd  = 'Render -r sw -s %d -e %d -b %d ' % (frame, frame, 1)
			rcmd += '-pad %d ' % padding  
			job_frames += [{
				'name'		:	'Rendering frame %d' % frame,
				'shell'		:	'bash',
				'command'	:	rcmd	
			}]
		
		job_spec = {
			'name'	:	'%s for project %s' % (rendering_scene, rendering_proj),
			'children': job_frames
		}
		job_id = hq_server.newjob(job_spec)
		print "Job %s submitted to HQserver %s" % (job_id, parms.get('hqserver'))
		return True
	else:
		return False	

def close(window):
	pm.deleteUI(window, window=True)

def open():
	if(pm.window("HQrender", exists=True) == False):
		hqparms = {}
		# Default and stored parms
		if cmds.objExists('cgfHQparms' ):
			# Stored parms
			cmds.select( 'cgfHQparms' )
			hqparms = attrToPy( 'cgfHQparms.globals' )
		else:
			# Default parms
			cmds.createNode( 'unknown', n='cgfHQparms' ) 
			
			hqparms = {
				'driver' : 1,
				'priority' : 6,
				'groups': '',
				'assignto': 1,
				'source' : 1,
				'clients': '',
				'hfs' : '$HQROOT/houdini_distros/hfs.$HQCLIENTARCH',
				'hqserver' : 'localhost:5000',
				'cli_list_state' : False,
				'grp_list_state' : False,
				'scn_state' : False,
				'prj_state' : False
			}	
			
			pyToAttr('cgfHQparms.globals', hqparms)
			
		# Create tool window
		window = pm.window( title="HQrender", iconName='HQ', sizeable=False)
		pm.columnLayout( columnAttach=('both', 5), rowSpacing=6, adjustableColumn=True )
		tabs = pm.tabLayout( innerMarginWidth=5, innerMarginHeight=5 )
		
		# general tab
		general = pm.columnLayout( columnAttach=('both', 8), rowSpacing=4, adjustableColumn=True )
		hq_server = pm.textFieldGrp( label='HQueue Server', text=hqparms.get('hqserver'), columnWidth2=[120,620] )
		driver_menu = pm.optionMenuGrp( label='Render using', columnWidth2=[120, 280] )
		pm.menuItem( label='Maya Software' )
		pm.menuItem( label='Mental Ray' )
		pm.optionMenuGrp( driver_menu, edit=True, select=hqparms.get('driver') )
		
		priority_menu = pm.optionMenuGrp( label='Priority', columnWidth2=[120, 80] )
		pm.menuItem( label='0' )
		pm.menuItem( label='1' )
		pm.menuItem( label='2' )
		pm.menuItem( label='3' )
		pm.menuItem( label='4' )
		pm.menuItem( label='5' )
		pm.menuItem( label='6' )
		pm.menuItem( label='7' )
		pm.menuItem( label='8' )
		pm.menuItem( label='9' )
		pm.menuItem( label='10' )	
		pm.optionMenuGrp( priority_menu, edit=True, select=hqparms.get('priority') )	
	
		pm.textFieldGrp( label='Target HFS', text='$HQROOT/houdini_distros/hfs.$HQCLIENTARCH', columnWidth2=[120,620], columnAttach2=['right','both'])
		source_menu = pm.optionMenuGrp( label='Render Source', changeCommand=clientCback, columnWidth2=[120, 280])
		pm.menuItem( label='Render Current Scene File' )
		pm.menuItem( label='Render Target Scene File' )
		pm.menuItem( label='Copy Project Files to Shared Folder and Render' )	
		pm.optionMenuGrp( source_menu, edit=True, select=hqparms.get('source') )
		pm.separator( height=14, style='in' )
		pm.textFieldButtonGrp( label='Target Scene', text='', buttonLabel='Browse', columnWidth3=[120,500,118], columnAttach=[(1, 'right', 0), (2, 'both', 0), (3, 'both', 0)])
		pm.textFieldGrp( label='Project Path', text='$HQROOT/projects', columnWidth2=[120,620] )
		pm.checkBoxGrp( numberOfCheckBoxes=1, label='' , label1='Automatically Save Scene File')
		pm.setParent( '..' )
	
		# advanced tab
		advanced = pm.columnLayout( columnAttach=('both', 8), rowSpacing=4, adjustableColumn=True )
		assign_menu = pm.optionMenuGrp( label='Assign To', changeCommand=clientCback, columnWidth2=[120, 280])
		pm.menuItem( label='Any Client' )
		pm.menuItem( label='Listed Clients' )
		pm.menuItem( label='Clients from Listed Groups' )
		pm.optionMenuGrp( assign_menu, edit=True, select=hqparms.get('assignto') )
		ast1 = pm.textFieldButtonGrp( label='Clients', text=hqparms.get('clients'), buttonLabel='Select Clients', editable=hqparms.get('cli_list_state'), columnWidth3=[120,500,118], columnAttach=[(1, 'right', 0), (2, 'both', 0), (3, 'both', 0)] )
		ast2 = pm.textFieldButtonGrp( label='Client Groups', text='', buttonLabel='Select Groups', editable=hqparms.get('grp_list_state'), columnWidth3=[120,500,118], columnAttach=[(1, 'right', 0), (2, 'both', 0), (3, 'both', 0)])
		pm.optionMenuGrp( assign_menu, edit=True, changeCommand=(lambda *args: hq_dialog_assignToCallback(*args, controls = [ast1, ast2], parms=hqparms)) )
		pm.separator( height=14, style='in' )
		pm.setParent( '..' )
		
		# mentalray tab
		mropts = cmds.columnLayout( columnAttach=('both', 8), rowSpacing=4, adjustableColumn=True )
		pm.text( label='*These options only apply for menatalray rendering engine mode', align='center' )
		pm.separator( height=14, style='in' )
		mr_assign_menu = pm.optionMenuGrp( label='Assign MI Job To', changeCommand=clientCback, columnWidth2=[120, 280])
		pm.menuItem( label='Same Clients Assigned to Render Jobs' )
		pm.menuItem( label='Listed Clients' )
		pm.menuItem( label='Clients from Listed Groups' )
		pm.optionMenuGrp( assign_menu, edit=True, select=hqparms.get('assignto') )
		pm.textFieldButtonGrp( label='Clients', text=hqparms.get('clients'), buttonLabel='Select Clients', columnWidth3=[120,500,118], columnAttach=[(1, 'right', 0), (2, 'both', 0), (3, 'both', 0)] )
		pm.textFieldButtonGrp( label='Client Groups', text='', buttonLabel='Select Groups', columnWidth3=[120,500,118], columnAttach=[(1, 'right', 0), (2, 'both', 0), (3, 'both', 0)])
		pm.separator( height=14, style='in' )
		pm.floatSliderGrp( label='Min. Clients / Frame', field=True, columnWidth3=[120,118,500], columnAttach=[(1, 'right', 0), (2, 'both', 0), (3, 'both', 0)] )
		pm.floatSliderGrp( label='Max. Clients / Frame', field=True, columnWidth3=[120,118,500], columnAttach=[(1, 'right', 0), (2, 'both', 0), (3, 'both', 0)] )
		pm.setParent( '..' )
		pm.setParent( '..' )
		
		pm.rowLayout( numberOfColumns=2, columnWidth2=[120, 120], columnAttach=[(1, 'both', 0), (2, 'both', 0)] )
		renderBtn = pm.uitypes.Button(label="Render")
		renderBtn.setCommand(lambda *args: render(hqparms))
		closeBtn  = pm.uitypes.Button(label="Close")
		closeBtn.setCommand(lambda *args: close(window))
		pm.setParent( '..' )
		
		pm.tabLayout( tabs, edit=True, tabLabel=((general, 'General'), (advanced, 'Advanced'), (mropts, "MR Options")) )
		pm.showWindow( window )
		return True
	else:
		# window already opened so just skip
		return True 

def hq_dialog_assignToCallback(mode, controls = [], parms={}):
	if mode == "Listed Clients":
		pm.textFieldButtonGrp( controls[0], edit=True, editable=True )
		parms['cli_list_state'] = True
		pm.textFieldButtonGrp( controls[1], edit=True, editable=False )
		parms['grp_list_state'] = False
	elif mode == "Clients from Listed Groups":
		pm.textFieldButtonGrp( controls[0], edit=True, editable=False )
		parms['cli_list_state'] = False
		pm.textFieldButtonGrp( controls[1], edit=True, editable=True )
		parms['grp_list_state'] = True
	else:
		pm.textFieldButtonGrp( controls[0], edit=True, editable=False )
		pm.textFieldButtonGrp( controls[1], edit=True, editable=False )
		parms['cli_list_state'] = False
		parms['grp_list_state'] = False
		
	pyToAttr('cgfHQparms.globals', hqparms)		
			
