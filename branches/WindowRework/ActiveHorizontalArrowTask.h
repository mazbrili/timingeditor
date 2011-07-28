#ifndef ACTIVEHORIZONTALARROWTASK_H
#define ACTIVEHORIZONTALARROWTASK_H

#include "HorizontalArrowTask.h"


class TimingDocument;
class ActiveHorizontalArrowTask : public HorizontalArrowTask
{
public:
    ActiveHorizontalArrowTask(const Task *task, int horizontalArrowIdx);
    virtual ~ActiveHorizontalArrowTask();
public:
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
    void OnMouse(const wxMouseEvent &event);
    void OnKey(const wxKeyEvent &event, bool down);
private:
    void CheckMovingArrow(const wxPoint &pos);
    void CheckMovingStartStop(const wxPoint &pos);
    void CheckMovingText(const wxPoint &pos);
    HoverDrawlet *GetActiveArrowStateDrawlet();
private:
    int m_horizontalArrowIdx;

     TimingDocument *m_doc;
     void Init();
private:
    enum states{
        activeArrow,
        movingArrow,
        movingStartPos,
        movingEndPos,
        movingText,
        deleting
    };
    states state;

    //wxInt32 old_yStart;
    //wxInt32 old_yStop;
    int m_secondVerticalLine;
    int m_overVerticalLine;
    wxPoint m_pos;
    //wxInt32 m_signalnmbr;
    bool m_isValidMove;
    bool m_isValidPos;

    //wxString text;
};

#endif // ACTIVEHORIZONTALARROWTASK_H
