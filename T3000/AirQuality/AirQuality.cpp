// AirQuality.cpp : implementation file
//

#include "stdafx.h"
#include "../T3000.h"
#include "AirQuality.h"

static BOOL ifAQdb = TRUE;
static BOOL writedb = TRUE;


// CAirQuality

IMPLEMENT_DYNCREATE(CAirQuality, CFormView)

CAirQuality::CAirQuality()
	: CFormView(CAirQuality::IDD)
	, m_inaddress(0)
{

}

CAirQuality::~CAirQuality()
{
}

void CAirQuality::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MSFLEXGRID1, m_msflesgrid);
	DDX_Text(pDX, IDC_EDIT7, m_inaddress);
	DDX_Text(pDX, IDC_EDIT9, m_inSerialNum);
	DDX_Text(pDX, IDC_EDIT8, m_flFirmware);
	DDX_Text(pDX, IDC_EDIT10, m_inHardware);
	DDX_Text(pDX, IDC_EDIT_MODEL, m_CStrModel);
	DDX_Text(pDX, IDC_EDIT12, m_inBaudrate);
	DDX_Text(pDX,IDC_STATIC_SPMODE,m_clock);
}

BEGIN_MESSAGE_MAP(CAirQuality, CFormView)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CAirQuality diagnostics

#ifdef _DEBUG
void CAirQuality::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CAirQuality::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}


#endif
#endif //_DEBUG


// CAirQuality message handlers


void CAirQuality::Fresh()
{
	SetTimer(AirQualityTimer,1000,NULL);

	GetDatabase();
	ShowAirqualityDialog();


}
void CAirQuality::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: Add your specialized code here and/or call the base class
}

BOOL CAirQuality::GetDatabase()
{

	CADO m_ado;
	m_ado.OnInitADOConn();

	if(ifAQdb)
	{
		//初始化flesgrid控件,在T3000关闭前只做一次这样的初始化
		InitialFlesgrid();

		//判断AirQuality数据库是否存在,Auto/Manual text,Range text

		CString temptable = _T("AirQuality");
		CString tempsql = _T("create table AirQuality(Address Number Primary Key,Data text)");//

		bool m_judge = m_ado.IsHaveTable(m_ado,temptable);

		//不存在 创建数据库表
		if (!m_judge) 
		{	
			m_ado.m_pRecordset = m_ado.OpenRecordset(tempsql);
			//初始化表
			CString sql = _T("select * from AirQuality");
			m_ado.m_pRecordset = m_ado.OpenRecordset(sql);
			if (m_ado.m_pRecordset->EndOfFile)
			{

				for(int i = 0;i<400;i++)
				{
					try 
					{
						m_ado.m_pRecordset->AddNew();
						m_ado.m_pRecordset->PutCollect("Address",(_bstr_t)(i+1));
						m_ado.m_pRecordset->PutCollect("Data",(_bstr_t)"0");
						m_ado.m_pRecordset->Update();

					}


					catch(...)
					{

					}
				}
			}


		}

//读取串口号
	GetSerialComm(m_szComm);	
	comnum = _wtoi(m_szComm[0].Mid(3));




		ifAQdb = FALSE;
	}

#if 1
	vecairquality.clear();
	CString SQL = _T("select * from AirQuality"); 
	m_ado.m_pRecordset = m_ado.OpenRecordset(SQL);
	_variant_t vartemp;
	while(!m_ado.m_pRecordset->EndOfFile)
	{
		m_vecairquality.iaddress =m_ado.m_pRecordset->GetCollect(_T("Address"));


		vartemp =m_ado.m_pRecordset->GetCollect(_T("Data"));
		if (vartemp.vt==VT_NULL)
			m_vecairquality.CStvalue =_T("");
		else
			m_vecairquality.CStvalue =vartemp;


		m_ado.m_pRecordset->MoveNext();

		vecairquality.push_back(m_vecairquality);
	}

	//读取本电脑当前正在使用的串口。
#if 0//会出问题，就是如果在数据存的这个串口与电脑的串口号不对应，则本类永远无法有正确数据采集？
	CString CSTcompot;
	BOOL BOdefault;
	SQL = _T("select * from Building");
	m_ado.m_pRecordset = m_ado.OpenRecordset(SQL);
	while(!m_ado.m_pRecordset->EndOfFile)
	{
		CSTcompot =(_variant_t)m_ado.m_pRecordset->GetCollect(_T("Com_Port"));
		BOdefault =(_variant_t)m_ado.m_pRecordset->GetCollect(_T("Default_SubBuilding"));//TRUE = -1即非0，FALSE就是0
		m_ado.m_pRecordset->MoveNext();

		if (BOdefault)
		{
			comnum = _wtoi(CSTcompot.Mid(3));
		}

	}

#endif

	m_ado.CloseRecordset();
	m_ado.CloseConn();



#endif 



	return TRUE;





}

