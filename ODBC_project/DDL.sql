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
	primary key (id, bid_price),
	foreign key (uid) references user(uid)
		on delete cascade,
	foreign key (id) references item(id)
		on delete cascade
);

create table watched (
	uid	int,
	id int,
	watchedAt datetime,
	primary key (uid, id, watchedAt),
 	foreign key (uid) references user(uid)
	 	on delete cascade,
	foreign key (id) references item(id)
		on delete cascade
);

insert into user(name,email,pw,level) values('admin','admin','admin1234',1);