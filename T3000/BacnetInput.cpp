﻿// BacnetInput.cpp : implementation file
// BacnetInput File Add by Fance 2013 0620

#include "stdafx.h"
#include "T3000.h"
#include "BacnetInput.h"
#include "afxdialogex.h"
#include "CM5/ud_str.h"
#include "Bacnet_Include.h"
#include "globle_function.h"
#include <math.h>
Str_in_point m_temp_Input_data[BAC_INPUT_ITEM_COUNT];
#include "gloab_define.h"
#include "BacnetRange.h"
extern void copy_data_to_ptrpanel(int Data_type);//Used for copy the structure to the ptrpanel.


// CBacnetInput dialog


IMPLEMENT_DYNAMIC(CBacnetInput, CDialogEx)

CBacnetInput::CBacnetInput(CWnd* pParent /*=NULL*/)
	: CDialogEx(CBacnetInput::IDD, pParent)
{

}


CBacnetInput::~CBacnetInput()
{
}

void CBacnetInput::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_input_list);
}


BEGIN_MESSAGE_MAP(CBacnetInput, CDialogEx)
	ON_MESSAGE(MY_RESUME_DATA, InputMessageCallBack)
	ON_MESSAGE(WM_REFRESH_BAC_INPUT_LIST,Fresh_Input_List)
	ON_MESSAGE(WM_LIST_ITEM_CHANGED,Fresh_Input_Item)	
	ON_BN_CLICKED(IDOK, &CBacnetInput::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_INPUT_APPLY, &CBacnetInput::OnBnClickedButtonApply)
	ON_BN_CLICKED(IDC_BUTTON_INPUT_READ, &CBacnetInput::OnBnClickedButtonRead)
	ON_NOTIFY(NM_CLICK, IDC_LIST1, &CBacnetInput::OnNMClickList1)
	ON_WM_TIMER()
	ON_WM_CLOSE()
END_MESSAGE_MAP()



// CBacnetInput message handlers
LRESULT  CBacnetInput::InputMessageCallBack(WPARAM wParam, LPARAM lParam)
{
	_MessageInvokeIDInfo *pInvoke =(_MessageInvokeIDInfo *)lParam;
	bool msg_result=WRITE_FAIL;
	msg_result = MKBOOL(wParam);
	CString Show_Results;
	CString temp_cs = pInvoke->task_info;
	if(msg_result)
	{
		Show_Results = temp_cs + _T("Success!");
		SetPaneString(BAC_SHOW_MISSION_RESULTS,Show_Results);

		//Post_Refresh_Message(g_bac_instance,READINPUT_T3000,pInvoke->mRow,pInvoke->mRow,sizeof(Str_in_point), BAC_INPUT_GROUP);
		//MessageBox(_T("Bacnet operation success!"));
	}
	else
	{
		memcpy_s(&m_Input_data.at(pInvoke->mRow),sizeof(Str_in_point),&m_temp_Input_data[pInvoke->mRow],sizeof(Str_in_point));//还原没有改对的值
		PostMessage(WM_REFRESH_BAC_INPUT_LIST,pInvoke->mRow,REFRESH_ON_ITEM);//这里调用 刷新线程重新刷新会方便一点;
		Show_Results = temp_cs + _T("Fail!");
		SetPaneString(BAC_SHOW_MISSION_RESULTS,Show_Results);

		//AfxMessageBox(Show_Results);
		//MessageBox(_T("Bacnet operation fail!"));
	}
	if((pInvoke->mRow%2)==0)	//恢复前景和 背景 颜色;
		m_input_list.SetItemBkColor(pInvoke->mRow,pInvoke->mCol,LIST_ITEM_DEFAULT_BKCOLOR,0);
	else
		m_input_list.SetItemBkColor(pInvoke->mRow,pInvoke->mCol,LIST_ITEM_DEFAULT_BKCOLOR_GRAY,0);
	m_input_list.RedrawItems(pInvoke->mRow,pInvoke->mRow);


	if(pInvoke)
		delete pInvoke;
	return 0;
}


