#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>

#define BUFFER_SIZE 4096

void error(const char *msg) {
    perror(msg);
    exit(1);
}

void send_404(int client_fd) {
    char response[] =
        "HTTP/1.1 404 Not Found\r\n"
        "Content-Type: text/html\r\n\r\n"
        "<html><body><h1>404 Not Found</h1></body></html>";
    send(client_fd, response, strlen(response), 0);
}

void send_header(int client_fd, const char *status, const char *content_type) {
    char header[BUFFER_SIZE];
    snprintf(header, sizeof(header),
             "HTTP/1.1 %s\r\n"
             "Content-Type: %s\r\n\r\n", status, content_type);
    send(client_fd, header, strlen(header), 0);
}

const char *get_mime_type(const char *path) {
    const char *ext = strrchr(path, '.'); // /dir/index.html -> .html
    if (!ext) return "application/octet-stream";
    if (strcmp(ext, ".html") == 0) return "text/html";
    return "application/octet-stream";
}

void list_directory(int client_fd, const char *path) {
    DIR *dir = opendir(path);
    if (!dir) {
        send_404(client_fd);
        return;
    }

    send_header(client_fd, "200 OK", "text/html");

    char buffer[BUFFER_SIZE];
    snprintf(buffer, sizeof(buffer),
             "<html><body><h1>Index of %s</h1><ul>", path);
    send(client_fd, buffer, strlen(buffer), 0);

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        snprintf(buffer, sizeof(buffer),
                 "<li><a href=\"%s/%s\">%s</a></li>",
                 path, entry->d_name, entry->d_name);
        send(client_fd, buffer, strlen(buffer), 0);
    }

    snprintf(buffer, sizeof(buffer), "</ul></body></html>\n");
    send(client_fd, buffer, strlen(buffer), 0);

    closedir(dir);
}

void send_file(int client_fd, const char *path) {
    FILE *fp = fopen(path, "rb");
    if (!fp) {
        send_404(client_fd);
        return;
    }

    send_header(client_fd, "200 OK", get_mime_type(path));

    char buffer[BUFFER_SIZE];
    size_t n;
    while ((n = fread(buffer, 1, sizeof(buffer), fp)) > 0) {
        send(client_fd, buffer, n, 0);
    }

    fclose(fp);
}

void handle_request(int client_fd) {
    char buffer[BUFFER_SIZE];

    while (1) {
        int received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
        if (received <= 0) {
            break; // klijent je zatvorio konekciju ili greška
        }

        buffer[received] = '\0';

        char method[8], path[1024];
        sscanf(buffer, "%s %s", method, path);

        if (strcmp(method, "GET") != 0) {
            send_404(client_fd);
            continue;
        }

        char file_path[1024] = ".";
        strcat(file_path, path);  // GET /index.html -> ./index.html

        struct stat st;
        if (stat(file_path, &st) < 0) {
            send_404(client_fd);
            continue;
        }

        if (S_ISDIR(st.st_mode)) {
            char index_path[1064];
            snprintf(index_path, sizeof(index_path), "%s/index.html", file_path);
            if (stat(index_path, &st) == 0) {
                send_file(client_fd, index_path);
            } else {
                list_directory(client_fd, file_path);
            }
        } else if (S_ISREG(st.st_mode)) {
            send_file(client_fd, file_path);
        } else {
            send_404(client_fd);
        }
    }
}


int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Port required.\n");
        exit(1);
    }

    int port = atoi(argv[1]);

    if (!(port = atoi(argv[1]))) {
      printf("Port not valid.\n");
      exit(1);
    }

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) error("Socket error.");

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);

    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("Bind error.");

    listen(sockfd, 10);
    printf("Server listening on port %d...\n", port);

    while (1) {
        struct sockaddr_in cli_addr;
        socklen_t clilen = sizeof(cli_addr);
        int client_fd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
        if (client_fd < 0) {
            perror("accept");
            continue;
        }

        pid_t pid = fork();
        if (pid == 0) {
            close(sockfd);
            handle_request(client_fd);
            close(client_fd);
            exit(0);
        } else {
            close(client_fd);
        }
    }

    close(sockfd);
    return 0;
}

