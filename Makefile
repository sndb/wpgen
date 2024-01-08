PROG = wpgen
SRCS = wpgen.c maze.c
OBJS = ${SRCS:.c=.o}

PREFIX = /usr/local

all: ${PROG}

.c.o:
	${CC} ${CFLAGS} ${CPPFLAGS} -c $<

${PROG}: ${OBJS}
	${CC} -o $@ ${OBJS} ${LDFLAGS}

install: all
	mkdir -p ${DESTDIR}${PREFIX}/bin
	install -m 755 ${PROG} ${DESTDIR}${PREFIX}/bin/${PROG}

uninstall:
	rm -f ${DESTDIR}${PREFIX}/bin/${PROG}

clean:
	rm -f ${PROG} ${OBJS}

.PHONY: all install uninstall clean
