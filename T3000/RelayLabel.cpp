// RelayLabel.cpp : implementation file
//

#include "stdafx.h"
#include "T3000.h"
#include "RelayLabel.h"
#include "globle_function.h"
#include "global_variable_extern.h"


// CRelayLabel

IMPLEMENT_DYNAMIC(CRelayLabel, CStatic)

CRelayLabel::CRelayLabel()
{
	m_bkClr=RGB(202,210,220);
}

CRelayLabel::~CRelayLabel()
{
}


BEGIN_MESSAGE_MAP(CRelayLabel, CStatic)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_DESTROY()
END_MESSAGE_MAP()



// CRelayLabel message handlers



BOOL CRelayLabel::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	//CRect rcClient;
//GetClientRect(&rcClient);
//	pDC->FillSolidRect(&rcClient,m_bkClr);
	//return 1;

	return CStatic::OnEraseBkgnd(pDC);
}

BOOL CRelayLabel::Create(LPCTSTR lpszText, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	// TODO: Add your specialized code here and/or call the base class
	//SetTimer(1,15000,NULL);
	m_fFirmwareVersion=get_curtstat_version();
	return CStatic::Create(lpszText, dwStyle, rect, pParentWnd, nID);
}

void CRelayLabel::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CStatic::OnPaint() for painting messages

	CRect rcClient;
	GetClientRect(&rcClient);
	dc.FillSolidRect(&rcClient,m_bkClr);
	dc.SetTextColor(m_bTxtClr);
	dc.DrawText(m_strValueText,&rcClient,DT_CENTER|DT_VCENTER|DT_SINGLELINE);

}
void CRelayLabel::SetLabelInfo_General(int ndeviceid,int type,int nStatus,COLORREF textClr,COLORREF bkClr)
{
	m_bkClr = bkClr;
	m_bTxtClr = textClr;
	if(type == 0)
	{
		DispalyInputValue_General(nStatus,textClr,bkClr);
	}
	else if(type == 1)
	{
		DispalyOutputValue_General(nStatus,textClr,bkClr);
	}
	else if(type == 2)
	{
		DispalyVariableValue_General(nStatus,textClr,bkClr);
	}
}

void CRelayLabel:: SetLabelInfo(int TstatID,int input_or_output,int nItem,COLORREF textClr,COLORREF bkClr)
{
	m_bkClr=bkClr;
	m_bTxtClr=textClr;
	if(input_or_output==0)//input
	{
		DispalyInputValue(nItem,textClr,bkClr);
	}
	if(input_or_output==1)//output
	{
		DispalyOutputValue(nItem,textClr,bkClr);

	}
	if(input_or_output==2)//customed
	{
		DispalyRigesterValue(nItem,textClr,bkClr);

	}
}

