#ifndef BASETASK_H
#define BASETASK_H

#include <wx/event.h>

class TimingView;
class DiagramLabelsWindow;
class DiagramAxisWindow;
class DiagramWavesWindow;
class TimingTextCtrl;
class Signal;

class Task
{
public:
    Task(TimingView *view, DiagramLabelsWindow *labelsWin, DiagramAxisWindow *axisWin, DiagramWavesWindow *waveWin);
    Task(const Task *task);
    virtual ~Task();

    virtual void LabelsMouse(const wxMouseEvent &event, const wxPoint &pos);
    virtual void WavesMouse(const wxMouseEvent &event, const wxPoint &pos);
    virtual void AxisMouse(const wxMouseEvent &event, const wxPoint &pos);
    virtual void LabelsKey(const wxKeyEvent &event, bool down);
    virtual void WavesKey(const wxKeyEvent &event, bool down);
    virtual void AxisKey(const wxKeyEvent &event, bool down);

    virtual wxInt32 GetSelectedSignalNumber()const;
    virtual wxInt32 GetSelectedDiscontinuity()const;
    virtual void Copy();
    virtual void Cut();
    virtual void Paste();
    virtual void Delete();
    virtual void SelectAll();
    virtual bool CanCopy()const;
    virtual bool CanCut()const;
    virtual bool CanPaste()const;
    virtual bool CanDelete()const;
    virtual void TextHasFocus(TimingTextCtrl *ctrl);

    virtual void UpdateTimeCompressorPanel(bool attach = true)const;
    virtual void UpdateClockSettingsPanel(bool attach = true)const;

    virtual void Update();

protected:
    // common methods
    wxInt32 GetTickFromPosition(const wxPoint &pos)const;
    wxInt32 GetSignalFromPosition(const wxPoint &pos)const;
    bool IsOverWavesRect(const wxPoint &pos)const;
    int IsOverVerticalLine(const wxPoint &pos)const;
    int IsOverHorizontalArrow(const wxPoint &pos)const;
    int IsOverWaves(const wxPoint &pos)const;
    const int GetVerticalLineSnapTolerance()const{return 3;}
    const int GetHorizontalArrowSnapTolerance()const{return 3;}
protected:
    const int GetRangeAddRemoveTime()const{return 5;}
    bool IsOnAddTimeRange(const wxPoint &pos)const;
    bool IsOnRemoveTimeRange(const wxPoint &pos)const;

protected:
    virtual void EndTask();
    void Init();
    bool IsSignalInClipboard()const;
    void AddSignal(Signal *sig)const;
    void PasteSignalFromClipboard()const;
    bool IsReadOnly()const;


    TimingView          *m_view;
    DiagramLabelsWindow *m_labelsWin;
    DiagramAxisWindow   *m_axisWin;
    DiagramWavesWindow  *m_waveWin;
};

#endif // BASETASK_H
