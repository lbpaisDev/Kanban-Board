#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>
#include <unistd.h>
#include <assert.h>
#include <D:/CORE/KANBAN/header.h>
#define MAXLINESIZE 1024
int TASKMAXIMUM=5;
int getInt()																											{
	int n = 0;
	char buffer[128];
	fgets(buffer,sizeof(buffer),stdin);
	n = atoi(buffer); 
	return ( n > 23 || n < 1 ) ? 0 : n;
}
int getID()																												{
	char tmp[1024];
	FILE*fp = fopen("id.txt", "r");
	while(!feof(fp))
    	fgets(tmp, 1024, fp);
	int ID = atoi(tmp) +1;
	fclose(fp);
	FILE*fp2  = fopen("id.txt","a");
	fprintf(fp2,"%d\n",ID);
	fclose(fp2);
	return ID;
}
int getPID()																											{
	char tmp[1024];
	FILE*fp = fopen("pid.txt", "r");
	while(!feof(fp))
    	fgets(tmp, 1024, fp);
	int ID = atoi(tmp) +1;
	fclose(fp);
	FILE*fp2  = fopen("pid.txt","a");
	fprintf(fp2,"%d\n",ID);
	fclose(fp2);
	return ID;
}
Date *DateCreate()																										{
	Date*aux=(Date*)malloc(sizeof(Date));
	assert(aux);
	return aux;
}
int DateToInt(Date*DATA)																								{
	int final=0;
	final = final + (DATA->Year*10000);
	final = final + (DATA->Month*100);
	final = final + (DATA->Day);
	return final;
}
/*FUNCOES AUXILIARES DA TAREFA*/
typedef struct node																										{
    int ID;
    int Priority;
    int Status;
    int IDperson;
    char *Description;
    Date   *creation;
    Date   *deadline;
    Date   *conclusion;	
							
    struct node*next; //no ultimo elemento da lista o next é igual a null,.. Next é um ponteiro pra struct node, sao sempre 8 bytes na struct
    //ponteiro tem 4 bytes
}Node;
Node* cria_lista()																										{
    Node*aux=(Node*)malloc(sizeof(Node)); //isto é o Header, pra garantir que nao há problemas na primeira lista, (ex: nao existir inteiros na primeiro)
    assert(aux); //O malloc tem probablidade de falhar, e se falhar devolve zero, isto é um codigo de porteçao
    aux->next=NULL;
    return aux;
}
void TaskInsertHead(Node*h,int key)																						{ 
    Node*aux=(Node*)malloc(sizeof(Node));
    aux->ID=key; // ou seja no auxiliar a infro na struct ( inteiro ) é igual á key
    h->next=aux; //aux é lista auxiliar diferente do header
    aux->next=NULL;
}
int TaskRemoveHead(Node*h)																								{ 
    if(TaskEmpty(h)) { return 0;}
    Node*first=h->next;
    h->next=first->next;
    free(first);
    return 1;
}
int TaskEmpty(Node*h)																									{
    return h->next==NULL;
}
void TaskInsertTail(Node*h,int ID,char*DESC,int PRIORITY,Date*DATAINICIO,Date*DATAPRAZO,Date*DATAFIM,int STATUS,int PID){ 
    Node*aux=(Node*)malloc(sizeof(Node));
    
	aux->ID=ID;
    aux->Status = STATUS;
  	aux->Description = strdup(DESC);
    aux->Priority = PRIORITY;
    aux->creation = DATAINICIO;
	aux->deadline = DATAPRAZO;
	aux->conclusion = DATAFIM;
    aux->next=NULL;
    aux->IDperson = PID;
    
    
    for(h=h->next;h;h=h->next){
        if(h->next==NULL){
            h->next=aux;
            aux->next=NULL;
        }
    }
}
void TaskPrint(Node*h)																									{
    for(h=h->next;h;h=h->next)
	{ 
        printf("\tID:%d PRIO:%d DESCRICAO-%s",h->ID,h->Priority,h->Description);
        printf("\tCriada:%.2d/%.2d/%.4d - Prazo:%.2d/%.2d/%.4d - ",h->creation->Day,h->creation->Month,h->creation->Year,h->deadline->Day,h->deadline->Month,h->deadline->Year);
        if (h->conclusion->Year==0)
        {
        	printf("Por concluir");
    	}
        else
        {
        	printf("Concluida:%.2d/%.2d/%.4d\n",h->conclusion->Day,h->conclusion->Month,h->conclusion->Year);
    	}
    	if (h->IDperson==0)
        {
        	printf("\n\tPor atribuir");
    	}
        else
        {
        	printf("\n\tAtribuida a : %d",h->IDperson);
    	}
        printf("\n\n");
	}
}
void TaskReload(Node*h)																									{
	int i;
	int ID, PRIO, STATUS, PID;
	char DESC[100];
	FILE *database;
	
    TaskInsertHead(h,5); //Introduz o header da lista
    database = fopen("tasks.txt","r");assert(database != NULL);
    
    while(fscanf(database, "%d", &ID) == 1){
     	Date *D1=DateCreate();Date *D2=DateCreate();Date *D3=DateCreate();
		fscanf(database,"%d%d", &PRIO, &STATUS, &D1->Day, &D1->Month, &D1->Year, &D2->Day, &D2->Month, &D2->Year); 
		fscanf(database,"%d%d%d", &D1->Day, &D1->Month, &D1->Year);
		fscanf(database,"%d%d%d", &D2->Day, &D2->Month, &D2->Year);
		fscanf(database,"%d%d%d", &D3->Day, &D3->Month, &D3->Year);
		fscanf(database,"%d ",&PID);
		fgets(DESC,101,database);
		TaskInsertTail(h,ID,DESC,PRIO,D1,D2,D3,STATUS,PID);
    }
    TaskRemoveHead(h);	//Remove o header da lista
}
void TaskReWrite(Node*h)																								{ 
	h=h->next;
	FILE*taskfile = fopen("tasks.txt","w+");
	while(h!=NULL){
		fprintf(taskfile,"%d %d %d",h->ID,h->Priority,h->Status);
		fprintf(taskfile," %d %d %d",h->creation->Day,h->creation->Month,h->creation->Year);
		fprintf(taskfile," %d %d %d",h->deadline->Day,h->deadline->Month,h->deadline->Year);
		fprintf(taskfile," %d %d %d",h->conclusion->Day,h->conclusion->Month,h->conclusion->Year);
		fprintf(taskfile," %d ",h->IDperson);
		fputs(h->Description,taskfile);
		fprintf(taskfile,"\n\n");
		h=h->next;
	}
	fclose(taskfile);
}
void TaskAppend(Node*h)																									{ 
	h=h->next;
	FILE*taskfile = fopen("tasks.txt","a+");
	while(h!=NULL){
		fprintf(taskfile,"%d %d %d",h->ID,h->Priority,h->Status);
		fprintf(taskfile," %d %d %d",h->creation->Day,h->creation->Month,h->creation->Year);
		fprintf(taskfile," %d %d %d",h->deadline->Day,h->deadline->Month,h->deadline->Year);
		fprintf(taskfile," %d %d %d",h->conclusion->Day,h->conclusion->Month,h->conclusion->Year);
		fprintf(taskfile," %d ",h->IDperson);
		fputs(h->Description,taskfile);
		fprintf(taskfile,"\n\n");
		h=h->next;
	}
	fclose(taskfile);
}
Node* TaskDelete(Node*h, int IDREMOVE)																					{
	Node *current = h;
	Node *previous = NULL;
	int val=0;
	 
	 while (current!=NULL)
	 {
	 	if (current->ID==IDREMOVE)
	 	{
	 		val++;
	 		if (previous==NULL)
	 		{
	 			current = current->next;
	 			free(h);
			}
			else
			{
				previous->next = current->next;
				free(current);
				current = previous->next;
			}
		}
		else
		{
			previous = current;
			current = current->next;
			
		}
	 }
	 if (val==0){
	 	sleep(1);
	 	printf("\n\n\tNao ha nenhuma tarefa com esse ID.");
	 	sleep(2);
	 }else{
	 	printf("\n\n\tEfetuando alteracao...");
	 	sleep(1);
	 	
	 }
}
/*FUNCOES AUXILIARES DA PESSOA*/
void TaskPersonNamePrint(int PERSONID)																					{
	char NAME[100];
	char EMAIL[100];
	char NAMEd[100];
	char EMAILd[100];
	int ID;
	int i=0;
	FILE *database;
 	database = fopen("pessoas.txt","r");
    while(fscanf(database, "%d", &ID) == 1){
		if (ID==PERSONID)
		{
    		fscanf(database,"\n");
			fgets(NAME,101,database);	
			fscanf(database,"\n");
			fgets(EMAIL,101,database);
		}
		else
		{
			fscanf(database,"\n");
			fgets(NAMEd,101,database);	
			fscanf(database,"\n");
			fgets(EMAILd,101,database);
		}
    }
    printf("%s",NAME,EMAIL);
}
int PersonFindID(IDINSPECT)																								{
	char NAME[100];
	char EMAIL[100];
	int val;
	int ID;
	FILE *database;
 	database = fopen("pessoas.txt","r");
    while(fscanf(database, "%d", &ID) == 1){
    		fscanf(database,"\n");
			fgets(NAME,101,database);	
			fscanf(database,"\n");
			fgets(EMAIL,101,database);
			if (ID==IDINSPECT)
			{
				val++;
			}
    }
    fclose(database);
    return val;
}
void inspect_personmenu()																								{
	int IDINSPECT;
	header_print();
	printf("\n\n\n\t// MENU PESSOA //\n\n\t--------------------------\n");
	print_personmenu();
	printf("\t--------------------------\n");
	printf("\n\tDigite o ID da pessoa que deseja inspecionar : ");
	scanf("%d",&IDINSPECT);
	inspect_person(IDINSPECT);
}
void inspect_person(int IDINSPECT)																						{
	int val = PersonFindID();
    //Agora, obteve o valor. Se o valor for 0 diz "nao há pessoa!" se for maior que zero, ele imprime o ficheiro tasks...
    if (val!=0)
    {
    		system("cls");
			header_print();
			printf("\n\n\n\t// INSPECIONANDO PESSOA //\n\n\t--------------------------\n\n");
			printf("\t");
			TaskPersonNamePrint(IDINSPECT);
			printf("\n");
			print_inspect_personmenu(IDINSPECT);
			printf("\n\t--------------------------\n");
			printf("\n\t[1] - Voltar");
			printf("\n\n\tEscolha uma opcao : ");
			while (1<10)
			{
				int num=getInt();
				switch(num)
				{
					case 1:	/*VAI PARA MENU TODO*/
						personmenu();
						break;
					default:
						continue;
				}
			break;
			}	
	}
	else
	{
		printf("\n\n\tNao ha nenhuma pessoa com esse ID.");
		sleep(2);personmenu();
	}
}
void print_inspect_personmenu(int IDINSPECT)																			{ 
	FILE *database;
	int ID, PRIO, STATUS;
	int IDperson;
	int val1=0;
	int val2=0;
	int update1=0;
	int update2=0;
	char DESC[100];
    database = fopen("tasks.txt","r");
    assert(database != NULL);
    while(fscanf(database, "%d", &ID) == 1) //DOING
	{ 
    	Date *D1=DateCreate();Date *D2=DateCreate();Date *D3=DateCreate();
		fscanf(database,"%d%d", &PRIO, &STATUS, &D1->Day, &D1->Month, &D1->Year, &D2->Day, &D2->Month, &D2->Year); 
		fscanf(database,"%d%d%d", &D1->Day, &D1->Month, &D1->Year);
		fscanf(database,"%d%d%d", &D2->Day, &D2->Month, &D2->Year);
		fscanf(database,"%d%d%d", &D3->Day, &D3->Month, &D3->Year);
		fscanf(database,"%d",&IDperson);
		fgets(DESC,101,database);	
		if ((IDperson==IDINSPECT) && (STATUS==2) && (update1==0))
		{
			printf("\n\tTarefas da Pessoa na coluna Doing:\n\n");
		}
		if ((IDperson==IDINSPECT) && (STATUS==2))
		{
			printf("\tID:%d PRIO:%d DESCRICAO - %s",ID,PRIO,DESC);
      	 	printf("\tCriada:%.2d/%.2d/%.4d - Prazo:%.2d/%.2d/%.4d - ",D1->Day,D1->Month,D1->Year,D2->Day,D2->Month,D2->Year);
        	if (D3->Year==0)
        	{
        		printf("Por concluir");
        	}
       	 	else
        	{
				printf("Concluida:%.2d/%.2d/%.4d",D3->Day, D3->Month, D3->Year);
			}
        	printf("\n\n"); 
        	val1++;
        	update1++;
        	continue;
    	}
    }
    database = fopen("tasks.txt","r");
    assert(database != NULL);
    while(fscanf(database, "%d", &ID) == 1) //DONE
	{ 
    	Date *D1=DateCreate();Date *D2=DateCreate();Date *D3=DateCreate();
		fscanf(database,"%d%d", &PRIO, &STATUS, &D1->Day, &D1->Month, &D1->Year, &D2->Day, &D2->Month, &D2->Year); 
		fscanf(database,"%d%d%d", &D1->Day, &D1->Month, &D1->Year);
		fscanf(database,"%d%d%d", &D2->Day, &D2->Month, &D2->Year);
		fscanf(database,"%d%d%d", &D3->Day, &D3->Month, &D3->Year);
		fscanf(database,"%d",&IDperson);
		fgets(DESC,101,database);	
		if ((IDperson==IDINSPECT) && (STATUS==3) && (update2==0))
		{
			printf("\n\tTarefas da Pessoa na coluna Done:\n\n");
		}
		
		if ((IDperson==IDINSPECT) && (STATUS==3))
		{
			printf("\tID:%d PRIO:%d DESCRICAO - %s",ID,PRIO,DESC);
      	 	printf("\tCriada:%.2d/%.2d/%.4d - Prazo:%.2d/%.2d/%.4d - ",D1->Day,D1->Month,D1->Year,D2->Day,D2->Month,D2->Year);
			printf("Concluida:%.2d/%.2d/%.4d",D3->Day, D3->Month, D3->Year);
        	printf("\n\n"); 
        	val2++;
        	update2++;
        	continue;
    	}
    }
	if (val1==0 && val2==0)
    	{
    		printf("\tEsta pessoa nao tem tarefas atribuidas.\n");
		}
}
/*FUNCOES PRINCIPAIS*/
void header_print()																										{ 
	system("cls");
	printf("\n  [Kanban_JsLp5.1]");
}
void mainmenu()																											{ 
	header_print();
	printf("\n\n\n\t// MENU PRINCIPAL //");
	printf("\n\n\t[1] - Ver e Editar Quadro");
	printf("\n\t[2] - Ver Tarefas");
	printf("\n\t[3] - Ver Pessoas");
	printf("\n\t[4] - Sair\n\n\n");
	printf("\tEscolha uma opcao : ");
}
/*FUNCOES DO QUADRO KANBAN*/
void boardmenu()																										{
	header_print();
	printf("\n\n\n\t// MENU QUADRO //\n\n\t-----------------------------------------------\n\n");
	matrixprint();
	printf("\t-----------------------------------------------\n");
	printf("\n\t[1] - Editar Coluna To-Do");
	printf("\n\t[2] - Editar Coluna Doing");
	printf("\n\t[3] - Editar Coluna Done");
	printf("\n\t[4] - Voltar\n\n");
	printf("\n\tEscolha uma opcao : ");
	while (1<10)
	{
		int num=getInt();
		switch(num)
		{
			case 1:	/*VAI PARA MENU TODO*/
				todomenu();
				break;
			case 2:	/*VAI PARA MENU DOING*/
				doingmenu();
				break;
			case 3:	/*VAI PARA MENU DONE*/
				donemenu();
				break;
			case 4:	/*VOLTAR*/
				menu();
				break;
			default:
				boardmenu();
		}
		break;
	}
}

