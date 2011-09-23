import os, sip, nuke, nukescripts
from PyQt4 import QtGui

class HQrenderDialog( nukescripts.PythonPanel ):
	p = None
	def __init__(self):
		nukescripts.PythonPanel.__init__( self, "HQrender", "uk.co.thefoundry.FramePanel" )
		
		# General tab
		self.tabGener = nuke.Tab_Knob( 'General' )
		self.addKnob (self.tabGener)
		
		self.server 	= nuke.String_Knob('server', 'HQueue Server', "localhost:5000")
		self.frame 		= nuke.Int_Knob( "frame", "Frame:" )
		self.priority 	= nuke.Enumeration_Knob( "priority", "Priority", ['0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '10'])
		self.hfs 		= nuke.String_Knob('hfs', 'Target HFS', "$HQROOT/nuke_distros/nuke.$HQCLIENTARCH")
		self.mode 		= nuke.Enumeration_Knob( "mode", "", ["Render Current Script File", "Render Target Script File", "w e r r t t t ttt"])
		self.tscript	= nuke.File_Knob( "file", "Target Script" )
		self.project 	= nuke.String_Knob('project', 'Project Path', "$HQROOT/projects")
		self.savescript	= nuke.Boolean_Knob('savescript', 'Automatically Save Script File', True)
		
		self.addKnob( self.server )
		self.addKnob( self.priority )
		self.addKnob( self.hfs )
		self.addKnob( self.mode )
		self.addKnob( self.tscript )
		self.addKnob( self.project )
		
		#self.addKnob( self.frame )
		#self.frame.setValue( nuke.frame() )

		# Advanced tab	
		self.tabAdvanc = nuke.Tab_Knob( 'Advanced' )
		self.addKnob( self.tabAdvanc )

	def showModalDialog( self ):
		result = nukescripts.PythonPanel.showModalDialog( self )
		if result:
			nuke.frame( self.frame.value() )
        
	def changeFileMode( self ):
