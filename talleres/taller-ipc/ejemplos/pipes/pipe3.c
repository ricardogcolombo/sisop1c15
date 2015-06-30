/* Mi ejemplo de pipes*/

#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>



int main()
{
	printf("Cantidad de procesos: \n");
	int cantidad_process;
	scanf("%d" , &cantidad_process);
	
	char comandos[cantidad_process][1025];
	char argumentos[cantidad_process][1025];
	for(int i = 0; i < cantidad_process ; i++){
		printf("Proceso: %d \n", i);
		scanf("%s", comandos[i]);

		printf("argumento: %d \n", i);
		scanf("%s", argumentos[i]);
	}
	
	int pipes[cantidad_process][2];

	for(int i = 0; i < cantidad_process; i++)
		if(pipe(pipes[i]) < 0)
			exit(EXIT_FAILURE);
	int mi_num = -1;
	int status;
	for(int i = 0; i < cantidad_process; i++)
		{	
			pid_t pid = fork();
			if(pid== 0){
				/*Cierro pipes*/
				for(int j = 0; j < cantidad_process; j++){
					if(j!=i)
						close(pipes[j][0]);
				}
				for(int j = 0; j < cantidad_process; j++){
					if(i+1 != j)
						close(pipes[j][1]);
				}
				/*Cierro pipes*/
				if(i != 0)
					dup2(pipes[i][1],0);
				if(i != cantidad_process -1)
					dup2(pipes[i+1][0],1);
				
				execvp(comandos[i], argumentos[i]);
				printf("LIO \n");
			}else{
				waitpid(pid,&status);
		}
	}
	printf("TERMINE! \n" );
}