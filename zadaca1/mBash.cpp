#include "mBash.hpp"

void printFileInfo(const std::string &path, const struct dirent *entry) {
  struct stat fileStat;
  std::string fullPath = path + "/" + entry->d_name;

  if (stat(fullPath.c_str(), &fileStat) == -1) {
    perror("stat");
    return;
  }

  std::cout << fileStat.st_ino << " ";
  std::cout << ((S_ISDIR(fileStat.st_mode)) ? "d " : "- ");
  std::cout << fileStat.st_nlink << " ";
  std::cout << fileStat.st_uid << " ";
  std::cout << fileStat.st_gid << " ";
  std::cout << fileStat.st_size << "B ";

  char timeBuff[20];
  strftime(timeBuff, sizeof(timeBuff), "%Y-%m-%d %H:%M",
           localtime(&fileStat.st_mtime));
  std::cout << timeBuff << " ";
  std::cout << entry->d_name << ((S_ISDIR(fileStat.st_mode)) ? "/" : "")
            << std::endl;
}

void mls(const std::string &path) {
  DIR *dir = opendir(path.c_str());
  if (!dir) {
    std::cout << "No such directory." << std::endl;
    return;
  }

  struct dirent *entry;
  while ((entry = readdir(dir)) != nullptr)
    printFileInfo(path, entry);
}

void mtouch(const std::string &path) {
  if (path == "") {
    std::cout << "Command mtouch requires argument." << std::endl;
    return;
  }
  struct stat fileStat;

  if (stat(path.c_str(), &fileStat) == 0) {
    // postoji fajl
    struct utimbuf newTime;
    newTime.actime = time(nullptr);
    newTime.modtime = time(nullptr);

    if (utime(path.c_str(), &newTime) == -1) {
      perror("utime");
      return;
    }

  } else {
    // ne postoji fajl
    int fd = open(path.c_str(), O_CREAT | O_WRONLY, 0644);
    if (fd == -1) {
      perror("open");
      return;
    }
    close(fd);
  }
}

void mcat(const std::string &path) {
  if (path == "") {
    std::cout << "Command mcat requires argument" << std::endl;
    return;
  }

  int fd = open(path.c_str(), O_RDONLY, 0644);

  if (fd == -1) {
    std::cout << "File does not exist" << std::endl;
    return;
  }

  char buffer[4096];
  ssize_t bytesRead;
  while ((bytesRead = read(fd, buffer, sizeof(buffer))) > 0) {
    write(STDOUT_FILENO, buffer, bytesRead);
  }
  std::cout << std::endl;

  close(fd);
}

void mecho(const std::string &arg) {
  std::string writeln, path;
  bool redirection = false;
  int i = 0;
  while (arg[i] != '>') {
    writeln.push_back(arg[i]);
    ++i;
  }
  if (arg[i] == '>')
    redirection = true;
  ++i;
  while (arg[i] == ' ')
    ++i;
  while (i != arg.size()) {
    path.push_back(arg[i]);
    ++i;
  }

  if (redirection) {
    int fd = open(path.c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (fd == -1) {
      perror("create");
      return;
    }

    write(fd, writeln.c_str(), writeln.size());

    close(fd);
    return;
  }

  write(STDOUT_FILENO, (arg + "\n").c_str(), arg.size() + 1);
}