void CRelayLabel::DispalyInputValue_General(int nStatus,COLORREF textClr,COLORREF bkClr)
{
	//m_Input_data.at(nStatus - 1)
		CString temp_unite;
		CString temp_value;
		int i = nStatus ;

		if(m_Input_data.at(i).digital_analog == BAC_UNITS_ANALOG)
		{

			//m_input_list.SetCellEnabled(i,INPUT_CAL,1);


			//m_input_list.SetItemText(i,INPUT_RANGE,Input_Analog_Units_Array[m_Input_data.at(i).range]);
			//m_input_list.SetItemText(i,INPUT_UNITE,Input_List_Analog_Units[m_Input_data.at(i).range]);
			temp_unite = Input_List_Analog_Units[m_Input_data.at(i).range];

			
			temp_value.Format(_T("%d"),m_Input_data.at(i).value);
			//m_input_list.SetItemText(i,INPUT_VALUE,temp_value);

			//temp_cal.Format(_T("%d"),(m_Input_data.at(i).calibration));
			//m_input_list.SetItemText(i,INPUT_CAL,temp_cal);
		}
		else if(m_Input_data.at(i).digital_analog == BAC_UNITS_DIGITAL)
		{

			//m_input_list.SetItemText(i,INPUT_CAL,_T(""));
			//m_input_list.SetItemText(i,INPUT_RANGE,Digital_Units_Array[m_Input_data.at(i).range]);
			//m_input_list.SetItemText(i,INPUT_UNITE,_T(""));

			if((m_Input_data.at(i).range>=12)&&(m_Input_data.at(i).range<=22))
			{
				CString temp1;
				CStringArray temparray;
				temp1 = Digital_Units_Array[m_Input_data.at(i).range - 11];//11 is the sizeof the array
				SplitCStringA(temparray,temp1,_T("/"));
				if((temparray.GetSize()==2)&&(!temparray.GetAt(1).IsEmpty()))
				{
					temp_value = temparray.GetAt(1);
					//m_input_list.SetItemText(i,INPUT_VALUE,temparray.GetAt(1));
				}
				//m_input_list.SetItemText(i,INPUT_RANGE,temp1);
			}
			else if((m_Input_data.at(i).range>=1)&&(m_Input_data.at(i).range<=11))
			{
				CString temp1;
				CStringArray temparray;
				temp1 = Digital_Units_Array[m_Input_data.at(i).range];
				SplitCStringA(temparray,temp1,_T("/"));
				if((temparray.GetSize()==2)&&(!temparray.GetAt(0).IsEmpty()))
				{
					temp_value = temparray.GetAt(0);
					//m_input_list.SetItemText(i,INPUT_VALUE,temparray.GetAt(0));
				}
				//m_input_list.SetItemText(i,INPUT_RANGE,temp1);
			}

		}

		m_strValueText = temp_value + _T(" ") + temp_unite;
}

void CRelayLabel::DispalyInputValue(int nStatus,COLORREF textClr,COLORREF bkClr)
{

	int nModel=multi_register_value[MODBUS_PRODUCT_MODEL];
	CString strUnit=GetTempUnit();
	CString strTemp;
	strTemp.Empty();
	if(nModel==2||nModel==1||nModel==4||nModel==12||nModel==17||nModel==18||nModel==3)//A,B,C,D ,F,G
	{
		
	
		strTemp.Format(_T("%.1f"),multi_register_value[101]/10.0);
		strTemp=strTemp+strUnit;

		if(nStatus==0)
		{
			//m_Input_Grid.put_TextMatrix(1,1,strTemp);
			//this->SetWindowText(strTemp);
			m_strValueText=strTemp;	
	
			return;
		}


		if(nStatus==1)
		{
			strTemp.Empty();
			if(product_register_value[MODBUS_ANALOG_IN1]==4||product_register_value[MODBUS_ANALOG_IN1]==1)
			{
				strTemp.Format(_T("%.1f"),(float)multi_register_value[180]/10);
				strTemp=strTemp+strUnit;
			}
			if (product_register_value[MODBUS_ANALOG_IN1]==0||product_register_value[MODBUS_ANALOG_IN1]==2)
			{
				strTemp.Format(_T("%d"),multi_register_value[180]);
			}
			if(product_register_value[MODBUS_ANALOG_IN1]==3)
			{
				if(multi_register_value[180]==1)
					strTemp=_T("On");
				if(multi_register_value[180]==0)
					strTemp=_T("Off");
			}
			//SetWindowText(strTemp);
			m_strValueText=strTemp;	
			return;
			

		}
		
	}
	strTemp.Empty();
	
	//in3 and in4 
	if(nModel==4||nModel==12||nModel==18||nModel==17)//C,D,F,G
	{
		if(nStatus==2)//input 3
		{
			strTemp.Empty();
			if(multi_register_value[189]==4||multi_register_value[189]==1)
			{
				strTemp.Format(_T("%.1f"),(float)multi_register_value[181]/10);	
				strTemp=strTemp+strUnit;
			}
			if (multi_register_value[189]==0||multi_register_value[189]==2)
			{
				strTemp.Format(_T("%d"),multi_register_value[181]);
			}
			if(multi_register_value[189]==3)
			{
				if(multi_register_value[181]==1)
					strTemp=_T("On");
				if(multi_register_value[181]==0)
					strTemp=_T("Off");
			}	
			//SetWindowText(strTemp);
			m_strValueText=strTemp;	
			return;
		}


		if(nStatus==3)
		{
			strTemp.Empty();
			if (multi_register_value[190]==0)
			{
				if (multi_register_value[311]==0)
				{
					strTemp=_T("Off");
				}else
				{
					strTemp=_T("On");
				}
			}
			if (multi_register_value[190]==1)
			{
				if (multi_register_value[311]==1)
				{
					strTemp=_T("Off");

				}else
				{
					strTemp=_T("On");
				}
			}
			//SetWindowText(strTemp);
			m_strValueText=strTemp;	
			return;
		
		}
		
		
	}
	
	

	if(nModel==16)//E
	{
		if(nStatus>=0&&nStatus<8)
		{	strTemp.Empty();
			if(m_fFirmwareVersion>33.3)
			{
				int nValue;
			
				if(multi_register_value[359+nStatus]==1)
				{
					nValue=(int)(multi_register_value[367+nStatus]/10.0);
					strTemp.Format(_T("%.1f"),nValue);
				}
				else
				{
					strTemp.Format(_T("%d"),multi_register_value[367+nStatus]);
				}
				m_strValueText=strTemp;	
				return;

			}
			else
			{
				int nValue;
				if(multi_register_value[341+nStatus]==1)
				{
					nValue=(int)(multi_register_value[349+nStatus]/10.0);
					strTemp.Format(_T("%.1f"),nValue);
				}
				else
				{
					strTemp.Format(_T("%d"),multi_register_value[349+nStatus]);
				}
				m_strValueText=strTemp;	
				return;
			}
		}
		
	}
	/*
	if(nModel==19)//H
	{

	}
	*/
//	ReleaseDC(pDC);
}