BOOL CBacnetInput::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	Initial_List();
	PostMessage(WM_REFRESH_BAC_INPUT_LIST,NULL,NULL);

	hIcon   = AfxGetApp()->LoadIcon(IDI_ICON_REFRESH);
	((CButton *)GetDlgItem(IDC_BUTTON_INPUT_READ))->SetIcon(hIcon);	
	hIcon   = AfxGetApp()->LoadIcon(IDI_ICON_OK);
	((CButton *)GetDlgItem(IDC_BUTTON_INPUT_APPLY))->SetIcon(hIcon);
	SetTimer(1,BAC_LIST_REFRESH_TIME,NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CBacnetInput::Initial_List()
{
	m_input_list.ModifyStyle(0, LVS_SINGLESEL|LVS_REPORT|LVS_SHOWSELALWAYS);
	//m_input_list.SetExtendedStyle(m_input_list.GetExtendedStyle() |LVS_EX_FULLROWSELECT |LVS_EX_GRIDLINES);
	m_input_list.SetExtendedStyle(m_input_list.GetExtendedStyle()  |LVS_EX_GRIDLINES&(~LVS_EX_FULLROWSELECT));//Not allow full row select.
	m_input_list.InsertColumn(INPUT_NUM, _T("NUM"), 50, ListCtrlEx::Normal, LVCFMT_CENTER, ListCtrlEx::SortByDigit);
	m_input_list.InsertColumn(INPUT_FULL_LABLE, _T("Full Label"), 100, ListCtrlEx::EditBox, LVCFMT_CENTER, ListCtrlEx::SortByString);
	m_input_list.InsertColumn(INPUT_AUTO_MANUAL, _T("Auto/Manual"), 80, ListCtrlEx::ComboBox, LVCFMT_CENTER, ListCtrlEx::SortByString);
	m_input_list.InsertColumn(INPUT_VALUE, _T("Value"), 80, ListCtrlEx::EditBox, LVCFMT_CENTER, ListCtrlEx::SortByString);
	m_input_list.InsertColumn(INPUT_UNITE, _T("Units"), 80, ListCtrlEx::Normal, LVCFMT_CENTER, ListCtrlEx::SortByString);
	m_input_list.InsertColumn(INPUT_RANGE, _T("Range"), 100, ListCtrlEx::ComboBox, LVCFMT_CENTER, ListCtrlEx::SortByString);
	m_input_list.InsertColumn(INPUT_CAL, _T("Calibration"), 80, ListCtrlEx::EditBox, LVCFMT_CENTER, ListCtrlEx::SortByString);
	m_input_list.InsertColumn(INPUT_FITLER, _T("Filter"), 80, ListCtrlEx::ComboBox, LVCFMT_CENTER, ListCtrlEx::SortByString);
	m_input_list.InsertColumn(INPUT_DECOM, _T("Status"), 80, ListCtrlEx::ComboBox, LVCFMT_CENTER, ListCtrlEx::SortByString);
	m_input_list.InsertColumn(INPUT_LABLE, _T("Label"), 80, ListCtrlEx::EditBox, LVCFMT_CENTER, ListCtrlEx::SortByString);
	m_input_dlg_hwnd = this->m_hWnd;
	g_hwnd_now = m_input_dlg_hwnd;

	CRect list_rect,win_rect;
	m_input_list.GetWindowRect(list_rect);
	ScreenToClient(&list_rect);
	::GetWindowRect(m_input_dlg_hwnd,win_rect);
	m_input_list.Set_My_WindowRect(win_rect);
	m_input_list.Set_My_ListRect(list_rect);


	m_input_list.DeleteAllItems();
	for (int i=0;i<(int)m_Input_data.size();i++)
	{
		CString temp_item,temp_value,temp_cal,temp_filter,temp_status,temp_lable;
		CString temp_des;
		CString temp_units;
		temp_item.Format(_T("%d"),i+1);
		m_input_list.InsertItem(i,temp_item);
		if(ListCtrlEx::ComboBox == m_input_list.GetColumnType(INPUT_AUTO_MANUAL))
		{
			ListCtrlEx::CStrList strlist;
			strlist.push_back(_T("Auto"));
			strlist.push_back(_T("Manual"));
			m_input_list.SetCellStringList(i, INPUT_AUTO_MANUAL, strlist);
		}


		if(ListCtrlEx::ComboBox == m_input_list.GetColumnType(INPUT_RANGE))
		{
			ListCtrlEx::CStrList strlist;
			for (int i=0;i<(int)sizeof(Units_Type)/sizeof(Units_Type[0]);i++)
			{
				strlist.push_back(Units_Type[i]);
			}
			m_input_list.SetCellStringList(i, INPUT_RANGE, strlist);		
		}



		//if(ListCtrlEx::ComboBox == m_input_list.GetColumnType(INPUT_RANGE))
		//{
		//	ListCtrlEx::CStrList strlist;
		//	int test = sizeof(Input_Range_Array)/sizeof(Input_Range_Array[0]);
		//	for (int loop=0;loop<(int)sizeof(Input_Range_Array)/sizeof(Input_Range_Array[0]);loop++)
		//	{
		//		strlist.push_back(Input_Range_Array[loop]) ;
		//	}
		//	m_input_list.SetCellStringList(i, INPUT_RANGE, strlist);
		//}

		if(ListCtrlEx::ComboBox == m_input_list.GetColumnType(INPUT_FITLER))
		{
			ListCtrlEx::CStrList strlist;
			for (int m=0;m<(int)(sizeof(Input_Filter_Array)/sizeof(Input_Filter_Array[0]));m++)
			{
				strlist.push_back(Input_Filter_Array[m]);
			}
			m_input_list.SetCellStringList(i, INPUT_FITLER, strlist);
		}

		if(ListCtrlEx::ComboBox == m_input_list.GetColumnType(INPUT_DECOM))
		{
			ListCtrlEx::CStrList strlist;
			for (int m=0;m<(int)(sizeof(Decom_Array)/sizeof(Decom_Array[0]));m++)
			{
				strlist.push_back(Decom_Array[m]);
			}
			m_input_list.SetCellStringList(i, INPUT_DECOM, strlist);
		}

		for (int x=0;x<INPUT_COL_NUMBER;x++)
		{
			if((i%2)==0)
				m_input_list.SetItemBkColor(i,x,LIST_ITEM_DEFAULT_BKCOLOR);
			else
				m_input_list.SetItemBkColor(i,x,LIST_ITEM_DEFAULT_BKCOLOR_GRAY);		
		}

		

	}
}

void CBacnetInput::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
//	CDialogEx::OnOK();
}
LRESULT CBacnetInput::Fresh_Input_Item(WPARAM wParam,LPARAM lParam)
{
	int cmp_ret ;//compare if match it will 0;
	int Changed_Item = (int)wParam;
	int Changed_SubItem = (int)lParam;

	memcpy_s(&m_temp_Input_data[Changed_Item],sizeof(Str_in_point),&m_Input_data.at(Changed_Item),sizeof(Str_in_point));

	CString temp_task_info;
	CString New_CString =  m_input_list.GetItemText(Changed_Item,Changed_SubItem);
	CString cstemp_value;
	if(Changed_SubItem == INPUT_LABLE)
	{
		CString cs_temp = m_input_list.GetItemText(Changed_Item,Changed_SubItem);
		if(cs_temp.GetLength()>= STR_IN_LABEL)	//长度不能大于结构体定义的长度;
		{
			MessageBox(_T("Length can not higher than 8"),_T("Warning"));
			PostMessage(WM_REFRESH_BAC_INPUT_LIST,NULL,NULL);
			return 0;
		}
		char cTemp1[255];
		memset(cTemp1,0,255);
		WideCharToMultiByte( CP_ACP, 0, cs_temp.GetBuffer(), -1, cTemp1, 255, NULL, NULL );
		memcpy_s(m_Input_data.at(Changed_Item).label,STR_IN_LABEL,cTemp1,STR_IN_LABEL);
	}

	if(Changed_SubItem == INPUT_FULL_LABLE)
	{
		CString cs_temp = m_input_list.GetItemText(Changed_Item,Changed_SubItem);
		if(cs_temp.GetLength()>= STR_IN_DESCRIPTION_LENGTH)	//长度不能大于结构体定义的长度;
		{
			MessageBox(_T("Length can not higher than 20"),_T("Warning"));
			PostMessage(WM_REFRESH_BAC_INPUT_LIST,NULL,NULL);
			return 0;
		}

		char cTemp1[255];
		memset(cTemp1,0,255);
		WideCharToMultiByte( CP_ACP, 0, cs_temp.GetBuffer(), -1, cTemp1, 255, NULL, NULL );
		memcpy_s(m_Input_data.at(Changed_Item).description,STR_IN_DESCRIPTION_LENGTH,cTemp1,STR_IN_DESCRIPTION_LENGTH);
	}


	if(Changed_SubItem == INPUT_AUTO_MANUAL)
	{
		CString temp_cs = m_input_list.GetItemText(Changed_Item,Changed_SubItem);
		if(temp_cs.CompareNoCase(_T("Auto"))==0)
		{
			m_input_list.SetCellEnabled(Changed_Item,INPUT_VALUE,0);
			m_Input_data.at(Changed_Item).auto_manual = BAC_AUTO ;
		}
		else
		{
			m_input_list.SetCellEnabled(Changed_Item,INPUT_VALUE,1);
			m_Input_data.at(Changed_Item).auto_manual = BAC_MANUAL ;
		}
	}

	if(Changed_SubItem == INPUT_VALUE)
	{
		CString temp_cs = m_input_list.GetItemText(Changed_Item,Changed_SubItem);

		int temp_int = (int)(_wtof(temp_cs) * 1000);
		//int temp_int = _wtoi(temp_cs);
		m_Input_data.at(Changed_Item).value = temp_int;
	}

	if(Changed_SubItem == INPUT_RANGE)
	{
		CString temp_cs = m_input_list.GetItemText(Changed_Item,Changed_SubItem);
		BacnetRange dlg;
		if(temp_cs.CompareNoCase(Units_Type[UNITS_TYPE_ANALOG])==0)
		{
			bac_ranges_type = INPUT_RANGE_ANALOG_TYPE;
			dlg.DoModal();
			if(range_cancel)
			{
				PostMessage(WM_REFRESH_BAC_INPUT_LIST,Changed_Item,REFRESH_ON_ITEM);//这里调用 刷新线程重新刷新会方便一点;
				return 0;
			}
			m_Input_data.at(Changed_Item).digital_analog =  BAC_UNITS_ANALOG;
			m_Input_data.at(Changed_Item).range =  bac_range_number_choose;
			m_input_list.SetItemText(Changed_Item,INPUT_RANGE,Input_Analog_Units_Array[bac_range_number_choose]);		
			m_input_list.SetItemText(Changed_Item,INPUT_UNITE,Input_List_Analog_Units[bac_range_number_choose]);	
			
			cstemp_value.Format(_T("%d"),m_Input_data.at(Changed_Item).calibration);
			m_input_list.SetItemText(Changed_Item,INPUT_CAL,cstemp_value);


			m_input_list.SetCellEnabled(Changed_Item,INPUT_CAL,1);
			m_input_list.SetCellEnabled(Changed_Item,INPUT_UNITE,1);
			//m_input_list.SetItemText(Changed_Item,OUTPUT_100_PERSENT,_T("10"));
			//m_input_list.SetCellEnabled(Changed_Item,OUTPUT_100_PERSENT,1);

			CString cstemp_value;
			float temp_float_value;
			temp_float_value = ((float)m_Input_data.at(Changed_Item).value) / 1000;
			cstemp_value.Format(_T("%.2f"),temp_float_value);
			m_input_list.SetItemText(Changed_Item,INPUT_VALUE,cstemp_value);	

			//cstemp_value.Format(_T("%d"),m_Input_data.at(Changed_Item).value);
			//m_input_list.SetItemText(Changed_Item,INPUT_VALUE,cstemp_value);	
		}
		else if(temp_cs.CompareNoCase(Units_Type[UNITS_TYPE_DIGITAL])==0)
		{
			bac_range_number_choose = m_Input_data.at(Changed_Item).range;
			bac_ranges_type = INPUT_RANGE_DIGITAL_TYPE;
			dlg.DoModal();
			if(range_cancel)
			{
				PostMessage(WM_REFRESH_BAC_INPUT_LIST,Changed_Item,REFRESH_ON_ITEM);//这里调用 刷新线程重新刷新会方便一点;
				return 0;
			}
			m_Input_data.at(Changed_Item).digital_analog =  BAC_UNITS_DIGITAL;
			m_Input_data.at(Changed_Item).range =  bac_range_number_choose;
			//m_input_list.SetItemText(Changed_Item,INPUT_RANGE,INPUT_Digital_Units_Show[bac_range_number_choose]);		

			m_input_list.SetItemText(Changed_Item,INPUT_CAL,_T(""));
			m_input_list.SetCellEnabled(Changed_Item,INPUT_CAL,0);
			m_input_list.SetItemText(Changed_Item,INPUT_UNITE,_T(""));
			m_input_list.SetCellEnabled(Changed_Item,INPUT_UNITE,0);

			CString temp1;
			CStringArray temparray;
			temp1 = Digital_Units_Array[bac_range_number_choose];//22 is the sizeof the array
			SplitCStringA(temparray,temp1,_T("/"));

			if(m_Input_data.at(Changed_Item).control == 1)
			{
				if((temparray.GetSize()==2)&&(!temparray.GetAt(1).IsEmpty()))
				{
					m_input_list.SetItemText(Changed_Item,INPUT_VALUE,temparray.GetAt(1));
				}
			}
			else
			{
				if((temparray.GetSize()==2)&&(!temparray.GetAt(0).IsEmpty()))
				{
					m_input_list.SetItemText(Changed_Item,INPUT_VALUE,temparray.GetAt(0));
				}			
			}
			m_input_list.SetItemText(Changed_Item,INPUT_RANGE,temp1);


		}
		else if(temp_cs.CompareNoCase(Units_Type[UNITS_TYPE_CUSTOM])==0)
		{
			bac_ranges_type = OUTPUT_RANGE_CUSTOM_DIG_TYPE;
			//dlg.DoModal();
		}	
		else
		{
			PostMessage(WM_REFRESH_BAC_INPUT_LIST,Changed_Item,REFRESH_ON_ITEM);//这里调用 刷新线程重新刷新会方便一点;
			//m_input_list.SetItemText(Changed_Item,INPUT_RANGE,temp_cs);
		}
	}


	//if(Changed_SubItem==INPUT_RANGE)
	//{
	//	CString temp_cs = m_input_list.GetItemText(Changed_Item,Changed_SubItem);
	//	if(temp_cs.CompareNoCase(_T("Not Used"))==0)
	//	{
	//		m_input_list.SetItemText(Changed_Item,INPUT_UNITE,Input_Unit[_unused]);
	//	}
	//	else if(temp_cs.CompareNoCase(_T("10K(-40->120)"))==0)
	//	{
	//		ListCtrlEx::CStrList strlist;
	//		strlist.clear();
	//		strlist.push_back(Input_Unit[degC]);
	//		//strlist.push_back(Input_Unit[degF]);
	//		m_input_list.SetCellStringList(Changed_Item, INPUT_UNITE, strlist);
	//		m_input_list.SetItemText(Changed_Item,INPUT_UNITE,Input_Unit[degC]);
	//	}
	//	else if(temp_cs.CompareNoCase(_T("I 4->20ma"))==0)
	//	{
	//		ListCtrlEx::CStrList strlist;
	//		strlist.clear();
	//		TRACE(Input_Unit[ma]);
	//		strlist.push_back(Input_Unit[ma]);
	//		m_input_list.SetCellStringList(Changed_Item, INPUT_UNITE, strlist);
	//		m_input_list.SetItemText(Changed_Item,INPUT_UNITE,Input_Unit[ma]);
	//	}
	//	else if(temp_cs.CompareNoCase(_T("V 0->10V"))==0)
	//	{
	//		ListCtrlEx::CStrList strlist;
	//		strlist.clear();
	//		TRACE(Input_Unit[Volts]);
	//		strlist.push_back(Input_Unit[Volts]);
	//		m_input_list.SetCellStringList(Changed_Item, INPUT_UNITE, strlist);
	//		m_input_list.SetItemText(Changed_Item,INPUT_UNITE,Input_Unit[Volts]);
	//	}
	//	else if(temp_cs.CompareNoCase(_T("V 0->5V"))==0)
	//	{
	//		ListCtrlEx::CStrList strlist;
	//		strlist.clear();
	//		TRACE(Input_Unit[Volts]);
	//		strlist.push_back(Input_Unit[Volts]);
	//		m_input_list.SetCellStringList(Changed_Item, INPUT_UNITE, strlist);
	//		m_input_list.SetItemText(Changed_Item,INPUT_UNITE,Input_Unit[Volts]);
	//	}
	//	else if(temp_cs.CompareNoCase(_T("V 0->24AC"))==0)
	//	{
	//		ListCtrlEx::CStrList strlist;
	//		strlist.clear();
	//		strlist.push_back(_T("ON/OFF"));
	//		m_input_list.SetCellStringList(Changed_Item, INPUT_UNITE, strlist);
	//		m_input_list.SetItemText(Changed_Item,INPUT_UNITE,_T("ON/OFF"));
	//	}
	//	else if(temp_cs.CompareNoCase(_T("TST Normal"))==0)
	//	{
	//		ListCtrlEx::CStrList strlist;
	//		strlist.clear();
	//		strlist.push_back(_T("ON/OFF"));
	//		m_input_list.SetCellStringList(Changed_Item, INPUT_UNITE, strlist);
	//		m_input_list.SetItemText(Changed_Item,INPUT_UNITE,_T("ON/OFF"));
	//	}
	//}

	if(Changed_SubItem==INPUT_CAL)
	{
		CString cs_temp=m_input_list.GetItemText(Changed_Item,INPUT_CAL);
		int cal_value = _wtoi(cs_temp);
		m_Input_data.at(Changed_Item).calibration = cal_value;
	}
	else if(Changed_SubItem==INPUT_FITLER)
	{
		CString cs_temp=m_input_list.GetItemText(Changed_Item,INPUT_FITLER);
		int  temp2 = _wtoi(cs_temp);
		m_Input_data.at(Changed_Item).filter = (int8_t)(log((double)temp2)/log((double)2));
	}
	else if(Changed_SubItem==INPUT_DECOM)
	{
		CString cs_temp = m_input_list.GetItemText(Changed_Item,INPUT_DECOM);
		int dec_index=-1;
		for (int m=0;m<(int)sizeof(Decom_Array)/sizeof(Decom_Array[0]);m++)
		{
			if(cs_temp.CompareNoCase(Decom_Array[m])==0)
			{
				dec_index = m;
				break;
			}
		}
		m_Input_data.at(Changed_Item).decom = dec_index;
	}

	cmp_ret = memcmp(&m_temp_Input_data[Changed_Item],&m_Input_data.at(Changed_Item),sizeof(Str_in_point));
	if(cmp_ret!=0)
	{
		m_input_list.SetItemBkColor(Changed_Item,Changed_SubItem,LIST_ITEM_CHANGED_BKCOLOR);
		temp_task_info.Format(_T("Write Input List Item%d .Changed to \"%s\" "),Changed_Item + 1,New_CString);
		Post_Write_Message(g_bac_instance,WRITEINPUT_T3000,Changed_Item,Changed_Item,sizeof(Str_in_point),m_input_dlg_hwnd ,temp_task_info,Changed_Item,Changed_SubItem);
	}
	//m_input_list.Invalidate();
	return 0;
}