void CAirQuality::InitialFlesgrid()
{



	//显示横标题
	m_msflesgrid.put_TextMatrix(0,0,_T("Address"));
	m_msflesgrid.put_TextMatrix(0,1,_T("Current Value"));
	m_msflesgrid.put_TextMatrix(0,2,_T("Description"));

	//设置排/行数量
	m_msflesgrid.put_Cols(3);
	m_msflesgrid.put_Rows(33);//包括标题栏


	//设置列宽	
	m_msflesgrid.put_ColWidth(0,1000);
	m_msflesgrid.put_ColWidth(1,1500);
	m_msflesgrid.put_ColWidth(2,5000);
	//居中显示
	for (int col=0;col<3;col++)
	{ 
		m_msflesgrid.put_ColAlignment(col,3);
	}

	//彩色显示
	for(int i=1;i<33;i++)		//排数量
	{

		for(int k=0;k<3;k++)	//列数量
		{
			if (i%2==1)
			{
				m_msflesgrid.put_Row(i);m_msflesgrid.put_Col(k);m_msflesgrid.put_CellBackColor(RGB(255,255,255));
			}
			else
			{
				m_msflesgrid.put_Row(i);m_msflesgrid.put_Col(k);m_msflesgrid.put_CellBackColor(COLOR_CELL);
			}
		}
	}


	m_msflesgrid.put_TextMatrix(1,2,_T("Temperature Calibration register"));	
	m_msflesgrid.put_TextMatrix(2,2,_T("Humidity Calibration"));
	m_msflesgrid.put_TextMatrix(3,2,_T("Humidity Calibration"));
	m_msflesgrid.put_TextMatrix(4,2,_T("Humidity Calibration"));
	m_msflesgrid.put_TextMatrix(5,2,_T("Humidity Calibration"));
	m_msflesgrid.put_TextMatrix(6,2,_T("Humidity Calibration"));
	m_msflesgrid.put_TextMatrix(7,2,_T("Humidity Calibration"));
	m_msflesgrid.put_TextMatrix(8,2,_T("Humidity Calibration"));
	m_msflesgrid.put_TextMatrix(9,2,_T("Humidity Calibration"));
	m_msflesgrid.put_TextMatrix(10,2,_T("Additional Humidity calibration points"));
	m_msflesgrid.put_TextMatrix(11,2,_T("Additional Humidity calibration points"));
	m_msflesgrid.put_TextMatrix(12,2,_T("Additional Humidity calibration points"));
	m_msflesgrid.put_TextMatrix(13,2,_T("Additional Humidity calibration points"));
	m_msflesgrid.put_TextMatrix(14,2,_T("Additional Humidity calibration points"));
	m_msflesgrid.put_TextMatrix(15,2,_T("Additional Humidity calibration points"));
	m_msflesgrid.put_TextMatrix(16,2,_T("Additional Humidity calibration points"));
	m_msflesgrid.put_TextMatrix(17,2,_T("Additional Humidity calibration points"));
	m_msflesgrid.put_TextMatrix(18,2,_T("Additional Humidity calibration points"));
	m_msflesgrid.put_TextMatrix(19,2,_T("Additional Humidity calibration points"));
 	m_msflesgrid.put_TextMatrix(20,2,_T("Additional Humidity calibration points"));
 	m_msflesgrid.put_TextMatrix(21,2,_T("Additional Humidity calibration points"));
// 	m_msflesgrid.put_TextMatrix(22,2,_T("Temperature Calibration register"));
// 	m_msflesgrid.put_TextMatrix(23,2,_T("Temperature Calibration register"));
// 	m_msflesgrid.put_TextMatrix(24,2,_T("Temperature Calibration register"));



#if 0
	//显示纵标题
	CString str;
	for(int i=1;i<33;i++)
	{

//		str.Format(_T("%d"),i);

//		m_msflesgrid.put_TextMatrix(i,0,str);	 

		// 		str = _T("Input ")+str;
		// 		m_msflexgridinput.put_TextMatrix(i,1,str);
		m_msflesgrid.put_TextMatrix(i,2,_T("Default"));	

	}
#endif




}

