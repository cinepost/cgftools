import os, sys, nuke, nukescripts
		
_hqRenderDialogState = nukescripts.DialogState()		
		
class HQrenderDialog( nukescripts.PythonPanel ):
	def __init__(self, mode=False):
		self._state = _hqRenderDialogState
		if mode == 'selected':
			title = "HQrender Selected"
		else:
			title = "HQrender All"	
		nukescripts.PythonPanel.__init__( self, title, "uk.co.thefoundry.FramePanel" )
		self.setMinimumSize(800, 260)

		self.rangeinput	= nuke.Enumeration_Knob( "rangeinput", "Frame range", ['input', 'custom', 'global'])
		self.rangeinput.clearFlag(nuke.STARTLINE)
		self.f1 		= nuke.Int_Knob( "f1", "", 0 )
		self.f1.clearFlag(nuke.STARTLINE)
		self.f2 		= nuke.Int_Knob( "f2", "", 10 )
		self.f2.clearFlag(nuke.STARTLINE)
		self.f3 		= nuke.Int_Knob( "f3", "", 1 )
		self.f3.clearFlag(nuke.STARTLINE)
		
		# General tab
		self.tabGener = nuke.Tab_Knob( 'General' )
		self.addKnob (self.tabGener)
		
		self.server 	= nuke.String_Knob('server', 'HQueue Server')
		self.frame 		= nuke.Int_Knob( "frame", "Frame:" )
		self.priority 	= nuke.Enumeration_Knob( "priority", "Priority", ['0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '10'])
		self.nukexec	= nuke.String_Knob('nukexec', 'Nuke Executable')
		self.mode 		= nuke.Enumeration_Knob( "mode", "", ["Render Current Script File", "Render Target Script File", "Copy Script to Shared Folder and Render"])
		self.tscript	= nuke.File_Knob( "file", "Target Script" )
		self.project 	= nuke.String_Knob('project', 'Project Path', "$HQROOT/projects")
		self.savescript	= nuke.Boolean_Knob('savescript', 'Automatically Save Script File')
		self.savescript.setFlag(nuke.STARTLINE)


		self.addKnob( self.rangeinput )
		self.addKnob( self.f1 )
		self.addKnob( self.f2 )
		self.addKnob( self.f3 )
		
		self.addKnob( self.server )
		self.addKnob( self.priority )
		self.addKnob( self.nukexec )
		self.addKnob( self.mode )
		self.addKnob( self.tscript )
		self.addKnob( self.project )
		self.addKnob( self.savescript )

		# Advanced tab	
		self.tabAdvanc	= nuke.Tab_Knob( 'Advanced' )
		self.addKnob( self.tabAdvanc )

		self.assignto 	= nuke.Enumeration_Knob( "assignto", "Assign To", ["Any Client", "Listed Clients", "Clients from Listed Groups"])
		self.clients	= nuke.String_Knob('clients', 'Clients')
		self.selclients = nuke.Script_Knob( "Select Clients" )
		self.selclients.clearFlag(nuke.STARTLINE)
		self.groups		= nuke.String_Knob('groups', 'Groups')
		self.selgroups  = nuke.Script_Knob( "Select Groups" )
		self.selgroups.clearFlag(nuke.STARTLINE)
		self.allinone	= nuke.Boolean_Knob('allinone', 'Batch All Frames in One Job')
		self.allinone.setFlag(nuke.STARTLINE)
		self.fperjob	= nuke.Int_Knob('fperjob', 'Frames Per Job')		
		
		self.addKnob( self.assignto )
		self.addKnob( self.clients )
		self.addKnob( self.selclients )
		self.addKnob( self.groups )
		self.addKnob( self.selgroups )
		self.addKnob( self.allinone )
		self.addKnob( self.fperjob )
		
		# Fill defaults	
		#self.mode.setValue(1)
		self.f1.setValue(self._state.get(self.f1, 0))
		self.f2.setValue(self._state.get(self.f2, 24))
		self.f3.setValue(self._state.get(self.f3, 1))
		self.server.setValue(self._state.get(self.server, "localhost:5000"))
		self.priority.setValue(self._state.get(self.priority, 5))
		self.nukexec.setValue(self._state.get(self.nukexec, "Nuke6.2"))
		self.mode.setValue(self._state.get(self.mode, 0))
		self.project.setValue(self._state.get(self.project, "$HQROOT/projects"))
		self.savescript.setValue(self._state.get(self.savescript, True))
        
	def knobChanged( self, knob):
		if knob == self.mode:
			if knob.value() == "Render Target Script File":
				self.tscript.setEnabled(True)
				self.project.setEnabled(False)
			elif knob.value() == "Copy Script to Shared Folder and Render":
				self.tscript.setEnabled(False)
				self.project.setEnabled(True)
			else:
				self.tscript.setEnabled(False)
				self.project.setEnabled(False)
		elif knob == self.assignto:
			if knob.value() == "Listed Clients":
				self.clients.setEnabled(True)
				self.selclients.setEnabled(True)
				self.groups.setEnabled(False)
				self.selgroups.setEnabled(False)
			elif knob.value() == "Clients from Listed Groups":
				self.clients.setEnabled(False)
				self.selclients.setEnabled(False)
				self.groups.setEnabled(True)
				self.selgroups.setEnabled(True)
			else:
				self.clients.setEnabled(False)
				self.selclients.setEnabled(False)
				self.groups.setEnabled(False)
				self.selgroups.setEnabled(False)										

		self._state.save(knob) 

	def showModalDialog( self ):
		result = nukescripts.PythonPanel.showModalDialog( self )
		if result:
			self.generateHQJob()
			
	def generateHQJob( self ):
		import xmlrpclib
		server_addr = "http://%s" % self.server.value()
		hq_server = xmlrpclib.ServerProxy( server_addr )
		try:
			hq_server.ping()
		except:
			raise BaseException("Unable to coonect HQueue server: %s" % server_addr)
		else:
			f1 = self.f1.value()
			f2 = self.f2.value()
			f3 = self.f3.value()
			nuke_exec = self.nukexec.value()
			
			job_frames = []
			for frame in range(f1, f2, f3):
				rcmd  = 'echo "Rendering frame %d"' % frame  
				job_frames += [{
					'name'		:	'Rendering frame %d' % frame,
					'shell'		:	'bash',
					'command'	:	rcmd	
				}]
			
			job_spec = {
				'name'	:	'Nuke script',
				'priority': self.priority.value(),
				'children': job_frames
			}
			job_id = hq_server.newjob(job_spec)						