LRESULT CBacnetInput::Fresh_Input_List(WPARAM wParam,LPARAM lParam)
{
	// Str_in_point Get_Str_in_Point(int index);
	int Fresh_Item;
	int isFreshOne = (int)lParam;
	if(isFreshOne == REFRESH_ON_ITEM)
	{
		Fresh_Item = (int)wParam;
	}
	else
	{
		if(m_input_list.IsDataNewer((char *)&m_Input_data.at(0),sizeof(Str_in_point) * BAC_INPUT_ITEM_COUNT))
		{
			//避免list 刷新时闪烁;在没有数据变动的情况下不刷新List;
			m_input_list.SetListData((char *)&m_Input_data.at(0),sizeof(Str_in_point) * BAC_INPUT_ITEM_COUNT);
		}
		else
		{
			return 0;
		}
	}
	if(0)
	{
		int temp_select_raw = 0;
		int temp_select_col = 0;
		m_input_list.Get_Selected_Item(temp_select_raw,temp_select_col);
		m_input_list.SetItemBkColor(temp_select_raw,temp_select_col,LIST_ITEM_SELECTED,0);
	}

	//m_input_list.DeleteAllItems();
	for (int i=0;i<(int)m_Input_data.size();i++)
	{
		CString temp_item,temp_value,temp_cal,temp_filter,temp_status,temp_lable;
		CString temp_des;
		CString temp_units;

		if(isFreshOne)
		{
			i = Fresh_Item;
		}

		MultiByteToWideChar( CP_ACP, 0, (char *)m_Input_data.at(i).description, (int)strlen((char *)m_Input_data.at(i).description)+1, 
			temp_des.GetBuffer(MAX_PATH), MAX_PATH );
		temp_des.ReleaseBuffer();

		m_input_list.SetItemText(i,INPUT_FULL_LABLE,temp_des);
		if(m_Input_data.at(i).auto_manual==0)
		{
			m_input_list.SetItemText(i,INPUT_AUTO_MANUAL,_T("Auto"));
			m_input_list.SetCellEnabled(i,INPUT_VALUE,0);
		}
		else
		{
			m_input_list.SetItemText(i,INPUT_AUTO_MANUAL,_T("Manual"));
			m_input_list.SetCellEnabled(i,INPUT_VALUE,1);
		}

		if(m_Input_data.at(i).digital_analog == BAC_UNITS_ANALOG)
		{

			m_input_list.SetCellEnabled(i,INPUT_CAL,1);


			m_input_list.SetItemText(i,INPUT_RANGE,Input_Analog_Units_Array[m_Input_data.at(i).range]);
			m_input_list.SetItemText(i,INPUT_UNITE,Input_List_Analog_Units[m_Input_data.at(i).range]);


			CString cstemp_value;
			float temp_float_value;
			temp_float_value = ((float)m_Input_data.at(i).value) / 1000;
			cstemp_value.Format(_T("%.2f"),temp_float_value);
			m_input_list.SetItemText(i,INPUT_VALUE,cstemp_value);

			//CString temp_value;
			//temp_value.Format(_T("%d"),m_Input_data.at(i).value);
			//m_input_list.SetItemText(i,INPUT_VALUE,temp_value);

			temp_cal.Format(_T("%d"),(m_Input_data.at(i).calibration));
			m_input_list.SetItemText(i,INPUT_CAL,temp_cal);
		}
		else if(m_Input_data.at(i).digital_analog == BAC_UNITS_DIGITAL)
		{

			m_input_list.SetItemText(i,INPUT_CAL,_T(""));
#if 0
			if(m_Input_data.at(i).range>=12)
				m_input_list.SetItemText(i,INPUT_RANGE,Digital_Units_Array[m_Input_data.at(i).range -11]);
			else
				m_input_list.SetItemText(i,INPUT_RANGE,Digital_Units_Array[m_Input_data.at(i).range]);
#endif
			if(m_Input_data.at(i).range<=22)
				m_input_list.SetItemText(i,INPUT_RANGE,Digital_Units_Array[m_Input_data.at(i).range]);
			else
				m_input_list.SetItemText(i,INPUT_RANGE,Digital_Units_Array[0]);
			m_input_list.SetItemText(i,INPUT_UNITE,_T(""));

			if((m_Input_data.at(i).range>22) || (m_Input_data.at(i).range == 0))
			{
				m_input_list.SetItemText(i,INPUT_UNITE,Digital_Units_Array[0]);
			}
			else
			{
				CString temp1;
				CStringArray temparray;
#if 0
				if(m_Input_data.at(i).range>=12)
					temp1 = Digital_Units_Array[m_Input_data.at(i).range - 11];//11 is the sizeof the array
				else
#endif
					temp1 = Digital_Units_Array[m_Input_data.at(i).range];
				SplitCStringA(temparray,temp1,_T("/"));
				if((temparray.GetSize()==2))
				{
					if(m_Input_data.at(i).control == 0)
						m_input_list.SetItemText(i,INPUT_VALUE,temparray.GetAt(0));
					else
						m_input_list.SetItemText(i,INPUT_VALUE,temparray.GetAt(1));
				}
			}

		}


		temp_filter.Format(_T("%d"),(int)pow((double)2,(int)m_Input_data.at(i).filter));
		m_input_list.SetItemText(i,INPUT_FITLER,temp_filter);

		if(m_Input_data.at(i).decom==0)
			temp_status.Format(Decom_Array[0]);
		else if(m_Input_data.at(i).decom==1)
			temp_status.Format(Decom_Array[1]);
		else
			temp_status.Empty();
		m_input_list.SetItemText(i,INPUT_DECOM,temp_status);


		CString temp_des2;
		MultiByteToWideChar( CP_ACP, 0, (char *)m_Input_data.at(i).label, (int)strlen((char *)m_Input_data.at(i).label)+1, 
			temp_des2.GetBuffer(MAX_PATH), MAX_PATH );
		temp_des2.ReleaseBuffer();

		m_input_list.SetItemText(i,INPUT_LABLE,temp_des2);
		if(isFreshOne)
		{
			break;
		}
		//ptr_panel
	}
	copy_data_to_ptrpanel(TYPE_INPUT);
	return 0;
}


