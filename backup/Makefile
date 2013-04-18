#

##### Environment configurable options #####

SHELL       = /bin/sh
ARCH        = solaris
MPI_HOME    = /export/home/yyin/mpich2-install/
CC          = /usr/local/bin/mpicc
CLINKER     = $(CC)
CCC         = /export/home/yyin/mpich2-install/bin/mpiCC
CCLINKER    = $(CCC)
AR          = ar cr
RANLIB      = ranlib
LOG_LIB     = -mpilog -lm
PROF_LIB    = -lmpe -lm
OPTFLAGS    = -L/export/home/yyin/mpich2-install/lib -DREAL_RANDOM
MPE_DIR     = /export/home/yyin/mpich2-install/mpe
MAKE        = make

CFLAGS	  = $(OPTFLAGS) 
CFLAGSMPE = $(CFLAGS) -I$(MPI_HOME)/include
CCFLAGS	  = $(CFLAGS)
#FFLAGS	  = '-qdpc=e' 
FFLAGS	  = $(OPTFLAGS)
EXECS	  = mpiMatrix2

all: ${EXECS}

mpiMatrix2: mpiMatrix2.o tdarray.o
	$(CLINKER) $(OPTFLAGS) -o $@ $^

clean:
	/bin/rm -f *.o *~ PI* $(EXECS) upshot rdb.* startup.* core

.c.o:
	$(CC) $(CFLAGS) -c $<