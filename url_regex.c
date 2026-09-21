#include <regex.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define REG_NUM 3
#define REG_STR  "<a[[:space:]]+[^>]*href=\"([^\"]+)\"[^>]*>[[:space:]]*([^<]+)</a>"

static void trim(char *s)
{
    size_t len = strlen(s);
    while (len > 0 && (s[len - 1] == ' '  || s[len - 1] == '\t' ||
                       s[len - 1] == '\r' || s[len - 1] == '\n'))
        s[--len] = '\0';
    char *head = s;
    while (*head == ' ' || *head == '\t' || *head == '\r' || *head == '\n')
        head++;
    if (head != s)
        memmove(s, head, strlen(head) + 1);
}

int main(int argc, char *argv[])
{
    const char *filename = (argc > 1) ? argv[1] : "url.html";
    int fd = open(filename, O_RDWR);
    if (fd < 0) {
        perror("open");
        return 1;
    }
    int size = lseek(fd, 0, SEEK_END);
    if (size <= 0) {
        fprintf(stderr, "文件为空或读取失败\n");
        close(fd);
        return 1;
    }
    char *mmap_ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    if (mmap_ptr == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return 1;
    }
    close(fd);
    regex_t reg;
    const char *regStr = REG_STR;
    int ret = regcomp(&reg, regStr, REG_EXTENDED);
    if (ret != 0) {
        char err_buffer[256];
        regerror(ret, &reg, err_buffer, sizeof(err_buffer));
        fprintf(stderr, "regcomp 错误: %s\n", err_buffer);
        munmap(mmap_ptr, size);
        return 1;
    }
    regmatch_t match[REG_NUM];
    char *p     = mmap_ptr;
    char *pend  = mmap_ptr + size;
    int   count = 0;
    printf("============================================================\n");
    printf(" 从 %s 中提取超链接（正则: %s）\n", filename, REG_STR);
    printf("============================================================\n");
    while (p < pend) {
        if (regexec(&reg, p, REG_NUM, match, 0) == REG_NOMATCH)
            break;
        int url_len   = match[1].rm_eo - match[1].rm_so;
        int title_len = match[2].rm_eo - match[2].rm_so;
        char url[1024]   = {0};
        char title[1024] = {0};
        if (url_len   > 0 && (size_t)url_len   < sizeof(url))
            memcpy(url,   p + match[1].rm_so, url_len);
        if (title_len > 0 && (size_t)title_len < sizeof(title))
            memcpy(title, p + match[2].rm_so, title_len);
        trim(title);
        printf("[%03d] 标题: %s\n      地址: %s\n", ++count, title, url);
        p += match[0].rm_eo;
    }
    printf("============================================================\n");
    printf(" 共提取超链接 %d 条\n", count);
    regfree(&reg);
    munmap(mmap_ptr, size);
    return 0;
}
