from hqdialog import *

hqRender = None

def getClientsFromServer(backstring=None):		
	global hqRender
	dialog = HQClientGroupsDialog( mode='clients', backstring=hqRender.clients )
	dialog.showModalDialog()	

def getGroupsFromServer(backstring=None):		
	global hqRender
	dialog = HQClientGroupsDialog( mode='groups', backstring=hqRender.groups )
	dialog.showModalDialog()	

def renderSelected(selected=True):
	global hqRender
	hqRender = HQrenderDialog('selected')
	return hqRender.showModalDialog()	

def renderAll():
	global hqRender 
	hqRender = HQrenderDialog('all')
	return hqRender.showModalDialog()	
