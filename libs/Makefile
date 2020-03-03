# prj中新增的全局lib,utils需要在此注册
ifndef LIBOBJ
   LIBOBJ =  $(SRCUTILSDIR)/matrixutils.o \
             $(SRCDIR)/frame_drop_detect.o \
             $(SRCCONFDIR)/conf.o \
             $(SRCLIBDIR)/psnrlib.o \
             $(SRCLIBDIR)/ocrlib.o \
             $(SRCUTILSDIR)/cmdlineutils.o \
             # 封装第三方库的lib，也统一放到这里，第三方的库封装统一用vce_作为前缀
             $(THIRDIR)/SITI/vce_siti.o
endif