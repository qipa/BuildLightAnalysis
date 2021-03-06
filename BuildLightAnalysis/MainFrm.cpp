
// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "BuildLightAnalysis.h"
#include "DlgProjectNew.h"
#include "MainFrm.h"

#include "Serializer.h"
#include <fstream>
#include <string>

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWndEx)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWndEx)
	ON_WM_CREATE()
	ON_COMMAND(ID_VIEW_CUSTOMIZE, &CMainFrame::OnViewCustomize)
	ON_REGISTERED_MESSAGE(AFX_WM_CREATETOOLBAR, &CMainFrame::OnToolbarCreateNew)
	ON_COMMAND(ID_EDIT_OUTWALL, &CMainFrame::OnEditOutwall)
	ON_COMMAND(ID_EDIT_INWALL, &CMainFrame::OnEditInwall)
	ON_COMMAND(ID_EDIT_OPTION, &CMainFrame::OnEditOption)
	ON_COMMAND(ID_EDIT_WINDOW, &CMainFrame::OnEditWindow)
	ON_COMMAND(ID_EDIT_ROOM, &CMainFrame::OnEditRoom)
	ON_COMMAND(ID_ROOM_CAL_GRID, &CMainFrame::OnRoomCalGrid)
	ON_UPDATE_COMMAND_UI(ID_EDIT_OUTWALL, &CMainFrame::OnUpdateEditOutwall)
	ON_UPDATE_COMMAND_UI(ID_EDIT_INWALL, &CMainFrame::OnUpdateEditInwall)
	ON_UPDATE_COMMAND_UI(ID_EDIT_OPTIMIZE, &CMainFrame::OnUpdateEditOptimize)
	ON_COMMAND(ID_ROOM_ADD_LUMINAIRE_SINGLE, &CMainFrame::OnRoomAddLuminaireSingle)
	ON_COMMAND(ID_ROOM_ADD_LUMINAIRE_SET, &CMainFrame::OnRoomAddLuminaireSet)
	ON_COMMAND(ID_ROOM_ADD_CONTROL_SET, &CMainFrame::OnRoomAddControlSet)
	ON_COMMAND(ID_ROOM_ADD_PERSON, &CMainFrame::OnRoomAddPerson)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame construction/destruction

CMainFrame::CMainFrame():m_eMode(MODE_OUTWALL)
{
	// TODO: add member initialization code here
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	BOOL bNameValid;

	// set the visual manager used to draw all user interface elements
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2008));

	if (!m_wndMenuBar.Create(this))
	{
		TRACE0("Failed to create menubar\n");
		return -1;      // fail to create
	}

	m_wndMenuBar.SetPaneStyle(m_wndMenuBar.GetPaneStyle() | CBRS_SIZE_DYNAMIC | CBRS_TOOLTIPS | CBRS_FLYBY);

	// prevent the menu bar from taking the focus on activation
	CMFCPopupMenu::SetForceMenuFocus(FALSE);

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(theApp.m_bHiColorIcons ? IDR_MAINFRAME_256 : IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	CString strToolBarName;
	bNameValid = strToolBarName.LoadString(IDS_TOOLBAR_STANDARD);
	ASSERT(bNameValid);
	m_wndToolBar.SetWindowText(strToolBarName);

	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);
	m_wndToolBar.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	// TODO: Delete these five lines if you don't want the toolbar and menubar to be dockable
	m_wndMenuBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndMenuBar);
	DockPane(&m_wndToolBar);


	// enable Visual Studio 2005 style docking window behavior
	CDockingManager::SetDockingMode(DT_SMART);
	// enable Visual Studio 2005 style docking window auto-hide behavior
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	// create docking windows
	if (!CreateDockingWindows())
	{
		TRACE0("Failed to create docking windows\n");
		return -1;
	}

	m_wndOutWallProperties.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndOutWallProperties);
	m_wndOutWallProperties.ShowPane(FALSE,FALSE,TRUE);
	
	m_wndInWallProperties.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndInWallProperties);
	m_wndInWallProperties.ShowPane(FALSE,FALSE,TRUE);

	m_wndOptimizeWallProperties.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndOptimizeWallProperties);
	m_wndOptimizeWallProperties.ShowPane(FALSE,FALSE,TRUE);

	m_wndOptionProperties.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndOptionProperties);
	m_wndOptionProperties.ShowPane(FALSE,FALSE,TRUE);

	m_wndWindowProperties.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndWindowProperties);
	m_wndWindowProperties.ShowPane(FALSE,FALSE,TRUE);

	m_wndRoomProperties.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndRoomProperties);
	m_wndRoomProperties.ShowPane(FALSE,FALSE,TRUE);

	// Enable toolbar and docking window menu replacement
	EnablePaneMenu(TRUE, ID_VIEW_CUSTOMIZE, strCustomize, ID_VIEW_TOOLBAR);

	// enable quick (Alt+drag) toolbar customization
	CMFCToolBar::EnableQuickCustomization();

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

