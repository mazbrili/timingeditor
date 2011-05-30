#ifndef EDITTIMECOMPRESSORTASK_H
#define EDITTIMECOMPRESSORTASK_H

#include "Task.h"


class EditTimeCompressorTask : public Task
{
public:
    EditTimeCompressorTask(const Task *task, wxInt32 tick);
    virtual ~EditTimeCompressorTask();

    virtual void LabelsMouse(const wxMouseEvent &event, const wxPoint &pos);
    virtual void WavesMouse(const wxMouseEvent &event, const wxPoint &pos);
    virtual void AxisMouse(const wxMouseEvent &event, const wxPoint &pos);
    virtual void LabelsKey(const wxKeyEvent &event, bool down);
    virtual void WavesKey(const wxKeyEvent &event, bool down);
    virtual void AxisKey(const wxKeyEvent &event, bool down);
    virtual void UpdateTimeCompressorPanel(bool attach = true);


    bool CanDelete();
    void Delete();
    wxInt32 GetSelectedDiscontinuity();
protected:

private:
    void EndTask();
    void OnMouse(const wxMouseEvent &event);
    void OnKey(const wxKeyEvent &event, bool down);

    //void UpdatePanel();

    wxInt32 m_tick;
};

#endif // EDITTIMECOMPRESSORTASK_H
