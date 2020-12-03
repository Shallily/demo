#include <stdio.h>
#include <memory.h>

static int _i32order = 0;

static void _print_strandblank(char *str)
{
    int i = 0;
    printf("\033[47;32m%-s\033[0m ",str);
    for(;i < 32 - strlen(str);i++)
        printf(" ");
}

static void _print_25byattr(char *str,char attr1)
{
    if(attr1)
        printf("\033[40;32m%-32s\033[0m ",str);
    else
        _print_strandblank(str);
}

static void _print_50byattr(char *str,char attr1)
{
    if(attr1)
        printf("\033[40;32m%-65s\033[0m ",str);
    else
        _print_strandblank(str);
}

static void _print_0byattr(char *str,char attr1)
{
    if(attr1)
        printf("\033[40;32m%-s\033[0m\n",str);
    else
        printf("\033[47;32m%-s\033[0m\n",str);
}

void print_byorder(char *str,char attr1)
{
    switch(_i32order % 3)
    {
        case 0:
            if(strlen(str) > 65)
            {
                _i32order = _i32order + 2;
                _print_0byattr(str,attr1);
                break;
            }
            else if(strlen(str) > 32)
            {
                _i32order = _i32order + 1;
                _print_50byattr(str,attr1);
                break;
            }
            else
            {
                _print_25byattr(str,attr1);
                break;
            }
        case 1:
            if(strlen(str) > 65)
            {
                printf("\n");
                _i32order = _i32order + 1;
                _print_0byattr(str,attr1);
                break;
            }
            else if(strlen(str) > 32)
            {
                _i32order = _i32order + 1;
                _print_0byattr(str,attr1);
                break;
            }
            else
            {
                _print_25byattr(str,attr1);
                break;
            }
            break;
        case 2:
            if(strlen(str) > 65)
            {
                printf("\n");
                _print_0byattr(str,attr1);
                break;
            }
            else if(strlen(str) > 34)
            {
                printf("\n");
                _i32order = _i32order + 2;
                _print_50byattr(str,attr1);
                break;
            }
            else
            {
                _print_0byattr(str,attr1);
                break;
            }
            break;
        default:
            break;
    }

    _i32order++;
}

void print_byorderstart(void)
{
    _i32order = 0;
}

