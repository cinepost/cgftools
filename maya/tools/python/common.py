import os
import sip
import cPickle
import maya.cmds as mc
import maya.OpenMayaUI as mui

def pyToAttr(objAttr, data):
	obj, attr = objAttr.split('.')
	if not mc.objExists(objAttr):
		mc.addAttr(obj, longName=attr, dataType='string')

	if mc.getAttr(objAttr, type=True) != 'string':
		raise Exception("Object '%s' already has an attribute called '%s', but it isn't type 'string'"%(obj,attr))

	stringData = cPickle.dumps(data)
	mc.setAttr(objAttr, edit=True, lock=False)
	mc.setAttr(objAttr, stringData, type='string')
	mc.setAttr(objAttr, edit=True, lock=True)

def attrToPy(objAttr):
	stringAttrData = str(mc.getAttr(objAttr))
	loadedData = cPickle.loads(stringAttrData)

	return loadedData

def getMayaWindow():
	'Get the maya main window as a QMainWindow instance'
	ptr = mui.MQtUtil.mainWindow()
	return sip.wrapinstance(long(ptr), QtCore.QObject)
