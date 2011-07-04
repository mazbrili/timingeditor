#ifndef ADDVERTICALLINETASK_H
#define ADDVERTICALLINETASK_H

#include "VerticalLineTask.h"

class AddVerticalLineTask : public VerticalLineTask
{
public:
    AddVerticalLineTask(const Task *task);
    virtual ~AddVerticalLineTask();

    virtual void LabelsMouse(const wxMouseEvent &event, const wxPoint &pos);
    virtual void WavesMouse(const wxMouseEvent &event, const wxPoint &pos);
    virtual void AxisMouse(const wxMouseEvent &event, const wxPoint &pos);
    virtual void LabelsKey(const wxKeyEvent &event, bool down);
    virtual void WavesKey(const wxKeyEvent &event, bool down);
    virtual void AxisKey(const wxKeyEvent &event, bool down);

    //virtual wxInt32 GetSelectedSignalNumber();
    //virtual wxInt32 GetSelectedDiscontinuity();
    //virtual void Copy();
    //virtual void Cut();
    //virtual void Paste();
    //virtual void Delete();
    //virtual void SelectAll();
    //virtual bool CanPaste();
    //virtual bool CanDelete();
    //virtual bool CanEditText(){return false;}
    virtual void Update();

private:
    void EndTask();
    void Init();

    typedef enum{
        waitingFirstPoint,
        waitingSecondPoint
    }states;
protected:
private:
    states state;

    void NonInvolvedWindowMouse(const wxMouseEvent &event);
    void HandleKey(const wxKeyEvent &event, bool down);

    void DoCheckWhenMouseDown(const wxPoint &pos);
    void DoCheckMoving(const wxPoint &pos);
    void DoCheckWhenMouseUp(const wxPoint &pos);
    void SetDrawlet();
};

#endif // ADDVERTICALLINETASK_H
