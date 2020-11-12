#include <stdio.h>
#include <stdlib.h>
#include <sql.h>
#include <sqlext.h>

int main () {
	SQLHENV   henv  = SQL_NULL_HENV;  
	SQLHDBC   hdbc  = SQL_NULL_HDBC;   
	SQLHSTMT  hstmt = SQL_NULL_HSTMT; 
	SQLRETURN retcode;

	SQLCHAR strName[20];
	SQLINTEGER nNum=0,nVal=0;

	retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);

	retcode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER*)SQL_OV_ODBC3, 0);

	retcode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);

	SQLSetConnectAttr(hdbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER)10, 0);

	retcode = SQLConnect(hdbc, (SQLCHAR*) "temp1", SQL_NTS,(SQLCHAR*) "user1", 0, "user1", 0);

	retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);

	retcode = SQLPrepare(hstmt, (SQLCHAR*) "INSERT INTO test_01(num,name,val) values(?,?,?)", SQL_NTS); 


 	retcode = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &nNum, 0, NULL);  

 	retcode = SQLBindParameter(hstmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 20, 0, strName, sizeof(strName),NULL);  

 	retcode = SQLBindParameter(hstmt, 3, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &nVal, 0, NULL);  

	nNum= 901;
	sprintf(strName,"test_insert1");
	nVal=8880;
	retcode = SQLExecute(hstmt);  

	nNum= 902;
	sprintf(strName,"test_insert2");
	nVal=8882;
	retcode = SQLExecute(hstmt);  

	nNum= 903;
	sprintf(strName,"test_insert3");
	nVal=8883;
	retcode = SQLExecute(hstmt);  

	printf("Insert Process Exit\n");

	return 0;
}

