#include "aesdsocket.h"

static struct sigaction sig_action_handler = {0};

static bool terminate = false;

void signal_handler(int signo)
{
    if (signo == SIGINT || signo == SIGTERM)
    {
        syslog(LOG_USER, "Caught termination signal. Exiting.");
        terminate = true;
    }
}

int init_signal_handler()
{
    sig_action_handler.sa_handler = signal_handler;
    memset(&sig_action_handler.sa_mask, 1, sizeof(sig_action_handler.sa_mask));
    if (sigaction(SIGINT, &sig_action_handler, NULL) != 0)
    {
        perror("sigaction");
        syslog(LOG_ERR, "sigaction failed for SIGINT.");
        return -1;
    }

    if (sigaction(SIGTERM, &sig_action_handler, NULL) != 0)
    {
        perror("sigaction");
        syslog(LOG_ERR, "sigaction failed for SIGTERM.");
        return -1;
    }
    syslog(LOG_USER, "Signal handler setup succeeded.");
    return 0;
}

int send_file_contents_over_socket(socket_context *socket_context)
{
    ssize_t bytesRead = 0;
    FILE *dataFile = fopen(FILENAME, "r");
    if (!dataFile)
    {
        syslog(LOG_ERR, "Error opening data file.");
        return -1;
    }

    char buffer[BUFFERSIZE];
    while ((bytesRead = fread(buffer, 1, sizeof(buffer), dataFile)))
    {
        if ((send(
                *socket_context->pointer_accept_connection_fd, buffer, bytesRead, 0)) !=
            bytesRead)
        {
            syslog(
                LOG_ERR, "Error sending file contents over socket connection.");
            return -1;
        }
    }
    fclose(dataFile);
    return 0;
}

int receive(socket_context *pointer_socket_context)
{
    FILE *dataFile = fopen(FILENAME, "a");
    if (!dataFile)
    {
        syslog(LOG_ERR, "Unable to open file for writing.");
        return -1;
    }

    size_t bytesReceived = 0;
    char buffer[BUFFERSIZE];

    while ((bytesReceived = recv(
                *pointer_socket_context->pointer_accept_connection_fd,
                buffer,
                sizeof(buffer),
                0)) > 0)
    {
        fwrite(buffer, 1, bytesReceived, dataFile);
        if (memchr(buffer, '\n', bytesReceived) != NULL)
        {
            break;
        }
    }
    fclose(dataFile);

    send_file_contents_over_socket(pointer_socket_context);
    return 0;
}

int accept_connections(socket_context *pointer_socket_context)
{
    while (1)
    {
        if (terminate)
        {
            return 0;
        }
        *pointer_socket_context->pointer_sock_addr_storage_size =
            sizeof(*pointer_socket_context->pointer_sock_addr_storage);
        int acceptedConnectionFd = accept(
            *pointer_socket_context->pointer_socket_fd,
            (struct sockaddr *)pointer_socket_context->pointer_sock_addr_storage,
            pointer_socket_context->pointer_sock_addr_storage_size);
        if (acceptedConnectionFd == -1)
        {
            syslog(LOG_ERR, "Eroor accepting connection.");
            continue;
        }
        else
        {
            pointer_socket_context->pointer_accept_connection_fd = &acceptedConnectionFd;
            syslog(LOG_USER, "Accepted connection.");

            receive(pointer_socket_context);
        }
    }
    return 0;
}

int cleanup(socket_context *pSocketContext)
{
    freeaddrinfo(pSocketContext->pointer_addr_info);
    pSocketContext->pointer_addr_info = NULL;
    pSocketContext->pointer_hints = NULL;
    pSocketContext->pointer_sock_addr_storage = NULL;
    pSocketContext->pointer_sock_addr_storage_size = NULL;
    pSocketContext->pointer_socket_fd = NULL;
    pSocketContext->pointer_accept_connection_fd = NULL;

    free(pSocketContext);
    remove(FILENAME);
    return 0;
}



