#include "ODBC.h"
void ODBC:: Error(SQLHSTMT stmt)
{
	//에러 체크
	SQLTCHAR szSQLState[SQL_SQLSTATE_SIZE + 1];//오류 클래스
	SQLTCHAR errorBuffer[SQL_MAX_MESSAGE_LENGTH + 1];//에러 텍스트
	SQLINTEGER iSQLCode;//고유 오류 코드
	SQLSMALLINT length;
	//       환경h   연결h   명령h 오류 클래스   고유 오류 코드  msg 텍스트버퍼   <-버퍼 최대 길이     <-<-버퍼에 리턴하는데 사용할 수 있는 길이
	SQLError(m_hEnv, m_hDbc, stmt, szSQLState,   &iSQLCode,      errorBuffer,     sizeof(errorBuffer), &length);
	
	MessageBox(NULL, errorBuffer, szSQLState, MB_OK);
}
bool ODBC:: Init()
{
	//환경 핸들 할당
	if (SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &m_hEnv) != SQL_SUCCESS)
	{
		return false;
	}
	//환경 핸들 속성 지정, 세팅
	if (SQLSetEnvAttr(m_hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3_80, SQL_IS_INTEGER) != SQL_SUCCESS)
	{
		return false;
	}
	//연결 핸들 할당
	if (SQLAllocHandle(SQL_HANDLE_DBC, m_hEnv, &m_hDbc) != SQL_SUCCESS)
	{
		return false;
	}
	return true;
}
bool ODBC:: Connect()
{
	//사용자 DSN 연결 3.8버전
	retcode = SQLConnect(m_hDbc, (SQLWCHAR*)L"LJS", SQL_NTS, (SQLWCHAR*)L"sa", SQL_NTS, (SQLWCHAR*)L"zhqhfdl147", SQL_NTS);

	if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
	{
		//Error();
		return false;
	}
}
bool ODBC:: Release()
{
	SQLFreeHandle(SQL_HANDLE_STMT, m_hStmt);
	SQLFreeHandle(SQL_HANDLE_STMT, m_hAccountStmt);
	SQLFreeHandle(SQL_HANDLE_STMT, m_hLoginStmt);
	SQLFreeHandle(SQL_HANDLE_STMT, m_hLogoutStmt);
	SQLFreeHandle(SQL_HANDLE_STMT, m_hUserdelStmt);
	SQLFreeHandle(SQL_HANDLE_STMT, m_hChangepsStmt);

	SQLDisconnect(m_hDbc);
	SQLFreeHandle(SQL_HANDLE_DBC, m_hDbc);

	SQLFreeHandle(SQL_HANDLE_ENV, m_hEnv);
	return true;
}
bool ODBC:: ExecTableInfo(const TCHAR* TableName, const wstring insql)
{
	//명령 핸들 할당
	if (SQLAllocHandle(SQL_HANDLE_STMT, m_hDbc, &m_hStmt) != SQL_SUCCESS)
	{
		return false;
	}
	TableInfo table;
	table.szTableName = TableName;
	std::wstring sql = insql;
	//std::wstring sql = L"select * from ";
	//sql += TableName;

	//명령문 직접 실행
	retcode = SQLExecDirect(m_hStmt, (SQLTCHAR*)sql.c_str(), SQL_NTS);
	if (retcode != SQL_SUCCESS)
	{
		Error(m_hStmt);
		//return false;
	}

	SQLSMALLINT iNumCols;
	SQLNumResultCols(m_hStmt, &table.iNumcol);//결과 컬럼 개수 얻음
	for (int iCols = 1; iCols <= table.iNumcol; iCols++)
	{
		ColumnInfo column;

		//컬럼 번호 1부터~ (테이블 시작은 0이 아닌 1)
		column.icol = iCols;

		//_countof - 배열 개수 구하는 매크로
		int iSize = _countof(column.szColName);

		//컬럼에 대한 세부적인 설명 얻음 - SQLDescribeCol
		SQLDescribeCol(m_hStmt, //명령 핸들
			iCols, //데이터 열의 수
			column.szColName, //컬럼 이름
			iSize, //컬럼 이름 버퍼의 길이
			&column.NameLengthPtr,
			&column.pfSqlType,// 데이터 형식을 반환할 버퍼 포인터
			&column.ColumnSizePtr, // 열의 크기를 반환할 버퍼 포인터
			&column.DecimalDigitsPtr, // 10진수 자리수
			&column.pfNullable);// NULL 허용여부
		table.ColumnList.push_back(column);
	}
	Record rData;
	SQLLEN lTemp;
	TCHAR szData[100][21] = { 0, };
	int iData[100];

	for (int iBind = 0; iBind < table.ColumnList.size(); iBind++)
	{
		switch (table.ColumnList[iBind].pfSqlType)
		{
		case SQL_TYPE_TIMESTAMP:
		{
			Field data;
			data.iDataType = SQL_UNICODE;
			retcode = SQLBindCol(m_hStmt, iBind + 1, SQL_TYPE_TIMESTAMP, &szData[iBind], 0, &lTemp);
			if (retcode != SQL_SUCCESS)
			{
				Error(m_hStmt);
				return false;
			}
			rData.record.push_back(data);
		}break;
		case SQL_WCHAR:
		case SQL_WVARCHAR:
		{
			Field data;
			data.iDataType = SQL_UNICODE;
			retcode = SQLBindCol(m_hStmt, iBind + 1, SQL_UNICODE, szData[iBind], sizeof(szData[iBind]), &lTemp);
			if (retcode != SQL_SUCCESS)
			{
				Error(m_hStmt);
				return false;
			}
			rData.record.push_back(data);
		}break;
		case SQL_INTEGER:
		{
			Field data;
			data.iDataType = SQL_INTEGER;
			retcode = SQLBindCol(m_hStmt, iBind + 1, SQL_INTEGER, &iData[iBind], 0, &lTemp);
			if (retcode != SQL_SUCCESS)
			{
				Error(m_hStmt);
				return false;
			}
			rData.record.push_back(data);
		}break;
		case -7:
		{
			Field data;
			data.iDataType = SQL_C_ULONG;
			retcode = SQLBindCol(m_hStmt, iBind + 1, SQL_C_ULONG, &iData[iBind], 0, &lTemp);
			if (retcode != SQL_SUCCESS)
			{
				Error(m_hStmt);
				return false;
			}
			rData.record.push_back(data);
		};
		}
	}
	//다음 행 패치
	while (SQLFetch(m_hStmt) != SQL_NO_DATA)
	{
		for (int iCol = 0; iCol < 3; iCol++)//table.ColumnList.size()
		{
			rData.record[iCol].iDataType = rData.record[iCol].iDataType;
			if (rData.record[iCol].iDataType == SQL_UNICODE)
			{
				rData.record[iCol].szData = szData[iCol];
				wcout << rData.record[iCol].szData<<"  ";
			}
			else
			{
				rData.record[iCol].szData = std::to_wstring(iData[iCol]);
				wcout << setw(2)<<rData.record[iCol].szData << "  ";
			}
			
		}wcout << endl;
		//컬럼을 제외한 데이터 정보 저장됨
		m_RealStringData.push_back(rData);
	}
	SQLCloseCursor(m_hStmt);
	return true;
}
bool ODBC:: ExecSelectAll(const TCHAR* TableName)
{
	std::wstring sql = L"select * from ";
	sql += TableName;
	ExecTableInfo(TableName, sql);
	return true;
}
bool ODBC:: ExecSelect(const TCHAR* TableName, const wstring userid)
{
	std::wstring sql = L"select * from ";
	sql += TableName;
	sql += L" where USERID='";
	sql += userid;
	sql += L"'";

	ExecTableInfo(TableName, sql);
	return true;
}
void ODBC:: AccountCreatePrepare()
{
	retcode = SQLAllocHandle(SQL_HANDLE_STMT, m_hDbc, &m_hAccountStmt);

	//SWORD sReturn = 0; //프로시저 반환값받는 곳
	SQLLEN cbRetParam = SQL_NTS;
	//SQLBindCol - 리턴을 받을때
	//SQLBindParameter - 이건 값을 넘겨줄때

	//1번 ? //?=call Account1Create(?,?)
	//                                            output - 반환                                      반환값
	retcode = SQLBindParameter(m_hAccountStmt, 1, SQL_PARAM_OUTPUT, SQL_C_SSHORT, SQL_INTEGER, 0, 0, &sReturn, 0, &cbRetParam);

	//2번 ? //?=call Account1Create(?,?)
	//                                            input - 입력     <-유니코드                                  id값
	retcode = SQLBindParameter(m_hAccountStmt, 2, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, sizeof(m_id), 0, m_id, sizeof(m_id), NULL);

	//3번 ? //?=call Account1Create(?,?)
	retcode = SQLBindParameter(m_hAccountStmt, 3, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, sizeof(m_ps), 0, m_ps, sizeof(m_ps), NULL);

	//프로시저 호출!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	TCHAR callsp[] = L"{?=call AccountCreate(?,?)}";
	//사전에 컴파일을 다 해두겠다/쿼리를 직접안하고 미리 생성->컴파일부터 문법 검증까지 다 해줌
	retcode = SQLPrepare(m_hAccountStmt, callsp, SQL_NTS);
}
void ODBC:: ExecAccountCreate(const TCHAR* szid, const TCHAR* szps)
{
	memcpy(m_id, szid, sizeof(m_id));
	memcpy(m_ps, szps, sizeof(m_ps));
	retcode = SQLExecute(m_hAccountStmt);
	if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO) //(0값과 1값을 갖고 있음 - 음수만 아님 된다라는 뜻)
	{
		Error(m_hAccountStmt);
		return;
	}
	//SQLMoreResults - 패치를 다 하고나서 결과를 보려는것 //반환값받을때 
	while (SQLMoreResults(m_hAccountStmt) != SQL_NO_DATA);

	//SQL_CLOSE - 하나의 명령을 끝내겠다
	SQLFreeStmt(m_hAccountStmt, SQL_CLOSE);
	SQLCloseCursor(m_hAccountStmt);
}
void ODBC:: LoginPrepare()
{
	retcode = SQLAllocHandle(SQL_HANDLE_STMT, m_hDbc, &m_hLoginStmt);

	//SWORD sReturn = 0; //프로시저 반환값받는 곳
	SQLLEN cbRetParam = SQL_NTS;

	retcode = SQLBindParameter(m_hLoginStmt, 1, SQL_PARAM_OUTPUT, SQL_C_SSHORT, SQL_INTEGER, 0, 0, &sReturn, 0, &cbRetParam);

	retcode = SQLBindParameter(m_hLoginStmt, 2, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, sizeof(m_id), 0, m_id, sizeof(m_id), NULL);

	retcode = SQLBindParameter(m_hLoginStmt, 3, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, sizeof(m_ps), 0, m_ps, sizeof(m_ps), NULL);

	TCHAR callsp[] = L"{?=call CheckPassword(?,?)}";
	retcode = SQLPrepare(m_hLoginStmt, callsp, SQL_NTS);
}
void ODBC:: ExecLogin(const TCHAR* szid, const TCHAR* szps)
{
	memcpy(m_id, szid, sizeof(m_id));
	memcpy(m_ps, szps, sizeof(m_ps));
	retcode = SQLExecute(m_hLoginStmt);
	if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO) //(0값과 1값을 갖고 있음 - 음수만 아님 된다라는 뜻)
	{
		Error(m_hLoginStmt);
		return;
	}
	while (SQLMoreResults(m_hLoginStmt) != SQL_NO_DATA);

	SQLFreeStmt(m_hLoginStmt, SQL_CLOSE);
	SQLCloseCursor(m_hLoginStmt);
}
void ODBC:: LogoutPrepare()
{
	retcode = SQLAllocHandle(SQL_HANDLE_STMT, m_hDbc, &m_hLogoutStmt);

	//SWORD sReturn = 0; //프로시저 반환값받는 곳
	SQLLEN cbRetParam = SQL_NTS;

	retcode = SQLBindParameter(m_hLogoutStmt, 1, SQL_PARAM_OUTPUT, SQL_C_SSHORT, SQL_INTEGER, 0, 0, &sReturn, 0, &cbRetParam);

	retcode = SQLBindParameter(m_hLogoutStmt, 2, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, sizeof(m_id), 0, m_id, sizeof(m_id), NULL);

	TCHAR callsp[] = L"{?=call UserLogout(?)}";
	retcode = SQLPrepare(m_hLogoutStmt, callsp, SQL_NTS);
}
void ODBC:: ExecLogout(const TCHAR* szid)
{
	memcpy(m_id, szid, sizeof(m_id));
	retcode = SQLExecute(m_hLogoutStmt);
	if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO) //(0값과 1값을 갖고 있음 - 음수만 아님 된다라는 뜻)
	{
		Error(m_hLogoutStmt);
		return;
	}
	while (SQLMoreResults(m_hLogoutStmt) != SQL_NO_DATA);

	SQLFreeStmt(m_hLogoutStmt, SQL_CLOSE);
	SQLCloseCursor(m_hLogoutStmt);
}
void ODBC:: UserDeletePrepare()
{
	retcode = SQLAllocHandle(SQL_HANDLE_STMT, m_hDbc, &m_hUserdelStmt);

	//SWORD sReturn = 0; //프로시저 반환값받는 곳
	SQLLEN cbRetParam = SQL_NTS;

	retcode = SQLBindParameter(m_hUserdelStmt, 1, SQL_PARAM_OUTPUT, SQL_C_SSHORT, SQL_INTEGER, 0, 0, &sReturn, 0, &cbRetParam);

	retcode = SQLBindParameter(m_hUserdelStmt, 2, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, sizeof(m_id), 0, m_id, sizeof(m_id), NULL);

	retcode = SQLBindParameter(m_hUserdelStmt, 3, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, sizeof(m_ps), 0, m_ps, sizeof(m_ps), NULL);

	TCHAR callsp[] = L"{?=call UserDelete(?,?)}";
	retcode = SQLPrepare(m_hUserdelStmt, callsp, SQL_NTS);
}
void ODBC:: ExecUserDelete(const TCHAR* szid, const TCHAR* szps)
{
	memcpy(m_id, szid, sizeof(m_id));
	memcpy(m_ps, szps, sizeof(m_ps));
	retcode = SQLExecute(m_hUserdelStmt);
	if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO) //(0값과 1값을 갖고 있음 - 음수만 아님 된다라는 뜻)
	{
		Error(m_hUserdelStmt);
		return;
	}
	while (SQLMoreResults(m_hUserdelStmt) != SQL_NO_DATA);

	SQLFreeStmt(m_hUserdelStmt, SQL_CLOSE);
	SQLCloseCursor(m_hUserdelStmt);
}
void ODBC:: ChangePsPrepare()
{
	retcode = SQLAllocHandle(SQL_HANDLE_STMT, m_hDbc, &m_hChangepsStmt);

	//SWORD sReturn = 0; //프로시저 반환값받는 곳
	SQLLEN cbRetParam = SQL_NTS;

	retcode = SQLBindParameter(m_hChangepsStmt, 1, SQL_PARAM_OUTPUT, SQL_C_SSHORT, SQL_INTEGER, 0, 0, &sReturn, 0, &cbRetParam);

	retcode = SQLBindParameter(m_hChangepsStmt, 2, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, sizeof(m_id), 0, m_id, sizeof(m_id), NULL);

	retcode = SQLBindParameter(m_hChangepsStmt, 3, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, sizeof(m_ps), 0, m_ps, sizeof(m_ps), NULL);

	retcode = SQLBindParameter(m_hChangepsStmt, 4, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, sizeof(m_newps), 0, m_newps, sizeof(m_newps), NULL);

	TCHAR callsp[] = L"{?=call ChangePassword(?,?,?)}";
	retcode = SQLPrepare(m_hChangepsStmt, callsp, SQL_NTS);
}
void ODBC:: ExecchangePs(const TCHAR* szid, const TCHAR* szps, const TCHAR* sznewps)
{
	memcpy(m_id, szid, sizeof(m_id));
	memcpy(m_ps, szps, sizeof(m_ps));
	memcpy(m_newps, sznewps, sizeof(m_newps));
	retcode = SQLExecute(m_hChangepsStmt);
	if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO) //(0값과 1값을 갖고 있음 - 음수만 아님 된다라는 뜻)
	{
		Error(m_hChangepsStmt);
		return;
	}
	while (SQLMoreResults(m_hChangepsStmt) != SQL_NO_DATA);

	SQLFreeStmt(m_hChangepsStmt, SQL_CLOSE);
	SQLCloseCursor(m_hChangepsStmt);
}
void ODBC:: AllPrepare()
{
	AccountCreatePrepare();
	LoginPrepare(); 
	LogoutPrepare();
	UserDeletePrepare();
	ChangePsPrepare();
}