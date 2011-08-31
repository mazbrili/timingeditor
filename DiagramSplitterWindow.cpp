#include "DiagramSplitterWindow.h"

#include "DiagramRightWindow.h"
#include "DiagramLabelsWindow.h"
#include "TimingView.h"

IMPLEMENT_DYNAMIC_CLASS(DiagramSplitterWindow, wxSplitterWindow)

BEGIN_EVENT_TABLE(DiagramSplitterWindow, wxSplitterWindow)
END_EVENT_TABLE()

//DiagramSplitterWindow::DiagramSplitterWindow(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
DiagramSplitterWindow::DiagramSplitterWindow(TimingView *v, wxWindow* parent, wxWindowID id,
        //ClockSettingsPanel *clkpanel, TransitionSettingsPanel *trnpanel, AxisSettingsPanel *axspanel, TimeCompressorSettingsPanel* tcpanel,
        const wxPoint& pos, const wxSize& size, long style)
: wxSplitterWindow( parent, id, pos, size, style | wxSUNKEN_BORDER | wxSP_3D ),
//    ClkSetPanel(clkpanel),
//    TranSetPanel(trnpanel),
//    AxisSetPanel(axspanel),
//    TmeCmprssrPanel(tcpanel),
    view(v)
{
    //ctor
    SetSplitMode(wxSPLIT_VERTICAL );

    const wxCoord MinimumOfLeftWidth = 10;
    const wxCoord HeightOfAxisWindow = view->GetHeightOfAxisWindow();

    m_right = new DiagramRightWindow();
    wxPanel *left = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER);
    m_labels = new DiagramLabelsWindow(view, left, m_right , wxID_ANY, wxDefaultPosition, wxSize(100,wxDefaultCoord) );
    m_right->Create(view, this, m_labels, wxID_ANY, wxDefaultPosition, wxDefaultSize );

    wxBoxSizer *lsizer = new wxBoxSizer(wxVERTICAL);
    lsizer->Add(MinimumOfLeftWidth,HeightOfAxisWindow);
    lsizer->Add(m_labels, 1, wxEXPAND);
    left->SetAutoLayout(true);
    left->SetSizer(lsizer);
    left->SetBackgroundColour(v->GetLabelsBackgroundColour());

    // set to a minimum width so it will not become invisible
    SetMinimumPaneSize(MinimumOfLeftWidth);
    SplitVertically(left, m_right , 100);
    Connect( wxEVT_IDLE, (wxObjectEventFunction)&DiagramSplitterWindow::OnIdleDoSetSashPosition );
    //::wxLogMessage(_T("SetSashPosition() does nothing %d"), GetClientSize().GetWidth());
    //SetSashPosition(100);
}

void DiagramSplitterWindow::OnIdleDoSetSashPosition(wxIdleEvent& WXUNUSED(event))
{
    if ( GetClientSize().GetWidth() > 20 )
    {
        SetSashPosition(100);
        Disconnect( wxEVT_IDLE, (wxObjectEventFunction)&DiagramSplitterWindow::OnIdleDoSetSashPosition );
    }
}

DiagramLabelsWindow *DiagramSplitterWindow::GelLabelsWindow()
{
    return m_labels;
}
DiagramRightWindow  *DiagramSplitterWindow::GetRightWindow()
{
    return m_right;
}
void DiagramSplitterWindow::DoUpdate()
{
    //m_labels->Refresh();
    m_labels->DoUpdate();
    m_right->DoUpdate();
}

void DiagramSplitterWindow::OnDragEnter(void)
{
    /*drop = true;
    dropcaret->Show();
    Refresh(true);*/
}
void DiagramSplitterWindow::OnDragLeave(void)
{
    /*drop = false;
    dropcaret->Hide();
    Refresh(true);*/
}
wxDragResult DiagramSplitterWindow::OnDragOver(wxPoint pt, wxDragResult def)
{
    if ( !view )
        return (wxDragNone);
//    TimingDocument *doc = (TimingDocument *)view->GetDocument();
//    if ( !doc )
//        return (wxDragNone);
/*
    wxClientDC dc(this);
    DoPrepareDC(dc);
    wxCoord xx, yy;
    CalcUnscrolledPosition( p.x, p.y, &xx, &yy);
    dc.SetFont(font);
    wxCoord dx, dy;
    //dc.GetTextExtent(_T("H"), &dx, &dy);
    dx = dc.GetCharWidth();
    dy = dc.GetCharHeight();

    wxPoint pt(xx, yy);

    if ( dndsource && def == wxDragCopy )
        dropcopy = true;
    else
        dropcopy = false;

/// ////////////////////////////////////////////////////////////////////////////
    /// cursor over text area?
    //bool overText = false;

    for (unsigned int n = 0 ; n < textSizes.size() ; ++n )
    {
        wxPoint offset( textOffsets[n] );
        wxPoint size ( textSizes[n] );
        if( pt.x > offset.x &&
            pt.x < offset.x + size.x &&
            pt.y > offset.y &&
            pt.y < offset.y + size.y
            )
        {
            //textOffset = offset;
            droppedStringPtr = textStringPtrs[n];
            /// calc with of the texts
            wxArrayInt widths;
            wxString text = *droppedStringPtr;
            dc.SetFont(font); /// change to font of selected text
            dc.GetPartialTextExtents(text, widths);
            widths.Insert(0, 0);

            /// get position of cursor in the text
            wxInt32 n;
            for ( n = 0 ; n < widths.GetCount()-1; n++ )
                if ( pt.x <= 10 + (widths[n] + widths[n+1] )/2 ) break;
            /// set size of dropcaret mathing to text
            ///move the cursor to this place
            if ( !dropcaret->IsVisible() ) dropcaret->Show();
            dropcaret->SetSize(1, dc.GetCharHeight() );
            droppos = n;
            this->Refresh(true);
            return def;
        }
        else
        {
            droppos = -1;
            droppedStringPtr = NULL;
            if ( dropcaret->IsVisible() ) dropcaret->Hide();
        }
    }
    /// check that a wxCaret is shown where needed and refresh
    this->Refresh(true);
*/
/// ////////////////////////////////////////////////////////////////////////////
    return(wxDragNone);
}
bool DiagramSplitterWindow::OnDrop(wxPoint pt, wxString str )
{

    if (!view)
        return( false );

    /*dropcaret->Hide();
    if ( dndsource )
    {
        DroppedText = str;
    }
    else
    {
        drop = false;
        InsertDroppedText(str);
    }
*/
    Refresh(true);
    /// /////////////////////////////////////////////////////////////////////////////////////////////////
    return true;
}
