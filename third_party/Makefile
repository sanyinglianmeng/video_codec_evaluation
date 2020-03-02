# set the third-party requirements
ifdef LIBS
    LIBS += $(shell pkg-config --libs opencv yaml-cpp libavcodec libavformat libavdevice libavutil python3)
else
    LIBS=$(shell pkg-config --libs opencv yaml-cpp libavcodec libavformat libavdevice libavutil python3)
endif

# set the third-party requirements
ifdef INCLUDES
    INCLUDES += $(shell pkg-config --cflags opencv yaml-cpp libavcodec libavformat libavdevice libavutil python3)
else
    INCLUDES=$(shell pkg-config --cflags opencv yaml-cpp libavcodec libavformat libavdevice libavutil python3)
endif

# 暂时没有用到，先注释
# ifdef LIBS
#     LIBS += $(THIRDIR)/SITI/siti.o
# endif