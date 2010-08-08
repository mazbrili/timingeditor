//
//    TimingEditor is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with TimingEditor.  If not, see <http://www.gnu.org/licenses/>.
//
//    Contact e-mail: daniel anselmi <danselmi@gmx.ch>
//    Program URL   : http://sourceforge.net/projects/timingeditor/
//
//
///////////////////////////////////////////////////////////////////////////

#ifndef __AxisSettingsPanel__
#define __AxisSettingsPanel__

// Define WX_GCH in order to support precompiled headers with GCC compiler.
// You have to create the header "wx_pch.h" and include all files needed
// for compile your gui inside it.
// Then, compile it and place the file "wx_pch.h.gch" into the same
// directory that "wx_pch.h".
#ifdef WX_GCH
#include <wx_pch.h>
#else
#include <wx/wx.h>
#endif

#include <wx/button.h>
#include <wx/choice.h>

///////////////////////////////////////////////////////////////////////////

#define ID_DEFAULT wxID_ANY // Default



class TimingWindow;
class AxisSettingsPanel : public wxPanel
{
	private:

    public:
        TimingWindow *wnd;
        void SetTickLength(wxInt32 ticklength);
        void SetLengthUnit(wxInt8 unit);
        void SetMarkerLength(wxInt32 markerlength);
        void SetOffset(wxInt32 offset);
        void SetTotalLengt(wxInt32 totallength);
    private:
        wxInt8 TickUnit;
        void SetUnmodified();

	protected:
		wxTextCtrl *m_textTickLenght;
		wxChoice   *m_choice1;
		wxTextCtrl *m_textTotalLength;
		wxTextCtrl *m_textMarkerLength;
		wxTextCtrl *m_textOffset;
		wxButton   *m_applyButton;

    private:
        void OnUpdateFields(wxUpdateUIEvent& event);
        void OnUpdatePanelApply(wxUpdateUIEvent& event);
        void OnApply(wxCommandEvent &event);

	public:
		AxisSettingsPanel( wxWindow* parent, int id = -1, wxPoint pos = wxDefaultPosition, wxSize size = wxSize( 500,300 ), int style = wxTAB_TRAVERSAL );

	DECLARE_EVENT_TABLE()
};

#endif //__AxisSettingsPanel__