void ReversePrintTodo(Node*h){
	if(!h){ 
		return; 
	}
	ReversePrintTodo(h->next);
	if (h->Status==1){
	printf("\tID:%d PRIO:%d DESCRICAO-%s",h->ID,h->Priority,h->Description);
    printf("\tCriada:%.2d/%.2d/%.4d - Prazo:%.2d/%.2d/%.4d - ",h->creation->Day,h->creation->Month,h->creation->Year,h->deadline->Day,h->deadline->Month,h->deadline->Year);
        if (h->conclusion->Year==0)
        {
        	printf("Por concluir");
    	}
        else
        {
        	printf("Concluida:%.2d/%.2d/%.4d\n",h->conclusion->Day,h->conclusion->Month,h->conclusion->Year);
    	}
    	if (h->IDperson==0)
        {
        	printf("\n\tPor atribuir");
    	}
        else
        {
        	printf("\n\tAtribuida a : ");TaskPersonNamePrint(h->IDperson);
    	}
        printf("\n\n");
    }
}
void print_todomenu2(){
	Task*h=cria_lista();
	TaskInsertHead(h,1);
	TaskReload(h);
	PrioritySortList(h);
	ReversePrintTodo(h);
	free(h);
}
void todomenuSORT(){
	Node*tarefaRemove=cria_lista();
	TaskInsertHead(tarefaRemove,1);
	TaskReload(tarefaRemove);
	PrioritySortList(tarefaRemove);
	TaskReWrite(tarefaRemove);
	free(tarefaRemove);
}
void print_todomenu()																									{ 
	FILE *database;
	int ID, PRIO, STATUS;
	int IDperson;
	char DESC[100];
    database = fopen("tasks.txt","r");
    assert(database != NULL);
    while(fscanf(database, "%d", &ID) == 1){ 
    	Date *D1=DateCreate();Date *D2=DateCreate();Date *D3=DateCreate();
    	
		fscanf(database,"%d%d", &PRIO, &STATUS, &D1->Day, &D1->Month, &D1->Year, &D2->Day, &D2->Month, &D2->Year); 
		fscanf(database,"%d%d%d", &D1->Day, &D1->Month, &D1->Year);
		fscanf(database,"%d%d%d", &D2->Day, &D2->Month, &D2->Year);
		fscanf(database,"%d%d%d", &D3->Day, &D3->Month, &D3->Year);
		fscanf(database,"%d",&IDperson);
		fgets(DESC,101,database);	
		
		if (STATUS==1){
		printf("\tID:%d PRIO:%d DESCRICAO - %s",ID,PRIO,DESC);
        printf("\tCriada:%.2d/%.2d/%.4d - Prazo:%.2d/%.2d/%.4d - ",D1->Day,D1->Month,D1->Year,D2->Day,D2->Month,D2->Year);
        if (D3->Year==0)
        	printf("Por concluir");
        else
        	printf("Concluida:%.2d/%.2d/%.4d",D3->Day, D3->Month, D3->Year);
		if (IDperson==0)
        {
        	printf("\n\tPor atribuir");
    	}
        else
        {
        	printf("\n\tAtribuida a : ");TaskPersonNamePrint(IDperson);
    	}
        printf("\n\n"); 
    	}
    }
}
void todomenu()																											{
	todomenuSORT();
	header_print();
	printf("\n\n\n\t// MENU TO-DO //\n\n\t--------------------------\n\n");
	print_todomenu2();
	printf("\n\t--------------------------\n");
	printf("\n\t[1] - Atribuir Tarefa ");
	printf("\n\t[2] - Voltar\n\n");
	printf("\tEscolha uma opcao : ");
	while (1<10)
	{
		int num=getInt();
		switch(num)
		{
			case 1:/*Atribuir Tarefa*/
				assign_todomenu();
				break;
			case 2:	/*VOLTAR*/
				boardmenu();
				break;
			default:
				todomenu();
		}
		break;
	}
}
void assign_todomenu()																									{
	int ID=0;
	int PID=0;
	header_print();
	printf("\n\n\n\t// ATRIBUIR //\n\n\t--------------------------\n\n");
	print_todomenu2();
	printf("\t--------------------------\n");
	printf("\n\tDigite o ID da Tarefa que deseja atribuir : ");
	scanf("%d",&ID);
	header_print();
	printf("\n\n\n\t// ATRIBUIR //\n\n\t--------------------------\n");
	print_personmenu();
	printf("\t--------------------------\n");
	printf("\n\tDigite o ID da pessoa a quem deseja atribuir a tarefa : ");
	scanf("%d",&PID);
	todo_assign(ID,PID,TASKMAXIMUM);
	todomenu();
}
void todo_assign(int IDassign, int IDpess, int TASKMAXIMUM)																				{ 
	FILE *database;
	int ID, PRIO, STATUS;
	int IDperson;
	int val;
	int Atribuidas=0;
	int DATATEMP=1111;
	int DATAATUAL=0;
	char DESC[100];
	//PROCURA NO FICHEIRO O ID PESSOA PARA VERIFICAR SE JA TEM O MAXIMO ATRIBUIDO
	database = fopen("tasks.txt","r");
    assert(database != NULL);
    while(fscanf(database, "%d", &ID) == 1)
	{ 
    	Date *D1=DateCreate();Date *D2=DateCreate();Date *D3=DateCreate();
		fscanf(database,"%d%d", &PRIO, &STATUS, &D1->Day, &D1->Month, &D1->Year, &D2->Day, &D2->Month, &D2->Year); 
		fscanf(database,"%d%d%d", &D1->Day, &D1->Month, &D1->Year);
		fscanf(database,"%d%d%d", &D2->Day, &D2->Month, &D2->Year);
		fscanf(database,"%d%d%d", &D3->Day, &D3->Month, &D3->Year);
		fscanf(database,"%d",&IDperson);
		fgets(DESC,101,database);	
		
		if (IDperson==IDpess)
		{
			Atribuidas++;
    	}
    }
	fclose(database);
	if (Atribuidas>TASKMAXIMUM && Atribuidas!=0){
		printf("\n\tAtribuicao nao concluida.\n\tPessoa ja tem o maximo de tarefas atribuidas (%d)",TASKMAXIMUM);sleep(2);
    	todomenu();
	}
	//PROCURA NO FICHEIRO O ID PESSOA PARA VERIFICAR SE O PRAZO QUE LÁ ESTA TEM MAIS DE UMA SEMANA DE DISTANCIA
	database = fopen("tasks.txt","r");
    assert(database != NULL);
    while(fscanf(database, "%d", &ID) == 1)
	{ 
    	Date *D1=DateCreate();Date *D2=DateCreate();Date *D3=DateCreate();
		fscanf(database,"%d%d", &PRIO, &STATUS, &D1->Day, &D1->Month, &D1->Year, &D2->Day, &D2->Month, &D2->Year); 
		fscanf(database,"%d%d%d", &D1->Day, &D1->Month, &D1->Year);
		fscanf(database,"%d%d%d", &D2->Day, &D2->Month, &D2->Year);
		fscanf(database,"%d%d%d", &D3->Day, &D3->Month, &D3->Year);
		fscanf(database,"%d",&IDperson);
		fgets(DESC,101,database);	
		if (ID = IDassign && IDperson==0)
    	{
    		DATAATUAL=DateToInt(D2);
		}
		if (STATUS==2 && IDperson==IDpess)
    	{
    		DATATEMP=DateToInt(D2);
}
    	if (((DATATEMP-DATAATUAL)>(-7))&&((DATATEMP-DATAATUAL)<7))
    	{
			printf("\n\tAtribuicao nao concluida.\n\tPessoa ja tem um prazo para esta semana");sleep(2);
			fclose(database);
    		todomenu();
		}
	}
	fclose(database);
	
    database = fopen("tasks.txt","r");
    assert(database != NULL);
    while(fscanf(database, "%d", &ID) == 1)
	{ 
    	Date *D1=DateCreate();Date *D2=DateCreate();Date *D3=DateCreate();
		fscanf(database,"%d%d", &PRIO, &STATUS, &D1->Day, &D1->Month, &D1->Year, &D2->Day, &D2->Month, &D2->Year); 
		fscanf(database,"%d%d%d", &D1->Day, &D1->Month, &D1->Year);
		fscanf(database,"%d%d%d", &D2->Day, &D2->Month, &D2->Year);
		fscanf(database,"%d%d%d", &D3->Day, &D3->Month, &D3->Year);
		fscanf(database,"%d",&IDperson);
		fgets(DESC,101,database);	
		
		
		
		if (ID==IDassign)
		{
			val++;
			assign(IDassign,PRIO,2,IDpess,D1,D2,D3,DESC);
    	}
    }
    if (val==0)
    	{
    		printf("\n\tNao ha nenhuma tarefa com esse ID");sleep(2);
    		todomenu();
		}
	fclose(database);
}
void assign(int sID, int PRIO, int STAT, int sPID, Date*D1,Date*D2,Date*D3,char*DESC)									{									
	//REMOVE UMA TAREFA DO FICHEIRO
	Node*tarefaRemove=cria_lista();
	TaskInsertHead(tarefaRemove,1);
	TaskReload(tarefaRemove);
	TaskDelete(tarefaRemove,sID);
	TaskReWrite(tarefaRemove);
	free(tarefaRemove);
	//ADICIONA-LA DE NOVO
	Node*tarefaAdd=cria_lista();
	TaskInsertHead(tarefaAdd,1);
	TaskInsertTail(tarefaAdd,sID,DESC,PRIO,D1,D2,D3,STAT,sPID);
	TaskRemoveHead(tarefaAdd);
	TaskAppend(tarefaAdd);
	free(tarefaAdd);
}

