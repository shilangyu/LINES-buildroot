CC=$(CROSS_COMPILE)gcc
OBJS := main.o 
NAME=dioed

$(NAME): $(OBJS)
	$(CC) -o $(NAME) $(CFLAGS) $(LDFLAGS) $(OBJS) -l gpiod -Wall

$(OBJS) : %.o : %.c
	$(CC) -c $(CFLAGS) $< -o $@

format:
	clang-format -i *.c

clean:
	rm *.o $(NAME)
