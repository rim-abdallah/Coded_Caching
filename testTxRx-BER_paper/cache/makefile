# Make file for cache directory

HDIR = /usr/local/include/caching
LOCAL_HDIR = ../Shared\ Library/include/caching
LIBDIR = ../Shared\ Library/lib
CACHEDIR = UserCache  # This creates UserCache in the cache directory
EFLAGS = -I$(HDIR) -I$(LOCAL_HDIR)
LDFLAGS = -L$(LIBDIR) -Wl,-rpath,$(LIBDIR)

CC = g++
CFLAGS = -O3 -Wall -std=c++11 -g
LIB = -lcaching -lcryptopp -lgsl -lgslcblas -lm


MakeCache :
	mkdir -p $(CACHEDIR)
	$(CC) $(CFLAGS) $(EFLAGS) -o $@ make_cache.cpp $(LDFLAGS) $(LIB)
	#clear
	./MakeCache

clean :
	rm -rf $(CACHEDIR)
	rm -f MakeCache