void CRelayLabel::DispalyOutputValue_General(int nStatus,COLORREF textClr,COLORREF bkClr)
{
	CString temp_unite;
	CString temp_value;
	int i = nStatus ;

	if(m_Output_data.at(i).digital_analog == BAC_UNITS_ANALOG)
	{
		//m_output_list.SetCellEnabled(i,OUTPUT_0_PERSENT,1);
		//m_output_list.SetCellEnabled(i,OUTPUT_100_PERSENT,1);

		//m_output_list.SetItemText(i,OUTPUT_RANGE,OutPut_List_Analog_Range[m_Output_data.at(i).range]);
		//m_output_list.SetItemText(i,OUTPUT_UNITE,OutPut_List_Analog_Units[m_Output_data.at(i).range]);
		temp_unite = OutPut_List_Analog_Units[m_Output_data.at(i).range];
		//CString temp_low,temp_high;
		//temp_low.Format(_T("%d"),m_Output_data.at(i).m_del_low);
		//temp_high.Format(_T("%d"),m_Output_data.at(i).s_del_high);

		temp_value.Format(_T("%d"),m_Output_data.at(i).value);
		//m_output_list.SetItemText(i,OUTPUT_VALUE,temp_value);
		//m_output_list.SetItemText(i,OUTPUT_0_PERSENT,temp_low);
		//m_output_list.SetItemText(i,OUTPUT_100_PERSENT,temp_high);
	}
	else if(m_Output_data.at(i).digital_analog == BAC_UNITS_DIGITAL)
	{
		//m_output_list.SetCellEnabled(i,OUTPUT_0_PERSENT,0);
		//m_output_list.SetCellEnabled(i,OUTPUT_100_PERSENT,0);
		//m_output_list.SetItemText(i,OUTPUT_0_PERSENT,_T(""));
		//m_output_list.SetItemText(i,OUTPUT_100_PERSENT,_T(""));
		//m_output_list.SetItemText(i,OUTPUT_RANGE,Digital_Units_Array[m_Output_data.at(i).range]);

		//m_output_list.SetItemText(i,OUTPUT_UNITE,_T(""));

		if((m_Output_data.at(i).range>=12)&&(m_Output_data.at(i).range<=22))
		{
			CString temp1;
			CStringArray temparray;
			temp1 = Digital_Units_Array[m_Output_data.at(i).range - 11];//11 is the sizeof the array
			SplitCStringA(temparray,temp1,_T("/"));
			if((temparray.GetSize()==2)&&(!temparray.GetAt(1).IsEmpty()))
			{
				temp_value = temparray.GetAt(1);
			//	m_output_list.SetItemText(i,OUTPUT_VALUE,temparray.GetAt(1));
			}
			//m_output_list.SetItemText(i,OUTPUT_RANGE,temp1);
		}
		else if((m_Output_data.at(i).range>=1)&&(m_Output_data.at(i).range<=11))
		{
			CString temp1;
			CStringArray temparray;
			temp1 = Digital_Units_Array[m_Output_data.at(i).range];
			SplitCStringA(temparray,temp1,_T("/"));
			if((temparray.GetSize()==2)&&(!temparray.GetAt(0).IsEmpty()))
			{
				temp_value = temparray.GetAt(0);
				//m_output_list.SetItemText(i,OUTPUT_VALUE,temparray.GetAt(0));
			}
			//m_output_list.SetItemText(i,OUTPUT_RANGE,temp1);
		}


	}
	m_strValueText = temp_value + _T(" ") + temp_unite;
}