void CBacnetInput::OnBnClickedButtonApply()
{
	// TODO: Add your control notification handler code here
	for (int i=0;i<(int)m_Input_data.size();i++)
	{
		CString cs_temp=m_input_list.GetItemText(i,INPUT_FULL_LABLE);
		char cTemp[255];
		memset(cTemp,0,255);
		WideCharToMultiByte( CP_ACP, 0, cs_temp.GetBuffer(), -1, cTemp, 255, NULL, NULL );
		memcpy_s(m_Input_data.at(i).description,21,cTemp,21);

		cs_temp=m_input_list.GetItemText(i,INPUT_AUTO_MANUAL);
		if(cs_temp.CompareNoCase(_T("Auto"))==0)
		{
			m_Input_data.at(i).auto_manual=0;
		}
		else
		{
			m_Input_data.at(i).auto_manual=1;
		}

		cs_temp=m_input_list.GetItemText(i,INPUT_RANGE);
		int index_number=0;
		for(int j=0;j<(int)sizeof(Input_Range_Array)/sizeof(Input_Range_Array[0]);j++)
		{
			if(cs_temp.CompareNoCase(Input_Range_Array[j])==0)
			{
				index_number=j;
				break;
			}
		}
		m_Input_data.at(i).range = index_number;

		cs_temp=m_input_list.GetItemText(i,INPUT_UNITE);
		index_number=0;
		for(int j=0;j<(int)sizeof(Input_Unit)/sizeof(Input_Unit[0]);j++)//unit单位很多，在列表中查找
		{
			if(cs_temp.CompareNoCase(Input_Unit[j])==0)
			{
				index_number=j;
				break;
			}
		}

		cs_temp = m_input_list.GetItemText(i,INPUT_VALUE);
		m_Input_data.at(i).value = _wtoi(cs_temp);


		cs_temp=m_input_list.GetItemText(i,INPUT_CAL);
		int cal_value = _wtoi(cs_temp);
		m_Input_data.at(i).calibration = cal_value;


		cs_temp=m_input_list.GetItemText(i,INPUT_FITLER);
		int  temp2 = _wtoi(cs_temp);
		m_Input_data.at(i).filter =(int8_t) (log((double)temp2)/log((double)2));


		cs_temp = m_input_list.GetItemText(i,INPUT_DECOM);
		int dec_index=-1;
		for (int m=0;m<(int)sizeof(Decom_Array)/sizeof(Decom_Array[0]);m++)
		{
			if(cs_temp.CompareNoCase(Decom_Array[m])==0)
			{
				dec_index = m;
				break;
			}
		}
		m_Input_data.at(i).decom = dec_index;

		cs_temp=m_input_list.GetItemText(i,INPUT_LABLE);
		char cTemp1[255];
		memset(cTemp1,0,255);
		WideCharToMultiByte( CP_ACP, 0, cs_temp.GetBuffer(), -1, cTemp1, 255, NULL, NULL );
		memcpy_s(m_Input_data.at(i).label,STR_IN_LABEL,cTemp1,STR_IN_LABEL);
	}
	Post_Write_Message(g_bac_instance,WRITEINPUT_T3000,0,19,sizeof(Str_in_point),BacNet_hwd);

}


