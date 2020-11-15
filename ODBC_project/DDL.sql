create table user
	(uid		int AUTO_INCREMENT,
	 name		varchar(20) not null,
	 email      varchar(20) not null,
	 password   varchar(20),
     primary key (uid)
	);

create table item
	(id      		int AUTO_INCREMENT,
     uid    		int,
	 category       varchar(20),
	 description    varchar(100),
     condition      varchar(20),
     CONSTRAINT check (condition in ('new', 'like-new', 'very-good', 'good', 'acceptable')),
     latest_bid     numeric(5,0),
     buy_it_now     numeric(5,0),
     status         varchar(20),
     posted_date    datetime,
     end_date       datetime,
	 primary key (dept_name),
	 foreign key (uid) references user(uid)
	);

create table transaction
	(tid				int AUTO_INCREMENT,
	 id					int,
	 transaction_date	datetime,
	 seller_id			int,
	 buyer_id			int,
	 sell_price			numeric(5,0),
	 primary key (tid),
	 foreign key (id) references item(id),
	 foreign key (seller_id) references user(uid),
	 foreign key (buyer_id) references user(uid),
	 foreign key (sell_price) references item(latest_bid)
	)

create table bid_history
	(uid	int,
	 id		int,
	 bid_price numeric(5,0),
	 primary key (id, bid_price),
	 foreign key (uid) references user(uid),
	 foreign key (id) references item(id),
	)

create table watched
	(uid		int,
	 id			int,
	 watchedAt	datetime,
	 primary key (uid, id, watchedAt),
	 foreign key (uid) references user(uid),
	 foreign key (id) references item(id),
	)