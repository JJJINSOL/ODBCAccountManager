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

	cout << "1. 유저 생성 2. 로그인 3. 로그아웃 4. ID 삭제 5. PW 변경 6. 전체 유저 리스트 조회 7. 유저 정보 검색 0. 종료"<<endl;

	while (1)
	{
		cout << "번호를 선택해주세요. -> ";
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
			if (odbc.sReturn == 0) { cout << "아이디 생성 성공!"<< endl; }
			else { cout << "생성 실패! (중복된 아이디입니다!)" << endl; }
		}break;
		case 2:
		{
			cout << "ID : "; wcin >> wid;
			cout << "PW : "; wcin >> wps;
			odbc.ExecLogin(wid.c_str(), wps.c_str());
			if (odbc.sReturn == 0) { cout << "로그인 성공!" << endl; }
			else { cout << "로그인 실패! (아이디가 없거나 패스워드가 다릅니다.)" << endl; }
		}break;
		case 3:
		{
			cout << "ID : "; wcin >> wid;
			odbc.ExecLogout(wid.c_str());
			if (odbc.sReturn == 0) { cout << "로그아웃 성공!" << endl; }
		}break;
		case 4:
		{
			cout << "ID : "; wcin >> wid;
			cout << "PW : "; wcin >> wps;
			odbc.ExecUserDelete(wid.c_str(), wps.c_str());
			if (odbc.sReturn == 0) { cout << "삭제 성공!" << endl; }
			else { cout << "삭제 실패! (아이디가 없거나 패스워드가 다릅니다.)" << endl; }
		}break;
		case 5:
		{
			cout << "ID : "; wcin >> wid;
			cout << "PW : "; wcin >> wps;
			cout << "NEW PW : "; wcin >> wnewps;
			odbc.ExecchangePs(wid.c_str(), wps.c_str(), wnewps.c_str());
			if (odbc.sReturn == 0) { cout << "변경 성공!" << endl; }
			else { cout << "변경 실패! (아이디가 없거나 패스워드가 다릅니다.)" << endl; }
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