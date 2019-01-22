#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>
#include <stdbool.h>

//gobalVariables
int P=10,R=5;

//defining Function lock and unlock
void sem_lock(struct sembuf sem_op, int sem_id)
{
    sem_op.sem_num = 0;
    sem_op.sem_op = -1;
    sem_op.sem_flg = 0;
    semop(sem_id, &sem_op, 1);
}
void sem_unlock(struct sembuf sem_op, int sem_id)
{
    sem_op.sem_num = 0;
    sem_op.sem_op = 1;
    sem_op.sem_flg = 0;
    semop(sem_id, &sem_op, 1);
}

// Banker's Algorithm Functions
void calNeedMatrix(int needMatrix[P][R], int maxRequestMatrix[P][R],int allocationMatrix[P][R])
{
    for (int i = 0 ; i < P ; i++)
    {
        for (int j = 0 ; j < R ; j++)
        {
             needMatrix[i][j] = maxRequestMatrix[i][j] - allocationMatrix[i][j];
        }
    }
}

bool find_Safe_Seq(int processes[], int avail[], int maxRequestMatrix[][R],
            int allocationMatrix[][R])
{
    struct sembuf sem_op;
    //defining Required matrix
    int needMatrix[P][R],safeSequenceProcess[P],workMatrix[R],flag=0;
    bool completedProcess[P];

    //Function call for needMatrix
    calNeedMatrix(needMatrix, maxRequestMatrix, allocationMatrix);

    //initializing completedProcess
    for(int i=0;i<P;i++){
        completedProcess[i]=false;
    }
    //updating workMatrix
    for (int i = 0; i < R ; i++)
        workMatrix[i] = avail[i];

    // while loop to get safe sequence
    while (flag< P)
    {
        bool flag1 = false;
        for (int p = 0; p < P; p++)
        {
            if (completedProcess[p] == 0)
            {
                int j;
                for (j = 0; j < R; j++)
                    if (needMatrix[p][j] > workMatrix[j]){
                        sem_lock(sem_op,p);
                        break;
                    }
                if (j == R)
                {
                    for (int k = 0 ; k < R ; k++)
                    {
                        workMatrix[k]=workMatrix[k]+allocationMatrix[p][k];
                    }
                    safeSequenceProcess[flag++] = p;
                    completedProcess[p] = 1;
                    flag1 = true;
                    sem_unlock(sem_op,p);
                }
            }
        }
        if (flag1 == false)
        {
            printf("System has no safe state");
            return false;
        }
    }
    printf("System is in safe state.\nSafe sequence is: ");
    for (int i = 0; i < P ; i++){
        printf("%d ",safeSequenceProcess[i]);
    }
    return true;
}

