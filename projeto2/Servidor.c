#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdbool.h>
typedef struct {
	int tipo;
	char descricao[100];
	int pid_consulta;
} Consulta;

Consulta lista_consultas[10];

int count_one;
int count_two;
int count_three;
int count_missed;
int array_size = sizeof(lista_consultas)/sizeof(lista_consultas[0]);

void incrementa_tipo_consultas(int tipo){
	switch(tipo){
		case 1:
			count_one++;
			break;

		case 2:
			count_two++;
			break;

		case 3:
			count_three++;
			break;
	}
	printf("tipos de consulta: %d %d %d\n", count_one, count_two, count_three);
}

void inicializador_consultas(){
	count_one = 0;
	count_two = 0;
	count_three = 0;
	count_missed = 0;
	for (int i = 0; i < array_size; i++){
		lista_consultas[i].tipo = -1;
	}
}

int verifica_vaga(){
	for(int i = 0; i < array_size; i++){
		if(lista_consultas[i].tipo == -1){ //significa que ha um indice sem consulta
			return 1;
		}
	}
	return 0;
}

void wait();
void inserir_consultas(int tipo, char descricao[100], int pid_consulta){
	int sala;
	for(int i = 0; i < array_size; i++){
		if(lista_consultas[i].tipo == -1){
			lista_consultas[i].tipo = tipo;
			strcpy(lista_consultas[i].descricao, descricao);
			lista_consultas[i].pid_consulta = pid_consulta;			
			sala = i;   
			break;
		}
	}
	incrementa_tipo_consultas(tipo);
	printf("Consulta agendada para a sala %d\n", sala);
	if(fork() == 0){
		kill(pid_consulta, SIGHUP);
		sleep(10);
		printf("Consulta terminada na sala %d\n", sala);
		kill(pid_consulta, SIGTERM);
		exit(0);
	}else{
		wait(NULL);

		lista_consultas[sala].tipo = -1;
	}
}

void ler_consultas(){
	int tipo;
	char descricao[100];
	int pid_consulta;
	Consulta consulta;
	FILE *ft = fopen("PedidoConsulta.txt", "r");
	if(!ft){
		printf("Nao existe o ficheiro PedidoConsultas\n");
		exit(-1);
	}
	fscanf(ft, "%d %s %d", &tipo, &descricao, &pid_consulta);
	printf("Chegou novo pedido de consulta do tipo %d, descrição %s e PID %d\n", tipo, descricao, pid_consulta);
	int a = verifica_vaga(); //verifica se ha vagas
	if(a == 0){
		printf("Lista de consultas cheia\n");
		kill(pid_consulta, SIGUSR2);
		count_missed++;
		return;
	}
	if(a == 1){
		inserir_consultas(tipo, descricao, pid_consulta);
	}
}


void trata_SIGUSR1(int sig){
	ler_consultas();
}

void trata_SIGINT(int sig){
	printf("\n");
	remove("SrvConsultas.pid");
	int a = count_missed;
	int b = count_one;
	int c = count_two;
	int d = count_three;
	FILE *fi = fopen ("StatsConsultas.dat", "rb");
	if(fi){
		int arr1[4];
		fread(arr1, sizeof(arr1), 1, fi);
		a += arr1[0];
		b += arr1[1];
		c += arr1[2];
		d += arr1[3];
		fclose(fi);
	}
	int arr[4] = {a, b, c, d};
	FILE *ft = fopen ("StatsConsultas.dat", "wb+");
	fwrite(arr, sizeof(arr), 1, ft);
	fclose(ft);
	exit(0);
}

int main(){
	FILE * file = fopen ("SrvConsultas.pid", "w");
	fprintf(file, "%d\n", getpid());
	fclose(file);
	inicializador_consultas();
	while(1){
		signal(SIGUSR1, &trata_SIGUSR1);
		signal(SIGINT, &trata_SIGINT);
		pause();
	}

}