void doingmenuSORT(){
	Node*tarefaRemove=cria_lista();
	TaskInsertHead(tarefaRemove,1);
	TaskReload(tarefaRemove);
	PIDSortList(tarefaRemove);
	TaskReWrite(tarefaRemove);
	free(tarefaRemove);
}
void print_doingmenu()																									{ 
	FILE *database;
	int ID, PRIO, STATUS;
	int IDperson;
	char DESC[100];
    database = fopen("tasks.txt","r");
    assert(database != NULL);
    while(fscanf(database, "%d", &ID) == 1){ 
    	Date *D1=DateCreate();Date *D2=DateCreate();Date *D3=DateCreate();
    	
		fscanf(database,"%d%d", &PRIO, &STATUS, &D1->Day, &D1->Month, &D1->Year, &D2->Day, &D2->Month, &D2->Year); 
		fscanf(database,"%d%d%d", &D1->Day, &D1->Month, &D1->Year);
		fscanf(database,"%d%d%d", &D2->Day, &D2->Month, &D2->Year);
		fscanf(database,"%d%d%d", &D3->Day, &D3->Month, &D3->Year);
		fscanf(database,"%d",&IDperson);
		fgets(DESC,101,database);	
		if (STATUS==2){
		
		printf("\tID:%d PRIO:%d DESCRICAO - %s",ID,PRIO,DESC);
        printf("\tCriada:%.2d/%.2d/%.4d - Prazo:%.2d/%.2d/%.4d - ",D1->Day,D1->Month,D1->Year,D2->Day,D2->Month,D2->Year);
        if (D3->Year==0)
        	printf("Por concluir");
        else
        	printf("Concluida:%.2d/%.2d/%.4d",D3->Day, D3->Month, D3->Year);
		if (IDperson==0)
        {
        	printf("\n\tPor atribuir");
    	}
        else
        {
        	printf("\n\tAtribuida a : ");TaskPersonNamePrint(IDperson);
    	}
        printf("\n\n"); 
    	}
    }
}
void doingmenu()																										{
	doingmenuSORT();
	header_print();
	printf("\n\n\n\t// MENU DOING //\n\n\t--------------------------\n\n");
	print_doingmenu();
	printf("\t--------------------------\n");
	printf("\n\t[1] - Concluir ");
    printf("\n\t[2] - Desatribuir");
    printf("\n\t[3] - Mudar Atribuição");
	printf("\n\t[4] - Voltar\n\n");
	printf("\tEscolha uma opcao : ");
	while (1<10)
	{
		int num=getInt();
		switch(num)
		{
			case 1:
			    conclude_doingmenu();
				break;
			case 2:
			    unassign_doingmenu();
				break;
            case 3:
                reassign_doingmenu();
                break;
            case 4:
                boardmenu();
                break;
			default:
				doingmenu();
		}
		break;
	}
}
void conclude_doingmenu()																								{
	int ID=0;
	header_print();
	printf("\n\n\n\t// CONCLUIR //\n\n\t--------------------------\n\n");
	print_doingmenu();
	printf("\t--------------------------\n");
	printf("\n\tDigite o ID da Tarefa que deseja concluir : ");
	scanf("%d",&ID);
	doing_conclude(ID);
	doingmenu();
}
void doing_conclude(int IDconc)																							{ 
	FILE *database;
	int ID, PRIO, STATUS;
	int IDperson;
	int val;
	char DESC[100];
    database = fopen("tasks.txt","r");
    assert(database != NULL);
    while(fscanf(database, "%d", &ID) == 1)
	{ 
    	Date *D1=DateCreate();Date *D2=DateCreate();Date *D3=DateCreate();
		fscanf(database,"%d%d", &PRIO, &STATUS, &D1->Day, &D1->Month, &D1->Year, &D2->Day, &D2->Month, &D2->Year); 
		fscanf(database,"%d%d%d", &D1->Day, &D1->Month, &D1->Year);
		fscanf(database,"%d%d%d", &D2->Day, &D2->Month, &D2->Year);
		fscanf(database,"%d%d%d", &D3->Day, &D3->Month, &D3->Year);
		fscanf(database,"%d",&IDperson);
		fgets(DESC,101,database);	
		
		if (ID==IDconc)
		{
			val++;
			printf("\n\tInsira a data de hoje (dd/mm/aaaa) : ");
			scanf("%d/%d/%d",&D3->Day,&D3->Month,&D3->Year);
			if ((D3->Day>31)||(D3->Month>12)||(D3->Year>9999))
			{
				do{
					printf("\n\tData invalida.Insira a data correta (dd/mm/aaaa) : ");
					scanf("%d/%d/%d",&D3->Day,&D3->Month,&D3->Year);
				}while((D3->Day>31)||(D3->Month>12)||(D3->Year>9999));
			}
			conclude(IDconc,PRIO,3,IDperson,D1,D2,D3,DESC);
    	}
    }
    if (val==0)
    	{
    		printf("\n\tNao ha nenhuma tarefa com esse ID");sleep(2);
    		doingmenu();
		}
	fclose(database);
}
void conclude(int sID, int PRIO, int STAT, int sPID, Date*D1,Date*D2,Date*D3,char*DESC)									{									
	//REMOVE UMA TAREFA DO FICHEIRO
	Node*tarefaRemove=cria_lista();
	TaskInsertHead(tarefaRemove,1);
	TaskReload(tarefaRemove);
	TaskDelete(tarefaRemove,sID);
	TaskReWrite(tarefaRemove);
	free(tarefaRemove);
	//ADICIONA-LA DE NOVO
	Node*tarefaAdd=cria_lista();
	TaskInsertHead(tarefaAdd,1);
	TaskInsertTail(tarefaAdd,sID,DESC,PRIO,D1,D2,D3,STAT,sPID);
	TaskRemoveHead(tarefaAdd);
	TaskAppend(tarefaAdd);
	free(tarefaAdd);
}
void unassign_doingmenu()																								{
	int ID=0;
	header_print();
	printf("\n\n\n\t// DESATRIBUIR //\n\n\t--------------------------\n\n");
	print_doingmenu();
	printf("\t--------------------------\n");
	printf("\n\tDigite o ID da Tarefa que deseja desatribuir : ");
	scanf("%d",&ID);
	doing_unassign(ID);
	doingmenu();
}
void doing_unassign(int IDconc)																							{ 
	FILE *database;
	int ID, PRIO, STATUS;
	int IDperson;
	int val;
	char DESC[100];
    database = fopen("tasks.txt","r");
    assert(database != NULL);
    while(fscanf(database, "%d", &ID) == 1)
	{ 
    	Date *D1=DateCreate();Date *D2=DateCreate();Date *D3=DateCreate();
		fscanf(database,"%d%d", &PRIO, &STATUS, &D1->Day, &D1->Month, &D1->Year, &D2->Day, &D2->Month, &D2->Year); 
		fscanf(database,"%d%d%d", &D1->Day, &D1->Month, &D1->Year);
		fscanf(database,"%d%d%d", &D2->Day, &D2->Month, &D2->Year);
		fscanf(database,"%d%d%d", &D3->Day, &D3->Month, &D3->Year);
		fscanf(database,"%d",&IDperson);
		fgets(DESC,101,database);	
		
		if (ID==IDconc)
		{
			val++;
			unassign(IDconc,PRIO,1,0,D1,D2,D3,DESC);
    	}
    }
    if (val==0)
    	{
    		printf("\n\tNao ha nenhuma tarefa com esse ID");sleep(2);
    		doingmenu();
		}
	fclose(database);
}
void unassign(int sID, int PRIO, int STAT, int sPID, Date*D1,Date*D2,Date*D3,char*DESC)									{									
	//REMOVE UMA TAREFA DO FICHEIRO
	Node*tarefaRemove=cria_lista();
	TaskInsertHead(tarefaRemove,1);
	TaskReload(tarefaRemove);
	TaskDelete(tarefaRemove,sID);
	TaskReWrite(tarefaRemove);
	free(tarefaRemove);
	//ADICIONA-LA DE NOVO
	Node*tarefaAdd=cria_lista();
	TaskInsertHead(tarefaAdd,1);
	TaskInsertTail(tarefaAdd,sID,DESC,PRIO,D1,D2,D3,STAT,sPID);
	TaskRemoveHead(tarefaAdd);
	TaskAppend(tarefaAdd);
	free(tarefaAdd);
}