void CBacnetInput::OnBnClickedButtonRead()
{
	//CPoint temp;
	//temp.x = 130;
	//temp.y = 201;
 //	::PostMessage((HWND)GetDlgItem(IDC_LIST1),WM_LBUTTONDOWN,1,0);
	// TODO: Add your control notification handler code here
	Post_Refresh_Message(g_bac_instance,READINPUT_T3000,0,BAC_INPUT_ITEM_COUNT - 1,sizeof(Str_in_point), BAC_INPUT_GROUP);
	PostMessage(WM_REFRESH_BAC_INPUT_LIST,NULL,NULL);
}






void CBacnetInput::OnNMClickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here

	long lRow,lCol;
	m_input_list.Set_Edit(true);
	DWORD dwPos=GetMessagePos();//Get which line is click by user.Set the check box, when user enter Insert it will jump to program dialog
	CPoint point( LOWORD(dwPos), HIWORD(dwPos));
	m_input_list.ScreenToClient(&point);
	LVHITTESTINFO lvinfo;
	lvinfo.pt=point;
	lvinfo.flags=LVHT_ABOVE;
	int nItem=m_input_list.SubItemHitTest(&lvinfo);

	lRow = lvinfo.iItem;
	lCol = lvinfo.iSubItem;


	if(lRow>m_input_list.GetItemCount()) //如果点击区超过最大行号，则点击是无效的
		return;
	if(lRow<0)
		return;

	if(lCol != INPUT_VALUE)	
		return;

	if(m_Input_data.at(lRow).digital_analog != BAC_UNITS_DIGITAL)
		return;

	if(m_Input_data.at(lRow).auto_manual == BAC_AUTO)	//If it is auto mode, disable to change the value.
		return;

	memcpy_s(&m_temp_Input_data[lRow],sizeof(Str_in_point),&m_Input_data.at(lRow),sizeof(Str_in_point));

	CString New_CString;
	CString temp_task_info;

	CString temp1;
	CStringArray temparray;
	if((m_Input_data.at(lRow).range < 23) &&(m_Input_data.at(lRow).range !=0))
		temp1 = Digital_Units_Array[m_Input_data.at(lRow).range];
	else
		return;
	SplitCStringA(temparray,temp1,_T("/"));

	if(m_Input_data.at(lRow).control == 0)
	{
		m_Input_data.at(lRow).control = 1;
		m_input_list.SetItemText(lRow,INPUT_VALUE,temparray.GetAt(1));
		New_CString = temparray.GetAt(1);
	}
	else
	{
		m_Input_data.at(lRow).control = 0;
		m_input_list.SetItemText(lRow,INPUT_VALUE,temparray.GetAt(0));
		New_CString = temparray.GetAt(0);
	}
