#include <sys/un.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define LINDA_SERVER
#include "linda.h"
#include "linda_internal.h"

int Linda_sd;
#ifdef USE_DOMAIN_SOCKETS
int Linda_udd = 0;
#endif
int Linda_port = 2102;
int Linda_active_connections;

char* process_id;
char* thread_id;

unsigned char Linda_serve() {
    int err;

#ifdef USE_DOMAIN_SOCKETS
    Linda_udd = socket(PF_UNIX, SOCK_STREAM, 0);
    if(Linda_udd != -1) {
        struct sockaddr_un addr_in;
        addr_in.sun_family = AF_UNIX;
        memcpy(addr_in.sun_path, "/tmp/pylinda", 13);

        err = bind(Linda_udd, (struct sockaddr*)&addr_in, sizeof(struct sockaddr));
        if(err == -1) {
            unlink("/tmp/pylinda");
            Linda_udd = 0;
        } else {
            err = listen(Linda_udd, 5);
            if(err == -1) {
                unlink("/tmp/pylinda");
                Linda_udd = 0;
            } else {
                Linda_active_connections += 1;
            }
        }
    }
#endif
    Linda_sd = socket(PF_INET, SOCK_STREAM, 0);
    if(Linda_sd == -1) return 0;

    struct sockaddr_in addr_in;
    addr_in.sin_family = AF_INET;
    addr_in.sin_port = htons(Linda_port);
    if(inet_aton("0.0.0.0", (struct in_addr*)&(addr_in.sin_addr.s_addr)) == 0) return 0;

    memset(&(addr_in.sin_zero), 0, 8);

    err = bind(Linda_sd, (struct sockaddr*)&addr_in, sizeof(struct sockaddr));
    if(err == -1) return 0;
    err = listen(Linda_sd, 5);
    if(err == -1) return 0;

    Linda_active_connections += 1;

#ifdef USE_DOMAIN_SOCKETS
    if(Linda_udd == 0 && Linda_sd == 0) {
#else
    if(Linda_sd == 0) {
#endif
        return 0;
    } else {
        return 1;
    }
}

int Linda_accept(int sd) {
    return accept(sd, NULL, NULL);
}

int Linda_server_disconnect() {
#ifdef USE_DOMAIN_SOCKETS
    shutdown(Linda_udd, SHUT_RDWR);
    unlink("/tmp/pylinda");
    Linda_active_connections -= 1;
#endif
    shutdown(Linda_sd, SHUT_RDWR);
    Linda_active_connections -= 1;
    if(Linda_active_connections == 0) {
        Message_shutdown();
    }
    return 1;
}

int Linda_connect(char* address) {
    int sd;
    int colon;
    int port;
    for(colon=0; colon<strlen(address); colon++) {
        if(address[colon] == ':') {
            break;
        }
    }
    if(colon == strlen(address)) {
        port = 2102;
    } else {
        address[colon] = '\0';
        port = atoi(&(address[colon+1]));
    }

    sd = socket(PF_INET, SOCK_STREAM, 0);
    if(sd == -1) return 0;

    struct sockaddr_in addr_in;
    addr_in.sin_family = AF_INET;
    addr_in.sin_port = htons(port);
    if(inet_aton(address, (struct in_addr*)&(addr_in.sin_addr.s_addr)) == 0) return 0;

    memset(&(addr_in.sin_zero), 0, 8);

    int err = connect(sd, (struct sockaddr*)&addr_in, sizeof(struct sockaddr));
    if(err == -1) return 0;

    Linda_active_connections += 1;

    return sd;
}

void Linda_disconnect(int sd) {
    shutdown(sd, SHUT_RDWR);
    Linda_active_connections -= 1;
}
