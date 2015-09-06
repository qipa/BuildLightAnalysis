
#include "stdafx.h"

#include "OptimizeWallWnd.h"
#include "Resource.h"
#include "MainFrm.h"
#include "BuildLightAnalysis.h"
#include "BuildLightAnalysisDoc.h"
#include "BuildLightAnalysisView.h"
#include "Serializer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


const int waiqiangID = 100;
/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar

COptimizeWallWnd::COptimizeWallWnd()
{
}

COptimizeWallWnd::~COptimizeWallWnd()
{
}

BEGIN_MESSAGE_MAP(COptimizeWallWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_EXPAND_ALL, OnExpandAllProperties)
	ON_UPDATE_COMMAND_UI(ID_EXPAND_ALL, OnUpdateExpandAllProperties)
	ON_COMMAND(ID_SORTPROPERTIES, OnSortProperties)
	ON_UPDATE_COMMAND_UI(ID_SORTPROPERTIES, OnUpdateSortProperties)
	ON_WM_SETFOCUS()
	ON_WM_SETTINGCHANGE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar message handlers

void COptimizeWallWnd::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient,rectButton;
	GetClientRect(rectClient);

	int cyBut = 0;//rectButton.Size().cy;
	m_wndPropList.SetWindowPos(NULL, rectClient.left, rectClient.top + cyBut, rectClient.Width(), rectClient.Height() -(cyBut), SWP_NOACTIVATE | SWP_NOZORDER);
}

int COptimizeWallWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	if (!m_wndPropList.Create(WS_VISIBLE | WS_CHILD, rectDummy, this, IDC_OPTIMIZE_PROPERTY_CTRL))
	{
		TRACE0("Failed to create Properties Grid \n");
		return -1;      // fail to create
	}

	InitPropList();

	AdjustLayout();
	return 0;
}

void COptimizeWallWnd::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void COptimizeWallWnd::OnExpandAllProperties()
{
	m_wndPropList.ExpandAll();
}

void COptimizeWallWnd::OnUpdateExpandAllProperties(CCmdUI* /* pCmdUI */)
{
}

void COptimizeWallWnd::OnSortProperties()
{
	m_wndPropList.SetAlphabeticMode(!m_wndPropList.IsAlphabeticMode());
}

void COptimizeWallWnd::OnUpdateSortProperties(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_wndPropList.IsAlphabeticMode());
}
void COptimizeWallWnd::DeletePos()
{
	CMFCPropertyGridProperty* pGroup = getCoodOutWallGroup();
	for (int i = 0; i < pGroup->GetSubItemsCount(); i++)
	{
		CMFCPropertyGridProperty* subItem = pGroup->GetSubItem(i);
		pGroup->RemoveSubItem(subItem);
		i--;
	}
	CMFCPropertyGridProperty* pGroup1 = getCoodInWallGroup();
	for (int i = 0; i < pGroup1->GetSubItemsCount(); i++)
	{
		CMFCPropertyGridProperty* subItem = pGroup1->GetSubItem(i);
		pGroup1->RemoveSubItem(subItem);
		i--;
	}
}
void COptimizeWallWnd::InsertPos(bool isOutWall, double x, double y, double x1, double y1)
{
	CMFCPropertyGridProperty* pGroup = NULL;
	if (isOutWall)
	{
		pGroup = getCoodOutWallGroup();
	}
	else
	{
		pGroup = getCoodInWallGroup();
	}
		
	int count = pGroup->GetSubItemsCount();
	CString strCount;
	strCount.Format(_T("%d"),count);

	PropertyGridProperty* pPos = new PropertyGridProperty(strCount, 0, TRUE);

	PropertyGridProperty* pStart = new PropertyGridProperty(_T("Start"), 0, TRUE);
	PropertyGridProperty* pProp = new PropertyGridProperty(_T("X"), (_variant_t) x, _T("Specifies the window's height"));
	pStart->AddSubItem(pProp);
	pProp = new PropertyGridProperty( _T("Y"), (_variant_t) y, _T("Specifies the window's width"));
	pStart->AddSubItem(pProp);

	PropertyGridProperty* pEnd = new PropertyGridProperty(_T("End"), 0, TRUE);
	pProp = new PropertyGridProperty(_T("X"), (_variant_t) x1, _T("Specifies the window's height"));
	pEnd->AddSubItem(pProp);
	pProp = new PropertyGridProperty( _T("Y"), (_variant_t) y1, _T("Specifies the window's width"));
	pEnd->AddSubItem(pProp);

	pPos->AddSubItem(pStart);
	pPos->AddSubItem(pEnd);


	pGroup->AddSubItem(pPos);

	m_wndPropList.UpdateProperty((PropertyGridProperty*)(pGroup));
	m_wndPropList.AdjustLayout();
}


void COptimizeWallWnd::OnUpdateProperties1(CCmdUI* /*pCmdUI*/)
{
	// TODO: Add your command update UI handler code here
}