#if 0
	if(m_Input_data.at(lRow).range > 11)
		temp1 = Digital_Units_Array[m_Input_data.at(lRow).range - 11];//11 is the sizeof the array
	else
		temp1 = Digital_Units_Array[m_Input_data.at(lRow).range];
	SplitCStringA(temparray,temp1,_T("/"));

	if(m_Input_data.at(lRow).range>=12)
	{

		if((temparray.GetSize()==2)&&(!temparray.GetAt(1).IsEmpty()))
		{
			m_input_list.SetItemText(lRow,INPUT_VALUE,temparray.GetAt(0));
			m_Input_data.at(lRow).range = m_Input_data.at(lRow).range - 11;
			New_CString = temparray.GetAt(0);
			m_Input_data.at(lRow).control = 0;	
		}

	}
	else if(m_Input_data.at(lRow).range>=1)
	{

		if((temparray.GetSize()==2)&&(!temparray.GetAt(0).IsEmpty()))
		{
			m_input_list.SetItemText(lRow,INPUT_VALUE,temparray.GetAt(1));
			m_Input_data.at(lRow).range = m_Input_data.at(lRow).range + 11;
			New_CString = temparray.GetAt(1);
			m_Input_data.at(lRow).control = 1;	
		}

	}
#endif
	m_input_list.Set_Edit(false);

	int cmp_ret = memcmp(&m_temp_Input_data[lRow],&m_Input_data.at(lRow),sizeof(Str_in_point));
	if(cmp_ret!=0)
	{
		m_input_list.SetItemBkColor(lRow,lCol,LIST_ITEM_CHANGED_BKCOLOR);
		temp_task_info.Format(_T("Write Input List Item%d .Changed to \"%s\" "),lRow + 1,New_CString);
		Post_Write_Message(g_bac_instance,WRITEINPUT_T3000,(int8_t)lRow,(int8_t)lRow,sizeof(Str_in_point),m_input_dlg_hwnd,temp_task_info,lRow,lCol);
	}



	*pResult = 0;
}


