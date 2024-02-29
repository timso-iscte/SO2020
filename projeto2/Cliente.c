#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>

typedef struct {
	int tipo;
	char descricao[100];
	int pid_consulta;
} Consulta;


Consulta consulta_new(int tipo_con, char desc_con[100]){
	Consulta a;
	a.tipo = tipo_con;
	strcpy(a.descricao, desc_con);
	a.pid_consulta = getpid();
	return a;

}

void escreve_consulta(Consulta consulta){
	FILE * ft = fopen ("PedidoConsulta.txt", "w");
	fprintf(ft, "%d %s %d\n", consulta.tipo, consulta.descricao, consulta.pid_consulta);
	fclose(ft);

}


int ler_pidsrv(){
	int pidsrv;
	FILE *ft = fopen("SrvConsultas.pid", "r");
	if(!ft){
		printf("O servidor não iniciou\n");
		exit(-1);
	}
	fscanf(ft, "%d", &pidsrv);
	return pidsrv;
}

void trata_SIGTERM(){
	printf("Consulta terminada com sucesso!\n");
	exit(0);
}

void trata_SIGINT(int sig){
	printf("\nPaciente cancelou o pedido\n");
	if(fork() == 0){
		//execl("bin/rm/", "-f", "PedidoConsulta.txt");
		remove("PedidoConsulta.txt");
		exit(0);
	}
	exit(0);
}


void wait();
void inicio_consulta(){
	printf("Consulta iniciada para o processo %d\n", getpid());
	if(fork() == 0){
		//	execl("bin/rm/", "-f", "PedidoConsulta.txt");
		remove("PedidoConsulta.txt");
		exit(0);
	}
	wait(NULL);
	signal(SIGINT, &trata_SIGINT);
	signal(SIGTERM, &trata_SIGTERM);
	pause();
}

void trata_SIGHUP(int sig){
	inicio_consulta();
}

void trata_SIGUSR2(int sig){
	printf("Consulta não é possivel para o processo %d\n", getpid());
	if(fork() == 0){
		//execl("bin/rm/", "-f", "PedidoConsulta.txt");
		remove("PedidoConsulta.txt");
		exit(0);
	}
	wait(NULL);
	exit(0);
}

int main(){
	int tipo_con;
	int pidsrv;
	char desc_con[100];
	pidsrv = ler_pidsrv();
	Consulta consulta;
	int a = 0;
	while(a == 0){
		printf("Digite o tipo de consulta:");
		scanf("%d", &tipo_con);
		if(tipo_con < 1 || tipo_con > 3){
			printf("O tipo de consulta tem de ser 1, 2 ou 3.\n");
		}else a = 1;
	}
	printf("Indique a especialidade da consulta:");
	scanf("%s", &desc_con);
	consulta = consulta_new(tipo_con, desc_con);
	escreve_consulta(consulta);
	kill(pidsrv, SIGUSR1);
	signal(SIGHUP, &trata_SIGHUP);
	signal(SIGUSR2, &trata_SIGUSR2);
	pause();
}
