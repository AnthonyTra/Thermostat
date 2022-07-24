# CSCI 2021 Project 2 Makefile

AN = p2
CLASS = 2021

# -Wno-comment: disable warnings for multi-line comments, present in some tests
CFLAGS = -Wall -Wno-comment -Werror -g 
CC     = gcc $(CFLAGS)
SHELL  = /bin/bash
CWD    = $(shell pwd | sed 's/.*\///g')

PROGRAMS = \
	thermo_main \
	test_thermo_update \
	puzzlebox   \


all : $(PROGRAMS)

clean :
	rm -f $(PROGRAMS) *.o

help :
	@echo 'Typical usage is:'
	@echo '  > make                          # build all programs'
	@echo '  > make clean                    # remove all compiled items'
	@echo '  > make zip                      # create a zip file for submission'
	@echo '  > make prob1                    # built targets associated with problem 1'
	@echo '  > make prob1 testnum=5          # run problem 1 test #5 only'
	@echo '  > make test-prob2               # run test for problem 2'
	@echo '  > make test                     # run all tests'


############################################################
# 'make zip' to create p2-code.zip for submission
zip : clean clean-tests
	rm -f $(AN)-code.zip
	cd .. && zip "$(CWD)/$(AN)-code.zip" -r "$(CWD)"
	@echo Zip created in $(AN)-code.zip
	@if (( $$(stat -c '%s' $(AN)-code.zip) > 10*(2**20) )); then echo "WARNING: $(AN)-code.zip seems REALLY big, check there are no abnormally large test files"; du -h $(AN)-code.zip; fi
	@if (( $$(unzip -t $(AN)-code.zip | wc -l) > 256 )); then echo "WARNING: $(AN)-code.zip has 256 or more files in it which may cause submission problems"; fi


################################################################################
# thermometer problem
prob1 : thermo_main test_thermo_update

thermo_main : thermo_main.o thermo_update.o thermo_sim.o
	$(CC) -o $@ $^

thermo_main.o : thermo_main.c thermo.h
	$(CC) -c $<

thermo_sim.o : thermo_sim.c thermo.h
	$(CC) -c $<

thermo_update.o : thermo_update.c thermo.h
	$(CC) -c $<

test_thermo_update : test_thermo_update.c thermo_sim.o thermo_update.o
	$(CC) -o $@ $^

################################################################################
# debugging problem
prob2 : puzzlebox

puzzlebox.o : puzzlebox.c
	$(CC) -c $<

puzzlebox : puzzlebox.o
	$(CC) -o $@ $^

################################################################################
# Testing Targets
test-setup :
	@chmod u+rx testy

test: test-prob1 test-prob2

test-prob1: test-setup prob1
	./testy test_thermo_update.org $(testnum)

test-prob2 : puzzlebox
	./puzzlebox input.txt

clean-tests : 
	rm -rf test-results/ 