int main(int argc, char *argv[])
{
    socket_context *pointer_socket_context = malloc(sizeof(socket_context));
    struct sockaddr_storage sock_addr_storage;
    socklen_t sock_addr_storage_size;
    struct addrinfo hints, *pointer_addr_info;
    int socketFd;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    pid_t pid;

    bool runAsDaemon = false;
    openlog(NULL, 0, LOG_USER);

    for (int i = 1; i < argc; i++)
    {
        if (strstr(argv[i], "-d"))
        {
            runAsDaemon = true;
            break;
        }
    }

    if (init_signal_handler() == -1)
    {
        return -1;
    }
    getaddrinfo(NULL, PORT, &hints, &pointer_addr_info);

    socketFd = socket(
        pointer_addr_info->ai_family,
        pointer_addr_info->ai_socktype,
        pointer_addr_info->ai_protocol);
    if (socketFd == -1)
    {
        free(pointer_socket_context);
        freeaddrinfo(pointer_addr_info);
        syslog(LOG_ERR, "Error creating socket");
        exit(EXIT_FAILURE);
    }

    int sockOpt = 1;
    if (setsockopt(
            socketFd,
            SOL_SOCKET,
            SO_REUSEADDR | SO_REUSEPORT,
            &sockOpt,
            sizeof(sockOpt)))
    {
        syslog(LOG_ERR, "Could not set socket options.");
        free(pointer_socket_context);
        freeaddrinfo(pointer_addr_info);
        exit(EXIT_FAILURE);
    }

    if (bind(socketFd, pointer_addr_info->ai_addr, pointer_addr_info->ai_addrlen) == -1)
    {
        syslog(LOG_ERR, "Could not bind socket to address.");
        free(pointer_socket_context);
        freeaddrinfo(pointer_addr_info);
        exit(EXIT_FAILURE);
    }

    if (listen(socketFd, BACKLOG) == -1)
    {
        syslog(LOG_ERR, "Could not invoke listen on socket.");
    }
    sock_addr_storage_size = sizeof(sock_addr_storage);

    pointer_socket_context->pointer_socket_fd = &socketFd;
    pointer_socket_context->pointer_sock_addr_storage = &sock_addr_storage;
    pointer_socket_context->pointer_sock_addr_storage_size = &sock_addr_storage_size;
    pointer_socket_context->pointer_hints = &hints;
    pointer_socket_context->pointer_addr_info = pointer_addr_info;

    if (runAsDaemon)
    {
        pid = fork();
        if (pid == -1)
        {
            syslog(LOG_ERR, "Unable to fork.");
            perror("fork");
            return -1;
        }
        else if (pid != 0)
        {
            syslog(LOG_USER, "Running in daemon mode.");
            exit(EXIT_SUCCESS);
        }

        if (setsid() == -1)
        {
            syslog(LOG_ERR, "setsid failed after fark.");
            perror("setsid");
            return -1;
        }

        if (chdir("/") == -1)
        {
            syslog(LOG_ERR, "chdir failed after fork.");
            perror("chdir");
            return -1;
        }

        int fd = open("/dev/null", O_WRONLY | O_CREAT, 0666);
        dup2(fd, 1);

        accept_connections(pointer_socket_context);
        syslog(LOG_USER, "Closing connections and cleaning up.");
        cleanup(pointer_socket_context);

        if (fd != -1)
        {
            close(fd);
        }

        if (socketFd != -1)
        {
            close(socketFd);
        }

        closelog();
        exit(EXIT_SUCCESS);
    }

    syslog(LOG_USER, "Running in interactive mode.");

    accept_connections(pointer_socket_context);
    syslog(LOG_USER, "Closing connections and cleaning up.");
    cleanup(pointer_socket_context);
    if (socketFd != -1)
    {
        close(socketFd);
    }
    closelog();
    exit(EXIT_SUCCESS);
}