#include <stdio.h>
#include <mysql.h>

MYSQL *conn;
MYSQL_RES *result;
char * show_tables = "show tables;";
char * show_user = "select * from user;";
char * show_item = "select * from item;";
char * show_transaction = "select * from transaction;";
char * show_bid_history = "select * from bid_history;";
char * show_watched = "select * from watched;";

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
    if(mysql_query(conn, sqlquery)){
        printf("mysql_query: %s\n", mysql_error(conn));
        mysql_close(conn);
        exit(1);
    }

    result = mysql_store_result(conn);
    if(result == NULL){
        printf("mysql_store_result: %s\n", mysql_error(conn));
        mysql_close(conn);
        exit(1);
    }

    int num_fields = mysql_num_fields(result);

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        for(int i=0; i<num_fields; i++){
            printf (" %s ", row[i]? row[i] : "NULL");
        }
        printf("\n");
    }
}

int main(int argc, char* argv[]) {
    conn = mysql_init(NULL);
    // Initialize mysql
    sql_init();
    // Connect to database
    sql_connect();
    // Implement DDL
    query(show_tables);
    query(show_user);
    query(show_item);
    query(show_transaction);
    query(show_bid_history);
    query(show_watched);

    mysql_free_result(result);
    mysql_close(conn);
    return 0;
}