void CBacnetInput::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	if(this->IsWindowVisible())
	{
	PostMessage(WM_REFRESH_BAC_INPUT_LIST,NULL,NULL);
	if(bac_select_device_online)
		Post_Refresh_Message(g_bac_instance,READINPUT_T3000,0,BAC_INPUT_ITEM_COUNT - 1,sizeof(Str_in_point), BAC_INPUT_GROUP);
	}
	CDialogEx::OnTimer(nIDEvent);
}


void CBacnetInput::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	m_input_dlg_hwnd = NULL;
	::PostMessage(BacNet_hwd,WM_DELETE_NEW_MESSAGE_DLG,DELETE_WINDOW_MSG,0);
	CDialogEx::OnClose();
}
void CBacnetInput::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class
//	m_input_dlg_hwnd = NULL;
	::PostMessage(BacNet_hwd,WM_DELETE_NEW_MESSAGE_DLG,DELETE_WINDOW_MSG,0);
//	CDialogEx::OnCancel();
}


BOOL CBacnetInput::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if(pMsg->message==WM_KEYDOWN && pMsg->wParam==VK_RETURN) 
	{
		CRect list_rect,win_rect;
		m_input_list.GetWindowRect(list_rect);
		ScreenToClient(&list_rect);
		::GetWindowRect(m_input_dlg_hwnd,win_rect);
		m_input_list.Set_My_WindowRect(win_rect);
		m_input_list.Set_My_ListRect(list_rect);

		m_input_list.Get_clicked_mouse_position();
		return TRUE;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}



