build:
	gcc main.cpp -o connect4 -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
run:
	./connect4
all:
	make build && make run