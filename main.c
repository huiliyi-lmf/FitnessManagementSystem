#define _CRT_SECURE_NO_WARNINGS
#include "fileio.h"
#include "gui.h"

int main(void) {
	if (!checkDataFileSign()) {
		initDataFile();
	}
	return showLoginWindow();
}