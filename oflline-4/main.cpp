#include<bits/stdc++.h>
#include<pthread.h>
#include<semaphore.h>
#include<unistd.h>
#include<cstdio>

#define PRINTER_NUM 4 
int N = 100 ,M,w,x,y ;

struct std_info {
    int group_num ;
    int student_id ;
    int leader_id ;
    sem_t std_semaphore ;
};

time_t st ;

std::vector<int>printers[PRINTER_NUM] ;
std::vector<pthread_mutex_t>printer_lock(PRINTER_NUM) ;
std::vector<int>grp_size ;
bool nfree_printer[PRINTER_NUM] ;


//semaphores
sem_t binding_locks ;
std::vector<sem_t>leader_locks ;
std::vector<sem_t> std_locks ;


void* execute(void* args) {
    time_t end ;
    std_info* info  = (std_info*) args ;
    sem_wait(&std_locks[info->student_id]) ;
    
    //-------------Locking state---------//
    pthread_mutex_lock(&printer_lock[info->student_id % PRINTER_NUM]) ;
    time(&end) ;
    printf("Student %d has arrived at the print station %d at time %d\n",info->student_id,info->student_id % PRINTER_NUM,end - st) ;
    fflush(stdout);
    pthread_mutex_unlock(&printer_lock[info->student_id % PRINTER_NUM]) ;
    

    sleep(w) ;
    
    //pthread_mutex_lock(&printer_lock[info->student_id % PRINTER_NUM]) ;
    time(&end) ;
    nfree_printer[info->student_id % PRINTER_NUM] = false ;
    printf("Student %d has finished printing at time %d\n",info->student_id,end - st) ;
    grp_size[info->group_num]-- ;
    fflush(stdout);
    
    
    
    //-------------------------thinking state---------------------
    if(grp_size[info->group_num] == 0) sem_post(&leader_locks[info->group_num]) ;
    for(int j = 0 ; j < printers[info->student_id % PRINTER_NUM].size() ; j++) {
        if((printers[info->student_id % PRINTER_NUM][j])/M == info->group_num ) {
            sem_post(&std_locks[printers[info->student_id % PRINTER_NUM][j]]) ;
            sleep(1+rand()%2) ;
            //printf("here%d\n",printers[info->student_id % PRINTER_NUM][j]) ;
        }
    }

    for(int j = 0 ; j < printers[info->student_id % PRINTER_NUM].size() ; j++) {
        if((printers[info->student_id % PRINTER_NUM][j])/M != info->group_num ) {
            sem_post(&std_locks[printers[info->student_id % PRINTER_NUM][j]]) ;
            sleep(1+rand()%2) ;
        }
    }

    if(info->student_id != info->leader_id) {
        delete info ;
        return (void*) 1 ;
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


    delete info ;
    return (void*) 1 ;
    //pthread_exit(0) ;

}

void init() {
    time(&st) ;
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
        sem_init(&std_locks[arr[i]], 0 , 1) ;
        if(nfree_printer[arr[i]%PRINTER_NUM] == false) {
            nfree_printer[arr[i]%PRINTER_NUM] = true ;
        }
        else {
            sem_wait(&std_locks[arr[i]]) ;
        }

        printers[arr[i]%PRINTER_NUM].push_back(arr[i]) ;

        //pthread_t t ;
        std_info* inf  = new std_info ;
        *inf = info ;

        pthread_create(&t[arr[i]] , NULL , execute , inf) ;
        //std::cout << i <<'\n' ;
        
    }

    for(int i = 0 ; i < N ;i++) {
        pthread_join(t[arr[i]],NULL) ;
    }
}

int main(){

    std::cin >> N >> M >> w >> x >> y ;
    grp_size = std::vector<int> (N/M) ;
    std_locks = std::vector<sem_t>(N) ;
    leader_locks = std::vector<sem_t>(N/M) ;

    for(int i = 0 ; i < N/M ; i++) {
        sem_init(&leader_locks[i] , 0 , 1) ;
        sem_wait(&leader_locks[i]) ;
        grp_size[i] = M ;
    }
    sem_init(&binding_locks,0,2) ;

    init() ;
}