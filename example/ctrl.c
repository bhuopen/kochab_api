#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

#include <netdb.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netpacket/packet.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <ifaddrs.h>
#include <syslog.h>

#include "typedef.h"
#include "dmdb.h"

#include "libcgi/cgi-lib.h"
#include "libcgi/html-lib.h"

#define CGI_SID_SIZE 32
#define HTTP_HEADER_COOKIE  "Set-Cookie"
#define SOCK_IO_TIMEOUT 150

struct http_header {
    char *line;
    struct http_header *next;
};

struct http_response {
    struct http_header *header;
    int status;
    int content_length;
    int total_length;
    char *data;
    char *content;
};

static const unsigned char Post_hdr[] =
            "POST /cgi-bin/cgiSrv.cgi HTTP/1.1\r\n"
            "Host: 127.0.0.1\r\n"
            "Cookie: sid=%s\r\n"
            "Accept-Encoding: identity\r\n"
            "Content-Length: %d\r\n"
            "Content-Type: text/xml\r\n"
            "\r\n";

static const unsigned char Get_hdr[] =
            "GET /cgi-bin/cgiSrv.cgi?%s HTTP/1.1\r\n"
            "Host: 127.0.0.1\r\n"
            "Cookie: sid=%s\r\n"
            "Accept-Encoding: identity\r\n"
//            "Content-Length: %d\r\n"
//            "Content-Type: text/xml\r\n"
            "\r\n";

static const unsigned char Op_hdr[] =
            "POST /cgi-bin/cgiSrv.cgi HTTP/1.1\r\n"
            "Host: 127.0.0.1\r\n"
            "Cookie: sid=%s\r\n"
            "Accept-Encoding: identity\r\n"
            "Content-Length: %d\r\n"
            "Content-Type: application/x-www-form-urlencoded; charset=UTF-8\r\n"
            "\r\n";

static char g_sid[CGI_SID_SIZE] = {0};
static char g_user[32];
static char g_pass[32];
static int g_fd = -1;


static void kurl_encode(const char *src, char *dst, size_t dst_len) {
    static const char *dont_escape = "._-$,;~()";
    static const char *hex = "0123456789abcdef";
    const char *end = dst + dst_len - 1;

    for (; *src != '\0' && dst < end; src++, dst++) {
        if (isalnum(*(const unsigned char *) src) ||
                strchr(dont_escape, * (const unsigned char *) src) != NULL) {
            *dst = *src;
        } else if (dst + 2 < end) {
            dst[0] = '%';
            dst[1] = hex[(* (const unsigned char *) src) >> 4];
            dst[2] = hex[(* (const unsigned char *) src) & 0xf];
            dst += 2;
        }
    }

    *dst = '\0';
}

static int kprase_http_status_code(struct http_response *resp, char *start, char *end)
{
    int ret = 0;
    char *s, *e;
    s = strchr(start, ' ');
    if(!s || s > end)
        return -1;
    while(*s == ' ' && s < end)
        s ++;
    e = s + 1;
    while(*e != ' ' && e < end)
        e ++;
    if(e == end || *e != ' ')
        return -1;
    *e = 0;
    resp->status = atoi(s);
    return 0;
}

static char *kget_http_header(struct http_response *resp, const char *header)
{
    struct http_header *h;
    int len;
    h = resp->header;
    len = strlen(header);
    while(h){
        if(memcmp(h->line, header, len) == 0 && h->line[len] == ':')
            return h->line;
        h = h->next;
    }
    return NULL;
}

static int kprase_http_response(struct http_response *resp)
{
    char *p = NULL;
    char *s = NULL;
    char *t = NULL;
    int end = 0;
    struct http_header *header = NULL;

    if(!resp)
        return -1;
    if(!resp->data)
        return -1;

    //For BHU device there is no keep-alive support. Server will close connection after send response

    resp->status = -1;
    s = resp->data;
    do{
        if(!(p = strstr(s, "\r\n"))){
            fprintf(stderr, "invalide http header, no header end flag found\n");
            return -1;
        }
        *p = 0;
        if(memcmp(p + 2, "\r\n", 2) == 0){
            resp->content = p + 4;
            resp->content_length = resp->total_length - (resp->content - resp->data);
            end = 1;
        }
        //handle current line
        if(resp->status == -1){
            if(memcmp(s, "HTTP/", 5) == 0){
                if(kprase_http_status_code(resp, s, p))
                    return -1;
                goto next_loop;
            }
        }
        if(!(header = (struct http_header*)malloc(sizeof(*header)))){
            fprintf(stderr, "oom\n");
            return -1;
        }
        header->line = s;
        header->next = resp->header;
        resp->header = header;
next_loop:
        s = p + 2;
    }while(!end);
    return 0;
}


