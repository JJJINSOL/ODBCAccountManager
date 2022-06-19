#include "ODBC.h"
void main()
{
	setlocale(LC_ALL, "korean");
	ODBC odbc;
	odbc.Init();
	odbc.Connect();
	odbc.AllPrepare();

	int num;
	std::wstring wid = L"";
	std::wstring wps = L"";
	std::wstring wnewps = L"";
	std::wstring findid = L"";

	cout << "1. ���� ���� 2. �α��� 3. �α׾ƿ� 4. ID ���� 5. PW ���� 6. ��ü ���� ����Ʈ ��ȸ 7. ���� ���� �˻� 0. ����"<<endl;

	while (1)
	{
		cout << "��ȣ�� �������ּ���. -> ";
		cin >> num;
		switch (num)
		{
		case 0:
		{
			return;
		}break;
		case 1:
		{
			cout << "ID : "; wcin >> wid;
			cout << "PW : "; wcin >> wps;
			odbc.ExecAccountCreate(wid.c_str(),wps.c_str());
			if (odbc.sReturn == 0) { cout << "���̵� ���� ����!"<< endl; }
			else { cout << "���� ����! (�ߺ��� ���̵��Դϴ�!)" << endl; }
		}break;
		case 2:
		{
			cout << "ID : "; wcin >> wid;
			cout << "PW : "; wcin >> wps;
			odbc.ExecLogin(wid.c_str(), wps.c_str());
			if (odbc.sReturn == 0) { cout << "�α��� ����!" << endl; }
			else { cout << "�α��� ����! (���̵� ���ų� �н����尡 �ٸ��ϴ�.)" << endl; }
		}break;
		case 3:
		{
			cout << "ID : "; wcin >> wid;
			odbc.ExecLogout(wid.c_str());
			if (odbc.sReturn == 0) { cout << "�α׾ƿ� ����!" << endl; }
		}break;
		case 4:
		{
			cout << "ID : "; wcin >> wid;
			cout << "PW : "; wcin >> wps;
			odbc.ExecUserDelete(wid.c_str(), wps.c_str());
			if (odbc.sReturn == 0) { cout << "���� ����!" << endl; }
			else { cout << "���� ����! (���̵� ���ų� �н����尡 �ٸ��ϴ�.)" << endl; }
		}break;
		case 5:
		{
			cout << "ID : "; wcin >> wid;
			cout << "PW : "; wcin >> wps;
			cout << "NEW PW : "; wcin >> wnewps;
			odbc.ExecchangePs(wid.c_str(), wps.c_str(), wnewps.c_str());
			if (odbc.sReturn == 0) { cout << "���� ����!" << endl; }
			else { cout << "���� ����! (���̵� ���ų� �н����尡 �ٸ��ϴ�.)" << endl; }
		}break;
		case 6:
		{
			odbc.ExecSelectAll(L"USERLIST");
		}break;
		case 7:
		{
			cout << "ID : "; wcin >> findid;
			odbc.ExecSelect(L"USERLIST", findid);
		}break;
		}
		//system("cls");
	}
	odbc.Release();
}