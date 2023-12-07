#include<bits/stdc++.h>
#include<pthread.h>
#include<semaphore.h>
#include<unistd.h>
#include<cstdio>
#include<random>
int N ;
sem_t count , t1, t2;
int c1 , c2 , x , p;
void* th1(void* args) {
    while(p>0) {
        sem_wait(&t1) ;
        c1-- ;
        printf("_") ;
        if(c1 == 0) {
            sem_post(&t2) ;   
        }
        else sem_post(&t1) ;
    }

}

void* th2(void* args) {
    while(p>0) {
        sem_wait(&t2);
        c2-- ;
        printf("+") ;
        if(c2 == 0) {
            x-- ;
            c1 = x ;
            c2 = 2*N - c1 ;
            p-- ;
            printf("\n") ;
            sem_post(&t1) ;
        }
        else sem_post(&t2) ;
    }

}


int main() {
    std::cin >> N ;
    sem_init(&count, 0, N+N) ;
    sem_init(&t1, 0 , 1) ;
    sem_init(&t2, 0, 0) ;
    c1 = 2*N-1 ;
    c2 = 1 ;
    x = c1 ;
    p = N ;

    pthread_t _t1,_t2,_t3 ;
    pthread_create(&_t1,NULL,th1,NULL) ;
    pthread_create(&_t2,NULL,th2,NULL) ;

    pthread_join(_t1,NULL) ;
    pthread_join(_t2,NULL) ; 

}