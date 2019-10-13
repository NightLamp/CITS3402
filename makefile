IDIR=./include
ODIR=./obj
LDIR=./lib
SDIR=./src
BDIR=./bin


CC=gcc 
CFLAGS=-Wall -Werror -pedantic -I$(IDIR) -Wno-incompatible-pointer-types 

ARGS = -f data/examples/4.in

SRCS=  $(wildcard $(SDIR)/*.c) 
OBJS=$(patsubst $(SDIR)/%.c,$(ODIR)/%.o,$(SRCS))
PROJECT=bin/project

DEBUG = no
PROFILE = no
PEDANTIC = yes
OPT = no
OPTIMIZATION = -O3

ifeq ($(DEBUG), yes)
	CFLAGS += -g
	OPTIMIZATION = -O0
endif

ifeq ($(PROFILE), yes)
	CFLAGS += -pg
endif

ifeq ($(PEDANTIC), yes)
	CFLAGS += -pedantic 
endif

ifeq ($(OPT), yes)
	CFLAGS += $(OPTIMIZATION)
endif

all: $(PROJECT)

$(PROJECT): $(OBJS) | $(BDIR)
	$(CC) $(LIBS) $^ -o $@  

$(ODIR)/%.o: $(SDIR)/%.c | $(ODIR)
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)



$(BDIR) $(ODIR):
	mkdir $@
	

clean:
	rm -f $(ODIR)/*.o *~ core $(IDIR)/*~ 

run: $(PROJECT)
	$< $(ARGS)

rebuild: clean all

.PHONY : clean
.SILENT : clean