#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
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
}Mensagem;


typedef struct {               //para meter na shm
	Consulta lista[10];
	int count_1;
	int count_2;
	int count_3;
	int count_m;

}Dados;

struct sembuf UP = {
	.sem_num = 0,
	.sem_op = +1,
	.sem_flg = 0
};

struct sembuf DOWN = {
	.sem_num = 0,
	.sem_op = -1,
	.sem_flg = 0
};


Consulta lista_consultas[10];
int count_one;
int count_two;
int count_three;
int count_missed;
int array_size = sizeof(lista_consultas)/sizeof(lista_consultas[0]);
int size = sizeof(lista_consultas[10]) + 4 * sizeof(int);

void cria_memoria(){
	int shmid; 
	Dados dados;

	shmid = shmget(IPC_KEY, sizeof(dados), 0666 | IPC_CREAT | IPC_EXCL);
	if(shmid < 0){

	}else {
		Dados *shm =(Dados *) shmat(shmid, NULL, 0);
		for(int i = 0; i != array_size; i++){
			(*shm).lista[i].tipo = -1;
			strcpy((*shm).lista[i].descricao, " ");
			(*shm).lista[i].pid_consulta = 0;
			(*shm).lista[i].status = 0;
		}
		(*shm).count_1 = 0;
		(*shm).count_2 = 0;
		(*shm).count_3 = 0;
		(*shm).count_m = 0;
	}
	printf("Inicei a memoria\n");
}

bool verifica_vaga(){
	int shmid;
    shmid = shmget(IPC_KEY, sizeof(Dados), 0666 | 0);
    Dados *shm = (Dados *) shmat(shmid, NULL, 0);
    exit_on_error(shmid, "erro ao ligar à shm");
	for(int i = 0; i != size; i++){
		if((*shm).lista[i].tipo == -1){
			return true	;
		}
	}
	return false;
}

void incrementa_tipo_con(int tipo){
	int shmid;
	shmid = shmget(IPC_KEY, sizeof(Dados), 0666 | 0);
	Dados *shm = (Dados *) shmat(shmid, NULL, 0);
	exit_on_error(shmid, "erro ao ligar à shm");
	switch(tipo){
		case 1:
			(*shm).count_1++;
			break;
		case 2:
			(*shm).count_2++;
			break;
		case 3:
			(*shm).count_3++;
			break;
	}
}

void servidor_dedicado(Consulta consulta){
	int status, id, semid, semdown, semup;
	Mensagem m;
	id = msgget(IPC_KEY, 0666 | 0);
	exit_on_error(id, "erro ao ligar à msgqueue");
	int shmid;
	Dados dados;
	shmid = shmget(IPC_KEY, sizeof(dados), 0666 | 0);
	exit_on_error(shmid, "erro ao ligar à shm");
	Dados *shm =(Dados *) shmat(shmid, NULL, 0);
	if(fork() == 0){
		semid = semget(IPC_KEY, 1, 0666 | 0);
		exit_on_error(semid, "erro ao ligar ao semaforo");
		int semdown = semop(semid, &DOWN, 1);

		m.tipo = consulta.pid_consulta;
		if(!verifica_vaga((*shm))){
			consulta.status = 4;
			m.conteudo = consulta;
			(*shm).count_m++;
			status = msgsnd(id, &m, sizeof(m.conteudo), 0);
			exit_on_error(status, "erro ao enviar");
			printf("A lista está cheia\n");

		}else{
			int sala;
			for(int i = 0; i < size; i++){
				if((*shm).lista[i].tipo == -1){    
					(*shm).lista[i] = consulta;
					sala = i;
					break;
				}
			}
			printf("Consulta agendada para a sala %d\n", sala);
			printf("pedido: %d, descricão: %s e pid: %d\n", consulta.tipo, consulta.descricao, consulta.pid_consulta);
			incrementa_tipo_con(consulta.tipo);
			(*shm).lista[sala].status = 2;
			m.conteudo = (*shm).lista[sala];
			status = msgsnd(id, &m, sizeof(m.conteudo), 0);
			exit_on_error(status, "erro ao enviar");
			sleep(10);
			(*shm).lista[sala].status = 3;
			m.conteudo = (*shm).lista[sala];
			printf("Consulta terminada para a sala %d\n", sala);
			(*shm).lista[sala].tipo = -1;
			status = msgsnd(id, &m, sizeof(m.conteudo), 0);
			exit_on_error(status, "erro ao enviar");

		}
		int semup = semop(semid, &UP, 1);
		exit(0);
	}

}

void trata_sigint(int sig){
	int shmid;
	shmid = shmget(IPC_KEY, sizeof(Dados), 0666 | 0);
	Dados *shm = (Dados *) shmat(shmid, NULL, 0);
	printf("Consultas normais: %d\n Consultas COVID19: %d\n Consultas urgentes: %d\n Consultas perdidas: %d\n", (*shm).count_1, (*shm).count_2, (*shm).count_3, (*shm).count_m);
	exit(0);
}

int main(){
	int id, status, semid, semstatus;
	id = msgget(IPC_KEY, 0666 | IPC_CREAT);
	exit_on_error(id, "erro ao criar msgqueue");
	semid = semget(IPC_KEY, 1, 0666 | IPC_CREAT | IPC_EXCL);
	if(semid < 0){
		semid = semget(IPC_KEY, 1, 0666 | IPC_CREAT);
		exit_on_error(semstatus, "erro ao meter o semaforo com valor 1");
		semstatus = semctl(semid, 0, SETVAL, 1);
	}else
		semstatus = semctl(semid, 0, SETVAL, 1);
	cria_memoria();
	while(1){
		Mensagem m;
		status = msgrcv(id, &m, sizeof(m.conteudo), 1, 0);
		exit_on_error(status, "erro ao receber");
		printf("Chegou um pedido de consulta: %d, descricão: %s e pid: %d\n", m.conteudo.tipo, m.conteudo.descricao, m.conteudo.pid_consulta);
		servidor_dedicado(m.conteudo);
		signal(SIGINT, trata_sigint);
	}
}
