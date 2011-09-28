import os, sys, nuke, nukescripts
from xml.dom import minidom, Node		
import cPickle as pickle
	
OS_ENV = ""		
		
if nuke.env["LINUX"]:
	OS_ENV = "LINUX"
elif nuke.env["WIN32"]:
	OS_ENV = "WIN32"
else:
	OS_ENV = "OSX"			
		
class DialogState:
	def __init__(self):
		self._state = {}

	def get(self, knob, defaultValue = None):
		return self.getValue(knob.name(), defaultValue)
	
	def save(self, knob):
		self.saveValue(knob.name(), knob.value())
	
	def setKnob(self, knob, defaultValue = None):
		knob.setValue(self.get(knob, defaultValue))
	
	def saveValue(self, id, value):
		self._state[id] = value
	
	def getValue(self, id, defaultValue = None):
		return self._state.get(id, defaultValue) 		
		
_hqRenderDialogState = DialogState()		
		
def readXMLElement( element ):
	if element.childNodes:
		retval = {}
		for child in element.childNodes:
			if child.nodeType == Node.ELEMENT_NODE:	
				retval[child.getAttribute('name')] = readXMLElement(child)
			else:
				continue	
		return retval	
	else:
		if element.nodeType == Node.ELEMENT_NODE:
			attr_type = element.getAttribute('type')
			if attr_type == "int":
				return int(element.getAttribute('value'))
			elif attr_type == "float":
				return float(element.getAttribute('value'))
				
			return element.getAttribute('value')
		
def readXMLtoState( xml, state ):
	xml_conf_root = xml.getElementsByTagName("hqrenderConf")[0]
	for child in xml_conf_root.childNodes:
		if child.nodeType != Node.ELEMENT_NODE:
			continue
		else:	
			state.saveValue(child.getAttribute('name'), readXMLElement(child))						
		
class HQClientGroupsDialog ( nukescripts.PythonPanel ):
	def __init__(self, mode='clients', server='localhost:5000', backstring=None, initlist = []):
		import xmlrpclib
		self.backstring = backstring
		server_addr = "http://%s" % server
		hq_server = xmlrpclib.ServerProxy( server_addr )
		try:
			hq_server.ping()
		except:
			raise BaseException("Unable to coonect HQueue server: %s" % server_addr)
		else:
			if mode == 'clients':
				title = "Select Clients"
				items = hq_server.getClients()
			else:		
				title = "Select Client Groups"
				items = hq_server.getClientGroups()
			
			nukescripts.PythonPanel.__init__( self, title, "uk.co.thefoundry.FramePanel" )
			self.setMinimumSize(180, 100)
				
			self.entries = []
			if items:	
				for item in items:
					entry = nuke.Boolean_Knob(item.get('hostname'), item.get('hostname'))
					self.addKnob( entry )
					self.entries += [entry]
			else:
				self.addKnob( nuke.Text_Knob("No %s available from server." % mode) )				

	def showModalDialog( self ):
		result = nukescripts.PythonPanel.showModalDialog( self )
		if result:
			result_items = ""
			for entry in self.entries:
				if entry.value():
					result_items += "%s;" % entry.name()
			self.backstring.setValue(result_items[:-1])		
				
