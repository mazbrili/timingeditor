#ifndef ACTIVEHORIZONTALARROWTASK_H
#define ACTIVEHORIZONTALARROWTASK_H

#include "HorizontalArrowTask.h"


class TimingDocument;
class HorizontalArrowText;

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

    virtual bool CanCut()const;
    virtual bool CanCopy()const;
    virtual bool CanPaste()const;
    virtual bool CanDelete()const;
    virtual void Copy();
    virtual void Cut();
    virtual void Paste();
    virtual void Delete();
    virtual void Update();
    virtual void SelectAll();
    virtual void TextHasFocus(TimingTextCtrl *ctrl);


    void OnText();
    void OnEnterText();
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
    bool IsTextSelected()const;
    void EndEditing();
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
        deleting,
        editingText
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

    HorizontalArrowText *m_txtCtrl;
    //wxString text;
};

#endif // ACTIVEHORIZONTALARROWTASK_H