BOOL CMainFrame::CreateDockingWindows()
{
	BOOL bNameValid;
	// Create properties window
	CString strPropertiesWnd;
	bNameValid = strPropertiesWnd.LoadString(IDS_OUTWALL_PROPERTIES_WND);
	ASSERT(bNameValid);
	if (!m_wndOutWallProperties.Create(strPropertiesWnd, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_OUTWALL_PROPERTIESWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create outWall window\n");
		return FALSE; // failed to create
	}

	bNameValid = strPropertiesWnd.LoadString(IDS_INWALL_PROPERTIES_WND);
	ASSERT(bNameValid);
	if (!m_wndInWallProperties.Create(strPropertiesWnd, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_INWALL_PROPERTIESWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create InWall window\n");
		return FALSE; // failed to create
	}

	bNameValid = strPropertiesWnd.LoadString(IDS_OPTIMIZEWALL_PROPERTIES_WND);
	ASSERT(bNameValid);
	if (!m_wndOptimizeWallProperties.Create(strPropertiesWnd, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_OPTIMIZEWALL_PROPERTIESWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create OptimizeWall window\n");
		return FALSE; // failed to create
	}

	bNameValid = strPropertiesWnd.LoadString(IDS_OPTION_PROPERTIES_WND);
	ASSERT(bNameValid);
	if (!m_wndOptionProperties.Create(strPropertiesWnd, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_OPTION_PROPERTIESWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create Option window\n");
		return FALSE; // failed to create
	}

	if (!m_wndWindowProperties.Create(_T("����"), this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_WINDOW_PROPERTIESWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create Option window\n");
		return FALSE; // failed to create
	}

	if (!m_wndRoomProperties.Create(_T("����"), this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_ROOM_PROPERTIESWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create ROOM window\n");
		return FALSE; // failed to create
	}

	SetDockingWindowIcons(theApp.m_bHiColorIcons);
	return TRUE;
}

void CMainFrame::SetDockingWindowIcons(BOOL bHiColorIcons)
{
	HICON hPropertiesBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_PROPERTIES_WND_HC : IDI_PROPERTIES_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndOutWallProperties.SetIcon(hPropertiesBarIcon, FALSE);

}

// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// CMainFrame message handlers

void CMainFrame::OnViewCustomize()
{
	CMFCToolBarsCustomizeDialog* pDlgCust = new CMFCToolBarsCustomizeDialog(this, TRUE /* scan menus */);
	pDlgCust->Create();
}

LRESULT CMainFrame::OnToolbarCreateNew(WPARAM wp,LPARAM lp)
{
	LRESULT lres = CFrameWndEx::OnToolbarCreateNew(wp,lp);
	if (lres == 0)
	{
		return 0;
	}

	CMFCToolBar* pUserToolbar = (CMFCToolBar*)lres;
	ASSERT_VALID(pUserToolbar);

	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
	return lres;
}


void CMainFrame::OnEditOutwall()
{
	m_eMode = MODE_OUTWALL;
	m_wndOutWallProperties.ShowPane(TRUE,FALSE,TRUE);
	m_wndInWallProperties.ShowPane(FALSE,FALSE,TRUE);
	m_wndOptimizeWallProperties.ShowPane(FALSE,FALSE,TRUE);
}


void CMainFrame::OnEditInwall()
{
	m_eMode = MODE_INWALL;
	m_wndOutWallProperties.ShowPane(FALSE,FALSE,TRUE);
	m_wndInWallProperties.ShowPane(TRUE,FALSE,TRUE);
	m_wndOptimizeWallProperties.ShowPane(FALSE,FALSE,TRUE);
}


void CMainFrame::OnEditOption()
{
	if (m_wndOptionProperties.IsPaneVisible())
		m_wndOptionProperties.ShowPane(FALSE,FALSE,TRUE);
	else
		m_wndOptionProperties.ShowPane(TRUE,FALSE,TRUE);
	
}


void CMainFrame::OnEditWindow()
{
	if (m_wndWindowProperties.IsPaneVisible())
		m_wndWindowProperties.ShowPane(FALSE,FALSE,TRUE);
	else
		m_wndWindowProperties.ShowPane(TRUE,FALSE,TRUE);
}


void CMainFrame::OnEditRoom()
{
	if (m_wndRoomProperties.IsPaneVisible())
		m_wndRoomProperties.ShowPane(FALSE,FALSE,TRUE);
	else
		m_wndRoomProperties.ShowPane(TRUE,FALSE,TRUE);
}
void CMainFrame::saveMode(ofstream& out)
{
	serializer<int>::write(out, &m_eMode);
}
void CMainFrame::loadMode(ifstream& in)
{
	serializer<int>::read(in, &m_eMode);
}

void CMainFrame::OnUpdateEditOutwall(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_eMode == MODE_OUTWALL);    
}

void CMainFrame::OnUpdateEditInwall(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_eMode == MODE_INWALL);
}


void CMainFrame::OnUpdateEditOptimize(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_eMode == MODE_OPTIMIZE);
}

void CMainFrame::OnRoomCalGrid()
{
	m_wndRoomProperties.OnRoomCalGrid();
}


void CMainFrame::OnRoomAddLuminaireSingle()
{
	m_wndRoomProperties.OnRoomAddLuminaireSingle();
}


void CMainFrame::OnRoomAddLuminaireSet()
{
	m_wndRoomProperties.OnRoomAddLuminaireSet();
}


void CMainFrame::OnRoomAddControlSet()
{
	m_wndRoomProperties.OnRoomAddControlSet();
}


void CMainFrame::OnRoomAddPerson()
{
	m_wndRoomProperties.OnRoomAddPerson();
}
