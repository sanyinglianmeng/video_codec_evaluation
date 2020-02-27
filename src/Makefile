# DSTMAKEFILE
ifndef DSTMAKEFILE

include third_party/Makefile
include libs/Makefile

# 底层增加，删除后请在此修改，顶层不用修改
DSTSUB=get_frame_seq vpsnr checkdropframe ssim msssim

# 顶层调用的入口
dst-all-sub: $(DSTSUB)

get_frame_seq: $(SRCDIR)/get_frame_seq.o $(LIBOBJ) 
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

vpsnr: $(SRCDIR)/vpsnr.o $(LIBOBJ) 
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

checkdropframe: $(SRCDIR)/check_dropframe.o $(LIBOBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

ssim: $(SRCDIR)/ssim.o $(LIBOBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

msssim: $(SRCDIR)/msssim.o $(LIBOBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)	

# build.
$(SRCDIR)/get_frame_seq.o: 
	$(CXX) $(CXXFLAGS) -c -o $(SRCDIR)/get_frame_seq.o $(SRCDIR)/get_frame_seq.cpp $(INCLUDES)

$(SRCDIR)/check_dropframe.o: 
	$(CXX) $(CXXFLAGS) -c -o $(SRCDIR)/check_dropframe.o $(SRCDIR)/check_dropframe.cpp $(INCLUDES)

$(SRCDIR)/frame_drop_detect.o: 
	$(CXX) $(CXXFLAGS) -c -o $(SRCDIR)/frame_drop_detect.o $(SRCDIR)/frame_drop_detect.cpp $(INCLUDES)

$(SRCDIR)/vpsnr.o: 
	$(CXX) $(CXXFLAGS) -c -o $(SRCDIR)/vpsnr.o $(SRCDIR)/vpsnr.cpp $(INCLUDES)

$(SRCDIR)/ssim.o:
	$(CXX) $(CXXFLAGS) -c -o $(SRCDIR)/ssim.o $(SRCDIR)/ssim.cpp $(INCLUDES)

$(SRCDIR)/msssim.o:
	$(CXX) $(CXXFLAGS) -c -o $(SRCDIR)/msssim.o $(SRCDIR)/msssim.cpp $(INCLUDES)

# src/lib/
$(SRCLIBDIR)/ocrlib.o: 
	$(CXX) $(CXXFLAGS) -c -o $(SRCLIBDIR)/ocrlib.o $(SRCLIBDIR)/ocrlib.cpp $(INCLUDES)

$(SRCLIBDIR)/psnrlib.o: 
	$(CXX) $(CXXFLAGS) -c -o $(SRCLIBDIR)/psnrlib.o $(SRCLIBDIR)/psnrlib.cpp $(INCLUDES)

# src/conf/ 
$(SRCCONFDIR)/conf.o: 
	$(CXX) $(CXXFLAGS) -c -o $(SRCCONFDIR)/conf.o $(SRCCONFDIR)/conf.cpp $(INCLUDES)

# src/utils/
$(SRCUTILSDIR)/matrixutils.o: 
	$(CXX) $(CXXFLAGS) -c -o $(SRCUTILSDIR)/matrixutils.o $(SRCUTILSDIR)/matrixutils.cpp $(INCLUDES)
endif