static int krecv(char *buf, int maxlen)
{
    int n = 0;

    n = recv(g_fd, buf, maxlen, 0);
    if(n < 0){
        fprintf(stderr, "oom\n");
        return -1;
    }
    return n;
}

static int ksend(char *buf, int len)
{
    int n = 0;
    int s = 0;
    while(s < len){
        n = send(g_fd, buf + s, len - s, 0);
        if(n <= 0){
            fprintf(stderr, "send error, - %d, %s\n", errno, strerror(errno));
            return -1;
        }
        s += n;
    }
    return 0;
}

static void kfree_response(struct http_response *resp)
{
    struct http_header *tmp;
    struct http_header *p;

    if(resp){
        if(resp->data)
            free(resp->data);
        p = resp->header;
        while(p){
            tmp = p->next;
            free(p);
            p = tmp;
        }
        free(resp);
    }
}

static int ksock_timeout(int sockfd, int tmout)
{
    struct timeval tmo = {0};

    tmo.tv_sec = tmout;
    tmo.tv_usec = 0;
    if (-1 == setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&tmo, sizeof(tmo)) \
            || -1 == setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, (char *)&tmo, sizeof(tmo))){   
        fprintf(stderr, "setsockopt error.\n");
        return -1; 
    }   

    return 0;
}

static struct http_response * khttp_talk(char *buf, int len, int tm)
{
    int size = 1024;
    char *tmp = NULL;
    struct http_response *resp = NULL;
    int n = 0;
    int r = 0;

    if(kconn() < 0){
        fprintf(stderr, "connect to server failed\n");
        goto fail;
    }
    ksock_timeout(g_fd, tm);
//printf("sending:[\n%s\n]\n", buf);
    if(ksend(buf, len) < 0)
        return NULL;
    if(!(resp = (struct http_response*)malloc(sizeof(*resp)))){
        fprintf(stderr, "oom\n");
        goto fail;
    }
    memset(resp, 0, sizeof(*resp));
    if(!(resp->data = (char*)malloc(size))){
        fprintf(stderr, "oom\n");
        goto fail;
    }
    do{
        if(size - r < 1024){
            size *= 2;
            if(!(tmp = (char*)realloc(resp->data, size))){
                fprintf(stderr, "oom\n");
                goto fail;
            }
            resp->data = tmp;
        }
        n = krecv(resp->data + r, size - r - 1);
        if(n < 0)
            goto fail;
        r += n;
    }while(n > 0);

    resp->data[r] = 0;
    resp->total_length = r;
//printf("got:[\n%s\n]\n", resp->data);
    if(kprase_http_response(resp)){
        fprintf(stderr, "http response prase error\n");
        goto fail;
    }
    return resp;
fail:
    kfree_response(resp);
    if(g_fd != -1){
        close(g_fd);
        g_fd = -1;
    }
    return NULL;
}

char * kget_sid(char *cookie, char *sid)
{
    int i;
    if (cookie == NULL || sid == NULL)
        return NULL;
    char *sid1 = strstr(cookie,"sid=");
    if (sid1 == NULL)
        return NULL;
    sid1 += 4;/* "sid=" size */
    for (i=0; i<CGI_SID_SIZE; i++) {
        if (sid1[i] == ';' || sid1[i] == 0)
            break;
    }   
    if (i >= CGI_SID_SIZE)
        return NULL;
    sid[i] = 0;
    strncpy(sid, sid1, i); 
    return sid;
}



int kconn(void)
{
    int fd;
    int opt = 1;

    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_addr = {
            .s_addr = htonl(0x7F000001),
        },
        .sin_port = htons(80),
    };

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        fprintf(stderr, "Unable to create socket - %d, %s\n", errno, strerror(errno));
        return -1;
    }
    if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        fprintf(stderr, "Unable to connect socket - %d, %s\n", errno, strerror(errno));
        close(fd);
        return -1;
    }
#ifdef SO_NOSIGPIPE 
    setsockopt(fd, SOL_SOCKET, SO_NOSIGPIPE, &opt, sizeof(opt));
#endif

    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1){   
        fprintf(stderr, "setsockopt SO_REUSEADDR fail.\n");
        return -1; 
    }   
    g_fd = fd;
    return 0;
}

struct http_response *kget_path(char *path)
{
    char buf[1024];
    char args[256];
    struct http_response *resp = NULL;

    kurl_encode(path, buf, sizeof(buf));
    sprintf(args, "xml=[%s]", buf);
    sprintf(buf, Get_hdr, args, g_sid);
    strcat(buf, args);

    if(!(resp = khttp_talk(buf, strlen(buf), SOCK_IO_TIMEOUT))){
        fprintf(stderr, "http talk fail\n");
        return NULL;
    }
    if(resp->status < 200 || resp->status > 299){
        fprintf(stderr, "http status code %d\n", resp->status);
        goto fail;
    }
    return resp;
fail:
    kfree_response(resp);
    return NULL;
}

