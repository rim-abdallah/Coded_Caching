# Make file

HDIR = /usr/local/include/caching
LOCAL_HDIR = ../Shared\ Library/include/caching
LIBDIR = ../Shared\ Library/lib
EFLAGS = -I$(HDIR) -I$(LOCAL_HDIR)
LDFLAGS = -L$(LIBDIR) -Wl,-rpath,$(LIBDIR)

CC = g++
CFLAGS = -O3 -Wall -std=c++11 -g
LIB = -lcaching -lcryptopp -lgsl -lgslcblas -lm


TestTxRx :
	$(CC) $(CFLAGS) $(EFLAGS) -o $@ TestTxRx.cpp ../Shared\ Library/src/TxRx.cpp ../Shared\ Library/src/aes.cpp $(LDFLAGS) $(LIB)
	#clear
	./TestTxRx

clean :
	rm -f TestTxRx  # Use -f to avoid error if file doesn't exist
