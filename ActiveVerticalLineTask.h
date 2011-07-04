#ifndef ACTIVEVERTICALLINETASK_H
#define ACTIVEVERTICALLINETASK_H

#include "VerticalLineTask.h"


class ActiveVerticalLineTask : public VerticalLineTask
{
public:
    ActiveVerticalLineTask(const Task *task, int vlineidx);
    virtual ~ActiveVerticalLineTask();
    virtual void LabelsMouse(const wxMouseEvent &event, const wxPoint &pos);
    virtual void WavesMouse(const wxMouseEvent &event, const wxPoint &pos);
    virtual void AxisMouse(const wxMouseEvent &event, const wxPoint &pos);
    virtual void LabelsKey(const wxKeyEvent &event, bool down);
    virtual void WavesKey(const wxKeyEvent &event, bool down);
    virtual void AxisKey(const wxKeyEvent &event, bool down);

    virtual bool CanDelete();
    //virtual bool CanCopy();
    //virtual void Copy();
    //virtual void Cut();
    virtual void Delete();
    //virtual void Paste();
    //virtual bool CanPaste();
    virtual void Update();


protected:
private:
    void SetDrawlets();
    void EndTask();
    void OnMouse(const wxMouseEvent &event);
    void OnKey(const wxKeyEvent &event, bool down);
private:
    int m_vlineidx;
private:
    enum states{
        activeLine,
        movingLine,
        movingStartPos,
        movingEndPos,
        deleting
    };
    states state;

    wxInt32 old_xpos;
    wxUint8 old_xposoffset;
};

#endif // ACTIVEVERTICALLINETASK_H