struct http_response *kxml_apply(char *xml)
{
    char buf[1024*10];
    struct http_response *resp = NULL;
    int len;
    int n = 0;
    
    if(!xml)
        return NULL;
    len = strlen(xml);
    n = sprintf(buf, Post_hdr, g_sid, len);
    if( n + len > sizeof(buf)){
        fprintf(stderr, "size too large, not enough buffer");
        return NULL;
    }
    strcat(buf, xml);

    if(!(resp = khttp_talk(buf, n + len, SOCK_IO_TIMEOUT))){
        fprintf(stderr, "http talk fail\n");
        return NULL;
    }
    if(resp->status < 200 || resp->status > 299){
        fprintf(stderr, "http status code %d\n", resp->status);
        goto fail;
    }
    return resp;
fail:
    kfree_response(resp);
    return NULL;
}

struct http_response *kop(char *opstr)
{
    char buf[1024];
    struct http_response *resp = NULL;
    int len;
    int n;

    if(!opstr)
        return NULL;
    len = strlen(opstr);
    n = sprintf(buf, Op_hdr, g_sid, len);
    if( n + len > sizeof(buf)){
        fprintf(stderr, "size too large, not enough buffer");
        return NULL;
    }
    strcat(buf, opstr);

    if(!(resp = khttp_talk(buf, n + len, SOCK_IO_TIMEOUT))){
        fprintf(stderr, "http talk fail\n");
        return NULL;
    }
    if(resp->status < 200 || resp->status > 299){
        fprintf(stderr, "http status code %d\n", resp->status);
        goto fail;
    }
    return resp;
fail:
    kfree_response(resp);
    return NULL;
}


int klogin(void)
{
    char args[256];
    char tmp[128];
    int ret = -1;
    struct http_response *resp = NULL;
    char *cookie = NULL;

    kurl_encode(g_user, tmp, sizeof(tmp));
    sprintf(args, "op=login&user=%s", tmp);
    kurl_encode(g_pass, tmp, sizeof(tmp));
    strcat(args, "&password=");
    strcat(args, tmp);

    if(!(resp = kop(args))){
        fprintf(stderr, "http talk fail\n");
        return -1;
    }

    if(!strstr(resp->content, "result=ok")){
        fprintf(stderr, "login failed, result not ok\n");
        goto out;
    }
    if(!(cookie = kget_http_header(resp, HTTP_HEADER_COOKIE))){
        fprintf(stderr, "login failed, no cookie get\n");
        goto out;
    }
    if(!kget_sid(cookie, g_sid)){
        fprintf(stderr, "login failed, no sid get\n");
        goto out;
    }
    ret = 0;
out:
    kfree_response(resp);
    return ret;
}

void wlanscan_demo()
{
    struct conf_l *l = NULL;
    struct http_response *resp = NULL;
    char buf[1024] = {0};
    char cmd[] = { "<cmd><ITEM cmd=\"wlanscan\" interface=\"wlan0\" /></cmd>" };
    int serial = 0;
    int wait = 0;
    char *s = NULL;
    struct conf_ct *ct;
    struct conf_tb *tb;
    struct conf_cs *cs;
    int count = 0;

    printf("demo how to do wlanscan\n");
    strcpy(buf, cmd);

    do{
        if(!(resp = kxml_apply(buf))){
            fprintf(stderr, "send wlanscan cmd fail\n");
            goto out;
        }
        if(!(l = conf_str2list(resp->content, strlen(resp->content)))){
            fprintf(stderr, "prase result error\n");
            goto out;
        }
        if(!(s = conf_path2value(l, "return.status", NULL, NULL))){
            fprintf(stderr, "get cmd status error\n");
            goto out;
        }
        if(!strcmp(s, "done"))
            goto done;
        else if(!strcmp(s, "error") || !strcmp(s, "deny") || !strcmp(s, "unknown")){
            fprintf(stderr, "cmd execute error\n");
            goto out;
        } else if(!strcmp(s, "doing")){
            if(serial == 0){
                if(!(s = conf_path2value(l, "return.serial", NULL, NULL))){
                    fprintf(stderr, "get cmd serial error\n");
                    goto out;
                }
                serial = atoi(s);
            }

            if(s = conf_path2value(l, "return.wait", NULL, NULL)){
                wait = atoi(s);
            }
            wait = (wait)?wait:1;
            printf("waiting for more result from Server...\n");
            sleep(wait);
            sprintf(buf, "<report><ITEM cmd=\"wlanscan\" serial=\"%d\" op=\"get\" /></report>", serial);
            kfree_response(resp);
            resp = NULL;
            conf_list_free(l);
            l = NULL;
        }
    }while(1);
done:
    s = conf_path2value(l, "return.ap_num", NULL, NULL);
    printf("Total %s BSSID found\n", s);
    conf_path2ct(l, "return", &ct);
    cs = ct->tb->cs;
    tb = cs->st;
    while(tb){
        printf("%d, BSSID[%s], ssid[%s], channel[%s], security[%s], signal[%s], rate[%s]\n",
                count++,
                conf_find_value(tb, "bssid"),
                conf_find_value(tb, "ssid"),
                conf_find_value(tb, "channel"),
                conf_find_value(tb, "security"),
                conf_find_value(tb, "signal"),
                conf_find_value(tb, "rate")
              );
        tb = tb->next;
    }

out:
    kfree_response(resp);
    if(l){
        conf_list_free(l);
    }
}