void reassign_doingmenu(){
	int ID=0;
	int PID=0;
	header_print();
	printf("\n\n\n\t// REATRIBUIR //\n\n\t--------------------------\n\n");
	print_doingmenu();
	printf("\t--------------------------\n");
	printf("\n\tDigite o ID da Tarefa que deseja reatribuir : ");
	scanf("%d",&ID);
	header_print();
	printf("\n\n\n\t// REATRIBUIR //\n\n\t--------------------------\n");
	print_personmenu();
	printf("\t--------------------------\n");
	printf("\n\tDigite o ID da pessoa a quem deseja atribuir a tarefa : ");
	scanf("%d",&PID);
	doing_reassign(ID,PID,TASKMAXIMUM);
	doingmenu();
}
void doing_reassign(int IDassign, int IDpess, int TASKMAXIMUM){
	FILE *database;
	int ID, PRIO, STATUS;
	int IDperson;
	int val;
	int Atribuidas=0;
	int DATATEMP=1111;
	int DATAATUAL=0;
	char DESC[100];
	//PROCURA NO FICHEIRO O ID PESSOA PARA VERIFICAR SE JA TEM O MAXIMO ATRIBUIDO
	database = fopen("tasks.txt","r");
    assert(database != NULL);
    while(fscanf(database, "%d", &ID) == 1)
	{ 
    	Date *D1=DateCreate();Date *D2=DateCreate();Date *D3=DateCreate();
		fscanf(database,"%d%d", &PRIO, &STATUS, &D1->Day, &D1->Month, &D1->Year, &D2->Day, &D2->Month, &D2->Year); 
		fscanf(database,"%d%d%d", &D1->Day, &D1->Month, &D1->Year);
		fscanf(database,"%d%d%d", &D2->Day, &D2->Month, &D2->Year);
		fscanf(database,"%d%d%d", &D3->Day, &D3->Month, &D3->Year);
		fscanf(database,"%d",&IDperson);
		fgets(DESC,101,database);	
		
		if (IDperson==IDpess)
		{
			Atribuidas++;
    	}
    }
	fclose(database);
	if (Atribuidas>TASKMAXIMUM && Atribuidas!=0){
		printf("\n\tAtribuicao nao concluida.\n\tPessoa ja tem o maximo de tarefas atribuidas (%d)",TASKMAXIMUM);sleep(2);
    	todomenu();
	}
	//PROCURA NO FICHEIRO O ID PESSOA PARA VERIFICAR SE O PRAZO QUE LÁ ESTA TEM MAIS DE UMA SEMANA DE DISTANCIA
	database = fopen("tasks.txt","r");
    assert(database != NULL);
    while(fscanf(database, "%d", &ID) == 1)
	{ 
    	Date *D1=DateCreate();Date *D2=DateCreate();Date *D3=DateCreate();
		fscanf(database,"%d%d", &PRIO, &STATUS, &D1->Day, &D1->Month, &D1->Year, &D2->Day, &D2->Month, &D2->Year); 
		fscanf(database,"%d%d%d", &D1->Day, &D1->Month, &D1->Year);
		fscanf(database,"%d%d%d", &D2->Day, &D2->Month, &D2->Year);
		fscanf(database,"%d%d%d", &D3->Day, &D3->Month, &D3->Year);
		fscanf(database,"%d",&IDperson);
		fgets(DESC,101,database);	
		if (ID = IDassign)
    	{
    		DATAATUAL=DateToInt(D2);
		}
		if (STATUS==2 && IDperson == IDpess)
    	{
    		DATATEMP=DateToInt(D2);
		}
    	if (((DATATEMP-DATAATUAL)>(-7))&&((DATATEMP-DATAATUAL)<7))
    	{
			printf("\n\tAtribuicao nao concluida.\n\tPessoa ja tem um prazo para esta semana");sleep(2);
    		todomenu();
		}
	}
	fclose(database);
	
    database = fopen("tasks.txt","r");
    assert(database != NULL);
    while(fscanf(database, "%d", &ID) == 1)
	{ 
    	Date *D1=DateCreate();Date *D2=DateCreate();Date *D3=DateCreate();
		fscanf(database,"%d%d", &PRIO, &STATUS, &D1->Day, &D1->Month, &D1->Year, &D2->Day, &D2->Month, &D2->Year); 
		fscanf(database,"%d%d%d", &D1->Day, &D1->Month, &D1->Year);
		fscanf(database,"%d%d%d", &D2->Day, &D2->Month, &D2->Year);
		fscanf(database,"%d%d%d", &D3->Day, &D3->Month, &D3->Year);
		fscanf(database,"%d",&IDperson);
		fgets(DESC,101,database);	
		
		if (ID==IDassign)
		{
			val++;
			reassign(IDassign,PRIO,2,IDpess,D1,D2,D3,DESC);
    	}
    }
    if (val==0)
    	{
    		printf("\n\tNao ha nenhuma tarefa com esse ID");sleep(2);
    		todomenu();
		}
	fclose(database);
}
void reassign(int sID, int PRIO, int STAT, int sPID, Date*D1,Date*D2,Date*D3,char*DESC)									{									
	//REMOVE UMA TAREFA DO FICHEIRO
	Node*tarefaRemove=cria_lista();
	TaskInsertHead(tarefaRemove,1);
	TaskReload(tarefaRemove);
	TaskDelete(tarefaRemove,sID);
	TaskReWrite(tarefaRemove);
	free(tarefaRemove);
	//ADICIONA-LA DE NOVO
	Node*tarefaAdd=cria_lista();
	TaskInsertHead(tarefaAdd,1);
	TaskInsertTail(tarefaAdd,sID,DESC,PRIO,D1,D2,D3,STAT,sPID);
	TaskRemoveHead(tarefaAdd);
	TaskAppend(tarefaAdd);
	free(tarefaAdd);
}




