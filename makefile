LINK_TARGET = client

OBJS = \
	common.o\
	client.o
	
REBUILDABLES = $(OBJS) $(LINK_TARGET)

all : $(LINK_TARGET)

clean: 
	rm -f $(REBUILDABLES)

$(LINK_TARGET) : $(OBJS)
	cc -g -o $@ $^ 

%.o : %.c
	cc -g  -Wall -o $@ -c $<

common.o : common.h
server.o : client.c common.h

 
