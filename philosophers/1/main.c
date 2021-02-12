#include "philo_one.h"

int		ft_atoi(char *str)
{
	int		i;
	int		ret;
	int		sign;

	sign = 1;
	ret = 0;
	i = -1;
	if (str[i+1] == '-' && i++)
		sign = -1;
	while (str[++i])
	{
		if (str[i] < '0' || '9' < str[i])
			return (-1);
		ret = ret * 10 + (str[i] - '0');
	}
	return (sign * ret);
}

int		parse(char **argv)
{
	if ((table.num_philo = ft_atoi(argv[1])) <= 1 ||
		(table.time_to_die = ft_atoi(argv[2])) <= 0 ||
		(table.time_to_eat = ft_atoi(argv[3])) <= 0 ||
		(table.time_to_sleep = ft_atoi(argv[4])) <= 0)
		return (1);
	if (argv[5] && (table.num_eat = ft_atoi(argv[5])) <= 0)
		return (1);
	else if (!argv[5])
		table.num_eat = -1;
	return (0);
}

unsigned long	get_time(void)
{
	struct timeval		tv;

	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

void	init_table(void)
{
	int		i;

	i = -1;
	while (++i < table.num_philo)
	{
		pthread_mutex_init(&table.fork[i], NULL);
		philos[i].nbr = i+1;
		philos[i].eat = 0;
		philos[i].fork1 = i-1;
		if (!i)
			philos[i].fork1 = table.num_philo - 1;
		philos[i].fork2 = i;
	}
	pthread_mutex_init(&table.m_msg, NULL);
	table.eat = 0;
	table.dead = 0;
	table.base_time = get_time();
}

void		less_error_sleep(unsigned long input)
{
	unsigned long	base;
	unsigned long	cur;

	base = get_time();
	while (1)
	{
		cur = get_time();
		if (input < cur - base)
			return ;
		usleep(100);
	}
}

int		msg(t_philo *philo, int msg, unsigned long cur)
{
	pthread_mutex_lock(&table.m_msg);
	if (table.dead)
	{
		pthread_mutex_unlock(&table.m_msg);
		return (1);
	}
	printf("%lu %d", cur - table.base_time, philo->nbr);
	if (msg == TAKEN_FORK)
		printf(" has taken a fork\n");
	else if (msg == EATING)
	{
		printf(" is eating\n");
		philo->last_eat = cur;
	}
	else if (msg == SLEEPING)
		printf(" is sleeping\n");
	else if(msg == THINKING)
		printf(" is thinking\n");
	else if (msg == DEAD)
	{
		printf(" died\n");
		table.dead = 1;
	}
	pthread_mutex_unlock(&table.m_msg);
	return (0);
}

void	*philo_monitor(void *phi)
{
	t_philo			*philo;
	unsigned long	cur;

	philo = (t_philo *)phi;
	while (1)
	{
		if (philo->eat == table.num_eat)
			break ;
		cur = get_time();
		if (table.time_to_die < cur - philo->last_eat)
		{
			msg(philo, DEAD, cur);
			return (NULL);
		}
		less_error_sleep(1);
	}
	return (NULL);
}

int		eat(t_philo *philo)
{
	pthread_mutex_lock(&table.fork[philo->fork1]);
	msg(philo, TAKEN_FORK, get_time());
	pthread_mutex_lock(&table.fork[philo->fork2]);
	msg(philo, TAKEN_FORK, get_time());
	msg(philo, EATING, get_time());
	less_error_sleep(table.time_to_eat);
	pthread_mutex_unlock(&table.fork[philo->fork1]);
	pthread_mutex_unlock(&table.fork[philo->fork2]);
	philo->eat++;
	if (philo->eat == table.num_eat)
	{
		table.eat++;
		return (1);
	}
	return (0);
}

void	*philo_act(void *phi)
{
	t_philo		*philo;
	pthread_t	tid;

	philo = (t_philo *)phi;
	if (!(philo->nbr % 2))
		less_error_sleep(1);
	pthread_create(&tid, NULL, philo_monitor, philo);
	while (1)
	{
		if (eat(philo))
			break ;
		if (msg(philo, SLEEPING, get_time()))
			break ;
		less_error_sleep(table.time_to_sleep);
		if (msg(philo, THINKING, get_time()))
			break ;
	}
	pthread_join(tid, NULL);
	return (NULL);
}

void	init_philos(void)
{
	int		i;

	i = -1;
	while (++i < table.num_philo)
	{
		philos[i].last_eat = get_time();
		pthread_create(&philos[i].tid, NULL, philo_act, &philos[i]);
	}
	i = -1;
	while (++i < table.num_philo)
		pthread_join(philos[i].tid, NULL);
}

void	clean_table(void)
{
	int		i;

	i = -1;
	while (++i < table.num_philo)
		pthread_mutex_destroy(&table.fork[i]);
	pthread_mutex_destroy(&table.m_msg);
	free(table.fork);
	free(philos);
}

int		main(int argc, char **argv)
{
	if (!(argc == 5 || argc == 6) || parse(argv))
		return (printf("argruments error!"));
	if (!(philos = malloc(sizeof(t_philo) * table.num_philo)))
		return (printf("philos malloc error!"));
	if (!(table.fork = malloc(sizeof(pthread_mutex_t) * table.num_philo)))
		return (printf("fork malloc error!"));
	init_table();
	init_philos();
	clean_table();
	return (0);
}