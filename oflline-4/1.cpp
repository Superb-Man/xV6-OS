#include<bits/stdc++.h>
#include<pthread.h>
#include<semaphore.h>
#include<unistd.h>
#include<cstdio>

//threads
void *staff(void* ) ;
void* execute(void* ) ;


//functions
void aquire_station(int) ;
void leave_station(int) ;
void test(int id , int d) ;



#define PRINTER_NUM 4 
#define BINDING_STATION 2
#define STAFF_NUMBER 2 


int N ,M,w,x,y ;
int reader  = 0 , submission =  0;

struct std_info {
    int group_num ;
    int student_id ;
    int leader_id ;
    sem_t std_semaphore ;
};

time_t st ;

std::vector<int>printers[PRINTER_NUM] ;
int* grp_size ;
bool nfree_printer[PRINTER_NUM] ;


//mutex

pthread_mutex_t p_lock ;
pthread_mutex_t printer_lock[PRINTER_NUM] ;
sem_t wrt , mutex ;


//semaphores
sem_t binding_locks ;
sem_t* leader_locks ;
sem_t* std_locks ;



//----------------------------------Function and thread definitions-------------------------------//
void aquire_station(int id) {
    time_t end ;
    pthread_mutex_lock(&p_lock) ;
    

    int idx = 0 ;
    for(int j = 0 ; j < printers[id % PRINTER_NUM].size() ; j++) {
        if(printers[id % PRINTER_NUM][j] == id) {
            idx = j ;
            break ;
        }
    }

    
    test(id,idx) ;

    time(&end) ;
    printf("Student %d has arrived at the print station %d at time %d\n",id,id % PRINTER_NUM,end - st) ;
    fflush(stdout);

    pthread_mutex_unlock(&p_lock) ;
    sem_wait(&std_locks[id]) ;
}

void leave_station(int id , int g_id) {
    // printf("ENTERING leave station phase\n") ;
    // printf("Printer id %d : with size %d\n",id%PRINTER_NUM,printers[id % PRINTER_NUM].size()) ;
    pthread_mutex_lock(&p_lock) ;
    nfree_printer[id % PRINTER_NUM] = false ;

    int idx = 0 ;
    for(int j = 0 ; j < printers[id % PRINTER_NUM].size() ; j++) {
        if(printers[id % PRINTER_NUM][j] == id) {
            idx = j ;
            break ;
        }
    }


    //Notifying the same team -members
    for(int j = 0 ; j < printers[id % PRINTER_NUM].size() ; j++) {
        if((printers[id % PRINTER_NUM][j])/M == g_id ) {
            test(printers[id % PRINTER_NUM][j],idx) ;
            printf("Student %d has sent text to him team-mate %d\n",id, printers[id % PRINTER_NUM][j]) ;
        }
    }

    //notifying the different team members
    for(int j = 0 ; j < printers[id % PRINTER_NUM].size() ; j++) {
        if((printers[id % PRINTER_NUM][j])/M != g_id ) {
            test(printers[id % PRINTER_NUM][j],idx) ;
            printf("Student %d has sent text to his non team-mate %d\n",id, printers[id % PRINTER_NUM][j]) ;
        }
    }
    grp_size[g_id]-- ;
    if(grp_size[g_id] == 0) sem_post(&leader_locks[g_id]) ;
    nfree_printer[id % PRINTER_NUM] = false ;
    pthread_mutex_unlock(&p_lock) ;
}

void test(int id, int d) {
    if(nfree_printer[id % PRINTER_NUM]  == false) {
        nfree_printer[id % PRINTER_NUM] = true ;
        sem_post(&std_locks[id]) ;
        printers[id % PRINTER_NUM].erase(printers[id % PRINTER_NUM].begin() + d) ;
    }
}



void* staff(void* args) {
    while(true){
        sleep(1+rand()%3) ;
        time_t end ;
        int* id = (int*) args ;
        sem_wait(&mutex) ;
            reader++ ;
        if(reader == 1) {
            sem_wait(&wrt) ;
        }
        //sleep(1+rand()%3) ;
        sem_post(&mutex) ;
        time(&end) ;
        printf("Staff %d has started reading the entry book at time %d. No. of submission = %d\n",*id , end - st , submission);
        //sleep(1+rand()%3) ;
        sem_wait(&mutex) ;
        reader-- ;
        if(reader == 0) {
            //sleep(1) ;
            sem_post(&wrt) ;
        }

        //sleep(1+rand()%3) ;
        sem_post(&mutex) ;

        if(submission == N/M) break ; 
    }

    return (void*) NULL ;

}

