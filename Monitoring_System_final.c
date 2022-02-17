#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define BUFSIZE 256
#define MAX 100

typedef struct {
    signed long user,
                nice,
                sys,
                idle,
                iowait,
                irq,
                softirq;
}cpu_t;

void view_processes();
void view_duration();
void view_utilization();
void terminate_process();
void view_users();
void logout_user();
void parse(char *, char **);
void execute(char **);
void *load_check(void *);
cpu_t* cpu_check(cpu_t *);
static void print_cpu(cpu_t*, cpu_t*);

int main()
{
	pthread_t tid1;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	int temp=0, n;
	pthread_create(&tid1, &attr, load_check, (void *)temp);

	int shmid1;
    	key_t key1;
    	char *shm1, *s1;
    	key1 = 5000;

    	if ((shmid1 = shmget(key1, MAX, IPC_CREAT | 0666)) < 0) 
	{
        	perror("shmget");
        	exit(1);
    	}
    	if ((shm1 = shmat(shmid1, NULL, 0)) == (char *) -1) 
	{
        	perror("shmat");
        	exit(1);
    	}

	char *str1 = "exit";
	char *str2;

	while(1)
	{
		if(*shm1=='^')
		{
			*shm1='@';
			while(*shm1=='@')
				sleep(1);				
			while(1)
			{				
				str2=shm1;					
				if((strncmp(str1,str2,4))==0)
				{
					break;
				}					
				for (s1 = shm1; *s1 != NULL; s1++)
        				putchar(*s1);
    				putchar('\n');
    				*shm1 = '*';
				while(*shm1=='*')
					sleep(1);				
			}
			*shm1='$';
		}				
		printf("Select an option\n1. View all running processes\n2. Duration from which a process is running\n3. CPU and memory utilization of all processes\n4. Terminate a process\n5. View all logged in users\n6. Logout an user\n7. Send message to all users\n8. Exit\n");
		scanf("%d", &n);
		switch(n)
		{
			case 1:
				view_processes();
				break;
			case 2:
				view_duration();
				break;
			case 3:
				view_utilization();
				break;		
			case 4:
				terminate_process();
				break;
			case 5:
				view_users();
				break;
			case 6:
				logout_user();				
				break;
			case 7:
				*shm1='^';
				while(*shm1 == '^')
					sleep(1);				
				while(*shm1 != '$')
				{
					s1 = shm1;
    					printf("Enter the message you want to send to all users\n");
					gets(s1);
    					while (*shm1 != '*' && *shm1 != '$')
        					sleep(1);
				}
				break;
			case 8:				
				return;				
				break;
		}
	}
	pthread_join(tid1,NULL);
}

void view_processes()
{
    	pid_t pid;
    	pid=fork();

    	if(pid<0)
    	{
        	printf("Error\n");
        	return 0;
    	}
    	else if(pid>0)
    	{
        	wait(NULL);
    	}
    	else
    	{
		execl("/bin/ps", "ps", "-few", (char *)NULL);
    	}
}

void view_duration()
{
	pid_t pid;
    	pid=fork();
	
    	if(pid<0)
    	{
        	printf("Error\n");
        	return 0;
    	}
    	else if(pid>0)
    	{
		wait(NULL);
    	}
    	else
    	{
		execl("/bin/ps", "ps", "-few", (char *)NULL);
    	}
}

void view_utilization()
{
	pid_t pid;
    	pid=fork();
	
    	if(pid<0)
    	{
        	printf("Error\n");
        	return 0;
    	}
    	else if(pid>0)
    	{
        	wait(NULL);
    	}
    	else
    	{
		execl("/bin/ps", "ps", "-aux", (char *)NULL);
    	}
}

void terminate_process()
{
	pid_t pid1, pid2;
    	pid1=fork();
	
    	if(pid1<0)
    	{
        	printf("Error\n");
        	return 0;
    	}
    	else if(pid1>0)
    	{
        	wait(NULL);
		pid2=fork();
		
    		if(pid2<0)
    		{
        		printf("Error\n");
        		return 0;
    		}
		else if(pid2>0)
		{
			wait(NULL);
		}
		else
		{
			char line[1024]="kill -9 ";
     			char *argv[64];
     			char id[10];              
     			int i=0, j=0;              
        		printf("Enter process id to kill:\n");
			scanf("%s", id);            
			i=8;
			for(j=0;id[j]!='\0';j++)
			{
				line[i]=id[j];
				i++;
			}
			parse(line, argv);       
        		if (strcmp(argv[0], "exit") == 0)
			{  
               			exit(0);
			}        
			execute(argv);
			printf("Process killed successfully\n");
		}
    	}
    	else
    	{
		execl("/bin/ps", "ps", "-few", (char *)NULL);
    	}
}