// Main Function
int main()
{
    //initializing shared memory for  max[] and alloc []
    int max_id=shmget(IPC_PRIVATE,100*sizeof(int),IPC_CREAT|0777);
    int alloc_id=shmget(IPC_PRIVATE,100*sizeof(int),IPC_CREAT|0777);
    int need_id=shmget(IPC_CREAT,100*sizeof(int),IPC_CREAT|0777);

    // shared memory array;
    int *max,*alloc;
    int semid;
    semid = semget(IPC_PRIVATE,1,IPC_CREAT|0777);
    struct sembuf maxmut;
    semctl(semid,0,SETVAL,1);
    int status;
    int pid1=fork();
    if(pid1 == 0)   //Child1
    {
        max=(int *)shmat(max_id,0,0);
        alloc=(int *)shmat(alloc_id,0,0);
        // printf("Enter the Maximum Resources for Process 1:");
        for(int i=0;i<5;i++){
            int x;
            // scanf("%d",&x);
            x = (rand()%15)+1;
            max[i]=x;
        }
        sem_unlock(maxmut,semid);
        // printf("Enter the Allocation Resources for Process 1:");
        for(int i=0;i<5;i++){
            int x;
            // scanf("%d",&x);
            x = (rand()%10)+1;
            alloc[i]=x;
        }
        shmdt(max);
        shmdt(alloc);

    }
    else
    {
        //wait(&status);
        int pid2=fork();
        if(pid2==0)     //Child 2
        {
            max=(int *)shmat(max_id,0,0);
            alloc=(int *)shmat(alloc_id,0,0);
            // printf("Enter the Maximum Resources for Process 2:");
            for(int i=5;i<10;i++){
                int x;
                // scanf("%d",&x);
                x = (rand()%5)+(rand()%5)+(rand()%5)+1;
                max[i]=x;
            }
            sem_unlock(maxmut,semid);
            printf("\n");
            // printf("Enter the Allocation Resources for Process 2:");
            for(int i=5;i<10;i++){
                int x;
                // scanf("%d",&x);
                x = (rand()%5)+(rand()%5)+1;
                alloc[i]=x;
            }

            shmdt(max);
            shmdt(alloc);

        }
        else
        {
           // wait(&status);
            int pid3=fork();
            if(pid3==0)     //Child 3
            {
                max=(int *)shmat(max_id,0,0);
                alloc=(int *)shmat(alloc_id,0,0);
                // printf("Enter the Maximum Resources for Process 3:");
                for(int i=10;i<15;i++){
                    int x;
                    // scanf("%d",&x);
                    x = (rand()%10)+(rand()%5)+1;
                    max[i]=x;
                }
                sem_unlock(maxmut,semid);
                printf("\n");
                // printf("Enter the Allocation Resources for Process 3:");
                for(int i=10;i<15;i++){
                    int x;
                    // scanf("%d",&x);
                    x = (rand()%10)+1;
                    alloc[i]=x;
                }
                shmdt(max);
                shmdt(alloc);
            }
            else
            {
                //wait(&status);
                int pid4=fork();
                if(pid4==0)     //Child 4
                {
                    max=(int *)shmat(max_id,0,0);
                    alloc=(int *)shmat(alloc_id,0,0);
                    // printf("Enter the Maximum Resources for Process 4:");
                    for(int i=15;i<20;i++){
                        int x;
                        // scanf("%d",&x);
                        x = (rand()%5)+(rand()%10)+1;
                        max[i]=x;
                    }
                    sem_unlock(maxmut,semid);
                    printf("\n");
                    // printf("Enter the Allocation Resources for Process 4:");
                    for(int i=15;i<20;i++){
                    int x;
                    // scanf("%d",&x);
                    x = (rand()%4)+(rand()%6)+1;
                    alloc[i]=x;
                    }
                    shmdt(max);
                    shmdt(alloc);
                }
            else
            {
                //wait(&status);
                int pid5=fork();
                if(pid5==0)     //Child 5
                {
                    max=(int *)shmat(max_id,0,0);
                   alloc=(int *)shmat(alloc_id,0,0);
                    // printf("Enter the Maximum Resources for Process 5:");
                    for(int i=20;i<25;i++){
                        int x;
                        // scanf("%d",&x);
                        x = (rand()%6)+(rand()%9)+1;
                        max[i]=x;
                    }
                    sem_unlock(maxmut,semid);
                    // printf("\nEnter the Allocation Resources for Process 5:");
                    for(int i=20;i<25;i++){
                        int x;
                        // scanf("%d",&x);
                        x = (rand()%3)+(rand()%7)+1;
                        alloc[i]=x;
                }
                shmdt(max);
                shmdt(alloc);
            }
            else
                {
                    //wait(&status);
                    int pid6=fork();
                    if(pid6==0)     //Child 6
                    {
                        max=(int *)shmat(max_id,0,0);
                        alloc=(int *)shmat(alloc_id,0,0);
                        // printf("Enter the Maximum Resources for Process 6:");
                        for(int i=25;i<30;i++){
                            int x;
                            // scanf("%d",&x);
                            x = (rand()%4)+(rand()%11)+1;
                            max[i]=x;
                        }
                        sem_unlock(maxmut,semid);
                        printf("\n");
                        // printf("Enter the Allocation Resources for Process 6:");
                        for(int i=25;i<30;i++){
                            int x;
                            // scanf("%d",&x);
                            x = (rand()%2)+(rand()%8)+1;
                            alloc[i]=x;
                        }
                        shmdt(max);
                        shmdt(alloc);
                    }
            else{
                    //wait(&status);
                    int pid7=fork();
                    if(pid7==0)     //Child 7
                    {
                        max=(int *)shmat(max_id,0,0);
                        alloc=(int *)shmat(alloc_id,0,0);
                        // printf("Enter the Maximum Resources for Process 7:");
                        for(int i=30;i<35;i++){
                            int x;
                            // scanf("%d",&x);
                            x = (rand()%12)+(rand()%3)+1;
                            max[i]=x;
                        }
                        sem_unlock(maxmut,semid);
                        printf("\n");
                        // printf("Enter the Allocation Resources for Process 7:");
                        for(int i=30;i<35;i++){
                            int x;
                            // scanf("%d",&x);
                            x = (rand()%6)+(rand()%4)+1;
                        alloc[i]=x;
                        }
                        shmdt(max);
                        shmdt(alloc);
                    }
                else
                {
                    //wait(&status);
                    int pid8=fork();
                    if(pid8==0)     //Child8
                    {
                        max=(int *)shmat(max_id,0,0);
                        alloc=(int *)shmat(alloc_id,0,0);
                        // printf("Enter the Maximum Resources for Process 8:");
                        for(int i=35;i<40;i++){
                            int x;
                            // scanf("%d",&x);
                            x = (rand()%8)+(rand()%5)+1;
                            max[i]=x;
                        }
                        sem_unlock(maxmut,semid);
                        // printf("\nEnter the Allocation Resources for Process 8:");
                        for(int i=35;i<40;i++){
                            int x;
                            // scanf("%d",&x);
                            x = (rand()%6)+(rand()%5)+1;
                            alloc[i]=x;
                        }
                        shmdt(max);
                        shmdt(alloc);
                    }
                else
                {
                   // wait(&status);
                    int pid9=fork();
                    if(pid9==0)     //Child 9
                    {
                        max=(int *)shmat(max_id,0,0);
                        alloc=(int *)shmat(alloc_id,0,0);
                        // printf("Enter the Maximum Resources for Process 9:");
                        for(int i=40;i<45;i++){
                        int x;
                        // scanf("%d",&x);
                        x = (rand()%9)+(rand()%8)+1;
                        max[i]=x;
                    }
                    sem_unlock(maxmut,semid);
                    // printf("\nEnter the Allocation Resources for Process 9:");
                    for(int i=40;i<45;i++){
                        int x;
                        // scanf("%d",&x);
                        x = (rand()%7)+(rand()%5)+1;
                        alloc[i]=x;
                    }
                    shmdt(max);
                    shmdt(alloc);
                }
                else
                {
                   // wait(&status);
                    int pid10=fork();
                    if(pid10==0)     //Child 10
                    {
                        max=(int *)shmat(max_id,0,0);
                        alloc=(int *)shmat(alloc_id,0,0);
                        // printf("Enter the Maximum Resources for Process 10:");
                        for(int i=45;i<50;i++){
                            int x;
                            // scanf("%d",&x);
                            x = (rand()%7)+(rand()%9)+1;

                            max[i]=x;

                        }
                        sem_unlock(maxmut,semid);
                        // printf("\nEnter the Allocation Resources for Process 10:");
                        for(int i=45;i<50;i++){
                            int x;
                            // scanf("%d",&x);
                            x = (rand()%7)+(rand()%7)+1;
                            alloc[i]=x;
                        }
                        shmdt(max);
                        shmdt(alloc);
                    }
                else{       //Parent
                       // wait(&status);
                       int i=0;

                       while(i<10){
                           sem_lock(maxmut,semid);
                           i++;
                       }
                        int Available[5],alloc_matrix[10][5],max_maxtrix[10][5];
                        int process[10]={0,1,4,3,2,5,6,7,9,8};
                        printf("Enter the Available Resources\n");
                        for(int i=0;i<5;i++){
                            int n;
                            scanf("%d",&Available[i]);
                        }
                        alloc=(int *)shmat(alloc_id,0,0);
                        max=(int *)shmat(max_id,0,0);
                        //loop for converting single shared array into Matrix
                    //     for(int i=0,k=0,j=0;i<50;i++,j++)
                    //     {
                    //         if(i%5==0 && i!=0){
                    //         k++;j=0;
                    //     }
                    //     max_maxtrix[k][j]=max[i];
                    //     alloc_matrix[k][j]=alloc[i];
                    // }
                    int count=0;
                    for(int i=0;i<10;i++){
                        for(int j=0;j<5;j++){
                            max_maxtrix[i][j]=max[count];
                            alloc_matrix[i][j]=alloc[count];
                            count++;
                        }
                    }
                    printf("\n");
                    printf("\n");
                    printf("This is Max matrix\n");
                    for (int i = 0; i < 10; i++)
                    {
                        for (int j = 0; j < 5; j++)
                        {
                            printf("%d\t",max_maxtrix[i][j] );
                        }
                        printf("\n");
                    }
                    printf("\n");
                    printf("\n");
                    printf("This is Allocation matrix\n");
                    for (int i = 0; i < 10; i++)
                    {
                        for (int j = 0; j < 5; j++)
                        {
                            printf("%d\t",alloc_matrix[i][j] );
                        }
                        printf("\n");
                    }
                    printf("\n");
                    printf("\n");
                find_Safe_Seq(process, Available, max_maxtrix, alloc_matrix);
            }
        }
    }
}
}
}
}
}
}
}
}

/*
Sample TestCase
      1 1 1 1 1
      0 0 0 0 0

      1 1 1 1 1
      0 0 0 0 0

      1 1 1 1 1
      0 0 0 0 0

      1 1 1 1 1
      0 0 0 0 0

      1 1 1 1 1
      0 0 0 0 0

      1 1 1 1 1
      0 0 0 0 0

      1 1 1 1 1
      0 0 0 0 0

      1 1 1 1 1
      0 0 0 0 0

      1 1 1 1 1
      0 0 0 0 0

      1 1 1 1 1
      0 0 0 0 0

      1 1 1 1 1
      0 0 0 0 0
Available
    10 10 10 10 10
*/
