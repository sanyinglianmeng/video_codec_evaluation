#include "cmdlineutils.h"

cmdline::parser initCmdLine(int argc, char *argv[]) {
	cmdline::parser cmdPara;

	cmdPara.add<std::string>("input", 'i', "the video need to process", true, "");
	cmdPara.add<int>("originWidth", 'w', "the video width before codec operator", true, 0);
	cmdPara.add<int>("originHeight", 'h', "the video height before codec operator", true, 0);
	cmdPara.add<std::string>("dir", 'd', "the path that the frame number area extract from the video", false, "data");

	return cmdPara;
}

bool isDirExist(std::string dir) {
	if (dir == "") {
		return false;
	}

	if (opendir(dir.c_str()) == NULL) {
		return false;
	}

	return true;
}