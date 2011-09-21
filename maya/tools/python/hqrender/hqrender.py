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

def render(parms={}):
	hq_server = getHQserver(parms)		
	if hq_server is not None:
		job_spec = {
         "name": "Print Hello World",
         "shell": "bash",
         "command": "echo 'Hello World!'"
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
		if cmds.objExists('hqparmsCfg' ):
			# Stored parms
			cmds.select( 'hqparmsCfg' )
			hqparms = attrToPy( 'hqparmsCfg.globlas' )
		else:
			# Default parms
			cmds.createNode( 'unknown', n='hqparmsCfg' ) 
			
			hqparms = {
				'priority' : 6,
				'groups': '',
				'assignto': 1,
				'source' : 1,
				'clients': '',
				'hfs' : '$HQROOT/houdini_distros/hfs.$HQCLIENTARCH',
				'hqserver' : 'localhost:5000',
			}	
			
			pyToAttr('hqparmsCfg.globlas', hqparms)
			
		# Create tool window
		window = pm.window( title="HQrender", iconName='HQ', sizeable=False)
		pm.columnLayout( columnAttach=('both', 5), rowSpacing=6, adjustableColumn=True )
		tabs = pm.tabLayout( innerMarginWidth=5, innerMarginHeight=5 )
		
		# general tab
		general = pm.columnLayout( columnAttach=('both', 8), rowSpacing=4, adjustableColumn=True )
		hq_server = pm.textFieldGrp( label='HQueue Server', text=hqparms.get('hqserver'), columnWidth2=[120,620] )
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
		pm.textFieldButtonGrp( label='Clients', text=hqparms.get('clients'), buttonLabel='Select Clients',  columnWidth3=[120,500,118], columnAttach=[(1, 'right', 0), (2, 'both', 0), (3, 'both', 0)] )
		pm.textFieldButtonGrp( label='Client Groups', text='', buttonLabel='Select Groups',  columnWidth3=[120,500,118], columnAttach=[(1, 'right', 0), (2, 'both', 0), (3, 'both', 0)])
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
		pm.textFieldButtonGrp( label='Clients', text=hqparms.get('clients'), buttonLabel='Select Clients',  columnWidth3=[120,500,118], columnAttach=[(1, 'right', 0), (2, 'both', 0), (3, 'both', 0)] )
		pm.textFieldButtonGrp( label='Client Groups', text='', buttonLabel='Select Groups',  columnWidth3=[120,500,118], columnAttach=[(1, 'right', 0), (2, 'both', 0), (3, 'both', 0)])
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
