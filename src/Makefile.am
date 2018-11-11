INCLUDES = \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" 


bin_PROGRAMS=gSTCISP

gSTCISP_SOURCES=main.c main.h uart.c uart.h protocol.c protocol.h

gSTCISP_CFLAGS  = @GTK_CFLAGS@

gSTCISP_LDADD = @GTK_LIBS@ -lvte