void CRelayLabel::DispalyOutputValue(int nStatus,COLORREF textClr,COLORREF bkClr)
{
	int nModel=multi_register_value[MODBUS_PRODUCT_MODEL];
	CString strTemp;
	int itemp=multi_register_value[108];

	//==========================================================================
	//ABCDEFG,noH

	if(nModel==2||nModel==1||nModel==4||nModel==12||nModel==16||nModel==17||nModel==18||nModel==3)
	{
		for(int i=1;i<=3;i++)
		{
			if(nStatus==i-1)
			{
				if(itemp &( 1<<(i-1)))
					strTemp=_T("On");
				else
					strTemp=_T("Off");
					
				m_strValueText=strTemp;	
				return;
			}
	
	

		}
	}

	////----------------------------------------------------------------------
	//4,5
	if(nModel==1||nModel==4||nModel==12||nModel==16)//B,C,D,E
	{
		if(nStatus==3)
		{
			if(itemp &( 1<<3))
				strTemp=_T("On");
			else
				strTemp=_T("Off");
			m_strValueText=strTemp;		
			return;
		}
		

		if(nStatus==4)
		{
			if(itemp &( 1<<4))
				strTemp=_T("On");
			else
					strTemp=_T("Off");
			m_strValueText=strTemp;		
			return;

		}
		
	}

		//:DEG 6. 7
		int nRange=multi_register_value[186];
		if(nModel==12||nModel==16||nModel==18)
		{
			if(nStatus==5)
			{
				if(nRange==0)
				{
					itemp=multi_register_value[102];
					if(itemp==0)
						strTemp=_T("Off");
					if(itemp==1)
						strTemp=_T("On");
				}
				else
				{
					//strTemp.Format(_T("%.1f"),multi_register_value[102]/100.0);
						//strTemp.Format(_T("%.1f"),multi_register_value[102]/100.0);
					float nvalue=0.0;
					if(nRange==1)//0-10v
					{
						//nvalue=multi_register_value[102]/100 /10.0 * 100%;
						nvalue=(float)(multi_register_value[102]/10.0);
					}
					if(nRange==2)//0-5v
					{
						nvalue=(float)(multi_register_value[102]/5.0);
					}
					if(nRange==3)//2-10v
					{
						nvalue=(float)(multi_register_value[102]/8.0);
					}
					if(nRange==4)//10-0v
					{
						nvalue=(float)((10-multi_register_value[102]/100.0)/10.0 *100);
					}
					strTemp.Format(_T("%.1f%%"),nvalue);
				}
				m_strValueText=strTemp;		
				return;
			}
			
			if(nStatus==6)
			{
				nRange=multi_register_value[187];
				if(nRange==0)
				{
					itemp=multi_register_value[103];
					if(itemp==0)
						strTemp=_T("Off");
					if(itemp==1)
						strTemp=_T("On");
				}
				else
				{
					//strTemp.Format(_T("%.1f"),multi_register_value[103]/100.0);
						float nvalue=0.0;
						if(nRange==1)//0-10v
						{
							//nvalue=multi_register_value[102]/100 /10.0 * 100%;
							nvalue=(float)(multi_register_value[103]/10.0);
						}
						if(nRange==2)//0-5v
						{
							nvalue=(float)(multi_register_value[103]/5.0);
						}
						if(nRange==3)//2-10v
						{
							nvalue=(float)(multi_register_value[103]/8.0);
						}
						if(nRange==4)//10-0v
						{
							nvalue=(float)((10-multi_register_value[103]/100.0)/10.0 *100);
						}
						strTemp.Format(_T("%.1f%%"),nvalue);

				}
				m_strValueText=strTemp;		
				return;

			}
		
		
		}
			
		if(nModel==2)//A 4,5
		{
			if(nStatus==3)
			{
				nRange=multi_register_value[186];
				if(nRange==0)
				{
					itemp=multi_register_value[102];
					if(itemp==0)
						strTemp=_T("Off");
					if(itemp==1)
						strTemp=_T("On");
				}
				else
				{
					//strTemp.Format(_T("%.1f"),multi_register_value[102]/100.0);
						//strTemp.Format(_T("%.1f"),multi_register_value[102]/100.0);
					float nvalue=0.0;
					if(nRange==1)//0-10v
					{
						//nvalue=multi_register_value[102]/100 /10.0 * 100%;
						nvalue=(float)(multi_register_value[102]/10.0);
					}
					if(nRange==2)//0-5v
					{
						nvalue=(float)(multi_register_value[102]/5.0);
					}
					if(nRange==3)//2-10v
					{
						nvalue=(float)(multi_register_value[102]/8.0);
					}
					if(nRange==4)//10-0v
					{
						nvalue=(float)((10-multi_register_value[102]/100.0)/10.0 *100);
					}
					strTemp.Format(_T("%.1f%%"),nvalue);
				
				}
				m_strValueText=strTemp;		
				return;
			}
		
			if(nStatus==4)
			{
				nRange=multi_register_value[187];
				if(nRange==0)
				{
					itemp=multi_register_value[103];
					if(itemp==0)
						strTemp=_T("Off");
					if(itemp==1)
						strTemp=_T("On");
				}
				else
				{
					//strTemp.Format(_T("%.1f"),multi_register_value[103]/100.0);
						float nvalue=0.0;
					if(nRange==1)//0-10v
					{
						//nvalue=multi_register_value[102]/100 /10.0 * 100%;
						nvalue=(float)(multi_register_value[103]/10.0);
					}
					if(nRange==2)//0-5v
					{
						nvalue=(float)(multi_register_value[103]/5.0);
					}
					if(nRange==3)//2-10v
					{
						nvalue=(float)(multi_register_value[103]/8.0);
					}
					if(nRange==4)//10-0v
					{
						nvalue=(float)((10-multi_register_value[103]/100.0)/10.0 *100);
					}
					strTemp.Format(_T("%.1f%%"),nvalue);
				}
				m_strValueText=strTemp;		
				return;
			}
			
		}//end A
		//F 4,5
		if(nModel==17||nModel==18)
		{
			if(nStatus==3)
			{
				int itemp=multi_register_value[108];
				if(multi_register_value[283]==0)
				{
					if(itemp &( 1<<3))
							strTemp=_T("On");
						else
							strTemp=_T("Off");

				}
				else
				{
					strTemp.Format(_T("%d%%"),multi_register_value[348]);

				}
				m_strValueText=strTemp;		
				return;	
			}
			

			if(nStatus==4)
			{
				int itemp=multi_register_value[108];
				if(multi_register_value[284]==0)
				{
					if(itemp &( 1<<4))
							strTemp=_T("On");
						else
							strTemp=_T("Off");

				}
				else
				{
					strTemp.Format(_T("%d%%"),multi_register_value[349]);

				}
				m_strValueText=strTemp;		
				return;	

			}
				
		}






/*



	//
	if(nModel==17)//F out4 out5
	{

	}

	if(nModel==18)//G out4 out5
	{

	}
	if(nModel==19)//H out1 out7
	{
	}
	*/

}