void* execute(void* args) {
    time_t end ;
    std_info* info  = (std_info*) args ;
    // sem_wait(&std_locks[info->student_id]) ;

    // //-------------Locking state---------//
    // pthread_mutex_lock(&printer_lock[info->student_id % PRINTER_NUM]) ;
    // time(&end) ;
    // printf("Student %d has arrived at the print station %d at time %d\n",info->student_id,info->student_id % PRINTER_NUM,end - st) ;
    // fflush(stdout);
    // pthread_mutex_unlock(&printer_lock[info->student_id % PRINTER_NUM]) ;
    

    aquire_station(info->student_id) ;

    sleep(w) ;
    time(&end) ;
    printf("Student %d has finished printing at time %d\n",info->student_id,end - st) ;
    fflush(stdout);

    leave_station(info->student_id , info->group_num) ;
    
    
    
    //-------------------------thinking state---------------------//
    // if(grp_size[info->group_num] == 0) sem_post(&leader_locks[info->group_num]) ;
    // for(int j = 0 ; j < printers[info->student_id % PRINTER_NUM].size() ; j++) {
    //     if((printers[info->student_id % PRINTER_NUM][j])/M == info->group_num ) {
    //         sem_post(&std_locks[printers[info->student_id % PRINTER_NUM][j]]) ;
    //         sleep(1+rand()%2) ;
    //         //printf("here%d\n",printers[info->student_id % PRINTER_NUM][j]) ;
    //     }
    //     //else sem_wait(&std_locks[printers[info->student_id % PRINTER_NUM][j]]) ;
    // }

    // for(int j = 0 ; j < printers[info->student_id % PRINTER_NUM].size() ; j++) {
    //     if((printers[info->student_id % PRINTER_NUM][j])/M != info->group_num ) {
    //         sem_post(&std_locks[printers[info->student_id % PRINTER_NUM][j]]) ;
    //         sleep(1+rand()%2) ;
    //     }
    // }

    if(info->student_id != info->leader_id) {
        delete info ;
        return (void*) NULL ;
    }

    //--------------------------No busy waiting--------------------//

    sem_wait(&leader_locks[info->group_num]) ;
    time(&end) ;
    printf("Group %d has finished printing at time %d\n",info->group_num , end -st) ;
    sleep(1+rand()%10) ;
    fflush(stdout);
    sem_wait(&binding_locks) ;

    //-----------------------------Binding phase------------------------//
    time(&end) ;
    printf("Group %d has started binding at time %d\n",info->group_num,end - st) ;
    fflush(stdout);
    sem_post(&binding_locks) ;
    //sem_post(&leader_locks[info->group_num]) ;
    sleep(x) ;

    //sem_wait(&binding_locks) ;
    time(&end) ;
    printf("Group %d has finished binding at time %d\n",info->group_num,end - st) ;
    fflush(stdout);
    //sem_wait(&binding_locks) ;

    //--------------------------------------TORIQE---------------------------------//


    sem_wait(&wrt) ;
    time(&end) ;
    printf("Group %d has started writing to the entry book at time %d\n",info->group_num , end -st) ;
    sleep(y) ;
    submission++ ;
    time(&end) ;
    printf("Group %d has finished writing to the entry book at time %d\n",info->group_num , end - st) ;
    sleep(1+rand()%3) ;
    sem_post(&wrt) ;

    delete info ;
    return (void*) NULL ;
    //pthread_exit(0) ;

}

void init() {
    pthread_t t[N] ;
    int arr[N] ; 
    for(int i = 0 ; i <N ;i++) arr[i] = i ;
    for (int i = N-1; i >= 0; i--) {
        int j = rand() % (i + 1);
        int temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
    }

    for(int i = 0 ; i <N ; i++) std::cout <<arr[i] <<" " ;
    std::cout << '\n' ;
    for(int i = 0 ; i < N ; i++) {
        sleep(1+rand()%3) ;
        //std::cout << "i = " <<i <<'\n' ;
        std_info info ;
        info.group_num = arr[i]/M ;
        info.student_id = arr[i] ;
        info.leader_id = (arr[i]/M)*M + M - 1 ;
        sem_init(&std_locks[arr[i]], 0 , 0) ;
        // if(nfree_printer[arr[i]%PRINTER_NUM] == false) {
        //     nfree_printer[arr[i]%PRINTER_NUM] = true ;
        // }
        // else {
        //     sem_wait(&std_locks[arr[i]]) ;
        // }


        printers[arr[i]%PRINTER_NUM].push_back(arr[i]) ;

        std_info* inf  = new std_info ;
        *inf = info ;

        pthread_create(&t[arr[i]] , NULL , execute , inf) ;
        //std::cout << i <<'\n' ;
        
    }

    for(int i = 0 ; i < N ;i++) {
        pthread_join(t[arr[i]],NULL) ;
    }
    for(int i = 0; i <N ; i++) {
        sem_destroy(&std_locks[i]) ;
    }
    delete []std_locks ;
}

int main(){
    time(&st) ;
    std::cin >> N >> M >> w >> x >> y ;
    grp_size = new int[N/M] ;//std::vector<int> (N/M) ;
    std_locks = new sem_t[N] ; //std::vector<sem_t>(N) ;
    leader_locks = new sem_t[N/M] ; //std::vector<sem_t>(N/M) ;

    for(int i = 0 ; i < N/M ; i++) {
        sem_init(&leader_locks[i] , 0 , 1) ;
        sem_wait(&leader_locks[i]) ;
        grp_size[i] = M ;
    }

    for(int i =  0; i < PRINTER_NUM ; i++) {
        pthread_mutex_init(&printer_lock[i] , NULL) ;
    }
    

    pthread_mutex_init(&p_lock , NULL) ;
    sem_init(&wrt,0,1) ;
    sem_init(&mutex,0,1) ;
    sem_init(&binding_locks,0,BINDING_STATION) ;

    pthread_t staf[STAFF_NUMBER] ;
    for(int i =  0 ; i < STAFF_NUMBER ; i++) {
        sleep(1) ;
        int* p = new int ;
        *p = i ;
        pthread_create(&staf[i] , NULL , staff , p) ;
    }

    init() ;

    for(int i = 0 ; i < STAFF_NUMBER ; i++) {
        pthread_join(staf[i], NULL) ;
    }

    for(int i = 0 ; i < N;i++) {
        //free(&)
    }
}