class HQrenderDialog( nukescripts.PythonPanel ):
	def __init__(self, mode ):
		self._state = _hqRenderDialogState
		self.hqmode = mode
		hqrenderConfig = minidom.parse(os.path.expanduser('~/.nuke/python/hqrender/hqrenderConfig.xml'))
		readXMLtoState(hqrenderConfig, self._state)
	
		if mode == 'selected':
			title = "HQrender Selected"
			self.input_node = nuke.selectedNode()
		else:	
			title = "HQrender All"
			self.input_node = None
	
		nukescripts.PythonPanel.__init__( self,title, "uk.co.thefoundry.FramePanel" )
		self.setMinimumSize(800, 260)
		
		try:
			stored_cfg = nuke.knob('root.hqcfg')
		except:
			# create pickled conf data
			nuke.Root().addKnob(nuke.String_Knob('hqcfg', 'HQueue Render Conf', pickle.dumps(self._state._state)))
		else:	
			# read pickled conf data
			try:
				self._state._state.update(pickle.loads(stored_cfg))
			except:
				nuke.knob('root.hqcfg', pickle.dumps({}))
		
		self.viewers = []
		for a in nuke.allNodes():
			if a.Class()=='Viewer':
				frange = a.frameRange()
				self._state.saveValue('%s_range' % a['name'].value(), {'f1':frange.first(),'f2':frange.last(),'f3':frange.increment()})
				self.viewers += [a['name'].value()]	

		self.rangeinput	= nuke.Enumeration_Knob( "rangeinput", "Frame range", ['input', 'custom', 'global'] + self.viewers)
		self.rangeinput.clearFlag(nuke.STARTLINE)
		self.f1 		= nuke.Int_Knob( "f1", "" )
		self.f1.clearFlag(nuke.STARTLINE)
		self.f2 		= nuke.Int_Knob( "f2", "" )
		self.f2.clearFlag(nuke.STARTLINE)
		self.f3 		= nuke.Int_Knob( "f3", "" )
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
		self.selclients = nuke.PyScript_Knob("selclients", "Select Clients","getClientsFromServer()")
		self.selclients.clearFlag(nuke.STARTLINE)
		self.groups		= nuke.String_Knob('groups', 'Groups')
		self.selgroups  = nuke.PyScript_Knob("selgroups", "Select Groups","getGroupsFromServer()") 
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
		
		
		# Mapping tab
		if self._state.getValue('cross_path'):
			self.tabMapping	= nuke.Tab_Knob( 'Mapping' )
			self.addKnob( self.tabMapping )
			self.disableMapping	= nuke.Boolean_Knob('disableMapping', 'Disable mapping')
			self.addKnob( self.disableMapping )
			groups = self._state.getValue('cross_path')
			for key in groups.keys():
				self.addKnob( nuke.Text_Knob(key))	
				paths = groups[key]
				for path in paths.keys():	
					self.addKnob( nuke.String_Knob(key + path, path, paths[path]) )
				
		# Fill defaults
		self.rangeinput.setValue(self._state.get(self.rangeinput, 0))
		if not self._state.getValue('custom_range'):	
			self._state.saveValue('custom_range', {'f1':0,'f2':24,'f3':1})
		self.server.setValue(self._state.get(self.server, "localhost:5000"))
		self.priority.setValue(self._state.get(self.priority, 5))
		self.nukexec.setValue(self._state.get(self.nukexec, "Nuke6.2"))
		self.mode.setValue(self._state.get(self.mode, 0))
		self.project.setValue(self._state.get(self.project, "$HQROOT/projects"))
		self.savescript.setValue(self._state.get(self.savescript, True))
		self.fperjob.setValue(self._state.get(self.fperjob, 1))
        
        # Update UI
		self.updateScriptMode()
		self.updateAssignment()
		self.updateFrameRange()	    
        
	def knobChanged( self, knob):
		if knob == self.mode:
			self.updateScriptMode()
		elif knob == self.assignto:
			self.updateAssignment()
		elif knob == self.rangeinput:
			self.updateFrameRange()	
		elif knob in [self.f1, self.f2, self.f3]:
			self.storeFrames()

		self._state.save(knob) 

	def storeFrames( self ):
		if self.rangeinput.value() == 'custom':
			self._state.saveValue('custom_range', {'f1':self.f1.value(),'f2':self.f2.value(),'f3':self.f3.value()})

	def updateFrameRange( self ):
		if self.rangeinput.value() == 'global':
			frange = nuke.root().frameRange()
			self.f1.setValue(frange.first())
			self.f2.setValue(frange.last())
			self.f3.setValue(frange.increment())
		elif self.rangeinput.value() == 'input':
			if self.input_node:	
				frange = self.input_node.frameRange()
				self.f1.setValue(frange.first())
				self.f2.setValue(frange.last())
				self.f3.setValue(frange.increment())
		else:
			frame_range = self._state.getValue('%s_range' % self.rangeinput.value())
			if frame_range:
				self.f1.setValue(frame_range.get('f1'))
				self.f2.setValue(frame_range.get('f2'))
				self.f3.setValue(frame_range.get('f3'))
			
	def updateScriptMode( self ):	
		if self.mode.value() == "Render Target Script File":
			self.tscript.setEnabled(True)
			self.project.setEnabled(False)
		elif self.mode.value() == "Copy Script to Shared Folder and Render":
			self.tscript.setEnabled(False)
			self.project.setEnabled(True)
		else:
			self.tscript.setEnabled(False)
			self.project.setEnabled(False)
						
	def updateAssignment( self ):	
		if self.assignto.value() == "Listed Clients":
			self.clients.setEnabled(True)
			self.selclients.setEnabled(True)
			self.groups.setEnabled(False)
			self.selgroups.setEnabled(False)
		elif self.assignto.value() == "Clients from Listed Groups":
			self.clients.setEnabled(False)
			self.selclients.setEnabled(False)
			self.groups.setEnabled(True)
			self.selgroups.setEnabled(True)
		else:
			self.clients.setEnabled(False)
			self.selclients.setEnabled(False)
			self.groups.setEnabled(False)
			self.selgroups.setEnabled(False)
				
	def showModalDialog( self ):
		result = nukescripts.PythonPanel.showModalDialog( self )			
		if result:
			nuke.knob('root.hqcfg', pickle.dumps(self._state._state)) # store knobs inside script
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
			frames_per_job = self.fperjob.value()
			nuke_exec = self.nukexec.value()
			
			out_nodes = ""
			if self.hqmode == 'selected':
				for n in nuke.selectedNodes():
					out_nodes += "%s;" % n.name()
			
			curScript = nuke.toNode("root").name()
			if self.mode.value() == "Render Current Script File":
				if self.savescript.value():
					if curScript in ["", "Root"]:
						# TODO: save untitled script into HQ shared folder. May be hidden...
						import uuid
						uid = uuid.uuid1()
						curScript = "/tmp/_nuke_tempScript_%s.nk" % uid
						
					nuke.scriptSaveAs(curScript)	
			elif self.mode.value() == "Render Target Script File":
				curScript = self.tscript.value()
				
			job_frames = []
			for frame in range(f1, f2, frames_per_job):
				job = {}
				job['name'] = 'Rendering %s script in range %d-%dx%d' % (curScript, frame, frame + frames_per_job - 1, f3)
				job['shell'] ='bash'
				job['command'] = '%s -t ~/.nuke/python/hqrender/render.py -s %d -e %d -i %d %s' % (nuke_exec, frame, frame + frames_per_job - 1, f3, curScript)  
				job['environment'] = {
					"HQ_NK_NODES"	: out_nodes,
					"HQ_NK_MODE"	: self.hqmode,
					"HQ_NK_PATHS"	: pickle.dumps(self._state.getValue('cross_path',[])),
					"HQ_NK_OS"		: OS_ENV
				}
				job['triesLeft'] = self._state.getValue('triesLeft',2)
				job_frames += [job]
			
			job_spec = {
				'name'	:	'Nuke script %s by %s' % (curScript, os.environ.get('USERNAME')),
				'priority': self.priority.value(),
				'conditions' : [],
				'children': job_frames
			}
			if self.assignto.value() == 1:
				job_spec['conditions'] += [{"type" : "client", "name":"hostname", "op":"any", "value":self.clients.value()}]
			elif self.assignto.value() == 2:
				job_spec['conditions'] += [{"type" : "client", "name":"group", "op":"==", "value":self.groups.value()}]
			job_id = hq_server.newjob(job_spec)						
