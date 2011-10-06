#ifndef __ROP_pbExporter_h__
#define __ROP_pbExporter_h__

#include <ROP/ROP_Node.h>

#define STR_PARM(name, vi, t) \
                { evalString(str, name, vi, t); }
#define INT_PARM(name, vi, t) \
                { return evalInt(name, vi, t); }

class OP_TemplatePair;
class OP_VariablePair;

enum {
    ROP_pbExporter_RENDER,
    ROP_pbExporter_RENDER_CTRL,
    ROP_pbExporter_TRANGE,
    ROP_pbExporter_FRANGE,
    ROP_pbExporter_TAKE,
    ROP_pbExporter_DOPPATH,
    ROP_pbExporter_DOPOUTPUT,
    ROP_pbExporter_DOPOBJECT,
    ROP_pbExporter_DOPDATA,
    ROP_pbExporter_INITSIM,
    ROP_pbExporter_ALFPROGRESS,
    ROP_pbExporter_TPRERENDER,
    ROP_pbExporter_PRERENDER,
    ROP_pbExporter_LPRERENDER,
    ROP_pbExporter_TPREFRAME,
    ROP_pbExporter_PREFRAME,
    ROP_pbExporter_LPREFRAME,
    ROP_pbExporter_TPOSTFRAME,
    ROP_pbExporter_POSTFRAME,
    ROP_pbExporter_LPOSTFRAME,
    ROP_pbExporter_TPOSTRENDER,
    ROP_pbExporter_POSTRENDER,
    ROP_pbExporter_LPOSTRENDER,

    ROP_pbExporter_MAXPARMS
};

class ROP_pbExporter : public ROP_Node {
public:

    /// Provides access to our parm templates.
    static OP_TemplatePair      *getTemplatePair();
    static OP_VariablePair      *getVariablePair();
    /// Creates an instance of this node.
    static OP_Node              *myConstructor(OP_Network *net, const char*name,
                                                OP_Operator *op);

protected:
             ROP_pbExporter(OP_Network *net, const char *name, OP_Operator *entry);
    virtual ~ROP_pbExporter();

    /// Called at the beginning of rendering to perform any intialization necessary.
    /// @param  nframes     Number of frames being rendered.
    /// @param  s           Start time, in seconds.
    /// @param  e           End time, in seconds.
    /// @return             True of success, false on failure (aborts the render).
    virtual int                  startRender(int nframes, float s, float e);

    /// Called once for every frame that is rendered.
    /// @param  time        The time to render at.
    /// @param  boss        Interrupt handler.
    /// @return             Return a status code indicating whether to abort the render, continue, or retry the current frame.
    virtual ROP_RENDER_CODE      renderFrame(float time, UT_Interrupt *boss);

    /// Called after the rendering is done to perform any post-rendering steps required.
    /// @return             Return a status code indicating whether to abort the render, continue, or retry.
    virtual ROP_RENDER_CODE      endRender();

public:

    /// A convenience method to evaluate our custom file parameter.
    void		OUTPUT(UT_String &str, float t){ STR_PARM("file",  0, t) }
    int         INITSIM(void){ INT_PARM("initsim", 0, 0) }
    bool        ALFPROGRESS(){ INT_PARM("alfprogress", 0, 0) }
    void        DOPPATH(UT_String &str, float t){ STR_PARM("doppath", 0, t)}
    void        DOPOBJECT(UT_String &str, float t){ STR_PARM("dopobject", 0, t)}
    void        DOPDATA(UT_String &str, float t){ STR_PARM("dopdata", 0, t)}


private:
    float                myEndTime;
    float                myStartTime;
};

#undef STR_PARM
#undef STR_SET
#undef STR_GET

#endif
