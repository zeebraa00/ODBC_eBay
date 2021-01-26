#include <stdio.h>
#include <mysql.h>

MYSQL *conn;
char * drop_tables = "drop tables user, item, bid_history, transaction, watched;";

// Initialize mysql
void sql_init() {
    if (conn==NULL) {
        printf("mysql_init: %s\n", mysql_error(conn));
        exit(1);
    }
}

// Connect to database
void sql_connect() {
    if (mysql_real_connect(conn, "localhost", "db16314728", "970321", "db16314728", 0, NULL,  0)==NULL){
        mysql_close(conn);
        printf("mysql_real_connect: %s\n", mysql_error(conn));
        exit(1);
    }
}

void query(char * sqlquery) {
    if (mysql_query(conn, sqlquery)){
        printf("mysql_query: %s\n", mysql_error(conn));
        mysql_close(conn);
        exit(1);
    }
}

int main(int argc, char* argv[]) {
    conn = mysql_init(NULL);
    // Initialize mysql
    sql_init();
    // Connect to database
    sql_connect();
    // Implement DDL
    query(drop_tables);

    return 0;
}