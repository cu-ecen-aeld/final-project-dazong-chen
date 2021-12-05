export ARCH:=arm
CROSS_COMPILE=

ifeq ($(CC),)
	CC=$(CROSS_COMPILE)gcc
endif

ifeq ($(CFLAGS),)
	CFLAGS= -g -Wall -Werror
endif

LFLAGS  = -Wall -I. -lm

LINKER = $(CROSS_COMPILE)gcc

BINDIR = bin

#client setup
CLIENTSRCDIR = client
CLIENTOBJDIR = client/obj

CLIENTSOURCES := $(wildcard $(CLIENTSRCDIR)/*.c)
CLIENTOBJECTS := $(CLIENTSOURCES:$(CLIENTSRCDIR)/%.c=$(CLIENTOBJDIR)/%.o)

#server setup
SERVERSRCDIR = server
SERVEROBJDIR = server/obj

SERVERSOURCES := $(wildcard $(SERVERSRCDIR)/*.c)
SERVEROBJECTS := $(SERVERSOURCES:$(SERVERSRCDIR)/%.c=$(SERVEROBJDIR)/%.o)

#htu21d_sensor setup
HTU21DSRCDIR = htu21d_sensor
HTU21DOBJDIR = htu21d_sensor/obj

HTU21DSOURCES := $(wildcard $(HTU21DSRCDIR)/*.c)
HTU21DOBJECTS := $(HTU21DSOURCES:$(HTU21DSRCDIR)/%.c=$(HTU21DOBJDIR)/%.o)

all: $(BINDIR)/client $(BINDIR)/server
default: $(BINDIR)/client $(BINDIR)/server

# CLIENT
$(BINDIR)/client: $(CLIENTOBJECTS)
	@$(LINKER) $(CLIENTOBJECTS) $(LFLAGS) -o $@
	@echo "client Linking complete!"

$(CLIENTOBJECTS): $(CLIENTOBJDIR)/%.o : $(CLIENTSRCDIR)/%.c
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo "client Compiled "

#SERVER
$(BINDIR)/server: $(SERVEROBJECTS) $(HTU21DOBJECTS)
	@$(LINKER) $(SERVEROBJECTS) $(HTU21DOBJECTS) $(LFLAGS) -o $@
	@echo "server Linking complete!"

$(SERVEROBJECTS): $(SERVEROBJDIR)/%.o: $(SERVERSRCDIR)/%.c
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo "server Compiled "

#htu21d_sensor
$(HTU21DOBJECTS): $(HTU21DOBJDIR)/%.o: $(HTU21DSRCDIR)/%.c
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo "htu21d Compiled "

clean :
	rm -f $(BINDIR)/client $(CLIENTOBJDIR)/*.o $(BINDIR)/server $(SERVEROBJDIR)/*.o $(HTU21DOBJDIR)/*.o
