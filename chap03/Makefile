CC = gcc
TARGETS = tcp_talkserv tcp_talkcli tcp_talkexec
all: $(TARGETS)
tcp_talkserv: tcp_talkserv.c netprog.h
    $(CC) -o tcp_talkserv tcp_talkserv.c
tcp_talkcli:
    $(CC) -o tcp_talkcli tcp_talkcli.c
tcp_talkexec:
    $(CC) -o tcp_talkexec tcp_talkexec.c
clean:
    rm -f $(TERGETS)