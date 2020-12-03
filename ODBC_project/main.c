#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql.h>

char * ddl1 = "create table user (uid int AUTO_INCREMENT, name varchar(20) not null, email varchar(20) not null, pw varchar(20), level int check (level in (0,1)), primary key (uid) );";
char * ddl2 = "create table item (id int AUTO_INCREMENT, uid int, category varchar(20), description varchar(100), cond varchar(15) check (cond in ('new', 'like-new', 'very-good', 'good', 'acceptable')), latest_bid numeric(15,0) not null, buy_it_now numeric(15,0), status varchar(20), posted_date datetime, end_date datetime, primary key (id), foreign key (uid) references user(uid) on delete cascade);";
char * ddl3 = "create table transaction (tid int AUTO_INCREMENT, id int, transaction_date datetime, seller_id int, buyer_id int, sell_price numeric(15,0) not null, primary key (tid), foreign key (id) references item(id) on delete cascade, foreign key (seller_id) references user(uid) on delete cascade, foreign key (buyer_id) references user(uid) on delete cascade );";
char * ddl4 = "create table bid_history (uid int, id int, bid_price numeric(15,0), status varchar(50), primary key (id, bid_price), foreign key (uid) references user(uid) on delete cascade, foreign key (id) references item(id) on delete cascade );";
char * ddl5 = "create table watched ( uid int, id int, watchedAt datetime, primary key (uid, id, watchedAt), foreign key (uid) references user(uid) on delete cascade, foreign key (id) references item(id) on delete cascade );";
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
    printf("Query : %s\n",query);
    output=sql_query(query, 1);
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
    output=sql_query(query, 0);
    printf("query output : %s\n",output);
    // puts("sign up success");
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
    output=sql_query(query, 1);
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
    char *output;
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
    output=sql_query(query, 0);
    output[strlen(output)-1]='\0';
    output[strlen(output)-1]='0';
    output[strlen(output)-2]='0';

    sprintf(query, "insert into item(uid, category, description, cond, latest_bid, buy_it_now, status, posted_date, end_date) values(%d,'%s','%s','%s',0,%d,'0 bids','%s','%s');",user_id,category,description,condition,is_num,output,bid_ending);
    // printf("Query : %s\n",query);
    sql_query(query, 0);
}

void chk_auction_status(int user_id) {
    char query[1000];
    char *output;
    char *id;
    int num;
    printf("----< Status of Your Item Listed on Auction >\n");
    sprintf(query, "select count(id) from item where uid=%d;",user_id);
    output=sql_query(query, 0);
    output[strlen(output)-1]='\0';
    num = atoi(output);
    for (int i=0; i<num; i++) {
        sprintf(query, "select id from item where uid=%d order by id asc limit %d,1;",user_id,i);
        id=sql_query(query, 0);    
        id[strlen(id)-1]='\0';

        printf("[Item %d]\n",i+1);
        
        sprintf(query, "select description from item where uid=%d order by id asc limit %d,1;",user_id,i);
        output=sql_query(query, 0);    
        output[strlen(output)-1]='\0';
        printf("   description: %s\n",output);
        
        sprintf(query, "select status from item where uid=%d order by id asc limit %d,1;",user_id,i);
        output=sql_query(query, 0);    
        output[strlen(output)-1]='\0';
        printf("   status: %s\n",output);
        
        sprintf(query, "select latest_bid from item where uid=%d order by id asc limit %d,1;",user_id,i);
        output=sql_query(query, 0);    
        output[strlen(output)-1]='\0';
        printf("   current bidding price: %s\n",output);
        
        sprintf(query, "select name from user where uid=(select uid from bid_history where id = %s order by bid_price desc limit 0,1);",id);
        output=sql_query(query, 0);    
        output[strlen(output)-1]='\0';
        printf("   current highest bidder: %s\n",output);
        
        sprintf(query, "select posted_date from item where uid=%d order by id asc limit %d,1;",user_id,i);
        output=sql_query(query, 0);    
        output[strlen(output)-1]='\0';
        printf("   date posted: %s\n",output);
        
        sprintf(query, "select end_date from item where uid=%d order by id asc limit %d,1;",user_id,i);
        output=sql_query(query, 0);    
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
        printf("%s\n",input);
        if (!strcmp(input,"buy")) {
            sprintf(status, "sold");
            sprintf(query, "update item set latest_bid=%s, status='%s' where id=%s;",buy_it_now,status,id);
            sql_query(query, 0);
            sprintf(history_status,"You won the item");
            sprintf(query, "insert into bid_history(uid,id,bid_price,status) values(%d,%s,%s,'%s');",user_id,id,buy_it_now,history_status);
            sql_query(query, 1);
            return;
        }
        is_num=atoi(input);
        printf("sex\n");
        printf("%d\n",is_num);
        if (!is_num) printf("wrong input. try again\n");
    }
    printf("i love you\n");
    int bidding_price = is_num;
    printf("i love you\n");
    printf("%s\n\n\n",buy_it_now);
    printf("%d\n\n\n",atoi(buy_it_now));
    if (bidding_price >= atoi(buy_it_now)) {
        printf("sex1\n\n\n");
        printf("Your bidding is higher or equal than buy-it-now price.\n");
        printf("Please use buy-it-now functionality.\n");
        return;
    }
    printf("sex2\n\n\n");
    if (bidding_price < atoi(latest_bid)) {
        printf("you are outbidded.\n");
        sprintf(history_status,"You are outbidded");
        sprintf(query, "insert into bid_history(uid,id,bid_price,status) values(%d,%s,%d,'%s');",user_id,id,bidding_price,history_status);
        printf("%s\n",query);
        sql_query(query, 1);
        return;
    }
    sprintf(history_status,"You are the highest bidder.");
    sprintf(query, "insert into bid_history(uid,id,bid_price,status) values(%d,%s,%d,'%s');",user_id,id,bidding_price,history_status);
    sql_query(query, 1);

    char bids_num = status[0]+1;
    sprintf(status, "%c bids", bids_num);
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

    char bids_num = status[0]+1;
    sprintf(status, "%c bids", bids_num);
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

    char bids_num = status[0]+1;
    sprintf(status, "%c bids", bids_num);
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

    char bids_num = status[0]+1;
    sprintf(status, "%c bids", bids_num);
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
        
        sprintf(query, "select description from item where id=%s order by id asc limit %d,1;",id,i);
        strcpy(output,sql_query(query,0));
        output[strlen(output)-1]='\0';
        printf("   description: %s\n",output);
        
        sprintf(query, "select status from bid_history where uid=%d and bid_price=%s and id=%s;",user_id,bid_price,id);
        strcpy(output,sql_query(query,0));   
        output[strlen(output)-1]='\0';
        printf("   status: %s\n",output);
        
        if (!strcmp(status,"sold")) {
            printf("  [fix it] sold item menu\n");
            // 트랜잭션 완성하고 해야함
            continue;
        }

        printf("   your bidding price: %s\n",bid_price);

        sprintf(query, "select latest_bid from item where uid=%d order by id asc limit %d,1;",user_id,i);  
        strcpy(output,sql_query(query,0));
        output[strlen(output)-1]='\0';
        printf("   current bidding price: %s\n",output);
        
        sprintf(query, "select end_date from item where uid=%d order by id asc limit %d,1;",user_id,i);
        strcpy(output,sql_query(query,0)); 
        output[strlen(output)-1]='\0';
        printf("   bid ending date: %s\n",output);
    }
}

void main_menu(int user_id) {
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
            // chk_account();
            continue;
        } else if (num == 6) {
            exit(0);
        } else {
            puts("wrong input");
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