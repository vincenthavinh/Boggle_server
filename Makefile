FLAGS = -g -Wall -pthread -lpthread

C_FILES = $(wildcard c_files/*.c)
H_FILES = $(wildcard headers/*.h)

server:	$(C_FILES) $(H_FILES)
	gcc $(FLAGS) $(C_FILES) -o server

clean:
	rm -f server