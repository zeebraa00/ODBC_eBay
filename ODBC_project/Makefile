# all:
# 	javac JDBCexample.java 
# 	gcc mysql.c -I /usr/include/mysql -lmysqlclient

# run_jdbc: 
# 	java -cp .:/usr/share/java/mysql-connector-java.jar JDBCexample "Comp. Sci."
# #	java -cp .:/usr/share/java/mysql-connector-java.jar JDBCexample "Comp. Sci.' or '1'='1"

CC=gcc
CCFLAGS= -I /usr/include/mysql -lmysqlclient

#add Csource files here
SRCS=mysql.c

TARGET=out.out

OBJS := $(patsubst %.c,%.o,$(SRCS))

all: $(TARGET)

%.o:%.c
	$(CC) $(CCFLAGS) $< -c -o $@

$(TARGET): $(OBJS)
	$(CC) $(CCFLAGS) $^ -o $@
                  
.PHONY=clean
.PHONY=run
run: $(TARGET)
	./$(TARGET)
clean:
	rm -f $(OBJS) $(TARGET)