/*=====================================================================
 * 作业：使用 regex 正则技术提取 url.html 中的超链接地址和链接标题
 *
 * 数据源  ：url.html（磁盘文件，通过 mmap 内存映射加载到进程）
 * 正则函数：regcomp / regexec / regfree  （#include <regex.h>）
 * 正则语句：<a[[:space:]]+[^>]*href="([^"]+)"[^>]*>[[:space:]]*([^<]+)</a>
 *           规则表达式：<a ... > ... </a>          （父表达式）
 *           关键表达式：([^"]+)   -> 超链接地址     （子表达式 1）
 *                      ([^<]+)   -> 链接标题       （子表达式 2）
 *           括号数量 n = 2，正则总数 = n + 1 = 3（含整体匹配 0 号）
 *
 * 编译：gcc url_regex.c -o url_regex
 * 运行：./url_regex url.html
 *===================================================================*/
#include <regex.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define REG_NUM 3                 /* 正则数量 = 括号数 2 + 1          */
#define REG_STR  "<a[[:space:]]+[^>]*href=\"([^\"]+)\"[^>]*>[[:space:]]*([^<]+)</a>"

/* 去掉标题首尾的空白字符（空格、\t、\r、\n） */
static void trim(char *s)
{
    size_t len = strlen(s);
    while (len > 0 && (s[len - 1] == ' '  || s[len - 1] == '\t' ||
                       s[len - 1] == '\r' || s[len - 1] == '\n'))
        s[--len] = '\0';                      /* 去尾部空白 */
    char *head = s;
    while (*head == ' ' || *head == '\t' || *head == '\r' || *head == '\n')
        head++;                               /* 去头部空白 */
    if (head != s)
        memmove(s, head, strlen(head) + 1);
}

int main(int argc, char *argv[])
{
    const char *filename = (argc > 1) ? argv[1] : "url.html";

    /*---------------- 1. mmap 将数据源（磁盘文件）映射到进程内存 ----------------*/
    int fd = open(filename, O_RDWR);
    if (fd < 0) {
        perror("open");
        return 1;
    }
    int size = lseek(fd, 0, SEEK_END);        /* 返回文件大小 */
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
    close(fd);                                /* 映射完成后即可关闭文件描述符 */

    /*---------------- 2. regcomp 将正则语句字符串转换为正则类型 ----------------*/
    regex_t reg;
    const char *regStr = REG_STR;             /* 匹配超链接标签的表达式 */
    int ret = regcomp(&reg, regStr, REG_EXTENDED);
    if (ret != 0) {                           /* 表达式语法错误，用 regerror 检查 */
        char err_buffer[256];
        regerror(ret, &reg, err_buffer, sizeof(err_buffer));
        fprintf(stderr, "regcomp 错误: %s\n", err_buffer);
        munmap(mmap_ptr, size);
        return 1;
    }

    /*---------------- 3. regexec 循环匹配，提取地址和标题并打印 ----------------*/
    regmatch_t match[REG_NUM];                /* 传出位置数组，长度取决于正则数量 */
    char *p     = mmap_ptr;                   /* 匹配起始位置，逐条向后推进 */
    char *pend  = mmap_ptr + size;            /* 数据末尾 */
    int   count = 0;

    printf("============================================================\n");
    printf(" 从 %s 中提取超链接（正则: %s）\n", filename, REG_STR);
    printf("============================================================\n");

    while (p < pend) {
        /* regexec 调用一次匹配一条结果，成功返回 0，失败返回 REG_NOMATCH */
        if (regexec(&reg, p, REG_NUM, match, 0) == REG_NOMATCH)
            break;                            /* 后面再也匹配不到，结束 */

        /* match[0] 整体匹配, match[1] 超链接地址, match[2] 链接标题 */
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

        p += match[0].rm_eo;                  /* 从本次匹配结束处继续向后匹配 */
    }

    printf("============================================================\n");
    printf(" 共提取超链接 %d 条\n", count);

    /*---------------- 4. regfree 释放正则类型，munmap 解除映射 ----------------*/
    regfree(&reg);
    munmap(mmap_ptr, size);
    return 0;
}
