create table user (
	uid int AUTO_INCREMENT,
	name varchar(40) not null,
	email varchar(45) not null,
	pw varchar(20),
	level int
		check (level in (0,1)),
    primary key (uid)
);

create table item (
	id int AUTO_INCREMENT,
    uid int,
	category varchar(20),
	description varchar(100),
    cond varchar(15)
		check (cond in ('new', 'like-new', 'very-good', 'good', 'acceptable')),
    latest_bid numeric(15,0) not null,
    buy_it_now numeric(15,0),
    status varchar(20),
    posted_date datetime,
    end_date datetime,
	primary key (id),
	foreign key (uid) references user(uid)
		on delete cascade
);

create table transaction (
	tid	int AUTO_INCREMENT,
	id int,
	transaction_date datetime default now(),
	seller_id int,
	buyer_id int,
	sell_price numeric(15,0) not null,
	primary key (tid),
	foreign key (id) references item(id)
		on delete cascade,
	foreign key (seller_id) references user(uid)
		on delete cascade,
	foreign key (buyer_id) references user(uid)
		on delete cascade
);

create table bid_history (
	uid	int,
	id int,
	bid_price numeric(15,0),
	status varchar(100),
	primary key (id, bid_price),
	foreign key (uid) references user(uid)
		on delete cascade,
	foreign key (id) references item(id)
		on delete cascade
);

create table watched (
	uid	int,
	id int,
	watchedAt datetime default now(),
	primary key (uid, id, watchedAt),
 	foreign key (uid) references user(uid)
	 	on delete cascade,
	foreign key (id) references item(id)
		on delete cascade
);

insert into user(name,email,pw,level) values('admin','admin','admin1234',1);

insert into user(name,email,pw,level) values('1 2', '3', '4', 0);
insert into user(name,email,pw,level) values('2 3', '4', '5', 0);

insert into item(uid,category,description,cond,latest_bid,buy_it_now,status,posted_date,end_date) values(2,'Electronics','1-1','new',0,1000,'0 bids','2020-12-04 06:18:00','2020-12-12 00:00:00');
insert into item(uid,category,description,cond,latest_bid,buy_it_now,status,posted_date,end_date) values(2,'Books','1-2','like-new',0,2000,'0 bids','2020-12-04 06:18:00','2020-12-13 00:00:00');
insert into item(uid,category,description,cond,latest_bid,buy_it_now,status,posted_date,end_date) values(2,'Home','1-3','very-good',0,3000,'0 bids','2020-12-04 06:18:00','2020-12-14 00:00:00');
insert into item(uid,category,description,cond,latest_bid,buy_it_now,status,posted_date,end_date) values(2,'Clothing','1-4','good',0,4000,'0 bids','2020-12-04 06:19:00','2020-12-15 00:00:00');
insert into item(uid,category,description,cond,latest_bid,buy_it_now,status,posted_date,end_date) values(2,'Sporting','1-5','acceptable',0,5000,'0 bids','2020-12-04 06:19:00','2020-12-16 00:00:00');

insert into item(uid,category,description,cond,latest_bid,buy_it_now,status,posted_date,end_date) values(3,'Electronics','2-1','new',0,1000,'0 bids','2020-12-04 06:19:00','2020-12-12 00:00:00');
insert into item(uid,category,description,cond,latest_bid,buy_it_now,status,posted_date,end_date) values(3,'Books','2-2','like-new',0,2000,'0 bids','2020-12-04 06:20:00','2020-12-13 00:00:00');
insert into item(uid,category,description,cond,latest_bid,buy_it_now,status,posted_date,end_date) values(3,'Home','2-3','very-good',0,3000,'0 bids','2020-12-04 06:20:00','2020-12-14 00:00:00');
insert into item(uid,category,description,cond,latest_bid,buy_it_now,status,posted_date,end_date) values(3,'Clothing','2-4','good',0,4000,'0 bids','2020-12-04 06:20:00','2020-12-15 00:00:00');
insert into item(uid,category,description,cond,latest_bid,buy_it_now,status,posted_date,end_date) values(3,'Sporting','2-5','acceptable',0,5000,'0 bids','2020-12-04 06:20:00','2020-12-16 00:00:00');