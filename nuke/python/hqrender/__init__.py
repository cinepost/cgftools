from hqdialog import *

def renderSelected(selected=True):
	hqRender = HQrenderDialog('selected')
	return hqRender.showModalDialog()	

def renderAll():
	hqRender = HQrenderDialog('all')
	return hqRender.showModalDialog()	
