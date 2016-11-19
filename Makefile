
CC = gcc
CFLAGS = -Wall
RM = rm -f

notification_receiver: notification_receiver.c
	$(CC) $(CFLAGS) $@.c -o $@

clean:
	$(RM) notification_receiver
	
