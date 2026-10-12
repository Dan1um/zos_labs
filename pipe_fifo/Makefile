CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11 -D_GNU_SOURCE -O2

TARGETS = mypipe fifo_writer fifo_reader

.PHONY: all clean rebuild test test-pipe test-fifo

all: $(TARGETS)

mypipe: mypipe.c
	$(CC) $(CFLAGS) -o $@ $<

fifo_writer: fifo_writer.c fifo_common.h
	$(CC) $(CFLAGS) -o $@ $<

fifo_reader: fifo_reader.c fifo_common.h
	$(CC) $(CFLAGS) -o $@ $<

# Заодно удаляем файл FIFO, если он остался от прерванного запуска
clean:
	rm -f $(TARGETS) /tmp/lab6_fifo

rebuild: clean all

test: test-pipe test-fifo

# Часть 1: pipe + fork. Занимает около 5 секунд.
test-pipe: mypipe
	@echo "===================== pipe (fork) ====================="
	./mypipe
	@echo

# Часть 2: FIFO + два отдельных процесса. Читатель запускается в фоне,
# писатель - в основном потоке make; "wait" ждёт завершения читателя.
# Занимает около 10 секунд.
test-fifo: fifo_writer fifo_reader
	@echo "===================== FIFO (два процесса) ====================="
	@./fifo_reader & ./fifo_writer; wait