void COptimizeWallWnd::OnDeletePos()
{
	//CMFCPropertyGridProperty* selItem = m_wndPropList.GetCurSel();
	//if (selItem && selItem->GetParent() && selItem->GetParent()->GetData() == waiqiangID)
	//{
	//	m_wndPropList.DeleteProperty(selItem);

	//	//重新设置一下坐标编号
	//	CMFCPropertyGridProperty* pGroup = getCoodGroup();
	//	if (!pGroup)
	//		return;
	//	
	//	int count = pGroup->GetSubItemsCount();
	//	CString strName;
	//	for (int i = 0; i < count; i++)
	//	{
	//		strName.Format(_T("%d"),i+1);
	//		pGroup->GetSubItem(i)->SetName(strName);
	//	}
	//}
	//
	////更新视图
	//CMainFrame* pMain=(CMainFrame*)AfxGetApp()->m_pMainWnd;     
	//pMain->GetActiveView()->Invalidate(); 
}

void COptimizeWallWnd::OnUpdateProperties2(CCmdUI* /*pCmdUI*/)
{
	// TODO: Add your command update UI handler code here
}

void COptimizeWallWnd::InitPropList()
{
	SetPropListFont();

	m_wndPropList.EnableHeaderCtrl(FALSE);
	m_wndPropList.EnableDescriptionArea();
	m_wndPropList.SetVSDotNetLook();
	m_wndPropList.MarkModifiedProperties();

	CMFCPropertyGridProperty* pGroup = new CMFCPropertyGridProperty(_T("处理后外墙"),0);
	CMFCPropertyGridProperty* pGroup1 = new CMFCPropertyGridProperty(_T("处理后内墙"),0);
	m_wndPropList.AddProperty(pGroup);
	m_wndPropList.AddProperty(pGroup1);
}

void COptimizeWallWnd::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);
	m_wndPropList.SetFocus();
}

void COptimizeWallWnd::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CDockablePane::OnSettingChange(uFlags, lpszSection);
	SetPropListFont();
}

void COptimizeWallWnd::SetPropListFont()
{
	::DeleteObject(m_fntPropList.Detach());

	LOGFONT lf;
	afxGlobalData.fontRegular.GetLogFont(&lf);

	NONCLIENTMETRICS info;
	info.cbSize = sizeof(info);

	afxGlobalData.GetNonClientMetrics(info);

	lf.lfHeight = info.lfMenuFont.lfHeight;
	lf.lfWeight = info.lfMenuFont.lfWeight;
	lf.lfItalic = info.lfMenuFont.lfItalic;

	m_fntPropList.CreateFontIndirect(&lf);

	m_wndPropList.SetFont(&m_fntPropList);
}

void COptimizeWallWnd::inputFromLines(vector<sLine>& sLines)
{
	
	DeletePos();	
	for (int i = 0; i < sLines.size(); i++)
	{
		if (sLines[i].type == sLine::OUT_WALL)
		{
			InsertPos(true, sLines[i].s.x, sLines[i].s.y, sLines[i].e.x, sLines[i].e.y);
		}
		else
		{
			InsertPos(false, sLines[i].s.x, sLines[i].s.y, sLines[i].e.x, sLines[i].e.y);
		}
		
	}

}
void COptimizeWallWnd::OutputToLines(vector<sLine>& sLines)
{
	//外墙
	CMFCPropertyGridProperty* outWallPos = getCoodOutWallGroup();
	Vec2d ps, pe;
	for (int i = 0; i < outWallPos->GetSubItemsCount(); i++)
	{
		ps.x = outWallPos->GetSubItem(i)->GetSubItem(0)->GetSubItem(0)->GetValue().dblVal;
		ps.y = outWallPos->GetSubItem(i)->GetSubItem(0)->GetSubItem(1)->GetValue().dblVal;

		pe.x = outWallPos->GetSubItem(i)->GetSubItem(1)->GetSubItem(0)->GetValue().dblVal;
		pe.y = outWallPos->GetSubItem(i)->GetSubItem(1)->GetSubItem(1)->GetValue().dblVal;

		sLines.push_back(sLine(ps,pe,sLine::OUT_WALL));
	}

	//内墙
	CMFCPropertyGridProperty* inWallPos = getCoodInWallGroup();
	for (int i = 0; i < inWallPos->GetSubItemsCount(); i++)
	{
		ps.x = inWallPos->GetSubItem(i)->GetSubItem(0)->GetSubItem(0)->GetValue().dblVal;
		ps.y = inWallPos->GetSubItem(i)->GetSubItem(0)->GetSubItem(1)->GetValue().dblVal;

		pe.x = inWallPos->GetSubItem(i)->GetSubItem(1)->GetSubItem(0)->GetValue().dblVal;
		pe.y = inWallPos->GetSubItem(i)->GetSubItem(1)->GetSubItem(1)->GetValue().dblVal;

		sLines.push_back(sLine(ps,pe,sLine::IN_WALL));
	}
}
void COptimizeWallWnd::save(ofstream& out)
{
	vector<sLine> sLines;
	OutputToLines(sLines);
	serializer<sLine>::write(out, &sLines);
}
void COptimizeWallWnd::load(ifstream& in)
{
	vector<sLine> sLines;
	serializer<sLine>::read(in, &sLines);
	inputFromLines(sLines);
}