void view_users()
{
	pid_t pid;
    	pid=fork();
	
    	if(pid<0)
    	{
        	printf("Error\n");
        	return 0;
    	}
    	else if(pid>0)
    	{
        	wait(NULL);
	}
    	else
    	{
		execlp("w", "w", (char *)NULL);
    	}
}

void logout_user()
{
	pid_t pid;
    	pid=fork();
	
    	if(pid<0)
    	{
        	printf("Error\n");
        	return 0;
    	}
    	else if(pid>0)
    	{
		wait(NULL);
    	}
    	else
    	{
		char *username;
		char root[5]="root";
		username=(char *)malloc(20*sizeof(char));
		cuserid(username);
		printf("%s\n", username);
		if(strcmp(username,root)==0)
		{
			char line[1024]="pkill -KILL -u ";
     			char *argv[64];
     			char kill_user[20];              
     			int i=0, j=0;              
        		printf("Enter username:\n");
			scanf("%s", kill_user);
			i=15;
			for(j=0;kill_user[j]!='\0';j++)
			{
				line[i]=kill_user[j];
				i++;
			}
			parse(line, argv);       
        		if (strcmp(argv[0], "exit") == 0)
			{  
               			exit(0);
			}        
			execute(argv);
		}		
		else
		{
			execlp("gnome-session-quit", "gnome-session-quit", (char *)NULL); 
		}		
    	}
}

void parse(char *line, char **argv)
{
     	while (*line != '\0') 
	{
          	while (*line == ' ' || *line == '\t' || *line == '\n')
		{
               		*line++ = '\0';
		}
          	*argv++ = line;
          	while (*line != '\0' && *line != ' ' && *line != '\t' && *line != '\n') 
		{               
			line++;
     		}
	}
     	*argv = '\0';
}

void execute(char **argv)
{
     	pid_t pid;
     	int status;

     	if ((pid = fork()) < 0) 
	{
        	printf("Error\n");
          	exit(1);
     	}
     	else if (pid == 0) 
	{
          	if (execvp(*argv, argv) < 0) 
		{
               		printf("Error: exec failed\n");
               		exit(1);
      		}
     	}
     	else 
	{
          	while (wait(&status) != pid);
     	}
}

void *load_check(void *param)
{	
	//int n=(int)param;
	cpu_t cpu1, cpu2;
    	while(1) 
	{
    		cpu_t *p_cpu1 = cpu_check(&cpu1);
        	sleep(1);
        	cpu_t *p_cpu2 = cpu_check(&cpu2);
        	print_cpu(p_cpu1, p_cpu2);
    	}
	pthread_exit(0);
}

cpu_t* cpu_check(cpu_t *cpus)
{
    	if (!cpus) 
	{
        	return NULL;
    	}
    	int fd = open("/proc/stat", O_RDONLY);
    	if (fd < 0) 
	{
        	perror("Failed to open /proc/stat");
        	return NULL;
    	}
    	char buf[BUFSIZE];
    	read(fd, buf, BUFSIZE);
    	close(fd);

    	sscanf(buf, "cpu %ld %ld %ld %ld %ld %ld %ld", &cpus->user, &cpus->nice, &cpus->sys ,&cpus->idle, &cpus->iowait, &cpus->irq, &cpus->softirq);
    	return cpus;
}

static void print_cpu(cpu_t* cpu1, cpu_t* cpu2)
{
    	if (!cpu1 || !cpu2) 
	{
        	return;
    	}
    	signed long jiff_0 = cpu1->user + cpu1->sys + cpu1->nice + cpu1->idle + cpu1->iowait + cpu1->irq + cpu1->softirq;
    	signed long jiff_1 = cpu2->user + cpu2->sys + cpu2->nice + cpu2->idle + cpu2->iowait + cpu2->irq + cpu2->softirq;
    	float sys_idle = 100 * (float)(cpu2->idle - cpu1->idle) / (jiff_1 - jiff_0);
    	float sys_usage = 100 - sys_idle;
    	char c[10];
    	sprintf(c,"%f",sys_usage);
    	setenv("y", c, 1);
    	system("echo \ $y \ > /dev/pts/2");
	if(sys_usage > 10)
	{
		printf("Caution: Load is above normal level!!\n");
		int res = system("java cpu_time");	
	}
}

