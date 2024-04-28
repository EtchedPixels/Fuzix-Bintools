/*
 *	Do template substitutions on an input file.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

void badmarker(void)
{
    fputs("Missing % marker\n", stderr);
    exit(1);
}

void badsize(void)
{
    fputs("Substitution out of range\n", stderr);
    exit(1);
}

void badeof(void)
{
    fputs("Unexpected EOF\n", stderr);
    exit(1);
}

void badquote(char c)
{
    fputs("Unexpected quoting '", stderr);
    fputc(c, stderr);
    fputs("'\n", stderr);
    exit(1);
}

void process_sub(int argc, char *argv[])
{
    int n;
    char *p;
    int c;
    c = getchar();
    if (c == EOF)
        badeof();
    while(isdigit(c)) {
        n *= 10;
        n += (c - '0');
        c = getchar();
    }
    if (c != '%')
        badmarker();
    if (n < 1 || n >= argc)
        badsize();
    
    p = argv[n];
    while(*p) {
        if (*p != '\\')
            putchar(*p);
        else switch(*++p) {
        case '\\':
            putchar(*p);
            break;
        case 'n':
            putchar('\n');
            break;
        case 'r':
            putchar('\r');
            break;
        case 't':
            putchar('\t');
            break;
        default:
            badquote(*p);
        }
        p++;
    }
}

int main(int argc, char *argv[])
{
    int c;

    while((c = getchar()) != EOF) {
        if (c == '%')
            process_sub(argc, argv);
        else
            putchar(c);
    }
}
            