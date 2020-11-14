#include <stdio.h>
#include <sql.h>
#include <sqlext.h>

int main (int argc, char* argv[]) {
    RETCODE error;
    HENV    env;     /* environment */ 
    HDBC    conn;    /* database connection */ 
    SQLAllocEnv(&env);
    SQLAllocConnect(env, &conn);
    int res = SQLConnect(conn, "dbswe3003", SQL_NTS, "swe3003", SQL_NTS, 
                    "welcome2dicl", SQL_NTS); 

    printf("res=%d\n", res);

    char deptname[80];
    float salary;
    SQLLEN lenOut1, lenOut2;
    HSTMT stmt;
    char * sqlquery = "select dept_name, sum (salary) \
                        from instructor \
                        group by dept_name";
    SQLAllocStmt(conn, &stmt);
    error = SQLExecDirect(stmt, sqlquery, SQL_NTS);
    if (error == SQL_SUCCESS) {
        SQLBindCol(stmt, 1, SQL_C_CHAR, deptname, 80, &lenOut1);
        SQLBindCol(stmt, 2, SQL_C_FLOAT, &salary, 0 , &lenOut2);
        while (SQLFetch(stmt) == SQL_SUCCESS) {
            printf (" %s %g\n", deptname, salary);
        }
    }
    SQLFreeStmt(stmt, SQL_DROP);
    SQLDisconnect(conn); 
    SQLFreeConnect(conn); 
    SQLFreeEnv(env); 
}