#include "header.h"

    
int syntax_check(const char* str)
{
    int i;
    if ((!str) || (str[0] == '\0'))
    return 0;
    i = 0;
    if (str[i + 1] != '\0' && (str[i] == '-' || str[i] == '+'))
    i++;
    while (str[i] != '\0')
    {
        if (!(str[i] >= '0' && str[i] <= '9'))
        return (0);
    i++;
}
return (1);

}
char *ft_lower(char *str)
{
    int i;
    i = 0;
    while (str[i])
    {
        if (str[i] >= 'A' && str[i] <= 'Z')
        {
            str[i] = str[i] + 32;
        }
        i++;
    }
    return str;
    
}
void ft_data(t_scheduler *data,char **arg)
{
    char *str;
    data->number_of_coders = atoi(arg[1]);
    data->time_to_burnout = atoi(arg[2]);
    data->time_to_compile = atoi(arg[3]);
    data->time_to_debug = atoi(arg[4]);
    data->time_to_refactor = atoi(arg[5]);
    data->number_of_compiles_required = atoi(arg[6]);
    data->dongle_cooldown = atoi(arg[7]);
    str = ft_lower(arg[8]);
    if (strcmp(str, "edf") == 0)
    {
        data->is_edf = 1;
    }
    else
    {
        data->is_edf = 0;
    }
    

}

int main(int arg, char **argc)
{
    
    
    if ((arg != 9))
    {
        printf("Please enter the argument");
        return(1);
    }
    int j = 1;
    long num;
    int boolin;

    while(j < 7)
    {
        
        boolin = syntax_check(argc[j]);
        if (boolin == 0)
        {
            printf("Erorr u\n");
            return 1;
        }
        num = atoi(argc[j]);
        if(num < 0)
        {
            printf("Error: Number must be int\n");
            return 1;

        }
        if (num > INT_MAX || num < INT_MIN)
        {
            
            printf("Number out of range\n");
            return 1;
        }
        j++;
    }
   
    
    t_scheduler data;
    ft_data(&data,argc);
    // printf("4it go here\n");
    main_thread(&data);
    return 0;
}    
// if there is n arg 7