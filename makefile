MODULE_PATH=modules
TEST_PATH=test

simulator: simulator.c trace.o
	gcc -Wall -o simulator simulator.c trace.o -lm
	rm trace.o

trace.o: $(MODULE_PATH)/trace.c
	gcc -Wall -o trace.o -c $(MODULE_PATH)/trace.c

test_program_1: $(TEST_PATH)/program_1.c trace.o
	gcc -Wall -o program_1 $(TEST_PATH)/program_1.c trace.o
	rm trace.o

test_program_2: $(TEST_PATH)/program_2.c trace.o
	gcc -Wall -o program_2 $(TEST_PATH)/program_2.c trace.o
	rm trace.o

test_program_3: $(TEST_PATH)/program_3.c trace.o
	gcc -Wall -o program_3 $(TEST_PATH)/program_3.c trace.o
	rm trace.o