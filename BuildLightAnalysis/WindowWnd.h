#include "PropertyGridCtrl.h"
#pragma once


class CWindowWnd : public CDockablePane
{
// Construction
public:
	CWindowWnd();

	void AdjustLayout();

// Attributes
public:
	void SetVSDotNetLook(BOOL bSet)
	{
		m_wndPropList.SetVSDotNetLook(bSet);
		m_wndPropList.SetGroupNameFullWidth(bSet);
	}
	CMFCPropertyGridProperty* getCoodGroup()
	{
		return m_wndPropList.GetProperty(0);
	}

	double getProperty(int dataIndex)
	{
		CMFCPropertyGridProperty* data = m_wndPropList.GetProperty(dataIndex);
		if (data)
		{
			return data->GetValue().dblVal;
		}
		return 0.0;
	}

	void InsertWindow(int outWallIndex, int inWallIndex);
	void save(ofstream& out);
	void load(ifstream& in);

protected:
	CFont m_fntPropList;
	PropertyGridCtrl m_wndPropList;

// Implementation
public:
	virtual ~CWindowWnd();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnExpandAllProperties();
	afx_msg void OnUpdateExpandAllProperties(CCmdUI* pCmdUI);
	afx_msg void OnSortProperties();
	afx_msg void OnUpdateSortProperties(CCmdUI* pCmdUI);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);

	DECLARE_MESSAGE_MAP()

	void InitPropList();
	void SetPropListFont();
};