void print_donemenu()																									{ 
	FILE *database;
	int ID, PRIO, STATUS;
	int IDperson;
	char DESC[100];
    database = fopen("tasks.txt","r");
    assert(database != NULL);
    while(fscanf(database, "%d", &ID) == 1){ 
    	Date *D1=DateCreate();Date *D2=DateCreate();Date *D3=DateCreate();
    	
		fscanf(database,"%d%d", &PRIO, &STATUS, &D1->Day, &D1->Month, &D1->Year, &D2->Day, &D2->Month, &D2->Year); 
		fscanf(database,"%d%d%d", &D1->Day, &D1->Month, &D1->Year);
		fscanf(database,"%d%d%d", &D2->Day, &D2->Month, &D2->Year);
		fscanf(database,"%d%d%d", &D3->Day, &D3->Month, &D3->Year);
		fscanf(database,"%d",&IDperson);
		fgets(DESC,101,database);	
		if (STATUS==3){
		
		printf("\tID:%d PRIO:%d DESCRICAO - %s",ID,PRIO,DESC);
        printf("\tCriada:%.2d/%.2d/%.4d - Prazo:%.2d/%.2d/%.4d - ",D1->Day,D1->Month,D1->Year,D2->Day,D2->Month,D2->Year);
        if (D3->Year==0)
        	printf("Por concluir");
        else
        	printf("Concluida:%.2d/%.2d/%.4d",D3->Day, D3->Month, D3->Year);
		if (IDperson==0)
        {
        	printf("\n\tPor atribuir");
    	}
        else
        {
        	printf("\n\tAtribuida a : ");TaskPersonNamePrint(IDperson);
    	}
        printf("\n\n"); 
    	}
    }
}
void donemenu()																											{
	header_print();
	printf("\n\n\n\t// MENU DONE //\n\n\t--------------------------\n\n");
	print_donemenu();
	printf("\t--------------------------\n");
	printf("\n\t[1] - Reciclar ");
	printf("\n\t[2] - Voltar\n\n");
	printf("\tEscolha uma opcao : ");
	while (1<10)
	{
		int num=getInt();
		switch(num)
		{
			case 1:
			    recycle_donemenu();
				break;
            case 2:
                boardmenu();
                break;
			default:
				donemenu();
		}
		break;
	}
}
void recycle_donemenu()																									{
	int ID=0;
	int PID=0;
	header_print();
	printf("\n\n\n\t// RECICLAR //\n\n\t--------------------------\n\n");
	print_donemenu();
	printf("\t--------------------------\n");
	printf("\n\tDigite o ID da Tarefa que deseja reciclar : ");
	scanf("%d",&ID);
	done_recycle(ID);
	donemenu();
}
void done_recycle(int IDrecyc)																							{ 
	FILE *database;
	int ID, PRIO, STATUS;
	int IDperson;
	int val;
	char DESC[100];
    database = fopen("tasks.txt","r");
    assert(database != NULL);
    while(fscanf(database, "%d", &ID) == 1)
	{ 
    	Date *D1=DateCreate();Date *D2=DateCreate();Date *D3=DateCreate();
		fscanf(database,"%d%d", &PRIO, &STATUS, &D1->Day, &D1->Month, &D1->Year, &D2->Day, &D2->Month, &D2->Year); 
		fscanf(database,"%d%d%d", &D1->Day, &D1->Month, &D1->Year);
		fscanf(database,"%d%d%d", &D2->Day, &D2->Month, &D2->Year);
		fscanf(database,"%d%d%d", &D3->Day, &D3->Month, &D3->Year);
		fscanf(database,"%d",&IDperson);
		fgets(DESC,101,database);	
		
		if (ID==IDrecyc)
		{
			val++;
			D3->Day=0;D3->Month=0;D3->Year=0;
			recycle(IDrecyc,PRIO,1,0,D1,D2,D3,DESC);
    	}
    }
    if (val==0)
    	{
    		printf("\n\tNao ha nenhuma tarefa com esse ID");sleep(2);
    		donemenu();
		}
	fclose(database);
}
void recycle(int sID, int PRIO, int STAT, int sPID, Date*D1,Date*D2,Date*D3,char*DESC)									{									
	//REMOVE UMA TAREFA DO FICHEIRO
	Node*tarefaRemove=cria_lista();
	TaskInsertHead(tarefaRemove,1);
	TaskReload(tarefaRemove);
	TaskDelete(tarefaRemove,sID);
	TaskReWrite(tarefaRemove);
	free(tarefaRemove);
	//ADICIONA-LA DE NOVO
	Node*tarefaAdd=cria_lista();
	TaskInsertHead(tarefaAdd,1);
	TaskInsertTail(tarefaAdd,sID,DESC,PRIO,D1,D2,D3,STAT,sPID);
	TaskRemoveHead(tarefaAdd);
	TaskAppend(tarefaAdd);
	free(tarefaAdd);
}
/*FUNCOES DO MENU TAREFAS*/ /*FALTA ADICIONAR AS PESSOAS À LISTA E ASSIM*/
void print_taskmenu()																									{ 
	FILE *database;
	int ID, PRIO, STATUS;
	int IDperson;
	char DESC[100];
    database = fopen("tasks.txt","r");
    assert(database != NULL);
    while(fscanf(database, "%d", &ID) == 1){ 
    	Date *D1=DateCreate();Date *D2=DateCreate();Date *D3=DateCreate();
    	//int D1Day,D1Month,D1Year, D2Day,D2Month,D2Year, D3Day,D3Month,D3Year;
		fscanf(database,"%d%d", &PRIO, &STATUS, &D1->Day, &D1->Month, &D1->Year, &D2->Day, &D2->Month, &D2->Year); 
		fscanf(database,"%d%d%d", &D1->Day, &D1->Month, &D1->Year);
		fscanf(database,"%d%d%d", &D2->Day, &D2->Month, &D2->Year);
		fscanf(database,"%d%d%d", &D3->Day, &D3->Month, &D3->Year);
		fscanf(database,"%d",&IDperson);
		fgets(DESC,101,database);	
		printf("\tID:%d PRIO:%d DESCRICAO - %s",ID,PRIO,DESC);
        printf("\tCriada:%.2d/%.2d/%.4d - Prazo:%.2d/%.2d/%.4d - ",D1->Day,D1->Month,D1->Year,D2->Day,D2->Month,D2->Year);
        if (D3->Year==0)
        	printf("Por concluir");
        else
        	printf("Concluida:%.2d/%.2d/%.4d",D3->Day, D3->Month, D3->Year);
		if (IDperson==0)
        {
        	printf("\n\tPor atribuir");
    	}
        else
        {
        	printf("\n\tAtribuida a : ");TaskPersonNamePrint(IDperson);
    	}
        printf("\n\n"); 
    }
}
void remove_task(int IDREMOVE)																							{
	Node*tarefaRemove=cria_lista();
	TaskInsertHead(tarefaRemove,1);
	TaskReload(tarefaRemove);
	TaskDelete(tarefaRemove,IDREMOVE);
	TaskReWrite(tarefaRemove);
	free(tarefaRemove);
}
void remove_taskmenu()																									{ 
	header_print();
	
	printf("\n\n\n\t// REMOVER TAREFA //\n\n\t--------------------------\n\n");
	print_taskmenu();
	printf("\t--------------------------\n");
	
	/* OBTENS O ID DA TAREFA QUE DEVES REMOVER */
	int IDREMOVE;
	printf("\n\n\tDigite o ID da tarefa a remover : ");
	scanf("%d",&IDREMOVE);

	/* VERIFICAÇÂO */
	printf("\n\n\tDeseja remover esta tarefa?\n");
	printf("\n\t[1] - Remover Tarefa");
	printf("\n\t[2] - Cancelar\n\n");
	printf("\tEscolha uma opcao : ");
	printf("\t");
	while (1<10)
	{
		int num=getInt();
		switch(num)
		{
			case 1:
				remove_task(IDREMOVE);
				taskmenu();
				break;
			case 2:
				remove_taskmenu();
				break;
			default:
				continue;
		}
	}
	
}
void add_task(int ID,char*DESC,int PRIORITY,Date*DATAINICIO,Date*DATAPRAZO,Date*DATAFIM,int STATUS,int PID)				{ 
	Node*tarefaAdd=cria_lista();
	TaskInsertHead(tarefaAdd,1);
	TaskInsertTail(tarefaAdd,ID,DESC,PRIORITY,DATAINICIO,DATAPRAZO,DATAFIM,STATUS,0);
	TaskRemoveHead(tarefaAdd);
	TaskAppend(tarefaAdd);
	free(tarefaAdd);
}
void add_taskmenu()																										{ 
	header_print();
	int IDTASKTEMP=getID();
	int PersonID;
	char PROTECT[1];
	int PRIOTEMP;
	char STRTEMP[100];
	printf("\n\n\n\t// ADICIONAR TAREFA //\n");
	printf("\n\n\tID da Tarefa = %d",IDTASKTEMP);
	printf("\n\tDescricao da Tarefa = ");
	gets(STRTEMP);
	printf("\tPrioridade da Tarefa (1 a 10) = ");
	PRIOTEMP = getInt();
	if ((PRIOTEMP>10)||(PRIOTEMP<1)){ /*Protecao Da Prioridade*/
		do{
			printf("\tInput invalido. Introduza a prioridade da tarefa (1 a 10) = ");
			PRIOTEMP = getInt();
		}while((PRIOTEMP>10)||(PRIOTEMP<1));
	}
	Date *DATAINICIO = DateCreate();
	Date *DATAPRAZO = DateCreate();
	Date *DATAFIM = DateCreate();
	printf("\n\tInsira a data de hoje (dd/mm/aaaa) : ");
	scanf("%d/%d/%d",&DATAINICIO->Day,&DATAINICIO->Month,&DATAINICIO->Year);
	if ((DATAINICIO->Day>31)||(DATAINICIO->Month>12)||(DATAINICIO->Year>9999)){ /*Protecao das dataas*/
		do{
			printf("\n\tData invalida.Insira a data correta (dd/mm/aaaa) : ");
			scanf("%d/%d/%d",&DATAINICIO->Day,&DATAINICIO->Month,&DATAINICIO->Year);
		}while((DATAINICIO->Day>31)||(DATAINICIO->Month>12)||(DATAINICIO->Year>9999));
	}
	gets(PROTECT);
	printf("\tInsira a data limite (dd/mm/aaaa) : ");
	scanf("%d/%d/%d",&DATAPRAZO->Day,&DATAPRAZO->Month,&DATAPRAZO->Year);
	if ((DATAPRAZO->Day>31)||(DATAPRAZO->Month>12)||(DATAPRAZO->Year>9999)){/*Protecao das dataas*/
		do{
			printf("\n\tData invalida.Insira a data correta (dd/mm/aaaa) : ");
			scanf("%d/%d/%d",&DATAPRAZO->Day,&DATAPRAZO->Month,&DATAPRAZO->Year);
		}while((DATAPRAZO->Day>31)||(DATAPRAZO->Month>12)||(DATAPRAZO->Year>9999));
	}
	DATAFIM->Day=0;
	DATAFIM->Month=0;
	DATAFIM->Year=0;
	int STATUS = 1;
	PersonID= 0;
	printf("\n\n\t------------------------\n\n\t");
	printf("ID: [%d]\tPRIORIDADE: %d\tDESC: '%s'\n\tCriada : %d/%d/%d",IDTASKTEMP,PRIOTEMP,STRTEMP,DATAINICIO->Day,DATAINICIO->Month,DATAINICIO->Year);
	printf("\tPrazo : %d/%d/%d",DATAPRAZO->Day,DATAPRAZO->Month,DATAPRAZO->Year);
	printf("\n\n\tDeseja adicionar esta tarefa?\n");
	printf("\n\t[1] - Adicionar Tarefa");
	printf("\n\t[2] - Cancelar\n\n");
	printf("\tEscolha uma opcao : ");
	while (1<10){
		int num=getInt();
		switch(num)
		{
			case 1:
				/*Aqui os diferentes dados que adquirimos são inseridos numa lista TASK(struct) e é guardado no ficheiro tarefas.txt*/
				add_task(IDTASKTEMP,STRTEMP,PRIOTEMP,DATAINICIO,DATAPRAZO,DATAFIM,STATUS,PersonID);
				taskmenu();
				break;
			case 2:
				menu();
				break;
			default:
				continue;
		}
	}
}
void taskmenu()																											{ 
	header_print();
	printf("\n\n\n\t// MENU TAREFA //\n\n\t--------------------------\n\n");
	print_taskmenu();
	printf("\t--------------------------\n");
	printf("\n\t[1] - Adicionar Tarefa");
	printf("\t[2] - Remover Tarefa");
	printf("\t[3] - Voltar\n\n");
	printf("\tEscolha uma opcao : ");
	while (1<10)
	{
		int num=getInt();
		switch(num)
		{
			case 1:
				add_taskmenu();
				/* O menu add_task apenas se encarrega de adicionar ao ficheiro
				uma nova tarefa através da escrita em append. Isto não implica
				manipulação de listas, de modo nenhum. Portanto, apenas usamos
				a função de escrita*/
				break;
			case 2:
				remove_taskmenu();
				/* Já o menu remove_task necessita da criação da lista, para 
				poder manipular a lista e depois reescrevê-la no ficheiro, através
				do modo de escrita w. Portanto, temos de utilizar a função TaskReload():
				*/
				break;
			case 3:
				menu();
				break;
			default:
				taskmenu();
		}
		break;
	}
}
/*FUNCOES DO MENU PESSOAS*/
void print_personmenu()																									{ 
	FILE *database;
	int ID;
	char NAME[100];
	char EMAIL[100];
    database = fopen("pessoas.txt","r");
    printf("\n");
    assert(database != NULL);
    while(fscanf(database, "%d", &ID) == 1){ 
    	fscanf(database,"\n");
		fgets(NAME,101,database);	
		fscanf(database,"\n");
		fgets(EMAIL,101,database);
		printf("\tID:%d Nome: %s \tEmail: %s",ID,NAME,EMAIL);
        printf("\n"); 
    }
}
void personmenu()																										{ 
	header_print();
	printf("\n\n\n\t// MENU PESSOA //\n\n\t--------------------------\n");
	print_personmenu();
	printf("\t--------------------------\n");
	printf("\n\t[1] - Inspecionar Pessoa");
	printf("\n\t[2] - Voltar\n");
	printf("\n\tEscolha uma opcao : ");
	while (1<10)
	{
		int num=getInt();
		switch(num)
		{
			case 1: /*Inspecionar*/
				inspect_personmenu();
				personmenu();
				break;
			case 2: /*VOLTAR*/
				menu();
				break;
			default:
				personmenu();
		}
		break;
	}
}
/*FUNCAO DE VERIFICACAO DE ESCOLHA*/
void proceed_check()																									{ 
	header_print();
	printf("\n\t\n\tTem a certeza?\n");
	printf("\n\t[1] - Sim");
	printf("\n\t[2] - Nao");
	printf("\n\n\tEscolha uma opcao : ");
	while (1<10)
	{
		int num=getInt();
		switch(num)
		{
			case 1:
				exit(0);
			case 2:
				menu();
				break;
			default:
				proceed_check();
		}
		break;
	}
}
/*FUNCAO MAE*/
void menu()																												{ 
	int num;
	char decision;
	system("cls");
	mainmenu();
	num=getInt();
	switch(num){
		case 1: /*Ver e Editar Quadro*/
			system("cls");
			boardmenu();
			num=getInt();
			break;
		case 2: /*Ver Tarefas*/
			system("cls");
			taskmenu();
			num=getInt();
			break;
		case 3: /*Ver Pessoas*/
			system("cls");
			personmenu();
			num=getInt();
			break;
		case 4: /*Sair*/
			proceed_check(num);
			exit(0);
	 	default:
	 		menu();
	}
}
void PrioritySortList(Node*h){ //Invertido for some reason
	Node*i;
	Node*j;
	int tempID,tempStatus,tempIDperson,tempPriority;
	char *tempDescription[100];
	int DiaInicio,MesInicio,AnoInicio,DiaPrazo,MesPrazo,AnoPrazo,DiaConclusao,MesConclusao,AnoConclusao;
	for(i=h; i->next!=NULL; i=i->next)
	{
		for(j=i->next; j!=NULL; j=j->next)
		{
			if (i->Priority > j->Priority)
			{
				tempID = i->ID;
				tempPriority = i->Priority;
				tempStatus = i->Status;
				tempIDperson = i->IDperson;
				strcpy(tempDescription,i->Description);
				DiaInicio=i->creation->Day;
				MesInicio=i->creation->Month;
				AnoInicio=i->creation->Year;
				DiaPrazo=i->deadline->Day;
				MesPrazo=i->deadline->Month;
				AnoPrazo=i->deadline->Year;
				DiaConclusao=i->conclusion->Day;
				MesConclusao=i->conclusion->Month;
				AnoConclusao=i->conclusion->Year;

				i->ID = j->ID;
				i->Priority = j->Priority;
				i->Status = j->Status;
				i->IDperson = j->IDperson;
				i->Description = strdup(j->Description);
				i->creation->Day = j->creation->Day;
				i->creation->Month = j->creation->Month;
				i->creation->Year = j->creation->Year;
				i->deadline->Day = j->deadline->Day;
				i->deadline->Month = j->deadline->Month;
				i->deadline->Year = j->deadline->Year;
				i->conclusion->Day = j->conclusion->Day;
				i->conclusion->Month = j->conclusion->Month;
				i->conclusion->Year = j->conclusion->Year;

				j->ID = tempID;
				j->Priority = tempPriority;
				j->Status = tempStatus;
				j->IDperson = tempIDperson;
				j->Description = strdup(tempDescription);
				j->creation->Day = DiaInicio;
				j->creation->Month = MesInicio;
				j->creation->Year = AnoInicio;
				j->deadline->Day = DiaPrazo;
				j->deadline->Month = MesPrazo;
				j->deadline->Year = AnoPrazo;
				j->conclusion->Day = DiaConclusao;
				j->conclusion->Month = MesConclusao;
				j->conclusion->Year = AnoConclusao;
			}
		}
	}
}
void PIDSortList(Node*h){ //Invertido for some reason
	Node*i;
	Node*j;
	int tempID,tempStatus,tempIDperson,tempPriority;
	char *tempDescription[100];
	int DiaInicio,MesInicio,AnoInicio,DiaPrazo,MesPrazo,AnoPrazo,DiaConclusao,MesConclusao,AnoConclusao;
	for(i=h; i->next!=NULL; i=i->next)
	{
		for(j=i->next; j!=NULL; j=j->next)
		{
			if (i->IDperson > j->IDperson)
			{
				tempID = i->ID;
				tempPriority = i->Priority;
				tempStatus = i->Status;
				tempIDperson = i->IDperson;
				strcpy(tempDescription,i->Description);
				DiaInicio=i->creation->Day;
				MesInicio=i->creation->Month;
				AnoInicio=i->creation->Year;
				DiaPrazo=i->deadline->Day;
				MesPrazo=i->deadline->Month;
				AnoPrazo=i->deadline->Year;
				DiaConclusao=i->conclusion->Day;
				MesConclusao=i->conclusion->Month;
				AnoConclusao=i->conclusion->Year;

				i->ID = j->ID;
				i->Priority = j->Priority;
				i->Status = j->Status;
				i->IDperson = j->IDperson;
				i->Description = strdup(j->Description);
				i->creation->Day = j->creation->Day;
				i->creation->Month = j->creation->Month;
				i->creation->Year = j->creation->Year;
				i->deadline->Day = j->deadline->Day;
				i->deadline->Month = j->deadline->Month;
				i->deadline->Year = j->deadline->Year;
				i->conclusion->Day = j->conclusion->Day;
				i->conclusion->Month = j->conclusion->Month;
				i->conclusion->Year = j->conclusion->Year;

				j->ID = tempID;
				j->Priority = tempPriority;
				j->Status = tempStatus;
				j->IDperson = tempIDperson;
				j->Description = strdup(tempDescription);
				j->creation->Day = DiaInicio;
				j->creation->Month = MesInicio;
				j->creation->Year = AnoInicio;
				j->deadline->Day = DiaPrazo;
				j->deadline->Month = MesPrazo;
				j->deadline->Year = AnoPrazo;
				j->conclusion->Day = DiaConclusao;
				j->conclusion->Month = MesConclusao;
				j->conclusion->Year = AnoConclusao;
			}
		}
	}
}
void matrixprint(){
	printf("\t\t\t// TO-DO //\n\t\t--------------------------\n\n");
	print_todomenu2();
	printf("\n\t\t\t// DOING //\n\t\t--------------------------\n\n");
	print_doingmenu();
	printf("\t\t\t// DONE //\n\t\t--------------------------\n\n");
	print_donemenu();

}

int main(){
	menu();
	return(0);
}

