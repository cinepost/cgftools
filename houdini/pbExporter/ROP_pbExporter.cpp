#include "ROP_pbExporter.h"

#include <fstream.h>
#include <UT/UT_DSOVersion.h>
#include <CH/CH_LocalVariable.h>
#include <PRM/PRM_Include.h>
#include <PRM/PRM_SpareData.h>
#include <OP/OP_OperatorTable.h>
#include <OP/OP_Director.h>
#include <DOP/DOP_Node.h>
#include <DOP/DOP_Parent.h>
#include <GU/GU_Detail.h>
#include <GU/GU_PrimVolume.h>
#include <SIM/SIM_Object.h>
#include <SIM/SIM_ScalarField.h>
#include <SIM/SIM_VectorField.h>
#include <SIM/SIM_MatrixField.h>
#include <ROP/ROP_Error.h>
#include <ROP/ROP_Templates.h>
#include <RBD/RBD_State.h>
#include <UT/UT_IOTable.h>

static PRM_Name         dopPathName("doppath",  "DOP Path");
static PRM_Name         theObjectName("dopobject", "DOP Object");
static PRM_Name         theDataName("dopdata", "DOP Data");
static PRM_Name         theFileName("file", "Output File");
static PRM_Default      theFileDefault(0, "$HIP/$F.bgeo");
static PRM_Name         alfprogressName("alfprogress", "Alfred Style Progress");


static PRM_Name collateName("collatevector", "Collate Vector Fields");

static PRM_Template      f3dTemplates[] = {
    PRM_Template(PRM_STRING, PRM_TYPE_DYNAMIC_PATH, 1, &dopPathName, 0, 0, 0, 0, &PRM_SpareData::dopPath),
    PRM_Template(PRM_FILE,      1, &theFileName, &theFileDefault,0, 0, 0, &PRM_SpareData::fileChooserModeWrite),
    PRM_Template(PRM_TOGGLE, 	1, &alfprogressName, PRMzeroDefaults),
    PRM_Template(PRM_STRING,    1, &theObjectName),
    PRM_Template(PRM_STRING,    1, &theDataName),
    PRM_Template()
                            
};

static PRM_Template *
getTemplates()
{
    static PRM_Template *theTemplate = 0;

    if (theTemplate)
        return theTemplate;

    theTemplate = new PRM_Template[ROP_pbExporter_MAXPARMS+1];
    theTemplate[ROP_pbExporter_RENDER] = theRopTemplates[ROP_RENDER_TPLATE];
    theTemplate[ROP_pbExporter_RENDER_CTRL] = theRopTemplates[ROP_RENDERDIALOG_TPLATE];
    theTemplate[ROP_pbExporter_TRANGE] = theRopTemplates[ROP_TRANGE_TPLATE];
    theTemplate[ROP_pbExporter_FRANGE] = theRopTemplates[ROP_FRAMERANGE_TPLATE];
    theTemplate[ROP_pbExporter_TAKE] = theRopTemplates[ROP_TAKENAME_TPLATE];
    theTemplate[ROP_pbExporter_DOPPATH] = f3dTemplates[0];
    theTemplate[ROP_pbExporter_DOPOUTPUT] = f3dTemplates[1];
    theTemplate[ROP_pbExporter_DOPOBJECT] = f3dTemplates[3];
    theTemplate[ROP_pbExporter_DOPDATA] = f3dTemplates[4];
    theTemplate[ROP_pbExporter_INITSIM] = theRopTemplates[ROP_IFD_INITSIM_TPLATE];
    theTemplate[ROP_pbExporter_ALFPROGRESS] = f3dTemplates[2];
    theTemplate[ROP_pbExporter_TPRERENDER] = theRopTemplates[ROP_TPRERENDER_TPLATE];
    theTemplate[ROP_pbExporter_PRERENDER] = theRopTemplates[ROP_PRERENDER_TPLATE];
    theTemplate[ROP_pbExporter_LPRERENDER] = theRopTemplates[ROP_LPRERENDER_TPLATE];
    theTemplate[ROP_pbExporter_TPREFRAME] = theRopTemplates[ROP_TPREFRAME_TPLATE];
    theTemplate[ROP_pbExporter_PREFRAME] = theRopTemplates[ROP_PREFRAME_TPLATE];
    theTemplate[ROP_pbExporter_LPREFRAME] = theRopTemplates[ROP_LPREFRAME_TPLATE];
    theTemplate[ROP_pbExporter_TPOSTFRAME] = theRopTemplates[ROP_TPOSTFRAME_TPLATE];
    theTemplate[ROP_pbExporter_POSTFRAME] = theRopTemplates[ROP_POSTFRAME_TPLATE];
    theTemplate[ROP_pbExporter_LPOSTFRAME] = theRopTemplates[ROP_LPOSTFRAME_TPLATE];
    theTemplate[ROP_pbExporter_TPOSTRENDER] = theRopTemplates[ROP_TPOSTRENDER_TPLATE];
    theTemplate[ROP_pbExporter_POSTRENDER] = theRopTemplates[ROP_POSTRENDER_TPLATE];
    theTemplate[ROP_pbExporter_LPOSTRENDER] = theRopTemplates[ROP_LPOSTRENDER_TPLATE];
    theTemplate[ROP_pbExporter_MAXPARMS] = PRM_Template();

    UT_ASSERT(PRM_Template::countTemplates(theTemplate) == ROP_pbExporter_MAXPARMS);

    return theTemplate;
}

