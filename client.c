#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>

void client(void)
{
    const int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1)
    {
        perror("Failed to create socket");
        exit(1);
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);

    const socklen_t addrlen = sizeof(addr);

    const int connection_id = connect(server_fd, (const struct sockaddr*)&addr, addrlen);
    if (connection_id == -1)
    {
        perror("Failed to connect");
        exit(1);
    }

    char username[256];
    printf("Enter username: ");
    fflush(stdout);

    const char* fg_name = fgets(username, sizeof(username), stdin);
    if (fg_name == NULL)
    {
        fprintf(stderr, "Failed to read username\n");
        exit(1);
    }

    username[strcspn(username, "\n")] = 0;
    write(server_fd, username, strlen(username));

    char buffer[8192];

    while (1)
    {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(0, &readfds);
        FD_SET(server_fd, &readfds);

        select(server_fd + 1, &readfds, NULL, NULL, NULL);

        if (FD_ISSET(0, &readfds))
        {
            if (fgets(buffer, sizeof(buffer), stdin) == NULL) break;
            write(server_fd, buffer, strlen(buffer));
        }

        if (FD_ISSET(server_fd, &readfds))
        {
            const ssize_t bytes_read = read(server_fd, buffer, sizeof(buffer));
            if (bytes_read <= 0)
            {
                printf("Server disconnected\n");
                break;
            }
            buffer[bytes_read] = '\0';
            printf("%s", buffer);
            fflush(stdout);
        }
    }
}