#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <iostream>
#include <string>
#include <tchar.h>
#include <sql.h>
#include <sqlext.h>
#include <vector>
#include<iomanip>
using namespace std;

struct ColumnInfo
{
	SQLUSMALLINT icol;
	SQLWCHAR szColName[20];
	SQLSMALLINT BufferLength;
	SQLSMALLINT	NameLengthPtr;
	SQLSMALLINT	pfSqlType;
	SQLULEN	ColumnSizePtr;
	SQLSMALLINT DecimalDigitsPtr;
	SQLSMALLINT	pfNullable;
};
struct TableInfo
{
	std::vector<ColumnInfo> ColumnList;
	SQLSMALLINT iNumcol;
	std::wstring szTableName;
};
struct Field
{
	std::wstring szData;
	int iDataType;
};
struct Record
{
	std::vector<Field> record;
};
class ODBC
{
public:
	SQLHENV m_hEnv;//환경
	SQLHDBC	m_hDbc;//연결
	//명령
	SQLHSTMT m_hStmt;
	SQLHSTMT m_hAccountStmt;
	SQLHSTMT m_hLoginStmt;
	SQLHSTMT m_hLogoutStmt;
	SQLHSTMT m_hUserdelStmt;
	SQLHSTMT m_hChangepsStmt;

	SQLRETURN retcode;
	SWORD sReturn;

	SQLWCHAR m_id[10] = L"NONE";
	SQLWCHAR m_ps[10] = L"NONE";
	SQLWCHAR m_newps[10] = L"NONE";

	std::vector<Record> m_RealStringData;

	void Error(SQLHSTMT stmt);
	bool Init();
	bool Connect();
	bool Release();

	bool ExecTableInfo(const TCHAR* TableName, const wstring sql);
	bool ExecSelectAll(const TCHAR* TableName);
	bool ExecSelect(const TCHAR* TableName, const wstring userid);

	void AccountCreatePrepare();
	void ExecAccountCreate(const TCHAR* szid, const TCHAR* szps);

	void LoginPrepare();
	void ExecLogin(const TCHAR* szid, const TCHAR* szps);

	void LogoutPrepare();
	void ExecLogout(const TCHAR* szid);

	void UserDeletePrepare();
	void ExecUserDelete(const TCHAR* szid, const TCHAR* szps);

	void ChangePsPrepare();
	void ExecchangePs(const TCHAR* szid, const TCHAR* szps, const TCHAR* sznewps);

	void AllPrepare();
};

