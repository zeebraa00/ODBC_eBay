#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql.h>

char * ddl1 = "create table user (uid int AUTO_INCREMENT, name varchar(20) not null, email varchar(20) not null, pw varchar(20), level int check (level in (0,1)), primary key (uid) );";
char * ddl2 = "create table item (id int AUTO_INCREMENT, uid int, category varchar(20), description varchar(100), cond varchar(15) check (cond in ('new', 'like-new', 'very-good', 'good', 'acceptable')), latest_bid numeric(15,0) not null, buy_it_now numeric(15,0), status varchar(20), posted_date datetime, end_date datetime, primary key (id), foreign key (uid) references user(uid) on delete cascade);";
char * ddl3 = "create table transaction (tid int AUTO_INCREMENT, id int, transaction_date datetime default now(), seller_id int, buyer_id int, sell_price numeric(15,0) not null, primary key (tid), foreign key (id) references item(id) on delete cascade, foreign key (seller_id) references user(uid) on delete cascade, foreign key (buyer_id) references user(uid) on delete cascade );";
char * ddl4 = "create table bid_history (uid int, id int, bid_price numeric(15,0), status varchar(100), primary key (id, bid_price), foreign key (uid) references user(uid) on delete cascade, foreign key (id) references item(id) on delete cascade );";
char * ddl5 = "create table watched ( uid int, id int, watchedAt datetime default now(), primary key (uid, id, watchedAt), foreign key (uid) references user(uid) on delete cascade, foreign key (id) references item(id) on delete cascade );";
char * admin = "insert into user(name,email,pw,level) values('admin','admin','admin1234',1);";

MYSQL *conn;
MYSQL_RES *result;

