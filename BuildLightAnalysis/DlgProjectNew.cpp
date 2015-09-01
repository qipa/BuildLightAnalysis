// DlgProjectNew.cpp : implementation file
//

#include "stdafx.h"
#include "BuildLightAnalysis.h"
#include "DlgProjectNew.h"
#include "afxdialogex.h"

#include <fstream>
#include <string>

using namespace std;


// DlgProjectNew dialog

IMPLEMENT_DYNAMIC(DlgProjectNew, CDialogEx)

DlgProjectNew::DlgProjectNew(CWnd* pParent /*=NULL*/)
	: CDialogEx(DlgProjectNew::IDD, pParent)
	, m_projectName(_T(""))
	, m_projectLocation(_T(""))
{

}

DlgProjectNew::~DlgProjectNew()
{
}

void DlgProjectNew::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_NEW_NAME, m_projectName);
	DDX_Text(pDX, IDC_EDIT_NEW_LOCATION, m_projectLocation);
}


BEGIN_MESSAGE_MAP(DlgProjectNew, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_NEW_BROWSE, &DlgProjectNew::OnBnClickedButtonNewBrowse)
	ON_BN_CLICKED(IDOK, &DlgProjectNew::OnBnClickedOk)
END_MESSAGE_MAP()


// DlgProjectNew message handlers


void DlgProjectNew::OnBnClickedButtonNewBrowse()
{
	CString    strFolderPath;  
	BROWSEINFO broInfo = {0};  
	TCHAR       szDisName[MAX_PATH] = {0};  

	broInfo.hwndOwner = this->m_hWnd;  
	broInfo.pidlRoot  = NULL;  
	broInfo.pszDisplayName = szDisName;  
	broInfo.lpszTitle = _T("ѡ�񱣴�·��");  
	broInfo.ulFlags   = BIF_NEWDIALOGSTYLE | BIF_DONTGOBELOWDOMAIN  
		| BIF_BROWSEFORCOMPUTER | BIF_RETURNONLYFSDIRS | BIF_RETURNFSANCESTORS;  
	broInfo.lpfn      = NULL;  
	broInfo.lParam    = NULL;  
	broInfo.iImage    = IDR_MAINFRAME;  

	LPITEMIDLIST pIDList = SHBrowseForFolder(&broInfo);  
	if (pIDList != NULL)  
	{  
		memset(szDisName, 0, sizeof(szDisName));  
		SHGetPathFromIDList(pIDList, szDisName);  
		strFolderPath = szDisName;  
		m_projectLocation = strFolderPath;
		UpdateData(FALSE);
	}  

}


void DlgProjectNew::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if (!PathIsDirectory(m_projectLocation)) 
	{
		if (!CreateDirectory(m_projectLocation,NULL)) 
		{ 
			CString strMsg;
			strMsg.Format (_T("����·��\"%s\"ʧ��"), m_projectLocation); 
			AfxMessageBox(strMsg);
			return; 
		}
	}

	CString cPath;
	cPath.Format(_T("%s\\%s.bla"), m_projectLocation, m_projectName);
	CStringA stra(cPath.GetBuffer(0));
	cPath.ReleaseBuffer();
	string path=stra.GetBuffer(0);
	stra.ReleaseBuffer();

	ofstream outFile(path);
	if (!outFile.is_open())
	{
		CString strMsg;
		strMsg.Format (_T("������Ŀ�ļ�\"%s\"ʧ��"), cPath); 
		AfxMessageBox(strMsg);
		return; 
	}
	
	CDialogEx::OnOK();
}