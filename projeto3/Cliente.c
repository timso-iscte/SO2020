#include<stdlib.h>
#include<string.h>
#include<sys/ipc.h>
#include<sys/types.h>
#include<sys/msg.h>
#include<sys/shm.h>
#include<sys/sem.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#define exit_on_error(s,m) if (s<0) { perror(m); exit(1);}
#define IPC_KEY 0x0a93911




typedef struct {
	int tipo;
	char descricao[100];
	int pid_consulta;
	int status;
} Consulta;

typedef struct {
	long tipo;
	Consulta conteudo;
} Mensagem;

Consulta consulta_new(int tipo_con, char desc_con[100]){
	Consulta consulta;
	consulta.tipo = tipo_con;
	strcpy(consulta.descricao, desc_con);
	consulta.pid_consulta = getpid();
	consulta.status = 1;
	return consulta;
}


void durante_consulta(){
	int id, status;
	Mensagem m;
	id = msgget(IPC_KEY, 0666 | IPC_CREAT);
	exit_on_error(id, "Erro no msgget.");
	status = msgrcv( id, &m, sizeof(m.conteudo), getpid(), 0);
	switch(m.conteudo.status){
		case 2:
			printf("Consulta iniciada para o processo %d\n", m.conteudo.pid_consulta);
			status = msgrcv( id, &m, sizeof(m.conteudo), getpid(), 0);
			if(m.conteudo.status == 3){
				printf("Consulta terminada para o pid %d\n", m.conteudo.pid_consulta);
				exit(0);
			} 
			break;
		case 3:
			printf("Não pode terminar uma consulta que não começou\n");
			exit(1);
			break;
		case 4:
			printf("Consulta não é possivel para o processo %d\n", m.conteudo.pid_consulta);
			exit(0);
				break;

	}

}
void trata_sigint(int sig){
	int id, status;
    id = msgget(IPC_KEY, 0666 | 0);
    exit_on_error(id, "Erro no msgget.");
	printf("Paciente cancelou o pedido\n");
	Mensagem m;
	m.tipo = getpid();
	m.conteudo.status = 5;
	status = msgsnd(id, &m, sizeof(m.conteudo), 0);
	exit(0);

}

int main(){
	int id, status;
	id = msgget(IPC_KEY, 0666 | IPC_CREAT);
	exit_on_error(id, "Erro no msgget.");
	int tipo_con;
	char desc_con[100];
	Consulta consulta;
	Mensagem m;
	while(1){
		printf("Digite o tipo de consulta:");
		scanf(" %d", &tipo_con);
		if(tipo_con < 1 || tipo_con > 3){
			printf("O tipo de consulta trm de ser 1, 2 ou 3.\n");
		}else break;
	}
	printf("Indique a especialidade da consulta:");
	scanf("%s", &desc_con);
	m.tipo = 1;
	consulta = consulta_new(tipo_con, desc_con);
	m.conteudo = consulta;
	status = msgsnd( id, &m, sizeof(m.conteudo), 0);
	durante_consulta();
	signal(SIGINT, trata_sigint);
}
