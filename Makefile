# Build library
BIN_DIR := ./bin
SRC_DIRS := ./src
INC_DIR := ./include ./src
LIB_DIR := ./lib
# INSTALL_PREFIX := /usr/local

# flag
# CPPFLAGS := 
CFLAGS = -g -O2 -fPIC -rdynamic -no-pie -I. -Isrc/common
LDFLAGS = -lhiredis

OBJS_COMMON = \
src/common/libposix.o \
src/common/libbt.o \
src/common/libmem.o \
src/common/libanet.o \
src/common/libsock.o \
src/common/libopt.o \
src/common/liblog.o \
src/common/libstr.o \
src/common/libvector.o \
src/common/libvty.o \
src/common/linenoise.o \
src/common/liblinklist.o \
src/common/libhashmap.o \
src/common/libinet.o \
src/common/cJSON.o \
src/common/json_config.o \
src/common/iniparser.o \
src/common/ini_config.o \
src/common/libconfig.o \
src/common/libae.o \

OBJS_APP = \
src/app/main.o 

OBJS_FOO = \
src/app/foo.o 

# Build step for c source
%.o:%.c
	$(CC) -c $(CFLAGS) $< -o $@

all: app 

# Build step for target source
app: $(OBJS_COMMON) $(OBJS_APP)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/$@ $^ $(LDFLAGS)

foo: $(OBJS_COMMON) $(OBJS_FOO)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/$@ $^ $(LDFLAGS)

clean:
	$(RM) $(OBJS_COMMON) $(OBJS_APP)
