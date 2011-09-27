from hqdialog import *

def renderSelected(selected=True):
	hqdialog = HQrenderDialog('selected')
	return hqdialog.showModalDialog()	

def renderAll():
	hqdialog = HQrenderDialog('all')
	return hqdialog.showModalDialog()	
