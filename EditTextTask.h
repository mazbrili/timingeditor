#ifndef EDIT_TEXT_TASK_H
#define EDIT_TEXT_TASK_H

#include "Task.h"

class TimingTextCtrl;

class EditTextTask : public Task
{
public:
    EditTextTask(const Task *task, TimingTextCtrl *txtctrl);
    virtual ~EditTextTask();

    virtual void LabelsMouse(const wxMouseEvent &event, const wxPoint &pos);
    virtual void WavesMouse(const wxMouseEvent &event, const wxPoint &pos);
    virtual void AxisMouse(const wxMouseEvent &event, const wxPoint &pos);
//    virtual void LabelsKey(const wxKeyEvent &event, bool down);
//    virtual void WavesKey(const wxKeyEvent &event, bool down);
//    virtual void AxisKey(const wxKeyEvent &event, bool down);

    //virtual wxInt32 GetSelectedSignalNumber();
    //virtual wxInt32 GetSelectedDiscontinuity();
    virtual void Copy();
    virtual void Cut();
    virtual void Paste();
    virtual void Delete();
    virtual void SelectAll();
    virtual bool CanCut()const;
    virtual bool CanCopy()const;
    virtual bool CanPaste()const;
    virtual bool CanDelete()const;
    virtual void TextHasFocus(TimingTextCtrl *ctrl);

private:
    friend class TimingTextCtrl;
    virtual void EndTask();
    bool IsTextSelected()const;

    void Init();
private:
    void OnMouse(const wxMouseEvent &event);

    void SendCommandToProcessor();
private:
    bool editdone;
protected:
    TimingTextCtrl          *m_txtctrl;
};

#endif // EDIT_TEXT_TASK_H