void CRelayLabel::DispalyVariableValue_General(int nStatus,COLORREF textClr,COLORREF bkClr)
{
	CString temp_unite;
	CString temp_value;
	int i = nStatus ;

	if(m_Variable_data.at(i).digital_analog == BAC_UNITS_DIGITAL)
	{
	//	m_variable_list.SetItemText(i,VARIABLE_UNITE,Digital_Units_Array[m_Variable_data.at(i).range]);	//单位 这个要商量 看要怎么搞;
		if((m_Variable_data.at(i).range >= 12)&&(m_Variable_data.at(i).range <= 22))
		{
			CString temp1;
			CStringArray temparray;
			temp1 = Digital_Units_Array[m_Variable_data.at(i).range - 11];//11 is the sizeof the array
			SplitCStringA(temparray,temp1,_T("/"));
			if((temparray.GetSize()==2)&&(!temparray.GetAt(1).IsEmpty()))
			{
				temp_value = temparray.GetAt(1);
				//m_variable_list.SetItemText(i,VARIABLE_VALUE,temparray.GetAt(1));
			}
			//m_variable_list.SetItemText(i,VARIABLE_UNITE,temp1);
		}
		else if((m_Variable_data.at(i).range >= 1)&&(m_Variable_data.at(i).range <= 11))
		{
			CString temp1;
			CStringArray temparray;
			temp1 = Digital_Units_Array[m_Variable_data.at(i).range];
			SplitCStringA(temparray,temp1,_T("/"));
			if((temparray.GetSize()==2)&&(!temparray.GetAt(0).IsEmpty()))
			{
				temp_value = temparray.GetAt(0);
				//m_variable_list.SetItemText(i,VARIABLE_VALUE,temparray.GetAt(0));
			}
			//m_variable_list.SetItemText(i,VARIABLE_UNITE,temp1);
		}
	}
	else
	{
		if(m_Variable_data.at(i).range == 20)	//如果是时间;
		{
			//m_variable_list.SetItemText(i,VARIABLE_UNITE,Variable_Analog_Units_Array[m_Variable_data.at(i).range]);
			char temp_char[50];
			int time_seconds = m_Variable_data.at(i).value / 1000;
			intervaltotext(temp_char,time_seconds,0,0);
			CString temp_11;
			MultiByteToWideChar( CP_ACP, 0, temp_char, strlen(temp_char) + 1, 
				temp_11.GetBuffer(MAX_PATH), MAX_PATH );
			temp_11.ReleaseBuffer();		
			//m_variable_list.SetItemText(i,VARIABLE_VALUE,temp_11);
			temp_value = temp_11;
			//temp_value.Format(_T("%d"),m_Variable_data.at(i).value);
			//m_variable_list.SetItemText(i,VARIABLE_VALUE,temp_value);
		}
		else if(m_Variable_data.at(i).range<=sizeof(Variable_Analog_Units_Array)/sizeof(Variable_Analog_Units_Array[0]))
		{
			//m_variable_list.SetItemText(i,VARIABLE_UNITE,Variable_Analog_Units_Array[m_Variable_data.at(i).range]);
			temp_unite = Variable_Analog_Units_Array[m_Variable_data.at(i).range];
			temp_value.Format(_T("%d"),m_Variable_data.at(i).value);
			//m_variable_list.SetItemText(i,VARIABLE_VALUE,temp_value);
		}
	}
		m_strValueText = temp_value + _T(" ") + temp_unite;
}

void CRelayLabel::DispalyRigesterValue(int nStatus,COLORREF textClr,COLORREF bkClr)
{
	//nStatus is rigister;
	if(nStatus<0||nStatus>=513)
		return;

	
	CString strTemp;
	strTemp.Format(_T("%d"),multi_register_value[nStatus]);
	m_strValueText=strTemp;	

}
void CRelayLabel::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	Invalidate();
	CStatic::OnTimer(nIDEvent);
}

void CRelayLabel::OnDestroy()
{
	CStatic::OnDestroy();
	KillTimer(1);
	// TODO: Add your message handler code here
}
