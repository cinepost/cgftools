0707070000010000000006660000000000000000010000001165545724600000700000000237.start fplayback -i on -r off -f 1 -e on -h on -t on -a on -k on -s 1
tcur 0
fps 24
tset 0 10
frange 1 240
unitlength 1
unitmass 1
prompt '`strcat(oppwf(), " -> ")`'
0707070000010000000006660000000000000000010000001165545724600001300000000700.variables set -g ACTIVETAKE = 'Main'
set -g E = '2.7182818284590452354'
set -g EYE = 'stereo'
set -g HIP = '/home/max/dev/cgftools/houdini/pbSolver/scenes'
set -g HIPNAME = 'subdata_02.hip'
set -g JOB = '/opt/hfs11.1.67'
set -g PI = '3.1415926535897932384'
set -g POSE = '/home/max/houdini11.1/poselib'
set -g _HIP_SAVEPLATFORM = 'linux-x86_64-gcc4.4'
set -g _HIP_SAVETIME = 'Sun Nov  6 14:17:10 2011'
set -g _HIP_SAVEVERSION = '11.1.67'
set -g status = '0'
0707070000010000000006660000000000000000010000001165545724600001100000000267.aliases alias cd 'opcf'
alias cp 'opcp'
alias h 'history'
alias hython 'python'
alias ls 'opls'
alias matramp 'opramp'
alias opcd 'opcf'
alias oppwd 'oppwf'
alias pwd 'oppwf'
alias rm 'oprm'
0707070000010000000006660000000000000000010000001165545724600001400000000021.takeconfig takename -g take
0707070000010000000006660000000000000000010000001165545724600001500000000000.hou.session 0707070000010000000006660000000000000000010000001165545724600001500000000000.OPlibraries 0707070000010000000006660000000000000000010000001165545724600001700000000000.OPpreferences 0707070000010000000006660000000000000000010000001165545724600002000000000012expression.func # 0 bytes
0707070000010000000006660000000000000000010000001165545724600001000000000557vex.def comment ""
position 5.5 2.5
cachesize 1
flags =  lock off model off template off footprint off xray off bypass off display off render off highlight off unload off savedata off compress off exposed on
inputs
{
}
stat
{
  create 1320573886
  modify 1320573887
  author max@ws09-home
  access 0777
}
color UT_Color RGB 0.8 0.8 0.8 
delscript ""
exprlanguage hscript
end
0707070000010000000006660000000000000000010000001165545724600001100000000020vex.parm {
version 0.8
}
0707070000010000000006660000000000000000010000001165545724600001000000000551obj.def comment ""
position 5 2
cachesize 1
flags =  lock off model off template off footprint off xray off bypass off display on render on highlight off unload off savedata off compress off exposed on
inputs
{
}
stat
{
  create 1320573881
  modify 1320574148
  author max@ws09-home
  access 0777
}
color UT_Color RGB 0.8 0.8 0.8 
delscript ""
exprlanguage hscript
end
0707070000010000000006660000000000000000010000001165545724600001100000000020obj.parm {
version 0.8
}
0707070000010000000006660000000000000000010000001165545724600002100000000035obj/dopnet1.init type = dopnet
matchesdef = 0
0707070000010000000006660000000000000000010000001165545724600002000000000735obj/dopnet1.def objflags objflags =  origin off
pretransform UT_DMatrix4 1 0 0 0 0 1 0 0 0 0 1 0 0 0 0 1
comment ""
position 1.25285 3.72938
cachesize 1
flags =  lock off model off template off footprint off xray off bypass off display on render off highlight off unload off savedata off compress off exposed on selectable on
inputs
{
}
stat
{
  create 1320574148
  modify 1320574180
  author max@ws09-home
  access 0777
}
color UT_Color RGB 0.8 0.8 0.8 
delscript ""
exprlanguage hscript
end
0707070000010000000006660000000000000000010000001165545724600002000000000521obj/dopnet1.chn {
    channel timestep {
      lefttype = extend
      righttype = extend
      flags = 0
      segment { length = 0 expr = ch(\"timescale\")/(ch(\"substep\")*$FPS) }
    }
    channel timeoffset {
      lefttype = extend
      righttype = extend
      flags = 0
      segment { length = 0 expr = (ch(\"startframe\")-1)/$FPS }
    }
  }
0707070000010000000006660000000000000000010000001165545724600002100000003151obj/dopnet1.parm {
version 0.8
stdswitcher	[ 0	locks=0 ]	(	0	0	)
resimulate	[ 0	locks=0 ]	(	0	)
tdisplay	[ 0	locks=0 ]	(	"off"	)
display	[ 0	locks=0 ]	(	1	)
initialstate	[ 0	locks=0 ]	(	""	)
timestep	[ 0	locks=0 ]	(	[ timestep	0.0416666679 ] 	)
substep	[ 0	locks=0 ]	(	1	)
timeoffset	[ 0	locks=0 ]	(	[ timeoffset	0 ] 	)
startframe	[ 0	locks=0 ]	(	1	)
timescale	[ 0	locks=0 ]	(	1	)
maxfeedback	[ 0	locks=0 ]	(	2	)
autoresim	[ 0	locks=0 ]	(	"on"	)
datahints	[ 0	locks=0 ]	(	"on"	)
interpolate	[ 0	locks=0 ]	(	"on"	)
cacheenabled	[ 0	locks=0 ]	(	"on"	)
compresssims	[ 0	locks=0 ]	(	"off"	)
cachemaxsize	[ 0	locks=0 ]	(	500	)
cachetodisk	[ 0	locks=0 ]	(	"on"	)
cachetodisknoninteractive	[ 0	locks=0 ]	(	"off"	)
cachesubsteps	[ 0	locks=0 ]	(	"off"	)
timeless	[ 0	locks=0 ]	(	"off"	)
explicitcache	[ 0	locks=0 ]	(	"off"	)
explicitcachename	[ 0	locks=0 ]	(	$HIP/cache_${OS}_$SF.sim	)
explicitcachensteps	[ 0	locks=0 ]	(	0	)
keeppos	[ 0	locks=0 ]	(	"off"	)
pre_xform	[ 0	locks=0 ]	(	0	)
xOrd	[ 0	locks=0 ]	(	"srt"	)
rOrd	[ 0	locks=0 ]	(	"xyz"	)
t	[ 0	locks=0 ]	(	0	0	0	)
r	[ 0	locks=0 ]	(	0	0	0	)
s	[ 0	locks=0 ]	(	1	1	1	)
p	[ 0	locks=0 ]	(	0	0	0	)
scale	[ 0	locks=0 ]	(	1	)
lookatpath	[ 0	locks=0 ]	(	""	)
lookup	[ 0	locks=0 ]	(	on	)
pathobjpath	[ 0	locks=0 ]	(	""	)
roll	[ 0	locks=0 ]	(	0	)
pos	[ 0	locks=0 ]	(	0	)
uparmtype	[ 0	locks=0 ]	(	"arc"	)
pathorient	[ 0	locks=0 ]	(	1	)
up	[ 0	locks=0 ]	(	0	1	0	)
bank	[ 0	locks=0 ]	(	0	)
forceresim	[ 0	locks=0 ]	(	"off"	)
displayfilter	[ 0	locks=0 ]	(	*	)
use_dcolor	[ 0	locks=0 ]	(	"off"	)
dcolor	[ 0	locks=0 ]	(	1	1	1	)
picking	[ 0	locks=0 ]	(	"on"	)
pickscript	[ 0	locks=0 ]	(	""	)
caching	[ 0	locks=0 ]	(	"off"	)
}
0707070000010000000006660000000000000000010000001165545724600003600000000042obj/dopnet1/emptyobject1.init type = emptyobject
matchesdef = 0
0707070000010000000006660000000000000000010000001165545724600003500000000570obj/dopnet1/emptyobject1.def comment ""
position 0.164612 3.99997
cachesize 1
flags =  lock off model off template off footprint off xray off bypass off display off render off highlight off unload off savedata off compress off exposed on
inputs
{
}
stat
{
  create 1320574155
  modify 1320574180
  author max@ws09-home
  access 0777
}
color UT_Color RGB 0.8 0.8 0.8 
delscript ""
exprlanguage hscript
end
0707070000010000000006660000000000000000010000001165545724600003500000000263obj/dopnet1/emptyobject1.chn {
    channel activation {
      lefttype = extend
      righttype = extend
      default = 1
      flags = 0
      segment { length = 0 value = 1 1 expr = "$ST == 0" }
    }
  }
0707070000010000000006660000000000000000010000001165545724600003600000000314obj/dopnet1/emptyobject1.parm {
version 0.8
activation	[ 8	locks=0 ]	(	[ activation	1 ] 	)
activationrules	[ 0	locks=0 ]	(	0	)
objname	[ 0	locks=0 ]	(	obj$OBJ	)
numobjects	[ 0	locks=0 ]	(	1	)
solvefirstframe	[ 0	locks=0 ]	(	"off"	)
}
0707070000010000000006660000000000000000010000001165545724600004300000000047obj/dopnet1/physbam_geometry1.init type = physbam_geometry
matchesdef = 0
0707070000010000000006660000000000000000010000001165545724600004200000000612obj/dopnet1/physbam_geometry1.def comment ""
position 1.35285 2.38821
cachesize 1
flags =  lock off model off template off footprint off xray off bypass off display on render off highlight off unload off savedata off compress off exposed on
inputs
{
0 	emptyobject1 0 1
}
stat
{
  create 1320574169
  modify 1320574180
  author max@ws09-home
  access 0777
}
color UT_Color RGB 0.8 0.8 0.8 
delscript ""
exprlanguage hscript
end
0707070000010000000006660000000000000000010000001165545724600004200000000263obj/dopnet1/physbam_geometry1.chn {
    channel activation {
      lefttype = extend
      righttype = extend
      default = 1
      flags = 0
      segment { length = 0 value = 1 1 expr = constant() }
    }
  }
0707070000010000000006660000000000000000010000001165545724600004300000000535obj/dopnet1/physbam_geometry1.parm {
version 0.8
parmop_mass	[ 0	locks=0 ]	(	"default"	)
mass	[ 0	locks=0 ]	(	0	)
defaultparmop	[ 0	locks=0 ]	(	"initial"	)
sharedata	[ 0	locks=0 ]	(	"off"	)
activation	[ 0	locks=0 ]	(	[ activation	1 ] 	)
activationrules	[ 0	locks=0 ]	(	0	)
group	[ 0	locks=0 ]	(	*	)
dataname	[ 0	locks=0 ]	(	PhysBAM_Geometry	)
uniquedataname	[ 0	locks=0 ]	(	"off"	)
}
0707070000010000000006660000000000000000010000001165545724600002200000000041obj/dopnet1.order 2
emptyobject1
physbam_geometry1
0707070000010000000006660000000000000000010000001165545724600002000000000002obj/dopnet1.net 1
0707070000010000000006660000000000000000010000001165545724600001000000000002obj.net 1
0707070000010000000006660000000000000000010000001165545724600001000000000555out.def comment ""
position 5 0.5
cachesize 1
flags =  lock off model off template off footprint off xray off bypass off display off render off highlight off unload off savedata off compress off exposed on
inputs
{
}
stat
{
  create 1320573885
  modify 1320573886
  author max@ws09-home
  access 0777
}
color UT_Color RGB 0.8 0.8 0.8 
delscript ""
exprlanguage hscript
end
0707070000010000000006660000000000000000010000001165545724600001100000000020out.parm {
version 0.8
}
0707070000010000000006660000000000000000010000001165545724600001000000000002out.net 1
0707070000010000000006660000000000000000010000001165545724600001100000000557part.def comment ""
position 5.5 1.5
cachesize 1
flags =  lock off model off template off footprint off xray off bypass off display off render off highlight off unload off savedata off compress off exposed on
inputs
{
}
stat
{
  create 1320573885
  modify 1320573886
  author max@ws09-home
  access 0777
}
color UT_Color RGB 0.8 0.8 0.8 
delscript ""
exprlanguage hscript
end
0707070000010000000006660000000000000000010000001165545724600001200000000020part.parm {
version 0.8
}
0707070000010000000006660000000000000000010000001165545724600001100000000002part.net 1
0707070000010000000006660000000000000000010000001165545724600000700000000555ch.def comment ""
position 4.5 1
cachesize 1
flags =  lock off model off template off footprint off xray off bypass off display off render off highlight off unload off savedata off compress off exposed on
inputs
{
}
stat
{
  create 1320573885
  modify 1320573886
  author max@ws09-home
  access 0777
}
color UT_Color RGB 0.8 0.8 0.8 
delscript ""
exprlanguage hscript
end
0707070000010000000006660000000000000000010000001165545724600001000000000020ch.parm {
version 0.8
}
0707070000010000000006660000000000000000010000001165545724600000700000000002ch.net 1
0707070000010000000006660000000000000000010000001165545724600001100000000553shop.def comment ""
position 5 3
cachesize 1
flags =  lock off model off template off footprint off xray off bypass off display off render off highlight off unload off savedata off compress off exposed on
inputs
{
}
stat
{
  create 1320573885
  modify 1320573886
  author max@ws09-home
  access 0777
}
color UT_Color RGB 0.8 0.8 0.8 
delscript ""
exprlanguage hscript
end
0707070000010000000006660000000000000000010000001165545724600001200000000020shop.parm {
version 0.8
}
0707070000010000000006660000000000000000010000001165545724600001100000000002shop.net 1
0707070000010000000006660000000000000000010000001165545724600001000000000557img.def comment ""
position 4.5 2.5
cachesize 1
flags =  lock off model off template off footprint off xray off bypass off display off render off highlight off unload off savedata off compress off exposed on
inputs
{
}
stat
{
  create 1320573886
  modify 1320573890
  author max@ws09-home
  access 0777
}
color UT_Color RGB 0.8 0.8 0.8 
delscript ""
exprlanguage hscript
end
0707070000010000000006660000000000000000010000001165545724600001100000000020img.parm {
version 0.8
}
0707070000010000000006660000000000000000010000001165545724600001700000000032img/comp1.init type = img
matchesdef = 0
0707070000010000000006660000000000000000010000001165545724600001600000000553img/comp1.def comment ""
position 0 0
cachesize 1
flags =  lock off model off template off footprint off xray off bypass off display off render off highlight off unload off savedata off compress off exposed on
inputs
{
}
stat
{
  create 1320573890
  modify 1320573890
  author max@ws09-home
  access 0777
}
color UT_Color RGB 0.8 0.8 0.8 
delscript ""
exprlanguage hscript
end
0707070000010000000006660000000000000000010000001165545724600001700000000020img/comp1.parm {
version 0.8
}
0707070000010000000006660000000000000000010000001165545724600001600000000002img/comp1.net 1
0707070000010000000006660000000000000000010000001165545724600001000000000002img.net 1
0707070000010000000006660000000000000000010000001165545724600001000000000002vex.net 1
0707070000010000000006660000000000000000010000001165545724600002400000000000.userdisplayoptions 0707070000010000000006660000000000000000010000001165545724600001500000046065.application neteditor -d Build -b 0 -l 0 -f '' -q 1.000000 -v /obj/dopnet1 0  0 1 panetab7
neteditor -d Build -b 0 -l 0 -f '' -q 1.000000 -v /obj -0.017647  -0.0117648 1 panetab7
imgdispopt -m 10 -P on -M on -A on -g 1 -G "C Left Right" -B 1 -v split -V 2 2 -d h -a off -i 2 -b 0 0 0 -o "" -h on -H on -c 0.8 0.8 0.8 -C 1 1 1 -S 1 1 0 -R 6 -F dc -D "" -I "" -f "" -s off -r 256 256 -p on -l off
imgview -z 1 -p (none) 0 -c 0 -l on -s off -i imgview2
imgview2d  -V on -L on -G on -H on -P off -m 0 -p 0 1 -b 1 1 0 -g 1 -d "" -c -1 -t off -i on -X on imgview2 1
imgview2d  -V on -L on -G on -H on -P off -m 0 -p 0 1 -b 1 1 0 -g 1 -d "" -c -1 -t off -i on -X on imgview2 2
imgview2d  -V on -L on -G on -H on -P off -m 0 -p 0 1 -b 1 1 0 -g 1 -d "" -c -1 -t off -i on -X on imgview2 3
imgview2d  -V on -L on -G on -H on -P off -m 0 -p 0 1 -b 1 1 0 -g 1 -d "" -c -1 -t off -i on -X on imgview2 4
imgview2d  -V on -L on -G on -H on -P off -m 0 -p 0 1 -b 1 1 0 -g 1 -d "" -c -1 -t off -i on -X on imgview2 5
imgview2d  -V on -L on -G on -H on -P off -m 0 -p 0 1 -b 1 1 0 -g 1 -d "" -c -1 -t off -i on -X on imgview2 6
imgview2d  -V on -L on -G on -H on -P off -m 0 -p 0 1 -b 1 1 0 -g 1 -d "" -c -1 -t off -i on -X on imgview2 7
imgview2d  -V on -L on -G on -H on -P off -m 0 -p 0 1 -b 1 1 0 -g 1 -d "" -c -1 -t off -i on -X on imgview2 8
imgview2d  -V on -L on -G on -H on -P off -m 0 -p 0 1 -b 1 1 0 -g 1 -d "" -c -1 -t off -i on -X on imgview2 9
imgview2d  -V on -L on -G on -H on -P off -m 0 -p 0 1 -b 1 1 0 -g 1 -d "" -c -1 -t off -i on -X on imgview2 10
imgview2d  -V on -L on -G on -H on -P off -m 0 -p 0 1 -b 1 1 0 -g 1 -d "" -c -1 -t off -i on -X on imgview2 11
imgview2d  -V on -L on -G on -H on -P off -m 0 -p 0 1 -b 1 1 0 -g 1 -d "" -c -1 -t off -i on -X on imgview2 12
imgview2d  -V on -L on -G on -H on -P off -m 0 -p 0 1 -b 1 1 0 -g 1 -d "" -c -1 -t off -i on -X on imgview2 13
imgview2d  -V on -L on -G on -H on -P off -m 0 -p 0 1 -b 1 1 0 -g 1 -d "" -c -1 -t off -i on -X on imgview2 14
imgview2d  -V on -L on -G on -H on -P off -m 0 -p 0 1 -b 1 1 0 -g 1 -d "" -c -1 -t off -i on -X on imgview2 15
imgview2d  -V on -L on -G on -H on -P off -m 0 -p 0 1 -b 1 1 0 -g 1 -d "" -c -1 -t off -i on -X on imgview2 16
imgviewtime -V on -L on -G on -H on -P off -r 24 -u fr -g 0 -f on -e on -b on -d imgview2
imgviewhist -V on -L on -G on -H on -P off -c -1 -h 0 -i on imgview2 1
imgviewhist -V on -L on -G on -H on -P off -c -1 -h 0 -i on imgview2 2
imgviewhist -V on -L on -G on -H on -P off -c -1 -h 0 -i on imgview2 3
imgviewhist -V on -L on -G on -H on -P off -c -1 -h 0 -i on imgview2 4
imgviewhist -V on -L on -G on -H on -P off -c -1 -h 0 -i on imgview2 5
imgviewhist -V on -L on -G on -H on -P off -c -1 -h 0 -i on imgview2 6
imgviewhist -V on -L on -G on -H on -P off -c -1 -h 0 -i on imgview2 7
imgviewhist -V on -L on -G on -H on -P off -c -1 -h 0 -i on imgview2 8
imgviewhist -V on -L on -G on -H on -P off -c -1 -h 0 -i on imgview2 9
imgviewhist -V on -L on -G on -H on -P off -c -1 -h 0 -i on imgview2 10
imgviewhist -V on -L on -G on -H on -P off -c -1 -h 0 -i on imgview2 11
imgviewhist -V on -L on -G on -H on -P off -c -1 -h 0 -i on imgview2 12
imgviewhist -V on -L on -G on -H on -P off -c -1 -h 0 -i on imgview2 13
imgviewhist -V on -L on -G on -H on -P off -c -1 -h 0 -i on imgview2 14
imgviewhist -V on -L on -G on -H on -P off -c -1 -h 0 -i on imgview2 15
imgviewhist -V on -L on -G on -H on -P off -c -1 -h 0 -i on imgview2 16
viewerstow -b open -x open -l open -m hide -c hide -d open Build.panetab1.world
vieweroption -a 2 -h 0 -m 1 -r 'View: Mantra' Build.panetab1.world
viewlayout -s 2 Build.panetab1.world
viewtype -t perspective Build.panetab1.world.`vpname("Build.panetab1.world",2)`
viewtype -t ortho_top Build.panetab1.world.`vpname("Build.panetab1.world",1)`
viewtype -t ortho_front Build.panetab1.world.`vpname("Build.panetab1.world",3)`
viewtype -t ortho_right Build.panetab1.world.`vpname("Build.panetab1.world",4)`
viewname -l on -n aaaa3 Build.panetab1.world.`vpname("Build.panetab1.world",2)`
viewname -l on -n aaaa2 Build.panetab1.world.`vpname("Build.panetab1.world",1)`
viewname -l on -n aaaa1 Build.panetab1.world.`vpname("Build.panetab1.world",3)`
viewname -l on -n aaaa0 Build.panetab1.world.`vpname("Build.panetab1.world",4)`
viewname -l off -n persp1 Build.panetab1.world.`vpname("Build.panetab1.world",2)`
viewname -l off -n top1 Build.panetab1.world.`vpname("Build.panetab1.world",1)`
viewname -l off -n front1 Build.panetab1.world.`vpname("Build.panetab1.world",3)`
viewname -l off -n right1 Build.panetab1.world.`vpname("Build.panetab1.world",4)`
viewdisplay -a on Build.panetab1.world.persp1
viewdisplay -M unsel shade -N unsel n off -E unsel n off -I unsel n off Build.panetab1.world.persp1
viewdisplay -N unsel t off -E unsel t off -I unsel t on Build.panetab1.world.persp1
viewdisplay -N unsel l off -E unsel g off -I unsel l off Build.panetab1.world.persp1
viewdisplay -N unsel p off -I unsel p off Build.panetab1.world.persp1
viewdisplay -N unsel m off -I unsel h off Build.panetab1.world.persp1
viewdisplay -I unsel b off Build.panetab1.world.persp1
viewdisplay -I unsel f off Build.panetab1.world.persp1
viewdisplay -M templ wire -N templ n off -E templ n off -I templ n off Build.panetab1.world.persp1
viewdisplay -N templ t off -E templ t off -I templ t off Build.panetab1.world.persp1
viewdisplay -N templ l off -E templ g off -I templ l off Build.panetab1.world.persp1
viewdisplay -N templ p off -I templ p off Build.panetab1.world.persp1
viewdisplay -N templ m off -I templ h off Build.panetab1.world.persp1
viewdisplay -I templ b off Build.panetab1.world.persp1
viewdisplay -I templ f off Build.panetab1.world.persp1
viewdisplay -M curr_unsel wire -N curr_unsel n off -E curr_unsel n off -I curr_unsel n off Build.panetab1.world.persp1
viewdisplay -N curr_unsel t off -E curr_unsel t off -I curr_unsel t on Build.panetab1.world.persp1
viewdisplay -N curr_unsel l off -E curr_unsel g off -I curr_unsel l off Build.panetab1.world.persp1
viewdisplay -N curr_unsel p off -I curr_unsel p off Build.panetab1.world.persp1
viewdisplay -N curr_unsel m off -I curr_unsel h off Build.panetab1.world.persp1
viewdisplay -I curr_unsel b off Build.panetab1.world.persp1
viewdisplay -I curr_unsel f off Build.panetab1.world.persp1
viewdisplay -A templ on -B bw -b off -d off -C 0 -e 1 -F off -f off -g on -G '*' -h off -H on -j off -J off Build.panetab1.world.persp1
viewdisplay -A curr_all on -l 1 -L on -n 0.2 -o on -O on -q on -r on -s off -S on -t on Build.panetab1.world.persp1
viewdisplay -V 2 -z off -y off -x off -X off -U 4 -Y 4 -T on -R on -i on -D on -k off Build.panetab1.world.persp1
viewdisplay -Q on -m on -v on Build.panetab1.world.persp1
viewdisplay -W  off bound_box_wire 0 Build.panetab1.world.persp1
viewdisplay -c off -K on -P points -p 3 Build.panetab1.world.persp1
vieweffect -a 0 -A 4 -b on -B 0 -d on -D on off -g 1 -h 1 -H off -L off -l '' -m on -M 512 -o off -n 8 -O 2 -q 1 -Q 2 -r 1024 1024 -s 1 -S on -t 100 -T 100 -u off -U off -v 128 128 128 -w 0.05 -W off Build.panetab1.world.persp1
viewbackground -b off -q 1 -S file -F '' -O off -r 1 1 -C '' -p '(Default Color)' 0 -t on -a on -o 0 0 -s 1 1 -i off -m 0 0 -M 1 1 Build.panetab1.world.persp1
viewtransform Build.panetab1.world.persp1 version ( 3 )  flag ( F )  orthowidth ( 25.1416 )  focal ( 50 )  aperture ( 41.4214 )  window ( 0 1 0 1 )  winroll ( 0 )  clear ( 1 )  nearfar ( 0.1 32025 )  resxy ( 1 1 )  aspect ( 1.33333 )  pixelaspect ( 1 )  txyz ( 0 0 24.9722 )  pxyz ( 0 0 0 )  rotation ( 0.707107 -0.353553 0.612373 0 0.866025 0.5 -0.707107 -0.353553 0.612372  )  homedistance ( 24.9722 )  homeadjboxwidth ( 12.5708 )  homeorthowidth ( 25.1416 )  homeradius ( 32 )  homeaxis ( 0 )  homecustom ( 0.707107 -0.353553 0.612373 0 0.866025 0.5 -0.707107 -0.353553 0.612372  )  adjustlimits ( 3 )  
viewcamera -c '' Build.panetab1.world.persp1
vieworthogrid -d on -o 0 0 0 -s 0.5 0.5 -r 8 8 Build.panetab1.world.persp1
viewuvgrid -r off -s 0.1 0.1 -P on -S 1 1 -O 0 0 -C on -t on Build.panetab1.world.persp1
viewdisplay -a on Build.panetab1.world.top1
viewdisplay -M unsel shade -N unsel n off -E unsel n off -I unsel n off Build.panetab1.world.top1
viewdisplay -N unsel t off -E unsel t off -I unsel t on Build.panetab1.world.top1
viewdisplay -N unsel l off -E unsel g off -I unsel l off Build.panetab1.world.top1
viewdisplay -N unsel p off -I unsel p off Build.panetab1.world.top1
viewdisplay -N unsel m off -I unsel h off Build.panetab1.world.top1
viewdisplay -I unsel b off Build.panetab1.world.top1
viewdisplay -I unsel f off Build.panetab1.world.top1
viewdisplay -M templ wire -N templ n off -E templ n off -I templ n off Build.panetab1.world.top1
viewdisplay -N templ t off -E templ t off -I templ t off Build.panetab1.world.top1
viewdisplay -N templ l off -E templ g off -I templ l off Build.panetab1.world.top1
viewdisplay -N templ p off -I templ p off Build.panetab1.world.top1
viewdisplay -N templ m off -I templ h off Build.panetab1.world.top1
viewdisplay -I templ b off Build.panetab1.world.top1
viewdisplay -I templ f off Build.panetab1.world.top1
viewdisplay -M curr_unsel wire -N curr_unsel n off -E curr_unsel n off -I curr_unsel n off Build.panetab1.world.top1
viewdisplay -N curr_unsel t off -E curr_unsel t off -I curr_unsel t on Build.panetab1.world.top1
viewdisplay -N curr_unsel l off -E curr_unsel g off -I curr_unsel l off Build.panetab1.world.top1
viewdisplay -N curr_unsel p off -I curr_unsel p off Build.panetab1.world.top1
viewdisplay -N curr_unsel m off -I curr_unsel h off Build.panetab1.world.top1
viewdisplay -I curr_unsel b off Build.panetab1.world.top1
viewdisplay -I curr_unsel f off Build.panetab1.world.top1
viewdisplay -A templ on -B bw -b off -d off -C 0 -e 1 -F off -f off -g on -G '*' -h off -H on -j off -J off Build.panetab1.world.top1
viewdisplay -A curr_all on -l 1 -L on -n 0.2 -o on -O on -q on -r on -s off -S on -t on Build.panetab1.world.top1
viewdisplay -V 2 -z off -y off -x off -X off -U 4 -Y 4 -T on -R on -i on -D on -k off Build.panetab1.world.top1
viewdisplay -Q on -m on -v on Build.panetab1.world.top1
viewdisplay -W  off bound_box_wire 0 Build.panetab1.world.top1
viewdisplay -c off -K on -P points -p 3 Build.panetab1.world.top1
vieweffect -a 0 -A 4 -b on -B 0 -d on -D on off -g 1 -h 1 -H off -L off -l '' -m on -M 512 -o off -n 8 -O 2 -q 1 -Q 2 -r 1024 1024 -s 1 -S on -t 100 -T 100 -u off -U off -v 128 128 128 -w 0.05 -W off Build.panetab1.world.top1
viewbackground -b off -q 1 -S file -F '' -O off -r 1 1 -C '' -p '(Default Color)' 0 -t on -a on -o 0 0 -s 1 1 -i off -m 0 0 -M 1 1 Build.panetab1.world.top1
viewtransform Build.panetab1.world.top1 version ( 3 )  flag ( Fo )  orthowidth ( 2.22222 )  focal ( 50 )  aperture ( 41.4214 )  window ( 0 1 0 1 )  winroll ( 0 )  clear ( 1 )  nearfar ( 0.1 3467.78 )  resxy ( 1 1 )  aspect ( 1.33333 )  pixelaspect ( 1 )  txyz ( 0 0 3.68246 )  pxyz ( 0 0 0 )  rotation ( 1 0 0 0 7.54979e-08 1 0 -1 7.54979e-08  )  homedistance ( 3.68246 )  homeadjboxwidth ( 2.22222 )  homeorthowidth ( 2.22222 )  homeradius ( 3.4641 )  homeaxis ( 0 )  homecustom ( 0.707107 -0.353553 0.612373 0 0.866025 0.5 -0.707107 -0.353553 0.612372  )  adjustlimits ( 3 )  
vieworthogrid -d on -o 0 0 0 -s 0.5 0.5 -r 8 8 Build.panetab1.world.top1
viewuvgrid -r off -s 0.1 0.1 -P on -S 1 1 -O 0 0 -C on -t on Build.panetab1.world.top1
viewdisplay -a on Build.panetab1.world.front1
viewdisplay -M unsel shade -N unsel n off -E unsel n off -I unsel n off Build.panetab1.world.front1
viewdisplay -N unsel t off -E unsel t off -I unsel t on Build.panetab1.world.front1
viewdisplay -N unsel l off -E unsel g off -I unsel l off Build.panetab1.world.front1
viewdisplay -N unsel p off -I unsel p off Build.panetab1.world.front1
viewdisplay -N unsel m off -I unsel h off Build.panetab1.world.front1
viewdisplay -I unsel b off Build.panetab1.world.front1
viewdisplay -I unsel f off Build.panetab1.world.front1
viewdisplay -M templ wire -N templ n off -E templ n off -I templ n off Build.panetab1.world.front1
viewdisplay -N templ t off -E templ t off -I templ t off Build.panetab1.world.front1
viewdisplay -N templ l off -E templ g off -I templ l off Build.panetab1.world.front1
viewdisplay -N templ p off -I templ p off Build.panetab1.world.front1
viewdisplay -N templ m off -I templ h off Build.panetab1.world.front1
viewdisplay -I templ b off Build.panetab1.world.front1
viewdisplay -I templ f off Build.panetab1.world.front1
viewdisplay -M curr_unsel wire -N curr_unsel n off -E curr_unsel n off -I curr_unsel n off Build.panetab1.world.front1
viewdisplay -N curr_unsel t off -E curr_unsel t off -I curr_unsel t on Build.panetab1.world.front1
viewdisplay -N curr_unsel l off -E curr_unsel g off -I curr_unsel l off Build.panetab1.world.front1
viewdisplay -N curr_unsel p off -I curr_unsel p off Build.panetab1.world.front1
viewdisplay -N curr_unsel m off -I curr_unsel h off Build.panetab1.world.front1
viewdisplay -I curr_unsel b off Build.panetab1.world.front1
viewdisplay -I curr_unsel f off Build.panetab1.world.front1
viewdisplay -A templ on -B bw -b off -d off -C 0 -e 1 -F off -f off -g on -G '*' -h off -H on -j off -J off Build.panetab1.world.front1
viewdisplay -A curr_all on -l 1 -L on -n 0.2 -o on -O on -q on -r on -s off -S on -t on Build.panetab1.world.front1
viewdisplay -V 2 -z off -y off -x off -X off -U 4 -Y 4 -T on -R on -i on -D on -k off Build.panetab1.world.front1
viewdisplay -Q on -m on -v on Build.panetab1.world.front1
viewdisplay -W  off bound_box_wire 0 Build.panetab1.world.front1
viewdisplay -c off -K on -P points -p 3 Build.panetab1.world.front1
vieweffect -a 0 -A 4 -b on -B 0 -d on -D on off -g 1 -h 1 -H off -L off -l '' -m on -M 512 -o off -n 8 -O 2 -q 1 -Q 2 -r 1024 1024 -s 1 -S on -t 100 -T 100 -u off -U off -v 128 128 128 -w 0.05 -W off Build.panetab1.world.front1
viewbackground -b off -q 1 -S file -F '' -O off -r 1 1 -C '' -p '(Default Color)' 0 -t on -a on -o 0 0 -s 1 1 -i off -m 0 0 -M 1 1 Build.panetab1.world.front1
viewtransform Build.panetab1.world.front1 version ( 3 )  flag ( Fo )  orthowidth ( 2.22222 )  focal ( 50 )  aperture ( 41.4214 )  window ( 0 1 0 1 )  winroll ( 0 )  clear ( 1 )  nearfar ( 0.1 3467.78 )  resxy ( 1 1 )  aspect ( 1.33333 )  pixelaspect ( 1 )  txyz ( 0 0 3.68246 )  pxyz ( 0 0 0 )  rotation ( 1 0 0 0 1 0 0 0 1  )  homedistance ( 3.68246 )  homeadjboxwidth ( 2.22222 )  homeorthowidth ( 2.22222 )  homeradius ( 3.4641 )  homeaxis ( 0 )  homecustom ( 0.707107 -0.353553 0.612373 0 0.866025 0.5 -0.707107 -0.353553 0.612372  )  adjustlimits ( 3 )  
vieworthogrid -d on -o 0 0 0 -s 0.5 0.5 -r 8 8 Build.panetab1.world.front1
viewuvgrid -r off -s 0.1 0.1 -P on -S 1 1 -O 0 0 -C on -t on Build.panetab1.world.front1
viewdisplay -a on Build.panetab1.world.right1
viewdisplay -M unsel shade -N unsel n off -E unsel n off -I unsel n off Build.panetab1.world.right1
viewdisplay -N unsel t off -E unsel t off -I unsel t on Build.panetab1.world.right1
viewdisplay -N unsel l off -E unsel g off -I unsel l off Build.panetab1.world.right1
viewdisplay -N unsel p off -I unsel p off Build.panetab1.world.right1
viewdisplay -N unsel m off -I unsel h off Build.panetab1.world.right1
viewdisplay -I unsel b off Build.panetab1.world.right1
viewdisplay -I unsel f off Build.panetab1.world.right1
viewdisplay -M templ wire -N templ n off -E templ n off -I templ n off Build.panetab1.world.right1
viewdisplay -N templ t off -E templ t off -I templ t off Build.panetab1.world.right1
viewdisplay -N templ l off -E templ g off -I templ l off Build.panetab1.world.right1
viewdisplay -N templ p off -I templ p off Build.panetab1.world.right1
viewdisplay -N templ m off -I templ h off Build.panetab1.world.right1
viewdisplay -I templ b off Build.panetab1.world.right1
viewdisplay -I templ f off Build.panetab1.world.right1
viewdisplay -M curr_unsel wire -N curr_unsel n off -E curr_unsel n off -I curr_unsel n off Build.panetab1.world.right1
viewdisplay -N curr_unsel t off -E curr_unsel t off -I curr_unsel t on Build.panetab1.world.right1
viewdisplay -N curr_unsel l off -E curr_unsel g off -I curr_unsel l off Build.panetab1.world.right1
viewdisplay -N curr_unsel p off -I curr_unsel p off Build.panetab1.world.right1
viewdisplay -N curr_unsel m off -I curr_unsel h off Build.panetab1.world.right1
viewdisplay -I curr_unsel b off Build.panetab1.world.right1
viewdisplay -I curr_unsel f off Build.panetab1.world.right1
viewdisplay -A templ on -B bw -b off -d off -C 0 -e 1 -F off -f off -g on -G '*' -h off -H on -j off -J off Build.panetab1.world.right1
viewdisplay -A curr_all on -l 1 -L on -n 0.2 -o on -O on -q on -r on -s off -S on -t on Build.panetab1.world.right1
viewdisplay -V 2 -z off -y off -x off -X off -U 4 -Y 4 -T on -R on -i on -D on -k off Build.panetab1.world.right1
viewdisplay -Q on -m on -v on Build.panetab1.world.right1
viewdisplay -W  off bound_box_wire 0 Build.panetab1.world.right1
viewdisplay -c off -K on -P points -p 3 Build.panetab1.world.right1
vieweffect -a 0 -A 4 -b on -B 0 -d on -D on off -g 1 -h 1 -H off -L off -l '' -m on -M 512 -o off -n 8 -O 2 -q 1 -Q 2 -r 1024 1024 -s 1 -S on -t 100 -T 100 -u off -U off -v 128 128 128 -w 0.05 -W off Build.panetab1.world.right1
viewbackground -b off -q 1 -S file -F '' -O off -r 1 1 -C '' -p '(Default Color)' 0 -t on -a on -o 0 0 -s 1 1 -i off -m 0 0 -M 1 1 Build.panetab1.world.right1
viewtransform Build.panetab1.world.right1 version ( 3 )  flag ( Fo )  orthowidth ( 2.22222 )  focal ( 50 )  aperture ( 41.4214 )  window ( 0 1 0 1 )  winroll ( 0 )  clear ( 1 )  nearfar ( 0.1 3467.78 )  resxy ( 1 1 )  aspect ( 1.33333 )  pixelaspect ( 1 )  txyz ( 0 0 3.68246 )  pxyz ( 0 0 0 )  rotation ( -4.37114e-08 0 1 0 1 0 -1 0 -4.37114e-08  )  homedistance ( 3.68246 )  homeadjboxwidth ( 2.22222 )  homeorthowidth ( 2.22222 )  homeradius ( 3.4641 )  homeaxis ( 0 )  homecustom ( 0.707107 -0.353553 0.612373 0 0.866025 0.5 -0.707107 -0.353553 0.612372  )  adjustlimits ( 3 )  
vieworthogrid -d on -o 0 0 0 -s 0.5 0.5 -r 8 8 Build.panetab1.world.right1
viewuvgrid -r off -s 0.1 0.1 -P on -S 1 1 -O 0 0 -C on -t on Build.panetab1.world.right1
viewsnapshotoption -s off -h off -a 1 -v '*' Build.panetab1.world
cplane -o -8 0 -8 -n 0 1 0 -x 0 0 1 -l n -s 0.5 0.5 -c 32 32 -r 8 8 -d on Build.panetab1.world
glcache -M 1024
panepath -d Build -f -p panetab1 /obj/dopnet1/physbam_geometry1
panepath -d Build -f -p panetab2 /obj/dopnet1/physbam_geometry1
panepath -d Build -f -p panetab3 /obj/dopnet1/physbam_geometry1
panepath -d Build -f -p panetab4 /img/comp1/
panepath -d Build -f -p panetab5 /obj/dopnet1/physbam_geometry1
panepath -d Build -f -p panetab6 /obj/dopnet1/physbam_geometry1
panepath -d Build -f -p panetab7 /obj/dopnet1/physbam_geometry1
panepath -d Build -f -p panetab8 /obj/
panepath -d Build -f -p panetab9 /obj/dopnet1/physbam_geometry1
panepath -d Build -f -p panetab10 /obj/dopnet1/physbam_geometry1
desk set Build
performance -l window -c on -g off -o on -v on -f on -m off -r on -i on -h off -p off -s off -e off -F off -U ms -T 500
viewupdate -u always
audiopanel -s p ''
audiopanel -s r off
audiopanel -s s 1
audiopanel -s f 45
audiopanel -t p stop
audiopanel -t l off
audiopanel -t r on
audiopanel -o m off
audiopanel -o t on
audiopanel -o u on
audiopanel -o l 1
audiopanel -o r 1
audiopanel -o d 0
audiopanel -r d 0.2
audiopanel -m 0
audiopanel -a ''
audiopanel -f 1
audiopanel -O 0
colorsettings -D -A -g 1 -l ""
compproject -r 640 480 -a 1 -d i8 -p 0 255 -P full -f 0 -F 0 -l on
texcache  -a off -r 2048 2048 -m 10000 -M 974
shopvisible +OGL +OGL2 +I3D +VMantra +MI +RIB
0707070000010000000006660000000000000000010000001165545724600000700000000060.takes HoudiniTakeFile 1.0
take name Main kids 0
   e
0707070000010000000006660000000000000000010000001165545724600000500000000024.cwd opcf /
takeset Main
0707070000010000000006660000000000000000010000001165545724600001600000000000.custompanels 0707070000010000000006660000000000000000010000001165545724600001300000000000TRAILER!!! 