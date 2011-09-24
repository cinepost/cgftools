import hqrender

render = nuke.menu("Nuke").findItem("Render")
render.addSeparator();
render.addCommand("HQrender All...","hqrender.renderAll()","^f5")
render.addCommand("HQrender Selected...","hqrender.renderSelected()","^f7")
