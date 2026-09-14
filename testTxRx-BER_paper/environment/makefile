HDIR = /usr/local/include/caching
LOCAL_HDIR = ../Shared\ Library/include/caching
LIBDIR = ../Shared\ Library/lib
EFLAGS = -I$(HDIR) -I$(LOCAL_HDIR)
LDFLAGS = -L$(LIBDIR) -Wl,-rpath,$(LIBDIR)  # Also set runtime path

CC = g++
CFLAGS = -O3 -Wall -std=c++11 -g
LIB = -lcaching -lcryptopp -lgsl -lgslcblas -lm


Environment :
	$(CC) $(CFLAGS) $(EFLAGS) -o $@ environment.cpp $(LDFLAGS) $(LIB)
	#clear
	./Environment

clean :
	rm -f Environment environment_file
