import maya.cmds as cmds
import pymel.core as pm
from common import *

def clientCback(item):
	print item

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
			
			hqparms['priority'] = 6
			hqparms['hqserver'] = 'localhost:5000'
			hqparms['hfs']	= '$HQROOT/houdini_distros/hfs.$HQCLIENTARCH'
			hqparms['assignto'] = 1
			hqparms['source'] = 1
			hqparms['clients'] = ''
			hqparms['groups'] = ''
			
			pyToAttr('hqparmsCfg.globlas', hqparms)
			
		# Create tool window
		window = pm.window( title="HQrender", iconName='HQ' )
		
		pm.columnLayout( columnAttach=('both', 5) , adjustableColumn=True )
		
		tabs = pm.tabLayout( innerMarginWidth=5, innerMarginHeight=5 )
		
		general = pm.columnLayout()
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
		
		pm.textFieldButtonGrp( label='Target Scene', text='', buttonLabel='Browse', columnWidth3=[120,500,118], columnAttach=[(1, 'right', 0), (2, 'both', 0), (3, 'both', 0)])
		pm.textFieldGrp( label='Project Path', text='$HQROOT/projects', columnWidth2=[120,620] )
		pm.checkBoxGrp( numberOfCheckBoxes=1, label='' , label1='Automatically Save Scene File')
		pm.setParent( '..' )
	
		advanced = pm.columnLayout()
		assign_menu = pm.optionMenuGrp( label='Assign To', changeCommand=clientCback, columnWidth2=[120, 280])
		pm.menuItem( label='Any Client' )
		pm.menuItem( label='Listed Clients' )
		pm.menuItem( label='Clients from Listed Groups' )
		pm.optionMenuGrp( assign_menu, edit=True, select=hqparms.get('assignto') )
		
		pm.textFieldButtonGrp( label='Clients', text=hqparms.get('clients'), buttonLabel='Select Clients',  columnWidth3=[120,500,118], columnAttach=[(1, 'right', 0), (2, 'both', 0), (3, 'both', 0)] )
		pm.textFieldButtonGrp( label='Client Groups', text='', buttonLabel='Select Groups',  columnWidth3=[120,500,118], columnAttach=[(1, 'right', 0), (2, 'both', 0), (3, 'both', 0)])
		pm.setParent( '..' )
		
		mropts = cmds.columnLayout()
		pm.text( label='*These options only apply for menatalray rendering engine mode', align='center' )
		mr_assign_menu = pm.optionMenuGrp( label='Assign MI Job To', changeCommand=clientCback, columnWidth2=[120, 280])
		pm.menuItem( label='Same Clients Assigned to Render JObs' )
		pm.menuItem( label='Listed Clients' )
		pm.menuItem( label='Clients from Listed Groups' )
		pm.optionMenuGrp( assign_menu, edit=True, select=hqparms.get('assignto') )
		pm.textFieldButtonGrp( label='Clients', text=hqparms.get('clients'), buttonLabel='Select Clients',  columnWidth3=[120,500,118], columnAttach=[(1, 'right', 0), (2, 'both', 0), (3, 'both', 0)] )
		pm.textFieldButtonGrp( label='Client Groups', text='', buttonLabel='Select Groups',  columnWidth3=[120,500,118], columnAttach=[(1, 'right', 0), (2, 'both', 0), (3, 'both', 0)])
		
		cmds.floatSliderGrp( label='Min. Clients / Frame', field=True, columnWidth3=[120,118,500], columnAttach=[(1, 'right', 0), (2, 'both', 0), (3, 'both', 0)] )
		cmds.floatSliderGrp( label='Max. Clients / Frame', field=True, columnWidth3=[120,118,500], columnAttach=[(1, 'right', 0), (2, 'both', 0), (3, 'both', 0)] )
		
		pm.setParent( '..' )
	
		pm.setParent( '..' )
		
		pm.rowLayout( numberOfColumns=2, columnWidth2=[120, 120], columnAttach=[(1, 'both', 0), (2, 'both', 0)] )
		pm.button( label='Render' , width=100)
		pm.button( label='Close', command=('cmds.deleteUI(\"' + window + '\", window=True)') , width=100)
		pm.setParent( '..' )
		
		pm.tabLayout( tabs, edit=True, tabLabel=((general, 'General'), (advanced, 'Advanced'), (mropts, "MR Options")) )
		
		pm.showWindow( window )
		return True
	else:
		# window already opened so just skip
		return True 