void CAirQuality::ShowAirqualityDialog()
{
#if 1

	//============================================================System Parameters===============================================================

	m_CStrModel = _T("Air Quality");
	//6	1	Modbus device address
	m_inaddress = _wtoi(vecairquality.at(6).CStvalue);

	UINT nSelectSerialNumber = _wtoi(vecairquality.at(0).CStvalue)+_wtoi(vecairquality.at(1).CStvalue)*256+_wtoi(vecairquality.at(2).CStvalue)*256*256+_wtoi(vecairquality.at(3).CStvalue)*256*256*256;
	m_inSerialNum=nSelectSerialNumber;

	m_flFirmware = ((float)(_wtoi(vecairquality.at(5).CStvalue)*256+_wtoi(vecairquality.at(4).CStvalue)))/10;

	m_inHardware = _wtoi(vecairquality.at(8).CStvalue);
	int intemp = _wtoi(vecairquality.at(185).CStvalue);
	if (intemp == 0)
		m_inBaudrate = 9600;
	else if (intemp == 1)
		m_inBaudrate = 19200;


	CString strresult;


	CString straddress;
	straddress.Format(_T("%d"),vecairquality.at(216).iaddress);
	m_msflesgrid.put_TextMatrix(1,0,straddress);
	m_msflesgrid.put_TextMatrix(1,1,vecairquality.at(216).CStvalue);


	for(int i = 219;i<239;i++)
	{  

		CString straddress;
		straddress.Format(_T("%d"),vecairquality.at(i).iaddress);
		m_msflesgrid.put_TextMatrix(i-219+2,0,straddress);
		m_msflesgrid.put_TextMatrix(i-219+2,1,vecairquality.at(i).CStvalue);
		//m_msflesgrid.put_TextMatrix(i,2,vecairquality.at(i).CStrange);
	}




	UpdateData(FALSE);
#endif


}
void CAirQuality::OnTimer(UINT_PTR nIDEvent)
{
	CMainFrame* pMain = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	CView* pNewView = pMain->m_pViews[7];
	CView* pActiveView = pMain->GetActiveView();

	if ( pNewView != pActiveView )    // current is minipanel dialog
	{
		KillTimer(AirQualityTimer);
	}

#if 1
	static int nCount = 0,nCount2 = 0,scantime = 30;

	nCount++;
	nCount2++;

	if (AirQualityTimer == nIDEvent)
	{
		CTime now;
		now = CTime::GetCurrentTime();
		m_clock = now.Format(_T("%A,%B %d,%Y  %H:%M:%S"));		// "Saturday, April 12, 2012"		

		UpdateData(FALSE);

		if (nCount%scantime == 0)
		{

			nCount = 0;

			if (prodtopc())
			{
				ShowAirqualityDialog();
				SetPaneString(1,_T("Online!"));

				if (writedb)
				{
					pctodb();
					writedb = FALSE;
					scantime = 60*3;
				}                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       
			}
			

		}



	}

#endif


	CFormView::OnTimer(nIDEvent);
}

