#-------------------------- VARS -----------------------------------------

CC=g++
CFLAGS= -D_POSIX_C_SOURCE=200112L
INCLUDE=-Iinc/ -Iinc/interfaces -Iinc/misc -Iinc/models -Iinc/network

# sources subdirectories
SRCDIR=src
SRCMISC=$(SRCDIR)/misc
SRCMODELS=$(SRCDIR)/models
SRCTESTS=$(SRCDIR)/tests
SRCNETWORK=$(SRCDIR)/network

# test data subdir
TESTDATA=$(SRCTESTS)/data

# binary subdirectories
BINSUBDIR=
BINBASEDIR=bin
DEBUGSUBDIR=$(BINBASEDIR)/debug
RELEASESUBDIR=$(BINBASEDIR)/release

# selector for compilation mode
ifeq ($(MODE),debug)
	#Setting environment for debug session
	BINSUBDIR=$(DEBUGSUBDIR)
	CFLAGS=-g -Wall -Wextra -O0 -Wconversion -pedantic
else
	#Setting environment for release
	BINSUBDIR=$(RELEASESUBDIR)
	CFLAGS=-Wall
endif

# full compiler line
CCFULL=$(CC) $(CFLAGS) -I$(INCLUDE) -o $(BINSUBDIR)/$@ -c

#------------------------------GENERAL------------------------------------

all: clean-qclient clean-qserver client.app server.app ship-qclient ship-qserver clean-bin 

qclient: clean-qclient client.app ship-qclient clean-bin

qserver: clean-qserver server.app ship-qserver clean-bin

# subdirectories

subdirs:
	mkdir -p $(BINSUBDIR)

subdirs-release:
	mkdir -p $(RELEASESUBDIR)

subdirs-debug:
	mkdir -p $(DEBUGSUBDIR)


# clean ups

clean: clean-bin clean-qserver clean-qclient

clean-bin:
	rm -rvf $(BINBASEDIR)/
	
clean-qserver:
	rm -rfv ./qserver
	rm -rfv *.data
	rm -rfv qserver.1
	rm -rfv qbank.5

clean-qclient:
	rm -rfv ./qclient
	rm -rfv qclient.1

# ships the applications

ship: ship-qclient ship-qserver

ship-qserver: manual-qserver
	cp $(BINSUBDIR)/server.app ./qserver
	chmod +x qserver

ship-qclient: manual-qclient
	cp $(BINSUBDIR)/client.app ./qclient
	chmod +x qclient

# manual entries

manual-qserver:
	groff -man -Tascii ./doc/qserver.troff > qserver.1
	groff -man -Tascii ./doc/qbank.troff > qbank.5

manual-qclient:
	groff -man -Tascii ./doc/qclient.troff > qclient.1

#------------------------------TESTS--------------------------------------

