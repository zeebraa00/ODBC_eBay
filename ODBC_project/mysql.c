#include <stdio.h>
#include <string.h>
#include <mysql.h>

char * ddl1 = "create table user (uid int AUTO_INCREMENT, name varchar(20) not null, email varchar(20) not null, pw varchar(20), primary key (uid) );";
char * ddl2 = "create table item (id int AUTO_INCREMENT, uid int, category varchar(20), description varchar(100), cond varchar(10) check (cond in ('new', 'like-new', 'very-good', 'good', 'acceptable')), latest_bid numeric(5,0) not null, buy_it_now numeric(5,0), status varchar(20), posted_date datetime, end_date datetime, primary key (id), foreign key (uid) references user(uid) on delete cascade );";
char * ddl3 = "create table transaction (tid int AUTO_INCREMENT, id int, transaction_date datetime, seller_id int, buyer_id int, sell_price numeric(5,0) not null, primary key (tid), foreign key (id) references item(id) on delete cascade, foreign key (seller_id) references user(uid) on delete cascade, foreign key (buyer_id) references user(uid) on delete cascade );";
char * ddl4 = "create table bid_history (uid int, id int, bid_price numeric(5,0), primary key (id, bid_price), foreign key (uid) references user(uid) on delete cascade, foreign key (id) references item(id) on delete cascade );";
char * ddl5 = "create table watched ( uid int, id int, watchedAt datetime, primary key (uid, id, watchedAt), foreign key (uid) references user(uid) on delete cascade, foreign key (id) references item(id) on delete cascade );";

// Initialize mysql
void sql_init(MYSQL *conn) {
    if (conn==NULL) {
        printf("mysql_init: %s\n", mysql_error(conn));
        exit(1);
    }
}

// Connect to database
void sql_connect(MYSQL *conn) {
    if (mysql_real_connect(conn, "localhost", "db16314728", "970321", "db16314728", 0, NULL,  0)==NULL){
        printf("mysql_real_connect: %s\n", mysql_error(conn));
        mysql_close(conn);
        exit(1);
    }
}

// Implement query
void query(MYSQL * conn, char * sqlquery) {
    if (mysql_query(conn, sqlquery)){
        printf("mysql_query: %s\n", mysql_error(conn));
        mysql_close(conn);
        exit(1);
    }
}

// Implement query and Print the result
void query_print(MYSQL * conn, char * sqlquery) {
    // char * sqlquery = "show tables;";
    query(conn, sqlquery);
    // Print the result of query
    MYSQL_RES *result = mysql_store_result(conn);
    if (result == NULL) {
        printf("mysql_store_result: %s\n", mysql_error(conn));
        mysql_close(conn);
        exit(1);
    }
    int num_fields = mysql_num_fields(result);
    MYSQL_ROW row;
    while (( row = mysql_fetch_row(result) )) {
        for (int i=0; i<num_fields; i++) {
            printf (" %s ", row[i]? row[i] : "NULL");
        }
        printf("\n");
    }
    mysql_free_result(result);
    mysql_close(conn);
}

int main(int argc, char* argv[]) {
    // Initialize mysql
    MYSQL *conn = mysql_init(NULL);
    sql_init(conn);
    // Connect to database
    sql_connect(conn);
    // Implement DDL
    query(conn, ddl1);query(conn, ddl2);query(conn, ddl3);query(conn, ddl4);query(conn, ddl5);
    
    query_print(conn,"show tables;");

    return 0;
}