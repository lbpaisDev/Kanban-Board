#ifndef HEADER_H_INCLUDED
#define HEADER_H_INCLUDED

typedef struct date{
        int Day;
        int Month;
        int Year;
}Date;

typedef struct person{
        int ID_person;
        char Name;
        char Email;
        struct person*next;
}Person;

typedef struct l_person{
        int n_person;
        Person*next;
}l_person;


typedef struct Inode2*Task;										
typedef struct Inode2{
        int ID;
        int Priority;
        int Status;
        char *Description;
        Person *person;
        Date   *creation;
        Date   *deadline;
        Date   *conclusion;	
									
        Task next;
   		Task previous;
}Task_node2;

#endif