OP_TemplatePair *
ROP_pbExporter::getTemplatePair()
{
    static OP_TemplatePair *ropPair = 0;
    if (!ropPair)
    {
        ropPair = new OP_TemplatePair(getTemplates());
    }
    return ropPair;
}

OP_VariablePair *
ROP_pbExporter::getVariablePair()
{
    static OP_VariablePair *pair = 0;
    if (!pair)
        pair = new OP_VariablePair(ROP_Node::myVariableList);
    return pair;
}

OP_Node *
ROP_pbExporter::myConstructor(OP_Network *net, const char *name, OP_Operator *op)
{
    return new ROP_pbExporter(net, name, op);
}

ROP_pbExporter::ROP_pbExporter(OP_Network *net, const char *name, OP_Operator *entry)
        : ROP_Node(net, name, entry)
{
}


ROP_pbExporter::~ROP_pbExporter()
{
}

//------------------------------------------------------------------------------
// The startRender(), renderFrame(), and endRender() render methods are
// invoked by Houdini when the ROP runs.

int
ROP_pbExporter::startRender(int /*nframes*/, fpreal tstart, fpreal tend)
{
    int                  rcode = 1;

    myEndTime = tend;
    myStartTime = tstart;

    if (INITSIM())
    {
        initSimulationOPs();
        OPgetDirector()->bumpSkipPlaybarBasedSimulationReset(1);
    }

    if (error() < UT_ERROR_ABORT)
    {
        if( !executePreRenderScript(tstart) )
            return 0;
    }

    return rcode;
}

ROP_RENDER_CODE
ROP_pbExporter::renderFrame(fpreal time, UT_Interrupt *)
{
    OP_Node             *op;
    DOP_Parent          *dopparent;
    const DOP_Engine	*engine;
    UT_String            doppath, savepath;
    UT_String            dopobject, dopdata;

    if( !executePreFrameScript(time) )
        return ROP_ABORT_RENDER;

    DOPPATH(doppath, time);

    if( !doppath.isstring() )
    {
        addError(ROP_MESSAGE, "Invalid DOP path");
        return ROP_ABORT_RENDER;
    }

    op = findNode(doppath);
    if (!op)
    {
        addError(ROP_COOK_ERROR, (const char *)doppath);
        return ROP_ABORT_RENDER;
    }

    dopparent = op ? op->castToDOPParent() : 0;
    if( !dopparent )
    {
        addError(ROP_COOK_ERROR, (const char *)doppath);
        return ROP_ABORT_RENDER;
    }

	engine = &dopparent->getEngine();
	int totalDopObjects = engine->getNumSimulationObjects();

    DOPOBJECT(dopobject, time);
    DOPDATA(dopdata, time);
    OUTPUT(savepath, time);


    time = DOPsetBestTime(dopparent, time);

    OP_Context          context(time);

	for(int i = 0; i < totalDopObjects; i++)
	{
		//Get current dop sim object
		SIM_Object *currObject = (SIM_Object *)engine->getSimulationObject(i);
	
		RBD_State *state = SIM_DATA_GET(*currObject, "Solver", RBD_State);
	
		std::cout << "Position: " << state->getPosition() << std::endl;          
		std::cout << "ObjectId: " << currObject->getObjectId() << std::endl;
	}


    /*const SIM_Object            *object;
    object = dopparent->findObjectFromString(dopobject, 0, 0, time);

    if (!object)
    {
        addError(ROP_COOK_ERROR, (const char *)dopobject);
        return ROP_ABORT_RENDER;
    }

    const SIM_Data              *data;

    data = object->getConstNamedSubData(dopdata);

    if (!data)
    {
        addError(ROP_COOK_ERROR, (const char *) dopdata);
        return ROP_ABORT_RENDER;
    }*/

    if (ALFPROGRESS() && (myEndTime != myStartTime))
    {
        fpreal          fpercent = (time - myStartTime) / (myEndTime - myStartTime);
        int             percent = (int)SYSrint(fpercent * 100);
        percent = SYSclamp(percent, 0, 100);
        fprintf(stdout, "ALF_PROGRESS %d%%\n", percent);
        fflush(stdout);
    }
    
    if (error() < UT_ERROR_ABORT)
    {
        if( !executePostFrameScript(time) )
            return ROP_ABORT_RENDER;
    }

    return ROP_CONTINUE_RENDER;
}

ROP_RENDER_CODE
ROP_pbExporter::endRender()
{
    if (INITSIM())
        OPgetDirector()->bumpSkipPlaybarBasedSimulationReset(-1);

    if (error() < UT_ERROR_ABORT)
    {
        if( !executePostRenderScript(myEndTime) )
            return ROP_ABORT_RENDER;
    }
    return ROP_CONTINUE_RENDER;
}

void
newDriverOperator(OP_OperatorTable *table)
{
    table->addOperator(new OP_Operator("hdk_dopfield",
                                        "Dop Field",
                                        ROP_pbExporter::myConstructor,
                                        ROP_pbExporter::getTemplatePair(),
                                        0,
                                        0,
                                        ROP_pbExporter::getVariablePair(),
                                        OP_FLAG_GENERATOR));
}	
