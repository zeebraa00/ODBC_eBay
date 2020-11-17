#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql.h>

char * ddl1 = "create table user (uid int AUTO_INCREMENT, name varchar(20) not null, email varchar(20) not null, pw varchar(20), level int check (level in (0,1)), primary key (uid) );";
char * ddl2 = "create table item (id int AUTO_INCREMENT, uid int, category varchar(20), description varchar(100), cond varchar(10) check (cond in ('new', 'like-new', 'very-good', 'good', 'acceptable')), latest_bid numeric(5,0) not null, buy_it_now numeric(5,0), status varchar(20), posted_date datetime, end_date datetime, primary key (id), foreign key (uid) references user(uid) on delete cascade );";
char * ddl3 = "create table transaction (tid int AUTO_INCREMENT, id int, transaction_date datetime, seller_id int, buyer_id int, sell_price numeric(5,0) not null, primary key (tid), foreign key (id) references item(id) on delete cascade, foreign key (seller_id) references user(uid) on delete cascade, foreign key (buyer_id) references user(uid) on delete cascade );";
char * ddl4 = "create table bid_history (uid int, id int, bid_price numeric(5,0), primary key (id, bid_price), foreign key (uid) references user(uid) on delete cascade, foreign key (id) references item(id) on delete cascade );";
char * ddl5 = "create table watched ( uid int, id int, watchedAt datetime, primary key (uid, id, watchedAt), foreign key (uid) references user(uid) on delete cascade, foreign key (id) references item(id) on delete cascade );";

MYSQL *conn;

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

// Implement DDL
void ddl(char * sqlquery) {
    if (mysql_query(conn, sqlquery)){
        printf("mysql_query: %s\n", mysql_error(conn));
        mysql_close(conn);
        exit(1);
    }
}

// Implement query and Print the result
// you should set do_print to false if there is no result of query
char * sql_query(char * sqlquery, bool do_print) {
    char *output = malloc(1000);
    strcpy(output, "");
    
    // Implement the query
    if (mysql_query(conn, sqlquery)){
        printf("mysql_query: %s\n", mysql_error(conn));
        mysql_close(conn);
        return output;
    }
    MYSQL_RES *result = mysql_store_result(conn);
    if (result == NULL) {
        printf("mysql_store_result: %s\n", mysql_error(conn));
        mysql_close(conn);
        return output;
    }
    
    int num_fields = mysql_num_fields(result);
    MYSQL_ROW row;
    
    // Print the result of query
    if (do_print) {
        if (result == NULL) {
            printf("mysql_store_result: %s\n", mysql_error(conn));
            mysql_close(conn);
            return output;
        }
        while (( row = mysql_fetch_row(result) )) {
            printf("Query result : ");
            for (int i=0; i<num_fields; i++) {
                printf("%s ", row[i]? row[i] : "NULL");
            }
            printf("\n");
        }
    }

    // Make return string : rt1+rt2+rt3+...
    int length=0;
    while (( row = mysql_fetch_row(result) )) {
        int i;
        for (i=0; i<num_fields; i++) {
            if (row[i]) {
                strcat(row[i],"+");
                length+=strlen(row[i]);
                strcat(output,row[i]);
            } else {
                strcat(output,"NULL+");
                length+=5;
            }
        }
        output[length]='\0';
    }
    mysql_free_result(result);
    mysql_close(conn);
    return output;
}

int login_menu() {
    int num;
    printf("----< Login menu >\n");
    printf("----(1) Login\n");
    printf("----(2) Sign up\n");
    printf("----(3) Login as Administrator\n");
    printf("----(4) Quit\n");
    scanf("%d",&num);
    return num;
}

void login() {
    char query[1000];
    char email[45];
    char pw[20];
    MYSQL_ROW result;
    printf("----< Login >\n");
    printf("email:");scanf("%s",email); 
    printf("password:");scanf("%s",pw);
    sprintf(query, "select uid from user where email='%s' and pw='%s';",email,pw);
    printf("Query : %s\n",query);
    sql_query(query, true);
}

void sign_up() {
    char query[1000];
    char name[40];
    char firstname[20];
    char lastname[20];
    char email[45];
    char pw[20];
    printf("----< Sign up >\n");
    printf("---- first name:");scanf("%s",firstname);
    printf("---- last name:");scanf("%s",lastname);
    printf("---- email:");scanf("%s",email);
    printf("---- password:");scanf("%s",pw);
    sprintf(name,"%s %s",firstname,lastname);
    sprintf(query, "insert into user(name,email,pw,level) values('%s','%s','%s',%d);",name,email,pw,0);
    printf("Query : %s\n",query);
    sql_query(query, false);
}

void login_admin() {
    char query[1000];
    char email[45];
    char pw[20];
    MYSQL_ROW result;
    printf("----< Login as Administrator >\n");
    printf("email:");scanf("%s",email);
    printf("password:");scanf("%s",pw);
    sprintf(query, "select level from user where email='%s' and pw='%s';",email,pw);
    printf("Query : %s\n",query);
    sql_query(query, true);
}

int main(int argc, char* argv[]) {
    conn = mysql_init(NULL);
    // Initialize mysql
    sql_init();
    // Connect to database
    sql_connect();
    // Implement DDL
    ddl(ddl1);ddl(ddl2);ddl(ddl3);ddl(ddl4);ddl(ddl5);
    
    while (true) {
        int chosen = login_menu();

        if (chosen == 1) {
            login();
        } else if (chosen == 2) {
            sign_up();
        } else if (chosen == 3) {
            login_admin();
        } else if (chosen == 4) {
            exit(0);
        } else {
            puts("wrong input");
        }
    }
    
    sql_query("show tables;",true);

    return 0;
}