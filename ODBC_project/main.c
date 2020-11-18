#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql.h>

char * ddl1 = "create table user (uid int AUTO_INCREMENT, name varchar(20) not null, email varchar(20) not null, pw varchar(20), level int check (level in (0,1)), primary key (uid) );";
char * ddl2 = "create table item (id int AUTO_INCREMENT, uid int, category varchar(20), description varchar(100), cond varchar(10) check (cond in ('new', 'like-new', 'very-good', 'good', 'acceptable')), latest_bid numeric(5,0) not null, buy_it_now numeric(5,0), status varchar(20), posted_date datetime, end_date datetime, primary key (id), foreign key (uid) references user(uid) on delete cascade );";
char * ddl3 = "create table transaction (tid int AUTO_INCREMENT, id int, transaction_date datetime, seller_id int, buyer_id int, sell_price numeric(5,0) not null, primary key (tid), foreign key (id) references item(id) on delete cascade, foreign key (seller_id) references user(uid) on delete cascade, foreign key (buyer_id) references user(uid) on delete cascade );";
char * ddl4 = "create table bid_history (uid int, id int, bid_price numeric(5,0), primary key (id, bid_price), foreign key (uid) references user(uid) on delete cascade, foreign key (id) references item(id) on delete cascade );";
char * ddl5 = "create table watched ( uid int, id int, watchedAt datetime, primary key (uid, id, watchedAt), foreign key (uid) references user(uid) on delete cascade, foreign key (id) references item(id) on delete cascade );";
char * admin = "insert into user(name,email,pw,level) values('admin','admin','admin1234',1);";

MYSQL *conn;
MYSQL_RES *result;

int user_input(int n) {
    int buff = 0;
    scanf("%d",&buff);
    while (getchar() != '\n');
    for (int i = 0; i < n; i++) {
        if (buff == i+1) {
            return buff;
        }
    }
    buff = 0;
    return buff;
}

char * process_query_result(char * output, int n) {
    char *sArr[100] = {NULL, };
    int i = 0;
    char *ptr = strtok(output, "+");
    while (ptr != NULL) {
        sArr[i]=ptr;
        i++;
        ptr = strtok(NULL, "+");
    }
    return sArr[n];
}

// Initialize mysql
void sql_init() {
    conn = mysql_init(NULL);
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
    char *output = (char *) malloc(1000);
    strcpy(output, "");
    // Implement the query
    if (mysql_query(conn, sqlquery)){
        printf("mysql_query: %s\n", mysql_error(conn));
        // mysql_close(conn);
        return output;
    }
    result = mysql_store_result(conn);
    if (result == NULL) {
        printf("mysql_store_result: %s\n", mysql_error(conn));
        // mysql_close(conn);
        return output;
    }
    
    int num_fields = mysql_num_fields(result);
    MYSQL_ROW row;
    
    // Print the result of query
    // Make return string : rt1+rt2+rt3+...
    if (do_print) {
        if (result == NULL) {
            printf("mysql_store_result: %s\n", mysql_error(conn));
            // mysql_close(conn);
            return output;
        }
        int length=0;
        while (( row = mysql_fetch_row(result) )) {
            printf("Query result : ");
            for (int i=0; i<num_fields; i++) {
                printf("row[i] : %s \n", row[i]? row[i] : "NULL");
                char * temp = (char *) malloc(strlen(row[i]));
                strcpy(temp, row[i]);
                printf("temp : %s \n", temp);
                if (temp) {
                    strcat(temp,"+");
                    length+=strlen(temp);
                    strcat(output,temp);
                } else {
                    strcat(output,"NULL+"); // NULL인 경우 확인 안해봤음 꼭 해보셈 @@@@@
                    length+=5;
                }
            }
            output[length]='\0';
            printf("\n");
        }
    }
    return output;
}

int login_menu() {
    printf("----< Login menu >\n");
    printf("----(1) Login\n");
    printf("----(2) Sign up\n");
    printf("----(3) Login as Administrator\n");
    printf("----(4) Quit\n");
    int num = user_input(4);
    return num;
}

