CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11 -D_GNU_SOURCE -O2

.PHONY: all clean rebuild test

all: myfork

myfork: myfork.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f myfork

rebuild: clean all

test: all
	@echo "===================== обычный запуск ====================="
	./myfork
	@echo
	@echo "===================== SIGINT потомку ====================="
	@( ./myfork & pid=$$!; \
	   sleep 1; \
	   child=$$(pgrep -P $$pid); \
	   if [ -n "$$child" ]; then kill -INT $$child; fi; \
	   wait $$pid )
	@echo
	@echo "===================== SIGTERM потомку ====================="
	@( ./myfork & pid=$$!; \
	   sleep 1; \
	   child=$$(pgrep -P $$pid); \
	   if [ -n "$$child" ]; then kill -TERM $$child; fi; \
	   wait $$pid )