int user_input(int n) {
    int buff = 0;
    while (!buff) {
        scanf("%d",&buff);
        while (getchar() != '\n');
        for (int i = 0; i < n; i++) {
            if (buff == i+1) {
                return buff;
            }
        }
        printf("wrong input. try again\n");
    }
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
// you should set do_print to 0 if there is no result of query
char * sql_query(char * sqlquery, int do_print) {
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
    if (result == NULL) {
        printf("mysql_store_result: %s\n", mysql_error(conn));
        // mysql_close(conn);
        return output;
    }
    int length=0;
    while (( row = mysql_fetch_row(result) )) {
        if (do_print) printf("Query result : ");
        for (int i=0; i<num_fields; i++) {
            if (do_print) printf("row[i] : %s \n", row[i]? row[i] : "NULL");
            char * temp = (char *) malloc(strlen(row[i]));
            strcpy(temp, row[i]);
            // printf("temp : %s \n", temp);
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
        if (do_print) printf("\n");
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
    // printf("Query : %s\n",query);
    output=sql_query(query, 0);
    // printf("query output : %s\n",output);
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
    // printf("Query : %s\n",query);
    sql_query(query, 0);
    // printf("query output : %s\n",output);
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
    output=sql_query(query, 0);
    if (!strcmp(output,"1+")) {
        puts("You are admin");
        return 1;
    } else {
        puts("You are not admin.");
        return 0;
    }
}

void sell_item(int user_id) {
    char query[1000];
    char output[100];
    char category[20];
    char condition[15];
    char description[100];
    char buy_now[15];
    char bid_ending[20];

    printf("----< Sell item >\n");
    printf("---- select item the following category : (Enter the number)\n");
    printf("----(1) Electronics\n");
    printf("----(2) Books\n");
    printf("----(3) Home\n");
    printf("----(4) Clothing\n");
    printf("----(5) Sporting Goods\n");
    int cate = user_input(5);
    if (cate == 1) {
        sprintf(category,"Electronics");
    } else if (cate == 2) {
        sprintf(category,"Books");
    } else if (cate == 3) {
        sprintf(category,"Home");
    } else if (cate == 4) {
        sprintf(category,"Clothing");
    } else {
        sprintf(category,"Sporting Goods");
    } 
    printf("---- condition : (Enter the number)\n");
    printf("----(1) new\n");
    printf("----(2) like-new\n");
    printf("----(3) very-good\n");
    printf("----(4) good\n");
    printf("----(5) acceptable\n");
    int cond = user_input(5);
    if (cond == 1) {
        sprintf(condition,"new");
    } else if (cond == 2) {
        sprintf(condition,"like-new");
    } else if (cond == 3) {
        sprintf(condition,"very-good");
    } else if (cond == 4) {
        sprintf(condition,"good");
    } else {
        sprintf(condition,"acceptable");
    }
    printf("---- decription : ");
    fgets(description,100,stdin);
    description[strlen(description)-1]='\0';
    int is_num=0;
    while (!is_num) {
        printf("---- buy-it-now price : ");
        scanf("%s",buy_now);
        is_num=atoi(buy_now);
        if (!is_num) printf("wrong input. try again\n");
    }
    getchar();
    printf("---- bid ending date (yyyy-mm-dd HH:mm, e.g. 2020-12-04 23:59) :");
    fgets(bid_ending,20,stdin);
    bid_ending[strlen(bid_ending)-1]='\0';
    // printf("%s\n",bid_ending);

    // posted_date 전처리
    sprintf(query, "select now()");
    strcpy(output,sql_query(query,0));
    output[strlen(output)-1]='\0';
    output[strlen(output)-1]='0';
    output[strlen(output)-2]='0';

    sprintf(query, "insert into item(uid, category, description, cond, latest_bid, buy_it_now, status, posted_date, end_date) values(%d,'%s','%s','%s',0,%d,'0 bids','%s','%s');",user_id,category,description,condition,is_num,output,bid_ending);
    // printf("Query : %s\n",query);
    sql_query(query, 0);
}

void chk_auction_status(int user_id) {
    char query[1000];
    char output[100];
    char id[10];
    int num;
    printf("----< Status of Your Item Listed on Auction >\n");
    sprintf(query, "select count(id) from item where uid=%d;",user_id);
    strcpy(output,sql_query(query, 0));
    output[strlen(output)-1]='\0';
    num = atoi(output);
    for (int i=0; i<num; i++) {
        sprintf(query, "select id from item where uid=%d order by id asc limit %d,1;",user_id,i);
        strcpy(id,sql_query(query, 0));    
        id[strlen(id)-1]='\0';

        printf("[Item %d]\n",i+1);
        
        sprintf(query, "select description from item where uid=%d order by id asc limit %d,1;",user_id,i);
        strcpy(output,sql_query(query, 0));    
        output[strlen(output)-1]='\0';
        printf("   description: %s\n",output);
        
        sprintf(query, "select status from item where uid=%d order by id asc limit %d,1;",user_id,i);
        strcpy(output,sql_query(query, 0));
        output[strlen(output)-1]='\0';
        printf("   status: %s\n",output);
        
        sprintf(query, "select latest_bid from item where uid=%d order by id asc limit %d,1;",user_id,i);
        strcpy(output,sql_query(query, 0));
        output[strlen(output)-1]='\0';
        printf("   current bidding price: %s\n",output);
        
        sprintf(query, "select name from user where uid=(select uid from bid_history where id = %s order by bid_price desc limit 0,1);",id);
        strcpy(output,sql_query(query, 0));
        output[strlen(output)-1]='\0';
        printf("   current highest bidder: %s\n",output);
        
        sprintf(query, "select posted_date from item where uid=%d order by id asc limit %d,1;",user_id,i);
        strcpy(output,sql_query(query, 0));
        output[strlen(output)-1]='\0';
        printf("   date posted: %s\n",output);
        
        sprintf(query, "select end_date from item where uid=%d order by id asc limit %d,1;",user_id,i);
        strcpy(output,sql_query(query, 0));
        output[strlen(output)-1]='\0';
        printf("   bid ending date: %s\n",output);
    }
}

void search_category(int user_id) {
    int num;
    char query[1000];
    char category[20];
    char description[100];
    char input[15];
    char output[100];
    char id[10];
    char uid[10];
    char latest_bid[100];
    char buy_it_now[100];
    char status[100];
    char history_status[100];

    printf("----< Search items by category > : (Enter the number)\n");
    printf("----(1) Electronics\n");
    printf("----(2) Books\n");
    printf("----(3) Home\n");
    printf("----(4) Clothing\n");
    printf("----(5) Sporting Goods\n");
    num = user_input(5);
    if (num == 1) {
        sprintf(category,"Electronics");
    } else if (num == 2) {
        sprintf(category,"Books");
    } else if (num == 3) {
        sprintf(category,"Home");
    } else if (num == 4) {
        sprintf(category,"Clothing");
    } else {
        sprintf(category,"Sporting Goods");
    }
    printf("----< Search results: Category >\n");
    sprintf(query, "select count(id) from item where category='%s';",category);
    strcpy(output, sql_query(query, 0));
    output[strlen(output)-1]='\0';
    num = atoi(output);
    if (!num) {
        printf("no such result\n");
        return;
    }
    for (int i=0; i<num; i++) {
        
        sprintf(query, "select id from item where category='%s' order by id asc limit %d,1;",category,i);
        strcpy(id,sql_query(query, 0));
        id[strlen(id)-1]='\0';

        printf("[Item %d]\n",i+1);
        
        sprintf(query, "select description from item where category='%s' order by id asc limit %d,1;",category,i);
        strcpy(output,sql_query(query, 0));
        output[strlen(output)-1]='\0';
        printf("   description: %s\n",output);
        
        sprintf(query, "select status from item where category='%s' order by id asc limit %d,1;",category,i);
        strcpy(output,sql_query(query,0)); 
        output[strlen(output)-1]='\0';
        printf("   status: %s\n",output);
        
        sprintf(query, "select latest_bid from item where category='%s' order by id asc limit %d,1;",category,i);
        strcpy(output,sql_query(query,0));
        output[strlen(output)-1]='\0';
        printf("   current bidding price: %s\n",output);
        
        sprintf(query, "select name from user where uid=(select uid from bid_history where id = %s order by bid_price desc limit 0,1);",id);
        strcpy(output,sql_query(query,0));
        output[strlen(output)-1]='\0';
        printf("   current highest bidder: %s\n",output);
        
        sprintf(query, "select posted_date from item where category='%s' order by id asc limit %d,1;",category,i);
        strcpy(output,sql_query(query,0));
        output[strlen(output)-1]='\0';
        printf("   date posted: %s\n",output);
        
        sprintf(query, "select end_date from item where category='%s' order by id asc limit %d,1;",category,i);
        strcpy(output,sql_query(query,0)); 
        output[strlen(output)-1]='\0';
        printf("   bid ending date: %s\n",output);
        
        sprintf(query, "insert into watched(uid,id) values(%d,%s);",user_id,id);
        sql_query(query,0);
    }
    printf("--- Which item do you want to bid? (Enter the number or 'B' to go back to the previous menu) :");
    int is_num=0;
    while (!is_num) {
        scanf("%s",input);
        if (!strcmp(input,"B")) return;
        is_num=atoi(input);
        if (!is_num) printf("wrong input. try again\n");
        int chk=0;
        for (int i=0; i<num; i++) {
            if (is_num == i+1) {
                chk++;
                break;
            }
        }
        if (!chk) {
            is_num=0;
            printf("wrong input. try again\n");
        }
    }
    int item = is_num-1;

    sprintf(query, "select id from item where category='%s' order by id asc limit %d,1;",category,item);
    strcpy(id,sql_query(query,0));
    id[strlen(id)-1]='\0';
    
    sprintf(query, "select latest_bid from item where category='%s' order by id asc limit %d,1;",category,item);
    strcpy(latest_bid,sql_query(query,0));
    latest_bid[strlen(latest_bid)-1]='\0';

    sprintf(query, "select buy_it_now from item where category='%s' order by id asc limit %d,1;",category,item);
    strcpy(buy_it_now,sql_query(query,0));
    buy_it_now[strlen(buy_it_now)-1]='\0';

    sprintf(query, "select uid from item where category='%s' order by id asc limit %d,1;",category,item);
    strcpy(uid,sql_query(query,0));
    uid[strlen(uid)-1]='\0';

    sprintf(query, "select status from item where category='%s' order by id asc limit %d,1;",category,item);
    strcpy(status,sql_query(query,0));
    status[strlen(status)-1]='\0';
    if (!strcmp(status,"sold")) {
        printf("already sold\n");
        return;
    }

    printf("--- Bidding price? (Enter the price or 'buy' to pay for the buy-it-now price) :");
    is_num=0;
    while (!is_num) {
        scanf("%s",input);
        if (!strcmp(input,"buy")) {
            sprintf(status, "sold");
            sprintf(query, "update item set latest_bid=%s, status='%s' where id=%s;",buy_it_now,status,id);
            sql_query(query, 0);
            sprintf(history_status,"You won the item");
            sprintf(query, "insert into bid_history(uid,id,bid_price,status) values(%d,%s,%s,'%s');",user_id,id,buy_it_now,history_status);
            sql_query(query, 0);
            sprintf(query, "insert into transaction(id,seller_id,buyer_id,sell_price) values(%s,%s,%d,%s);",id,uid,user_id,buy_it_now);
            sql_query(query, 0);
            sprintf(status, "You are outbidded and the item is sold");
            sprintf(query, "update bid_history set status='%s' where id=%s and bid_price < %s;",status,id,buy_it_now);
            sql_query(query, 0);
            return;
        }
        is_num=atoi(input);
        if (!is_num) printf("wrong input. try again\n");
    }
    int bidding_price = is_num;
    if (bidding_price >= atoi(buy_it_now)) {
        printf("Your bidding is higher or equal than buy-it-now price.\n");
        printf("Please use buy-it-now functionality.\n");
        return;
    }
    if (bidding_price < atoi(latest_bid)) {
        printf("you are outbidded.\n");
        sprintf(history_status,"You are outbidded");
        sprintf(query, "insert into bid_history(uid,id,bid_price,status) values(%d,%s,%d,'%s');",user_id,id,bidding_price,history_status);
        printf("%s\n",query);
        sql_query(query, 0);
        return;
    }
    sprintf(history_status,"You are the highest bidder.");
    sprintf(query, "insert into bid_history(uid,id,bid_price,status) values(%d,%s,%d,'%s');",user_id,id,bidding_price,history_status);
    sql_query(query, 0);
    sprintf(history_status, "You are outbidded");
    sprintf(query, "update bid_history set status='%s' where id=%s and bid_price < %d;",history_status,id,bidding_price);
    sql_query(query, 0);

    char bids_num = status[0];
    sprintf(status, "%d bids", bids_num+1-48);
    sprintf(query, "update item set latest_bid=%d, status='%s' where id=%s;",bidding_price,status,id);
    sql_query(query, 0);
}

void search_description(int user_id) {
    int num;
    char query[1000];
    char category[20];
    char description[100];
    char input[15];
    char output[100];
    char id[10];
    char uid[10];
    char latest_bid[100];
    char buy_it_now[100];
    char status[100];
    char history_status[100];

    printf("----< Search items by description keyword >\n");
    printf("---- search keyword : ");
    fgets(description,100,stdin);
    description[strlen(description)-1]='\0';
    
    printf("----< Search results: keyword search >\n");
    sprintf(query, "select count(id) from item where description='%s';",description);
    strcpy(output,sql_query(query, 0));
    output[strlen(output)-1]='\0';
    num = atoi(output);
    if (!num) {
        printf("no such result\n");
        return;
    }
    for (int i=0; i<num; i++) {
        sprintf(query, "select id from item where description='%s' order by id asc limit %d,1;",description,i);
        strcpy(id,sql_query(query, 0));   
        id[strlen(id)-1]='\0';

        printf("[Item %d]\n",i+1);
        
        sprintf(query, "select description from item where description='%s' order by id asc limit %d,1;",description,i);
        strcpy(output,sql_query(query, 0));
        output[strlen(output)-1]='\0';
        printf("   description: %s\n",output);
        
        sprintf(query, "select status from item where description='%s' order by id asc limit %d,1;",description,i);
        strcpy(output,sql_query(query, 0));
        output[strlen(output)-1]='\0';
        printf("   status: %s\n",output);
        
        sprintf(query, "select latest_bid from item where description='%s' order by id asc limit %d,1;",description,i);
        strcpy(output,sql_query(query, 0));  
        output[strlen(output)-1]='\0';
        printf("   current bidding price: %s\n",output);
        
        sprintf(query, "select name from user where uid=(select uid from bid_history where id = %s order by bid_price desc limit 0,1);",id);
        strcpy(output,sql_query(query, 0)); 
        output[strlen(output)-1]='\0';
        printf("   current highest bidder: %s\n",output);
        
        sprintf(query, "select posted_date from item where description='%s' order by id asc limit %d,1;",description,i);
        strcpy(output,sql_query(query, 0)); 
        output[strlen(output)-1]='\0';
        printf("   date posted: %s\n",output);
        
        sprintf(query, "select end_date from item where description='%s' order by id asc limit %d,1;",description,i);
        strcpy(output,sql_query(query, 0)); 
        output[strlen(output)-1]='\0';
        printf("   bid ending date: %s\n",output);
        
        sprintf(query, "insert into watched(uid,id) values(%d,%s);",user_id,id);
        sql_query(query,0);
    }
    
    printf("--- Which item do you want to bid? (Enter the number or 'B' to go back to the previous menu) :");
    int is_num=0;
    while (!is_num) {
        scanf("%s",input);
        if (!strcmp(input,"B")) return;
        is_num=atoi(input);
        if (!is_num) printf("wrong input. try again\n");
        int chk=0;
        for (int i=0; i<num; i++) {
            if (is_num == i+1) {
                chk++;
                break;
            }
        }
        if (!chk) {
            is_num=0;
            printf("wrong input. try again\n");
        }
    }
    int item = is_num-1;

    sprintf(query, "select id from item where description='%s' order by id asc limit %d,1;",description,item);
    strcpy(id,sql_query(query, 0));
    id[strlen(id)-1]='\0';
    
    sprintf(query, "select latest_bid from item where description='%s' order by id asc limit %d,1;",description,item);
    strcpy(latest_bid,sql_query(query, 0));
    latest_bid[strlen(latest_bid)-1]='\0';

    sprintf(query, "select buy_it_now from item where description='%s' order by id asc limit %d,1;",description,item);
    strcpy(buy_it_now,sql_query(query, 0));
    buy_it_now[strlen(buy_it_now)-1]='\0';

    sprintf(query, "select uid from item where description='%s' order by id asc limit %d,1;",description,item);
    strcpy(uid,sql_query(query, 0));
    uid[strlen(uid)-1]='\0';

    sprintf(query, "select status from item where description='%s' order by id asc limit %d,1;",description,item);
    strcpy(status,sql_query(query, 0));
    status[strlen(status)-1]='\0';
    if (!strcmp(status,"sold")) {
        printf("already sold\n");
        return;
    }

    printf("--- Bidding price? (Enter the price or 'buy' to pay for the buy-it-now price) :");
    is_num=0;
    while (!is_num) {
        scanf("%s",input);
        if (!strcmp(input,"buy")) {
            sprintf(status, "sold");
            sprintf(query, "update item set latest_bid=%s, status='%s' where id=%s;",buy_it_now,status,id);
            sql_query(query, 0);
            sprintf(history_status,"You won the item");
            sprintf(query, "insert into bid_history(uid,id,bid_price,status) values(%d,%s,%s,'%s');",user_id,id,buy_it_now,history_status);
            sql_query(query, 0);
            sprintf(query, "insert into transaction(id,seller_id,buyer_id,sell_price) values(%s,%s,%d,%s);",id,uid,user_id,buy_it_now);
            sql_query(query, 0);
            sprintf(status, "You are outbidded and the item is sold");
            sprintf(query, "update bid_history set status='%s' where id=%s and bid_price < %s;",status,id,buy_it_now);
            sql_query(query, 0);
            return;
        }
        is_num=atoi(input);
        if (!is_num) printf("wrong input. try again\n");
    }
    int bidding_price = is_num;
    if (bidding_price >= atoi(buy_it_now)) {
        printf("Your bidding is higher or equal than buy-it-now price.\n");
        printf("Please use buy-it-now functionality.\n");
        return;
    }

    if (bidding_price < atoi(latest_bid)) {
        printf("you are outbidded.\n");
        sprintf(history_status,"You are outbidded");
        sprintf(query, "insert into bid_history(uid,id,bid_price,status) values(%d,%s,%d,'%s');",user_id,id,bidding_price,history_status);
        sql_query(query, 0);
        return;
    }
    sprintf(history_status,"You are the highest bidder.");
    sprintf(query, "insert into bid_history(uid,id,bid_price,status) values(%d,%s,%d,'%s');",user_id,id,bidding_price,history_status);
    sql_query(query, 0);
    sprintf(status, "You are outbidded");
    sprintf(query, "update bid_history set status='%s' where id=%s and bid_price < %d;",status,id,bidding_price);
    sql_query(query, 0);

    char bids_num = status[0];
    sprintf(status, "%c bids", bids_num+1-48);
    sprintf(query, "update item set latest_bid=%d, status='%s' where id=%s;",bidding_price,status,id);
    sql_query(query, 0);
}

void search_seller(int user_id) {
    int num;
    char query[1000];
    char category[20];
    char seller[40];
    char input[15];
    char output[100];
    char id[10];
    char uid[10];
    char latest_bid[100];
    char buy_it_now[100];
    char status[100];
    char history_status[100];

    printf("----< Search items by seller >\n");
    printf("---- search seller : ");
    fgets(seller,40,stdin);
    seller[strlen(seller)-1]='\0';
    
    printf("----< Search results: seller search >\n");
    sprintf(query, "select count(id) from item where uid=(select uid from user where name='%s');",seller);
    strcpy(output,sql_query(query, 0));
    output[strlen(output)-1]='\0';
    num = atoi(output);
    if (!num) {
        printf("no such result\n");
        return;
    }
    for (int i=0; i<num; i++) {
        sprintf(query, "select id from item where uid=(select uid from user where name='%s') order by id asc limit %d,1;",seller,i);
        strcpy(id,sql_query(query, 0));  
        id[strlen(id)-1]='\0';

        printf("[Item %d]\n",i+1);
        
        sprintf(query, "select description from item where uid=(select uid from user where name='%s') order by id asc limit %d,1;",seller,i);
        strcpy(output,sql_query(query, 0));
        output[strlen(output)-1]='\0';
        printf("   description: %s\n",output);
        
        sprintf(query, "select status from item where uid=(select uid from user where name='%s') order by id asc limit %d,1;",seller,i);
        strcpy(output,sql_query(query, 0));
        output[strlen(output)-1]='\0';
        printf("   status: %s\n",output);
        
        sprintf(query, "select latest_bid from item where uid=(select uid from user where name='%s') order by id asc limit %d,1;",seller,i);
        strcpy(output,sql_query(query, 0));
        output[strlen(output)-1]='\0';
        printf("   current bidding price: %s\n",output);
        
        sprintf(query, "select name from user where uid=(select uid from bid_history where id = %s order by bid_price desc limit 0,1);",id);
        strcpy(output,sql_query(query, 0));
        output[strlen(output)-1]='\0';
        printf("   current highest bidder: %s\n",output);
        
        sprintf(query, "select posted_date from item where uid=(select uid from user where name='%s') order by id asc limit %d,1;",seller,i);
        strcpy(output,sql_query(query, 0));
        output[strlen(output)-1]='\0';
        printf("   date posted: %s\n",output);
        
        sprintf(query, "select end_date from item where uid=(select uid from user where name='%s') order by id asc limit %d,1;",seller,i);
        strcpy(output,sql_query(query, 0));
        output[strlen(output)-1]='\0';
        printf("   bid ending date: %s\n",output);

        sprintf(query, "insert into watched(uid,id) values(%d,%s);",user_id,id);
        sql_query(query,0);
    }
    
    printf("--- Which item do you want to bid? (Enter the number or 'B' to go back to the previous menu) :");
    int is_num=0;
    while (!is_num) {
        scanf("%s",input);
        if (!strcmp(input,"B")) return;
        is_num=atoi(input);
        if (!is_num) printf("wrong input. try again\n");
        int chk=0;
        for (int i=0; i<num; i++) {
            if (is_num == i+1) {
                chk++;
                break;
            }
        }
        if (!chk) {
            is_num=0;
            printf("wrong input. try again\n");
        }
    }
    int item = is_num-1;

    sprintf(query, "select id from item where uid=(select uid from user where name='%s') order by id asc limit %d,1;",seller,item);
    strcpy(id,sql_query(query, 0));
    id[strlen(id)-1]='\0';
    
    sprintf(query, "select latest_bid from item where uid=(select uid from user where name='%s') order by id asc limit %d,1;",seller,item);
    strcpy(latest_bid,sql_query(query, 0));
    latest_bid[strlen(latest_bid)-1]='\0';

    sprintf(query, "select buy_it_now from item where uid=(select uid from user where name='%s') order by id asc limit %d,1;",seller,item);
    strcpy(buy_it_now,sql_query(query, 0));
    buy_it_now[strlen(buy_it_now)-1]='\0';

    sprintf(query, "select uid from user where name='%s';",seller);
    strcpy(uid,sql_query(query, 0));
    uid[strlen(uid)-1]='\0';

    sprintf(query, "select status from item where uid=(select uid from user where name='%s') order by id asc limit %d,1;",seller,item);
    strcpy(status,sql_query(query, 0));
    status[strlen(status)-1]='\0';
    if (!strcmp(status,"sold")) {
        printf("already sold\n");
        return;
    }

    printf("--- Bidding price? (Enter the price or 'buy' to pay for the buy-it-now price) :");
    is_num=0;
    while (!is_num) {
        scanf("%s",input);
        if (!strcmp(input,"buy")) {
            sprintf(status, "sold");
            sprintf(query, "update item set latest_bid=%s, status='%s' where id=%s;",buy_it_now,status,id);
            sql_query(query, 0);
            sprintf(history_status,"You won the item");
            sprintf(query, "insert into bid_history(uid,id,bid_price,status) values(%d,%s,%s,'%s');",user_id,id,buy_it_now,history_status);
            sql_query(query, 0);
            sprintf(query, "insert into transaction(id,seller_id,buyer_id,sell_price) values(%s,%s,%d,%s);",id,uid,user_id,buy_it_now);
            sql_query(query, 0);
            sprintf(status, "You are outbidded and the item is sold");
            sprintf(query, "update bid_history set status='%s' where id=%s and bid_price < %s;",status,id,buy_it_now);
            sql_query(query, 0);
            return;
        }
        is_num=atoi(input);
        if (!is_num) printf("wrong input. try again\n");
    }
    int bidding_price = is_num;
    if (bidding_price >= atoi(buy_it_now)) {
        printf("Your bidding is higher or equal than buy-it-now price.\n");
        printf("Please use buy-it-now functionality.\n");
        return;
    }

    if (bidding_price < atoi(latest_bid)) {
        printf("you are outbidded.\n");
        sprintf(history_status,"You are outbidded");
        sprintf(query, "insert into bid_history(uid,id,bid_price,status) values(%d,%s,%d,'%s');",user_id,id,bidding_price,history_status);
        sql_query(query, 0);
        return;
    }
    sprintf(history_status,"You are the highest bidder.");
    sprintf(query, "insert into bid_history(uid,id,bid_price,status) values(%d,%s,%d,'%s');",user_id,id,bidding_price,history_status);
    sql_query(query, 0);
    sprintf(status, "You are outbidded");
    sprintf(query, "update bid_history set status='%s' where id=%s and bid_price < %d;",status,id,bidding_price);
    sql_query(query, 0);

    char bids_num = status[0];
    sprintf(status, "%c bids", bids_num+1-48);
    sprintf(query, "update item set latest_bid=%d, status='%s' where id=%s;",bidding_price,status,id);
    sql_query(query, 0);
}

void search_date(int user_id) {
    int num;
    char query[1000];
    char date[20];
    char description[100];
    char input[15];
    char output[100];
    char id[10];
    char uid[10];
    char latest_bid[100];
    char buy_it_now[100];
    char status[100];
    char history_status[100];

    printf("----< Search items by date posted >\n");
    printf("---- search date posted : (yyyy-mm-dd HH:mm, e.g. 2020-12-04 23:59) :");
    fgets(date,20,stdin);
    date[strlen(date)-1]='\0';
    printf("----< Search results: posted date search >\n");
    sprintf(query, "select count(id) from item where posted_date='%s';",date);
    strcpy(output,sql_query(query,0));
    output[strlen(output)-1]='\0';
    num = atoi(output);
    if (!num) {
        printf("no such result\n");
        return;
    }
    for (int i=0; i<num; i++) {
        sprintf(query, "select id from item where posted_date='%s' order by id asc limit %d,1;",date,i);
        strcpy(id,sql_query(query,0));
        id[strlen(id)-1]='\0';

        printf("[Item %d]\n",i+1);
        
        sprintf(query, "select description from item where posted_date='%s' order by id asc limit %d,1;",date,i);
        strcpy(output,sql_query(query,0));
        output[strlen(output)-1]='\0';
        printf("   description: %s\n",output);
        
        sprintf(query, "select status from item where posted_date='%s' order by id asc limit %d,1;",date,i);  
        strcpy(output,sql_query(query,0));
        output[strlen(output)-1]='\0';
        printf("   status: %s\n",output);
        
        sprintf(query, "select latest_bid from item where posted_date='%s' order by id asc limit %d,1;",date,i); 
        strcpy(output,sql_query(query,0));
        output[strlen(output)-1]='\0';
        printf("   current bidding price: %s\n",output);
        
        sprintf(query, "select name from user where uid=(select uid from bid_history where id = %s order by bid_price desc limit 0,1);",id);
        strcpy(output,sql_query(query,0));
        output[strlen(output)-1]='\0';
        printf("   current highest bidder: %s\n",output);
        
        sprintf(query, "select posted_date from item where posted_date='%s' order by id asc limit %d,1;",date,i);  
        strcpy(output,sql_query(query,0));
        output[strlen(output)-1]='\0';
        printf("   date posted: %s\n",output);
        
        sprintf(query, "select end_date from item where posted_date='%s' order by id asc limit %d,1;",date,i); 
        strcpy(output,sql_query(query,0));
        output[strlen(output)-1]='\0';
        printf("   bid ending date: %s\n",output);

        sprintf(query, "insert into watched(uid,id) values(%d,%s);",user_id,id);
        sql_query(query,0);
    }
    
    printf("--- Which item do you want to bid? (Enter the number or 'B' to go back to the previous menu) :");
    int is_num=0;
    while (!is_num) {
        scanf("%s",input);
        if (!strcmp(input,"B")) return;
        is_num=atoi(input);
        if (!is_num) printf("wrong input. try again\n");
        int chk=0;
        for (int i=0; i<num; i++) {
            if (is_num == i+1) {
                chk++;
                break;
            }
        }
        if (!chk) {
            is_num=0;
            printf("wrong input. try again\n");
        }
    }
    int item = is_num-1;

    sprintf(query, "select id from item where posted_date='%s' order by id asc limit %d,1;",date,item);
    strcpy(id,sql_query(query,0));
    id[strlen(id)-1]='\0';
    
    sprintf(query, "select latest_bid from item where posted_date='%s' order by id asc limit %d,1;",date,item);
    strcpy(latest_bid,sql_query(query,0));
    latest_bid[strlen(latest_bid)-1]='\0';

    sprintf(query, "select buy_it_now from item where posted_date='%s' order by id asc limit %d,1;",date,item);
    strcpy(buy_it_now,sql_query(query,0));
    buy_it_now[strlen(buy_it_now)-1]='\0';

    sprintf(query, "select uid from item where posted_date='%s' order by id asc limit %d,1;",date,item);
    strcpy(uid,sql_query(query,0));
    uid[strlen(uid)-1]='\0';

    sprintf(query, "select status from item where posted_date='%s' order by id asc limit %d,1;",date,item);
    strcpy(status,sql_query(query,0));
    status[strlen(status)-1]='\0';
    if (!strcmp(status,"sold")) {
        printf("already sold\n");
        return;
    }

    printf("--- Bidding price? (Enter the price or 'buy' to pay for the buy-it-now price) :");
    is_num=0;
    while (!is_num) {
        scanf("%s",input);
        if (!strcmp(input,"buy")) {
            sprintf(status, "sold");
            sprintf(query, "update item set latest_bid=%s, status='%s' where id=%s;",buy_it_now,status,id);
            sql_query(query, 0);
            sprintf(history_status,"You won the item");
            sprintf(query, "insert into bid_history(uid,id,bid_price,status) values(%d,%s,%s,'%s');",user_id,id,buy_it_now,history_status);
            sql_query(query, 0);
            sprintf(query, "insert into transaction(id,seller_id,buyer_id,sell_price) values(%s,%s,%d,%s);",id,uid,user_id,buy_it_now);
            sql_query(query, 0);
            sprintf(status, "You are outbidded and the item is sold");
            sprintf(query, "update bid_history set status='%s' where id=%s and bid_price < %s;",status,id,buy_it_now);
            sql_query(query, 0);
            return;
        }
        is_num=atoi(input);
        if (!is_num) printf("wrong input. try again\n");
    }
    int bidding_price = is_num;
    if (bidding_price >= atoi(buy_it_now)) {
        printf("Your bidding is higher or equal than buy-it-now price.\n");
        printf("Please use buy-it-now functionality.\n");
        return;
    }

    if (bidding_price < atoi(latest_bid)) {
        printf("you are outbidded.\n");
        sprintf(history_status,"You are outbidded");
        sprintf(query, "insert into bid_history(uid,id,bid_price,status) values(%d,%s,%d,'%s');",user_id,id,bidding_price,history_status);
        sql_query(query, 0);
        return;
    }
    sprintf(history_status,"You are the highest bidder.");
    sprintf(query, "insert into bid_history(uid,id,bid_price,status) values(%d,%s,%d,'%s');",user_id,id,bidding_price,history_status);
    sql_query(query, 0);
    sprintf(status, "You are outbidded");
    sprintf(query, "update bid_history set status='%s' where id=%s and bid_price < %d;",status,id,bidding_price);
    sql_query(query, 0);

    char bids_num = status[0];
    sprintf(status, "%c bids", bids_num+1-48);
    sprintf(query, "update item set latest_bid=%d, status='%s' where id=%s;",bidding_price,status,id);
    sql_query(query, 0);
}

void search_item(int user_id) {
    int num;
    printf("----< Search item > : (Enter the number)\n");
    printf("----(1) Search items by category\n");
    printf("----(2) Search items by description keyword\n");
    printf("----(3) Search items by seller\n");
    printf("----(4) Search items by date posted\n");
    printf("----(5) Go Back\n");
    printf("----(6) Quit\n");
    num = user_input(6);
    if (num == 1) {
        search_category(user_id);
    } else if (num == 2) {
        search_description(user_id);
    } else if (num == 3) {
        search_seller(user_id);
    } else if (num == 4) {
        search_date(user_id);
    } else if (num == 5) {
        return;
    } else {
        exit(0);
    }
}

void chk_bid_status(int user_id) {
    char query[1000];
    char output[100];
    char id[10];
    char bid_price[100];
    char buy_it_now[100];
    char status[100];
    char sold_price[40];
    char sold_date[40];
    int num;

    printf("----< Check Status of your Bid >\n");
    sprintf(query, "select count(id) from bid_history where uid=%d;",user_id);
    strcpy(output,sql_query(query,0));
    output[strlen(output)-1]='\0';
    num = atoi(output);
    for (int i=0; i<num; i++) {
        sprintf(query, "select id from bid_history where uid=%d order by id asc limit %d,1;",user_id,i);
        strcpy(id,sql_query(query,0));
        id[strlen(id)-1]='\0';

        sprintf(query, "select bid_price from bid_history where uid=%d order by id asc limit %d,1;",user_id,i);
        strcpy(bid_price,sql_query(query,0));
        bid_price[strlen(bid_price)-1]='\0';

        sprintf(query, "select status from item where id=%s;",id);
        strcpy(status,sql_query(query,0));
        status[strlen(status)-1]='\0';

        printf("[Item %d]\n",i+1);
        
        sprintf(query, "select description from item where id=%s;",id);
        strcpy(output,sql_query(query,0));
        output[strlen(output)-1]='\0';
        printf("   description: %s\n",output);
        
        sprintf(query, "select status from bid_history where uid=%d and bid_price=%s and id=%s;",user_id,bid_price,id);
        strcpy(output,sql_query(query,0));   
        output[strlen(output)-1]='\0';
        printf("   status: %s\n",output);
        
        if (!strcmp(status,"sold")) {
            sprintf(query, "select sell_price from transaction where id=%s;",id);
            strcpy(sold_price,sql_query(query,0));
            sold_price[strlen(sold_price)-1]='\0';

            sprintf(query, "select transaction_date from transaction where id=%s;",id);
            strcpy(sold_date,sql_query(query,0));
            sold_date[strlen(sold_date)-1]='\0';
            printf("   sold price :%s\n",sold_price);
            printf("   sold date :%s\n",sold_date);
            continue;
        }

        printf("   your bidding price: %s\n",bid_price);

        sprintf(query, "select latest_bid from item where id=%s;",id);  
        strcpy(output,sql_query(query,0));
        output[strlen(output)-1]='\0';
        printf("   current highest bidding price: %s\n",output);
        
        sprintf(query, "select end_date from item where id=%s;",id);
        strcpy(output,sql_query(query,0)); 
        output[strlen(output)-1]='\0';
        printf("   bid ending date: %s\n",output);
    }
}

void chk_account(int user_id) {
    char query[1000];
    char output[100];
    char id[10];
    char tid[10];
    int num;
    int income=0;
    int outcome=0;
    int commission=0;

    printf("----< Check your Account >\n");
    sprintf(query, "select count(tid) from transaction where seller_id=%d;",user_id);
    strcpy(output,sql_query(query,0));
    output[strlen(output)-1]='\0';
    num = atoi(output);
    for (int i=0; i<num; i++) {
        sprintf(query, "select id from transaction where seller_id=%d order by tid asc limit %d,1;",user_id,i);
        strcpy(id,sql_query(query,0));
        id[strlen(id)-1]='\0';

        sprintf(query, "select tid from transaction where id=%s;",id);
        strcpy(tid,sql_query(query,0));
        tid[strlen(tid)-1]='\0';

        printf("[Sold Item %d]\n",i+1);
        
        sprintf(query, "select description from item where id=%s;",id);
        strcpy(output,sql_query(query,0));
        output[strlen(output)-1]='\0';
        printf("   description: %s\n",output);
        
        sprintf(query, "select sell_price from transaction where tid=%s;",tid);
        strcpy(output,sql_query(query,0));
        output[strlen(output)-1]='\0';
        printf("   sold price: %s\n",output);
        income += atoi(output);
        if (i < 2) {
            commission -= atoi(output)*0.02;
        } else {
            commission -= atoi(output)*0.01;
        }
    }

    sprintf(query, "select count(tid) from transaction where buyer_id=%d;",user_id);
    strcpy(output,sql_query(query,0));
    output[strlen(output)-1]='\0';
    num = atoi(output);
    for (int i=0; i<num; i++) {
        sprintf(query, "select id from transaction where buyer_id=%d order by tid asc limit %d,1;",user_id,i);
        strcpy(id,sql_query(query,0));
        id[strlen(id)-1]='\0';

        sprintf(query, "select tid from transaction where id=%s;",id);
        strcpy(tid,sql_query(query,0));
        tid[strlen(tid)-1]='\0';

        printf("\n[Purchased Item %d]\n",i+1);
        
        sprintf(query, "select description from item where id=%s;",id);
        strcpy(output,sql_query(query,0));
        output[strlen(output)-1]='\0';
        printf("   description: %s\n",output);
        
        sprintf(query, "select sell_price from transaction where tid=%s;",tid);
        strcpy(output,sql_query(query,0));
        output[strlen(output)-1]='\0';
        printf("   purchased price: %s\n",output);
        outcome-=atoi(output);
    }
    printf("\n[Your Balance Summary]\n");
    printf("sold : %d won\n",income);
    printf("commission : %d won\n",commission);
    printf("purchased : %d won\n",outcome);
    printf("Total balance : %d won\n",income+commission+outcome);
};

void admin_user_list(int user_id) {
    char query[100];
    char output[100];
    char uid[10];
    char name[10];
    char email[10];
    char password[10];
    char level[10];
    int num;

    sprintf(query, "select count(uid) from user;");
    strcpy(output,sql_query(query,0));
    num = atoi(output);
    printf("\n#### total number of users : %d\n",num);
    for (int i = 0; i < num; i++){
        sprintf(query, "select * from user order by uid asc limit %d,1;",i);
        strcpy(output,sql_query(query,0));
        int count=0;
        char *ptr = strtok(output, "+");
        while (ptr != NULL) {
            // printf("%s\n", ptr);
            if(count==0) {
                count++;
                strcpy(uid,ptr);
            } else if (count==1) {
                count++;
                strcpy(name,ptr);
            } else if (count==2) {
                count++;
                strcpy(email,ptr);
            } else if (count==3) {
                count++;
                strcpy(password,ptr);
            } else if (count==4) {
                count++;
                strcpy(level,ptr);
            }
            ptr = strtok(NULL, "+");
        }
        
        printf("\n[user %d]\n",i+1);
        printf("uid : %s\n",uid);
        printf("name : %s\n",name);
        printf("email : %s\n",email);
        printf("password : %s\n",password);
        printf("level : %s\n",level);
    }
}

void admin_item_list(int user_id) {
    char query[100];
    char output[100];
    char id[10];
    char uid[10];
    char category[10];
    char description[10];
    char condition[10];
    char latest_bid[10];
    char buy_it_now[10];
    char status[10];
    char posted_date[10];
    char end_date[10];
    int num;

    sprintf(query, "select count(id) from item;");
    strcpy(output,sql_query(query,0));
    num = atoi(output);
    printf("\n#### total number of items : %d\n",num);
    for (int i = 0; i < num; i++){
        sprintf(query, "select * from item order by id asc limit %d,1;",i);
        strcpy(output,sql_query(query,0));
        int count=0;
        char *ptr = strtok(output, "+");
        while (ptr != NULL) {
            // printf("%s\n", ptr);
            if(count==0) {
                count++;
                strcpy(id,ptr);
            } else if (count==1) {
                count++;
                strcpy(uid,ptr);
            } else if (count==2) {
                count++;
                strcpy(category,ptr);
            } else if (count==3) {
                count++;
                strcpy(description,ptr);
            } else if (count==4) {
                count++;
                strcpy(condition,ptr);
            } else if (count==5) {
                count++;
                strcpy(latest_bid,ptr);
            } else if (count==6) {
                count++;
                strcpy(buy_it_now,ptr);
            } else if (count==7) {
                count++;
                strcpy(status,ptr);
            } else if (count==8) {
                count++;
                strcpy(posted_date,ptr);
            } else if (count==9) {
                count++;
                strcpy(end_date,ptr);
            }
            ptr = strtok(NULL, "+");
        }
        
        printf("\n[item %d]\n",i+1);
        printf("id : %s\n",id);
        printf("uid : %s\n",uid);
        printf("category : %s\n",category);
        printf("description : %s\n",description);
        printf("condition : %s\n",condition);
        printf("latest_bid : %s\n",latest_bid);
        printf("buy_it_now : %s\n",buy_it_now);
        printf("status : %s\n",status);
        printf("posted_date : %s\n",posted_date);
        printf("end_date : %s\n",end_date);
    }
}

void admin_watch_list(int user_id) {
    char query[100];
    char output[100];
    char uid[10];
    char id[10];
    char watchedAt[10];
    int num;

    sprintf(query, "select count(*) from watched;");
    strcpy(output,sql_query(query,0));
    num = atoi(output);
    printf("\n#### total number of watched : %d\n",num);
    for (int i = 0; i < num; i++){
        sprintf(query, "select * from watched order by uid,id,watchedAt asc limit %d,1;",i);
        strcpy(output,sql_query(query,0));
        int count=0;
        char *ptr = strtok(output, "+");
        while (ptr != NULL) {
            // printf("%s\n", ptr);
            if(count==0) {
                count++;
                strcpy(uid,ptr);
            } else if (count==1) {
                count++;
                strcpy(id,ptr);
            } else if (count==2) {
                count++;
                strcpy(watchedAt,ptr);
            }
            ptr = strtok(NULL, "+");
        }
        
        printf("\n[watched %d]\n",i+1);
        printf("uid : %s\n",uid);
        printf("id : %s\n",id);
        printf("watchedAt : %s\n",watchedAt);
    }
}

void admin_transaction_list(int user_id) {
    char query[100];
    char output[100];
    char tid[10];
    char id[10];
    char transaction_date[10];
    char seller_id[10];
    char buyer_id[10];
    char sell_price[10];
    int num;

    sprintf(query, "select count(tid) from transaction;");
    strcpy(output,sql_query(query,0));
    num = atoi(output);
    printf("\n#### total number of transactions : %d\n",num);
    for (int i = 0; i < num; i++){
        sprintf(query, "select * from transaction order by tid asc limit %d,1;",i);
        strcpy(output,sql_query(query,0));
        int count=0;
        char *ptr = strtok(output, "+");
        while (ptr != NULL) {
            // printf("%s\n", ptr);
            if(count==0) {
                count++;
                strcpy(tid,ptr);
            } else if(count==1) {
                count++;
                strcpy(id,ptr);
            } else if (count==2) {
                count++;
                strcpy(transaction_date,ptr);
            } else if (count==3) {
                count++;
                strcpy(seller_id,ptr);
            } else if (count==4) {
                count++;
                strcpy(buyer_id,ptr);
            } else if (count==5) {
                count++;
                strcpy(sell_price,ptr);
            }
            ptr = strtok(NULL, "+");
        }
        
        printf("\n[transaction %d]\n",i+1);
        printf("tid: %s\n",tid);
        printf("id : %s\n",id);
        printf("transaction_date : %s\n",transaction_date);
        printf("seller_id : %s\n",seller_id);
        printf("buyer_id : %s\n",buyer_id);
        printf("sell_price : %s\n",sell_price);
    }
}

void admin_bid_list(int user_id) {
    char query[100];
    char output[100];
    char uid[10];
    char id[10];
    char bid_price[10];
    char status[10];
    int num;

    sprintf(query, "select count(*) from bid_history;");
    strcpy(output,sql_query(query,0));
    num = atoi(output);
    printf("\ntotal number of bid_history : %d\n",num);
    for (int i = 0; i < num; i++){
        sprintf(query, "select * from bid_history order by id, bid_price asc limit %d,1;",i);
        strcpy(output,sql_query(query,0));
        int count=0;
        char *ptr = strtok(output, "+");
        while (ptr != NULL) {
            // printf("%s\n", ptr);
            if(count==0) {
                count++;
                strcpy(uid,ptr);
            } else if (count==1) {
                count++;
                strcpy(id,ptr);
            } else if (count==2) {
                count++;
                strcpy(bid_price,ptr);
            } else if (count==3) {
                count++;
                strcpy(status,ptr);
            }
            ptr = strtok(NULL, "+");
        }
        
        printf("\n[bid %d]\n",i+1);
        printf("uid : %s\n",uid);
        printf("id : %s\n",id);
        printf("bid_price : %s\n",bid_price);
        printf("status : %s\n",status);
    }
}

void main_menu(int user_id) {
    if (user_id != 1) {
        while (1) {
            int num;
            printf("\n----< Main menu >\n");
            printf("----(1) Sell item\n");
            printf("----(2) Status of Your Item Listed on Auction\n");
            printf("----(3) Search item\n");
            printf("----(4) Check Status of your Bid\n");
            printf("----(5) Check your Account\n");
            printf("----(6) Quit\n");
            num = user_input(6);
            
            if (num == 1) {
                sell_item(user_id);
            } else if (num == 2) {
                chk_auction_status(user_id);
            } else if (num == 3) {
                search_item(user_id);
            } else if (num == 4) {
                chk_bid_status(user_id);
            } else if (num == 5) {
                chk_account(user_id);
            } else if (num == 6) {
                exit(0);
            } else {
                puts("wrong input");
            }
        }
    } else {
        while (1) {
            int num;
            printf("\n----< ADMIN Main menu >\n");
            printf("----(1) View User Statics\n");
            printf("----(2) View Item Statics\n");
            printf("----(3) View Watch Statics\n");
            printf("----(4) View Transaction Statics\n");
            printf("----(5) View Bid Statics\n");
            printf("----(6) Quit\n");
            num = user_input(6);
            
            if (num == 1) {
                admin_user_list(user_id);
            } else if (num == 2) {
                admin_item_list(user_id);
            } else if (num == 3) {
                admin_watch_list(user_id);
            } else if (num == 4) {
                admin_transaction_list(user_id);
            } else if (num == 5) {
                admin_bid_list(user_id);
            } else if (num == 6) {
                exit(0);
            } else {
                puts("wrong input");
            }
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
    
    while (1) {
        int chosen1 = login_menu();
        if (chosen1 == 1) {
            int user_id=login(); // admin if user_id == 1, login failed if user_id == 0
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