int login() {
    char query[1000];
    char email[45];
    char pw[20];
    char *output;
    MYSQL_ROW result;
    printf("----< Login >\n");
    printf("email:");scanf("%s",email); 
    printf("password:");scanf("%s",pw);
    sprintf(query, "select uid from user where email='%s' and pw='%s';",email,pw);
    printf("Query : %s\n",query);
    output=sql_query(query, true);
    printf("query output : %s\n",output);
    if (strcmp(output,"")) {
        puts("login success");
        char *temp = (char *) malloc(10);
        strcpy(temp,process_query_result(output,0));
        return atoi(temp);
    } else {
        puts("login failed");
        return 0;
    }
}

void sign_up() {
    char query[1000];
    char name[40];
    char firstname[20];
    char lastname[20];
    char email[45];
    char pw[20];
    char *output;
    printf("----< Sign up >\n");
    printf("---- first name:");scanf("%s",firstname);
    printf("---- last name:");scanf("%s",lastname);
    printf("---- email:");scanf("%s",email);
    printf("---- password:");scanf("%s",pw);
    sprintf(name,"%s %s",firstname,lastname);
    sprintf(query, "insert into user(name,email,pw,level) values('%s','%s','%s',%d);",name,email,pw,0);
    printf("Query : %s\n",query);
    output=sql_query(query, false);
    printf("query output : %s\n",output);
    puts("sign up success");
}

int login_admin() {
    char query[1000];
    char email[45];
    char pw[20];
    char *output;
    MYSQL_ROW result;
    printf("----< Login as Administrator >\n");
    printf("email:");scanf("%s",email);
    printf("password:");scanf("%s",pw);
    sprintf(query, "select uid from user where email='%s' and pw='%s';",email,pw);
    printf("Query : %s\n",query);
    output=sql_query(query, true);
    if (!strcmp(output,"1+")) {
        puts("You are admin");
        return 1;
    } else {
        puts("You are not admin.");
        return 0;
    }
}

void sell_item(int user_id) {
    printf("----< Sell item >\n");
    printf("---- select item the following category : \n");
    printf("----(1) Electronics\n");
    printf("----(2) Books\n");
    printf("----(3) Home\n");
    printf("----(4) Clothing\n");
    printf("----(5) Sporting Goods\n");
    int buff = user_input(5);
    printf("category : %d\n",buff);
}

void main_menu(int user_id) {
    printf("----< Main menu >\n");
    printf("----(1) Sell item\n");
    printf("----(2) Status of Your Item Listed on Auction\n");
    printf("----(3) Search item\n");
    printf("----(4) Check Status of your Bid\n");
    printf("----(5) Check your Account\n");
    printf("----(6) Quit\n");

    while (true) {
        int num = user_input(6);
        
        if (num == 1) {
            sell_item(user_id);
            continue;
        } else if (num == 2) {
            // chk_auction_status();
            continue;
        } else if (num == 3) {
            // search_item();
            continue;
        } else if (num == 4) {
            // chk_bid_status();
            continue;
        } else if (num == 5) {
            // chk_account();
            continue;
        } else if (num == 6) {
            break;
        } else {
            puts("wrong input");
            continue;
        }
    }
}

int main(int argc, char* argv[]) {
    // Initialize mysql
    sql_init();
    // Connect to database
    sql_connect();
    // Implement DDL
    // ddl(ddl1);ddl(ddl2);ddl(ddl3);ddl(ddl4);ddl(ddl5);ddl(admin);
    
    while (true) {
        int chosen1 = login_menu();

        if (chosen1 == 1) {
            int user_id=login(); // admin if user_id == 1
            if (user_id) {
                main_menu(user_id);
            } else {
                continue;
            };
        } else if (chosen1 == 2) {
            sign_up();
            continue;
        } else if (chosen1 == 3) {
            if (login_admin()) {
                main_menu(1); // admin if user_id == 1
            } else {
                continue;
            };
            continue;
        } else if (chosen1 == 4) {
            exit(0);
        } else {
            puts("wrong input");
        }
    }

    mysql_free_result(result);
    mysql_close(conn);
    
    return 0;
}