# put test data in binary subdir
copy-test-data:
	cp $(TESTDATA)/* $(BINSUBDIR)

# >> test-uniformrandom <<
test-uniformrandom.test: subdirs test-uniformrandom.o UniformRandom.o
	$(CC) \
	$(BINSUBDIR)/test-uniformrandom.o \
	$(BINSUBDIR)/UniformRandom.o \
	$(INCLUDE) \
	-o $(BINSUBDIR)/test-uniformrandom.test

test-uniformrandom.o:
	$(CCFULL) $(SRCTESTS)/test-uniformrandom.cpp

# >> test-choice <<
test-choice.test: subdirs test-choice.o ChoiceCollection.o Choice.o
	$(CC) \
	$(BINSUBDIR)/test-choice.o \
	$(BINSUBDIR)/ChoiceCollection.o \
	$(BINSUBDIR)/Choice.o \
	$(INCLUDE) \
	-o $(BINSUBDIR)/test-choice.test

test-choice.o:
	$(CCFULL) $(SRCTESTS)/test-choice.cpp

# >> test-tag <<
test-tag.test: subdirs test-tag.o TagCollection.o Tag.o
	$(CC) \
	$(BINSUBDIR)/test-tag.o \
	$(BINSUBDIR)/TagCollection.o \
	$(BINSUBDIR)/Tag.o \
	$(INCLUDE) \
	-o $(BINSUBDIR)/test-tag.test

test-tag.o:
	$(CCFULL) $(SRCTESTS)/test-tag.cpp

# >> test-questiontitle <<
test-questiontitle.test: subdirs test-questiontitle.o QuestionTitle.o
	$(CC) \
	$(BINSUBDIR)/test-questiontitle.o \
	$(BINSUBDIR)/QuestionTitle.o \
	$(INCLUDE) \
	-o $(BINSUBDIR)/test-questiontitle.test

test-questiontitle.o:
	$(CCFULL) $(SRCTESTS)/test-questiontitle.cpp

# >> test-question <<
test-question.test: subdirs test-question.o Question.o TagCollection.o Tag.o QuestionTitle.o ChoiceCollection.o Choice.o
	$(CC) \
	$(BINSUBDIR)/test-question.o \
	$(BINSUBDIR)/Question.o \
	$(BINSUBDIR)/TagCollection.o \
	$(BINSUBDIR)/Tag.o \
	$(BINSUBDIR)/QuestionTitle.o \
	$(BINSUBDIR)/ChoiceCollection.o \
	$(BINSUBDIR)/Choice.o \
	$(INCLUDE) \
	-o $(BINSUBDIR)/test-question.test

test-question.o:
	$(CCFULL) $(SRCTESTS)/test-question.cpp

# >> test-solvedquestion <<
test-solvedquestion.test: subdirs test-solvedquestion.o Utils.o SolvedQuestion.o Question.o TagCollection.o Tag.o QuestionTitle.o ChoiceCollection.o Choice.o
	$(CC) \
	$(BINSUBDIR)/test-solvedquestion.o \
	$(BINSUBDIR)/Utils.o \
	$(BINSUBDIR)/SolvedQuestion.o \
	$(BINSUBDIR)/Question.o \
	$(BINSUBDIR)/TagCollection.o \
	$(BINSUBDIR)/Tag.o \
	$(BINSUBDIR)/QuestionTitle.o \
	$(BINSUBDIR)/ChoiceCollection.o \
	$(BINSUBDIR)/Choice.o \
	$(INCLUDE) \
	-o $(BINSUBDIR)/test-solvedquestion.test 

test-solvedquestion.o:
	$(CCFULL) $(SRCTESTS)/test-solvedquestion.cpp

# >> test-quizbook <<
test-quizbook.test: subdirs copy-test-data test-quizbook.o Utils.o QuizBook.o UniformRandom.o SolvedQuestion.o Question.o TagCollection.o Tag.o QuestionTitle.o ChoiceCollection.o Choice.o
	$(CC) \
	$(BINSUBDIR)/test-quizbook.o \
	$(BINSUBDIR)/Utils.o \
	$(BINSUBDIR)/QuizBook.o \
	$(BINSUBDIR)/UniformRandom.o \
	$(BINSUBDIR)/SolvedQuestion.o \
	$(BINSUBDIR)/Question.o \
	$(BINSUBDIR)/TagCollection.o \
	$(BINSUBDIR)/Tag.o \
	$(BINSUBDIR)/QuestionTitle.o \
	$(BINSUBDIR)/ChoiceCollection.o \
	$(BINSUBDIR)/Choice.o \
	$(INCLUDE) \
	-o $(BINSUBDIR)/test-quizbook.test

test-quizbook.o:
	$(CCFULL) $(SRCTESTS)/test-quizbook.cpp

# >> test-socket <<
test-socket.test: subdirs test-socket.o UniformRandom.o Socket.o Host.o
	$(CC) \
	$(BINSUBDIR)/test-socket.o \
	$(BINSUBDIR)/UniformRandom.o \
	$(BINSUBDIR)/Host.o \
	$(BINSUBDIR)/Socket.o \
	$(INCLUDE) \
	-o $(BINSUBDIR)/test-socket.test

test-socket.o:
	$(CCFULL) $(SRCTESTS)/test-socket.cpp

# >> test-quizserver <<
test-quizserver.test: subdirs test-quizserver.o Request.o QuizServer.o Socket.o Host.o QuizBook.o UniformRandom.o SolvedQuestion.o Question.o TagCollection.o Tag.o QuestionTitle.o ChoiceCollection.o Choice.o Utils.o
	$(CC) \
	$(BINSUBDIR)/test-quizserver.o \
	$(BINSUBDIR)/QuizServer.o \
	$(BINSUBDIR)/Request.o \
	$(BINSUBDIR)/Host.o \
	$(BINSUBDIR)/Socket.o \
	$(BINSUBDIR)/QuizBook.o \
	$(BINSUBDIR)/UniformRandom.o \
	$(BINSUBDIR)/SolvedQuestion.o \
	$(BINSUBDIR)/Question.o \
	$(BINSUBDIR)/TagCollection.o \
	$(BINSUBDIR)/Tag.o \
	$(BINSUBDIR)/QuestionTitle.o \
	$(BINSUBDIR)/ChoiceCollection.o \
	$(BINSUBDIR)/Choice.o \
	$(BINSUBDIR)/Utils.o \
	$(INCLUDE) \
	-o $(BINSUBDIR)/test-quizserver.test

test-quizserver.o:
	$(CCFULL) $(SRCTESTS)/test-quizserver.cpp

# >> test-quizclient <<
test-quizclient.test: subdirs test-quizclient.o Request.o QuizClient.o Socket.o Host.o SolvedQuestion.o Question.o TagCollection.o Tag.o QuestionTitle.o ChoiceCollection.o Choice.o Utils.o
	$(CC) \
	$(BINSUBDIR)/test-quizclient.o \
	$(BINSUBDIR)/QuizClient.o \
	$(BINSUBDIR)/Request.o \
	$(BINSUBDIR)/Host.o \
	$(BINSUBDIR)/Socket.o \
	$(BINSUBDIR)/SolvedQuestion.o \
	$(BINSUBDIR)/Question.o \
	$(BINSUBDIR)/TagCollection.o \
	$(BINSUBDIR)/Tag.o \
	$(BINSUBDIR)/QuestionTitle.o \
	$(BINSUBDIR)/ChoiceCollection.o \
	$(BINSUBDIR)/Choice.o \
	$(BINSUBDIR)/Utils.o \
	$(INCLUDE) \
	-o $(BINSUBDIR)/test-quizclient.test

test-quizclient.o:
	$(CCFULL) $(SRCTESTS)/test-quizclient.cpp

# >> test-persistent.test <<
test-persistent.test: subdirs test-persistent.o Request.o QuizServer.o QuizBook.o QuizClient.o UniformRandom.o Socket.o Host.o SolvedQuestion.o Question.o TagCollection.o Tag.o QuestionTitle.o ChoiceCollection.o Choice.o Utils.o
	$(CC) \
	$(BINSUBDIR)/test-persistent.o \
	$(BINSUBDIR)/QuizServer.o \
	$(BINSUBDIR)/QuizBook.o \
	$(BINSUBDIR)/QuizClient.o \
	$(BINSUBDIR)/Request.o \
	$(BINSUBDIR)/Host.o \
	$(BINSUBDIR)/Socket.o \
	$(BINSUBDIR)/SolvedQuestion.o \
	$(BINSUBDIR)/Question.o \
	$(BINSUBDIR)/TagCollection.o \
	$(BINSUBDIR)/Tag.o \
	$(BINSUBDIR)/QuestionTitle.o \
	$(BINSUBDIR)/ChoiceCollection.o \
	$(BINSUBDIR)/Choice.o \
	$(BINSUBDIR)/UniformRandom.o \
	$(BINSUBDIR)/Utils.o \
	$(INCLUDE) \
	-o $(BINSUBDIR)/test-persistent.test

test-persistent.o:
	$(CCFULL) $(SRCTESTS)/test-persistent.cpp

# >> test-all.test <<
test-all: clean-bin subdirs copy-test-data test-choice.test test-tag.test test-questiontitle.test test-question.test test-solvedquestion.test test-quizbook.test test-socket.test
	$(BINSUBDIR)/test-choice.test
	$(BINSUBDIR)/test-tag.test
	$(BINSUBDIR)/test-questiontitle.test
	$(BINSUBDIR)/test-question.test
	$(BINSUBDIR)/test-solvedquestion.test
	$(BINSUBDIR)/test-quizbook.test

# >> test-thread.test <<
test-thread.test : subdirs test-thread.o
	$(CC) \
	$(BINSUBDIR)/test-thread.o \
	$(INCLUDE) \
	-pthread \
	-o $(BINSUBDIR)/test-thread.test

test-thread.o : 
	$(CCFULL) $(SRCTESTS)/test-thread.cpp

# >> test-getopt.test <<
test-getopt.test : subdirs test-getopt.o
	$(CC) \
	$(BINSUBDIR)/test-getopt.o \
	$(INCLUDE) \
	-o $(BINSUBDIR)/test-getopt.test

test-getopt.o : 
	$(CCFULL) $(SRCTESTS)/test-getopt.cpp

#------------------------------MODELS-------------------------------------

Choice.o:
	$(CCFULL) $(SRCMODELS)/Choice.cpp

ChoiceCollection.o:
	$(CCFULL) $(SRCMODELS)/ChoiceCollection.cpp

Tag.o:
	$(CCFULL) $(SRCMODELS)/Tag.cpp

TagCollection.o:
	$(CCFULL) $(SRCMODELS)/TagCollection.cpp

QuestionTitle.o:
	$(CCFULL) $(SRCMODELS)/QuestionTitle.cpp

Question.o:
	$(CCFULL) $(SRCMODELS)/Question.cpp

SolvedQuestion.o:
	$(CCFULL) $(SRCMODELS)/SolvedQuestion.cpp

QuizBook.o:
	$(CCFULL) $(SRCMODELS)/QuizBook.cpp

#------------------------------NETWORK------------------------------------
Socket.o:
	$(CCFULL) $(SRCNETWORK)/Socket.cpp

Host.o:
	$(CCFULL) $(SRCNETWORK)/Host.cpp

QuizServer.o:
	$(CCFULL) $(SRCNETWORK)/QuizServer.cpp

QuizClient.o:
	$(CCFULL) $(SRCNETWORK)/QuizClient.cpp

Request.o:
	$(CCFULL) $(SRCNETWORK)/Request.cpp


#------------------------------EXTRA--------------------------------------

Utils.o:
	$(CCFULL) $(SRCMISC)/Utils.cpp

UniformRandom.o:
	$(CCFULL) $(SRCMISC)/UniformRandom.cpp

#------------------------------SERVER-------------------------------------

# server.app
server.app: subdirs server.o QuizServer.o Request.o Socket.o Host.o QuizBook.o UniformRandom.o SolvedQuestion.o Question.o TagCollection.o Tag.o QuestionTitle.o ChoiceCollection.o Choice.o Utils.o
	$(CC) \
	$(BINSUBDIR)/QuizServer.o \
	$(BINSUBDIR)/Request.o \
	$(BINSUBDIR)/Socket.o \
	$(BINSUBDIR)/Host.o \
	$(BINSUBDIR)/QuizBook.o \
	$(BINSUBDIR)/UniformRandom.o \
	$(BINSUBDIR)/SolvedQuestion.o \
	$(BINSUBDIR)/Question.o \
	$(BINSUBDIR)/TagCollection.o \
	$(BINSUBDIR)/Tag.o \
	$(BINSUBDIR)/QuestionTitle.o \
	$(BINSUBDIR)/ChoiceCollection.o \
	$(BINSUBDIR)/Choice.o \
	$(BINSUBDIR)/Utils.o \
	$(BINSUBDIR)/server.o \
	$(INCLUDE) \
	-o $(BINSUBDIR)/server.app

server.o:
	$(CCFULL) $(SRCDIR)/server.cpp

#------------------------------CLIENT-------------------------------------

# client.app
client.app: subdirs client.o QuizClient.o Request.o Socket.o Host.o UniformRandom.o SolvedQuestion.o Question.o TagCollection.o Tag.o QuestionTitle.o ChoiceCollection.o Choice.o Utils.o
	$(CC) \
	$(BINSUBDIR)/QuizClient.o \
	$(BINSUBDIR)/Request.o \
	$(BINSUBDIR)/Socket.o \
	$(BINSUBDIR)/Host.o \
	$(BINSUBDIR)/UniformRandom.o \
	$(BINSUBDIR)/SolvedQuestion.o \
	$(BINSUBDIR)/Question.o \
	$(BINSUBDIR)/TagCollection.o \
	$(BINSUBDIR)/Tag.o \
	$(BINSUBDIR)/QuestionTitle.o \
	$(BINSUBDIR)/ChoiceCollection.o \
	$(BINSUBDIR)/Choice.o \
	$(BINSUBDIR)/Utils.o \
	$(BINSUBDIR)/client.o \
	$(INCLUDE) \
	-o $(BINSUBDIR)/client.app

client.o:
	$(CCFULL) $(SRCDIR)/client.cpp