# DSTMAKEFILE
ifndef DSTMAKEFILE

include third_party/Makefile
include libs/Makefile

# 底层增加，删除后请在此修改，顶层不用修改
DSTSUB=vce get_frame_seq vpsnr checkdropframe ssim msssim

# 顶层调用的入口
dst-all-sub: $(DSTSUB)

vce: $(SRCDIR)/vce.o $(LIBOBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

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

siti: $(SRCDIR)/siti.o $(LIBOBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)	

# build.
$(SRCDIR)/vce.o: 
	$(CXX) $(CXXFLAGS) -c -o $(SRCDIR)/vce.o $(SRCDIR)/vce.cpp $(INCLUDES)

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

$(SRCDIR)/siti.o:
	$(CXX) $(CXXFLAGS) -c -o $(SRCDIR)/siti.o $(SRCDIR)/siti.cpp $(INCLUDES)

# src/lib/
$(SRCLIBDIR)/ocrlib.o: 
	$(CXX) $(CXXFLAGS) -c -o $(SRCLIBDIR)/ocrlib.o $(SRCLIBDIR)/ocrlib.cpp $(INCLUDES)

$(SRCLIBDIR)/psnrlib.o: 
	$(CXX) $(CXXFLAGS) -c -o $(SRCLIBDIR)/psnrlib.o $(SRCLIBDIR)/psnrlib.cpp $(INCLUDES)

$(SRCLIBDIR)/command1.o: 
	$(CXX) $(CXXFLAGS) -c -o $(SRCLIBDIR)/command1.o $(SRCLIBDIR)/command1.cpp $(INCLUDES)

# src/conf/ 
$(SRCCONFDIR)/conf.o: 
	$(CXX) $(CXXFLAGS) -c -o $(SRCCONFDIR)/conf.o $(SRCCONFDIR)/conf.cpp $(INCLUDES)

# src/utils/
$(SRCUTILSDIR)/matrixutils.o: 
	$(CXX) $(CXXFLAGS) -c -o $(SRCUTILSDIR)/matrixutils.o $(SRCUTILSDIR)/matrixutils.cpp $(INCLUDES)

# third_parth/SITI/
$(THIRDIR)/SITI/vce_siti.o: 
	$(CXX) $(CXXFLAGS) -c -o $(THIRDIR)/SITI/vce_siti.o $(THIRDIR)/SITI/vce_siti.cpp $(INCLUDES)
endif