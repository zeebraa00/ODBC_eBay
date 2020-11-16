all:
	javac JDBCexample.java 
	gcc mysql.c -I /usr/include/mysql -lmysqlclient 

run_jdbc: 
	java -cp .:/usr/share/java/mysql-connector-java.jar JDBCexample "Comp. Sci."
#	java -cp .:/usr/share/java/mysql-connector-java.jar JDBCexample "Comp. Sci.' or '1'='1"

