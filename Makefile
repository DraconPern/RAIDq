CC=    gcc
CXX=   g++
LD=    gcc

CFLAGS= -O2 -mavx2 -std=gnu99 -Wall -Wextra
CXXFLAGS= -O2 -mavx2 -Wall -Wextra -fno-exceptions -fno-rtti -nostdinc++
INCPATH= -I/usr/local/include -I/opt/local/include -I/usr/include
LDFLAGS= 
LIBPATH= -L/usr/local/lib -L/opt/local/lib -L/usr/lib
LIBS=    -lm


CPPOBJ = raid6_ssse3.o raid7_sqrt.o raid7_plank.o\
	raid_rs.o raidq.o
#	raid8_plank.o

OBJ= gf256.o raid_table.o rs_table.o recover.o\
	raid_rs.o raidq.o
#gen_syndrome.o
#	raid6_ssse3.o raid6_gen_synd.o\
#	raid7_sqrt.o raid7_sqrt_gen_synd.o\
#	raid7_plank.o raid7_gen_synd.o\
#	raid8_plank.o

CSRC= $(wildcard *.cpp)

ifdef KERNEL
	CFLAGS += -mcmodel=kernel
	CXXFLAGS += -mcmodel=kernel
endif

ifdef DEBUG
        CFLAGS+=  -D_DEBUG_
        CXXFLAGS+= -D_DEBUG_
endif

ifdef BM_INV_MAT
	CFLAGS += -D_BM_INV_MAT_
	CXXFLAGS += -D_BM_INV_MAT_
endif

ifdef NO_SSE
	CFLAGS += -D_NO_SSE_
	CXXFLAGS += -D_NO_SSE_
endif

ifdef NUM_XMM
	CFLAGS += -D_NUM_XMM_=$(NUM_XMM)
	CXXFLAGS += -D_NUM_XMM_=$(NUM_XMM)
endif

ifdef K
	CFLAGS += -DK=$(K)
	CXXFLAGS += -DK=$(K)
endif

ifdef P
	CFLAGS += -DP=$(P)
	CXXFLAGS += -DP=$(P)
endif

ifdef AVX2
	CFLAGS += -mavx2 -D_USE_AVX2_
	CXXFLAGS += -mavx2 -D_USE_AVX2_
endif

ifdef AVX
	CFLAGS += -mavx -D_USE_AVX_
	CXXFLAGS += -mavx -D_USE_AVX_
endif

ifdef GPROF
	CFLAGS += -pg
	CXXFLAGS += -pg
	LDFLAGS += -pg
endif

.PHONY: all tests tables clean

all: $(OBJ) tests


modules: $(CPPOBJ)
	cp *.c modules
	cp $(CPPOBJ) modules
	cd modules; make

%-test: $(OBJ) %-test.o
	$(LD) $(LDFLAGS) $(LIBPATH) -o $@ $^ $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) $(INCPATH) -c $<

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCPATH) -c $<

tests:
	cd tests; make

tables:
	cd supplement; make

clean:
	rm -f *.o; cd tests; make clean
