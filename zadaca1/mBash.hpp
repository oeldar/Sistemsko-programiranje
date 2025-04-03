#pragma once
#include <iostream>
#include <dirent.h>
#include <sys/stat.h>
#include <utime.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string>

void mecho(const std::string&);
void mcat(const std::string&);
void mtouch(const std::string&);
void mls(const std::string&);
void printFileInfo(const std::string&, const struct dirent*);