int main(int argc, char **argv)
{
    struct conf_l *l = NULL;
    char path[] = {"dev.mod.basic.mode"};
    char path2[] = {"dev.wifi.vap"};
    strcpy(g_user, "admin");
    strcpy(g_pass, "admin");
    struct http_response *resp = NULL;
    char work_mode[32] = {0};
    char *s = NULL;

    if(klogin()){
        fprintf(stderr, "login fail\n");
        goto out;
    }
    if(!(resp = kget_path(path))){
        fprintf(stderr, "get path fail\n");
        goto out;
    }
    printf("get path[%s] succeed!\n", path);
    if(!(l = conf_str2list(resp->content, strlen(resp->content)))){
        fprintf(stderr, "prase result error\n");
        goto out;
    }

    //get xml value from path
    s = conf_path2value(l, "dev.mod.basic.mode.mode", NULL, NULL);
    printf("config file version:[%s]\n", conf_pi_path2value(l, "binfo.config_model_ver", NULL, NULL));
    printf("current work mode:[%s]\n", s);
    printf("current scene:[%s]\n", conf_path2value(l, "dev.mod.basic.mode.scene", NULL, NULL));
    strcpy(work_mode, s);
    conf_list_free(l);
    l = NULL;
    kfree_response(resp);
    resp = NULL;

    if(!(resp = kget_path(path2))){
        fprintf(stderr, "get path fail\n");
        goto out;
    }
    printf("get path[%s] succeed!\n", path2);
    if(!(l = conf_str2list(resp->content, strlen(resp->content)))){
        fprintf(stderr, "prase result error\n");
        goto out;
    }
    //get xml value from path by index
    printf("ssid of wlan0:[%s]\n", conf_path2value(l, "dev.wifi.vap.ssid", "name", "wlan0"));

    //browse all vap
    struct conf_ct *ct;
    struct conf_tb *tb;
    if(conf_path2ct(l, "dev.wifi.vap", &ct)){
        fprintf(stderr, "get xml ct fail\n");
        goto out;
    }
    tb = ct->tb;
    printf("all vap infos:\n");
    while(tb){
        printf("tb[%s], ssid[%s], enable[%s], mode[%s], auth[%s]\n",
                conf_find_value(tb, "name"),
                conf_find_value(tb, "ssid"),
                conf_find_value(tb, "enable"),
                conf_find_value(tb, "mode"),
                conf_find_value(tb, "auth")
              );
        tb = tb->next;
    }
    //no need to free ct, tb.
    conf_list_free(l);
    l = NULL;
    kfree_response(resp);
    resp = NULL;


    if(strcmp(work_mode, "bridge-ap") == 0 || 
            strcmp(work_mode, "router-ap") == 0){
        printf("device running in AP mode, modify ssid now");
        char buf[1024] = {0};
        sprintf(buf, "<dev><wifi><vap>"
                "<ITEM name=\"wlan0\" ssid=\"%s\" />"
                "</vap></wifi></dev>", "hello-world");
        resp = kxml_apply(buf);
        if(!resp){
            fprintf(stderr, "apply xml setting failed!");
            goto out;
        }
        if(!(l = conf_str2list(resp->content, strlen(resp->content)))){
            fprintf(stderr, "prase result error\n");
            goto out;
        }
        char *result;
        result = conf_path2value(l, "return.result", NULL, NULL);
        if(strcmp(result, "ok")){
            printf("apply setting succeed, new config sequence:[%s]\n", conf_path2value(l, "return.config_sequence", NULL, NULL));
        } else {
            fprintf(stderr, "apply setting failed, errno[%s], errmsg[%s], config path[%s]\n",
                conf_path2value(l, "return.errno", NULL, NULL),
                conf_path2value(l, "return.err", NULL, NULL),
                conf_path2value(l, "return.path", NULL, NULL)
                );
        }
        conf_list_free(l);
        l = NULL;
        kfree_response(resp);
        resp = NULL;
    }


    wlanscan_demo();
out:
    kfree_response(resp);
    if(l){
        conf_list_free(l);
    }
    return 0;
}







