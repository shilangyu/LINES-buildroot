CC=$(CROSS_COMPILE)gcc
OBJS := main.o 
NAME=dioed

$(NAME): $(OBJS)
	$(CC) -o $(NAME) $(CFLAGS) $(LDFLAGS) $(OBJS) -l gpiod -Wall -g

$(OBJS) : %.o : %.c
	$(CC) -c $(CFLAGS) $< -o $@ -g

format:
	clang-format -i *.c

clean:
	rm *.o $(NAME)
