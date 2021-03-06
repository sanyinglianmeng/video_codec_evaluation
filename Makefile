# include the third-party requirements
include third_party/Makefile
# 项目封装的libs需要在libs/Makefile中注册，以供全局使用
include libs/Makefile

# configuration
DEBUG ?= 0     # 0: -DNDEBUG   1:-DDEBUG

# set the complier.
ifndef CXX
    CXX=g++ 
endif
 
ifeq (c++, $(findstring c++,$(CXX)))
    CXX=g++
endif

# set the compiler's flags.
ifndef CXXFLAGS
    CXXFLAGS=-O2 -Wall -Wno-sign-compare -g
endif

ifeq ($(DEBUG), 1)
    override CXXFLAGS += -DDEBUG
else
    override CXXFLAGS += -DNDEBUG
endif

ifeq (g++, $(findstring g++,$(CXX)))
    override CXXFLAGS += -std=c++11
else ifeq (clang++, $(findstring clang++,$(CXX)))
    override CXXFLAGS += -std=c++11
else ifeq ($(CXX), c++)
    ifeq ($(shell uname -s), Darwin)
        override CXXFLAGS += -std=c++11
    endif
endif

# set the PREFIEX
ifndef PREFIX
    PREFIX=.
endif

ifndef SRCDIR
    SRCDIR=src
endif

ifndef SRCLIBDIR
    SRCLIBDIR=src/lib
endif

ifndef SRCCONFDIR
    SRCCONFDIR=src/conf
endif

ifndef SRCUTILSDIR
    SRCUTILSDIR=src/utils
endif

ifndef TESTDIR
    TESTDIR=test
endif

ifndef PRJDIR
    PRJDIR=.
endif

ifndef THIRDIR
    THIRDIR=third_party
endif

ifndef OUTPUTDIR
    OUTPUTDIR=output
endif

# 强耦合，底层修改，这里也会生效
DST=dst-all
TEST=test-all
# 第三方库的编译产出
THIRDDST=siti

all: $(DST) $(TEST) $(THIRDDST)

include $(TESTDIR)/Makefile
test-all:
	make test-all-sub

include $(SRCDIR)/Makefile
dst-all:
	make dst-all-sub

# 第三方库的siti编译产出
# siti_t: $(THIRDIR)/SITI/siti.o
# 	-sh $(THIRDIR)/SITI/build.sh

install:
	make all
	$(shell sh build.sh)

.PHONY : clean
clean:
	-rm -rf $(SRCDIR)/*.o $(SRCCONFDIR)/*.o $(SRCLIBDIR)/*.o $(SRCUTILSDIR)/*.o $(TESTDIR)/*.o \
    -rm -rf $(THIRDIR)/SITI/*.o \
    -rm -rf $(DSTSUB) \
    -rm -rf $(TESTSUB) \
    -rm -rf $(THIRDDST) \
    -rm -rf $(OUTPUTDIR)

.PHONY : clean_data
clean_data:
	-rm -rf data/* psnr/data/*