BOOL CAirQuality::prodtopc()
{
	KillTimer(AirQualityTimer);
#if 1
	//读取串口号，从数据库

	//open_com(comnum);

	int sumret = 0;
	BOOL flag = FALSE;

	//读取AirQuality数据
	BOOL ret_bool = FALSE;
	long start = clock(),end(0); 		


	memset(&shmPRegister,0,sizeof(shmPRegister)); 
	for(int i=0;i<4;i++)
	{
		int ret = Read_Multi(g_tstat_id,&shmPRegister[i*64],i*64,64,2);
		sumret += ret;

		if (sumret<0)
		{
			TRACE(_T("write_multi ret sum = %d\n"),sumret);
			SetTimer(AirQualityTimer,1000,NULL);
			return FALSE;
		}

	}



	for (int i = 0;i<400;i++)
	{
		//	vecminipanel.at(i).CStvalue = shmPRegister[i];		// error C2593: 'operator =' is ambiguous
		vecairquality.at(i).CStvalue.Format(_T("%d"),shmPRegister[i]);	


	}

	if (sumret>200)
	{
		flag = TRUE;
	}
	TRACE(_T("write_multi ret sum = %d\n"),sumret);

	end = clock();
	long resultvalue = (end - start)/1000;
	TRACE(_T("GetData() usetime %d second\n"),resultvalue);


	SetTimer(AirQualityTimer,1000,NULL);
#endif
	return flag;
}

void CAirQuality::pctodb()
{
	TRACE(_T("Updatedatabase()start!\n"));
	KillTimer(AirQualityTimer);
	//存入数据库中
	CADO saveADO;
	saveADO.OnInitADOConn();
	CString sql = _T("select * from AirQuality");
	saveADO.m_pRecordset = saveADO.OpenRecordset(sql);
	if (!saveADO.m_pRecordset->EndOfFile)
	{
		saveADO.m_pRecordset->MoveFirst();

		for(int i = 0;i<400;i++)
		{
			try 
			{

				saveADO.m_pRecordset->PutCollect("Address",(_bstr_t)vecairquality.at(i).iaddress);
				saveADO.m_pRecordset->PutCollect("Data",(_bstr_t)vecairquality.at(i).CStvalue);


				saveADO.m_pRecordset->Update();
				saveADO.m_pRecordset->MoveNext();
			}


			catch(...)
			{
				SetPaneString(2,_T("Write dababase false!"));
				return ;
			}
		}
	}
	else
	{
		for(int i = 0;i<400;i++)
		{
			try 
			{

				saveADO.m_pRecordset->AddNew();
				saveADO.m_pRecordset->PutCollect("Address",(_bstr_t)vecairquality.at(i).iaddress);
				saveADO.m_pRecordset->PutCollect("Data",(_bstr_t)vecairquality.at(i).CStvalue);

				saveADO.m_pRecordset->Update();

			}


			catch(...)
			{
				SetPaneString(2,_T("Write dababase false!"));
				return;
			}
		}

	}

	saveADO.CloseRecordset();
	saveADO.CloseConn(); 

	SetTimer(AirQualityTimer,1000,NULL);

}

BOOL CAirQuality::GetSerialComm( vector<CString>& szComm )
{
	LPCTSTR strRegEntry = _T("HARDWARE\\DEVICEMAP\\SERIALCOMM\\");

	HKEY   hKey;   
	LONG   lReturnCode=0;   
	lReturnCode=::RegOpenKeyEx(HKEY_LOCAL_MACHINE, strRegEntry, 0, KEY_READ, &hKey);   

	if(lReturnCode==ERROR_SUCCESS)   
	{  
		DWORD dwIndex = 0;
		WCHAR lpValueName[MAX_PATH];
		ZeroMemory(lpValueName, MAX_PATH);
		DWORD lpcchValueName = MAX_PATH; 
		DWORD lpReserved = 0; 
		DWORD lpType = REG_SZ; 
		BYTE		lpData[MAX_PATH]; 
		ZeroMemory(lpData, MAX_PATH);
		DWORD lpcbData = MAX_PATH;
		dwIndex = 0;
		while(RegEnumValue(	hKey, dwIndex, lpValueName, &lpcchValueName, 0, &lpType, lpData, &lpcbData ) != ERROR_NO_MORE_ITEMS)
		{   
			//TRACE("Registry's   Read!");   
			dwIndex++;

			lpcchValueName = MAX_PATH; 
			lpValueName[0] = '\0'; 

			WCHAR ch[MAX_PATH];
			ZeroMemory(ch, MAX_PATH);
			memcpy(ch, lpData, lpcbData);
			CString str = CString(ch);
			szComm.push_back(str);

			ZeroMemory(lpData, MAX_PATH);
			lpcbData = MAX_PATH;

		}   
		::RegCloseKey(hKey);   		   

		return TRUE;
	}

	return FALSE;  

}



