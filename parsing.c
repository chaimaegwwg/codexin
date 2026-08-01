#include <string.h>
#include <stdio.h>
# include <unistd.h>
#include <limits.h>
#include "header.h"


// typedef struct s_scheduler
// {
//     int number_of_coders;
//     long time_to_burnout;
//     long time_to_compile;
//     long time_to_debug;
//     long time_to_refactor;
//     int number_of_compiles_required;
// } t_scheduler;


long ft_atoi(const char *str)
{
    int sign;
    int i;
    long result;
    result = 0;
    i = 0;
    sign = 1;
    while((str[i] >= 9 && str[i] <= 13) || str[i] == 32)
        i++;
    if (str[i] == '+' || str[i] == '-')
	{
		if (str[i] == '-')
		{
			sign = -1;
		}
		i++;
	}
    while (str[i])
	{
        if (!(str[i] >= '0' && str[i] <= '9'))
            return -1;
        result = result * 10 + (str[i] - '0');
        i++;
    }
    return (sign * result);
}
    
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

void ft_data(t_scheduler *data,char **arg)
{
    // printf("it go here\n");
    data->number_of_coders = ft_atoi(arg[1]);
    data->time_to_burnout = ft_atoi(arg[2]);
    data->time_to_compile = ft_atoi(arg[3]);
    data->time_to_debug = ft_atoi(arg[4]);
    data->time_to_refactor = ft_atoi(arg[5]);
    data->number_of_compiles_required = ft_atoi(arg[6]);
    data->dongle_cooldown = ft_atoi(arg[7]);
    // printf("it go here3\n");
    // if (strcmp(arg[7], "edf") == 0)
    // {
    //     data->is_edf = 1;
    // }
    // else
    // {
    //     data->is_edf = 0;
    // }
    

}
int ft_check_parameter(char *arg)
{
    if (arg == NULL)
        return (0);
    
    if (strcmp(arg, "fifo") == 0)
    {
        return (0);
    }
    else if (strcmp(arg, "edf") == 0)
    {
        return (0);
    }
    else
    {
        return (1);
    }
}
int main(int arg, char **argc)
{
    
    if ((arg < 2) || (arg > 8))
    {
        printf("Please enter the argument");
        return(1);
    }
    int i = 1, j = 0;
    int boolin, num;
    char *args;
    while(i < arg)
    {
        args = strtok(argc[i], " ");
        if (!args)
            return 1;
        j = 0;
        // printf("i :%d,j:%d\n",i,j);
        while(args[j])
        {
            
            boolin = syntax_check(&args[j]);
            if (boolin == 0)
            {
                write(2, "Error\n", 6);
                return 1;
            }
            num = ft_atoi(&args[j]);
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
        // if (ft_check_parameter(argc[7]) != 0)
        // {
        //     write(2, "Error\n", 6);
        //     return (1);
        // }
        i++;
    }
    // printf("it go here\n");
    t_scheduler data;
    ft_data(&data,argc);
    // printf("4it go here\n");
    main_thread(&data);
    return 0;
}    