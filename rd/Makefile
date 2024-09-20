#
BINTYPE = linux235_64
HOME    = ./
BINDIR  = ./
M_INC   = 
#
# DEBUG = -O2 for optimize, -g for debug:
DEBUG   = -O2
CC      = gcc
LN      = gcc
#
LINUXCPFLAGS = -I./ -DLINUX -DSVR4 -DSYSV -DTRAD_C -D__NO_STRING_INLINES -DNMR64
LINUXCFLAGS  = $(DEBUG) $(LINUXCPFLAGS)
#
CPFLAGS    = $(LINUXCPFLAGS)
CFLAGS     = $(LINUXCFLAGS)
LNMODE     =
LDFLAGS    = $(LNMODE) $(DEBUG) -lm
EXE        = -o $(BINDIR)
RM         = echo
#
#
PROGS      =  example
#
all:          $(PROGS)
	
#
example:      $(M_INC) \
              example.o readnmr.o fdatap.o cmndargs.o token.o stralloc.o memory.o   \
              fdataio.o dataio.o inquire.o testsize.o namelist.o vutil.o syscalls.o \
              getstat.o rand.o specunit.o raise.o                                   \
              conrecnx.o drawaxis.o paper.o nmrgraphics.o;
	$(LN) example.o readnmr.o fdatap.o cmndargs.o token.o stralloc.o memory.o   \
	      fdataio.o dataio.o inquire.o testsize.o namelist.o vutil.o syscalls.o \
              getstat.o rand.o specunit.o raise.o                                   \
              conrecnx.o drawaxis.o paper.o nmrgraphics.o                           \
              $(LDFLAGS) $(EXE)example
#
clean:        
	      touch o.o.o; /bin/rm -f *.o [xy].tab.[ch] y.output $(PROGS);
#
