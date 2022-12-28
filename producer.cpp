#include <iostream>
#include <bits/stdc++.h>
#include <random>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include<unistd.h>   
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <chrono>
#include <sys/time.h>
using namespace std;

map<string,int> mp;

void print_msg(string msg)
{
    timeval jetzt;
    gettimeofday(&jetzt,NULL);
    char time_string[64];
    strftime(time_string,100,"%m/%d/%Y %H:%M:%S",localtime(&jetzt.tv_sec));
    cerr<<"["<<time_string<<"."<<jetzt.tv_usec<<"]---> "<<msg<<" "<<endl;
}
void sem_signal_wait(int semaphore_id,int index, int signal)
{
    struct sembuf semaphore_operator;
    semaphore_operator.sem_num = index;
    semaphore_operator.sem_op = signal;   
    semaphore_operator.sem_flg = 0;
    semop(semaphore_id, &semaphore_operator, 1);
} 

int main(int argc, char** argv)
{

mp["ALUMINIUM"]=1;
mp["COPPER"]=2;
mp["COTTON"]=3;
mp["CRUDEOIL"]=4;
mp["GOLD"]=5;
mp["LEAD"]=6;
mp["MENTHAOIL"]=7;
mp["NATURALGAS"]=8;
mp["NICKEL"]=9;
mp["SILVER"]=10;
mp["ZINC"]=11;
string name;
double mean,variance;
int time;
name=argv[1];
mean=stod(argv[2]);
variance=stod(argv[3]);
time=stoi(argv[4]);
int buffer_size=stoi(argv[5]);
std::default_random_engine generator;std::normal_distribution<double> distribution(mean,variance);
int semaphore_id = semget(250, 3, IPC_CREAT | 0666);
if(semaphore_id<0)
{
cout<<"INVALID SYSTEM CALL --> SEMAPHORE ALLOCATION"<<endl;
exit(1);
}
int flag=1;
while(flag)
{

//produce
    double number = distribution(generator);
    print_msg(name+": generated a random price "+to_string(number));
    sem_signal_wait(semaphore_id,2,-1);
    sem_signal_wait(semaphore_id,0,-1);
    //append
    print_msg(name+": trying to get mutex on shared buffer !");
    key_t key = ftok("mohamed",200);
    int shared_memory_id = shmget(key,buffer_size*sizeof(pair<pair<int,double>,int>),0666|IPC_CREAT);
    if(shared_memory_id<0)
    {
    cout<<"ERROR IN SHMGET COULDN'T ALLOCATE SHARED MEMORY"<<endl;
    exit(1);
    }
    pair<pair<int,double>,int> * arr = (pair<pair<int,double>,int> *) shmat(shared_memory_id,(void*)0,0);
    
    for(int i=1;i<buffer_size;i++)
    {
    if(arr[i].second==0)
    {
      print_msg(name+": placing "+to_string(number)+" on shared buffer");
      arr[i].first.first=mp[name];
      arr[i].first.second=number;
      arr[i].second=arr[0].second; 
      arr[0].second+=1;
      break;  
    }
    else {
    //cout<<"NO"<<endl;
    }
    }
    shmdt(arr);  
    sem_signal_wait(semaphore_id,0,1);
    sem_signal_wait(semaphore_id,1,1);
    print_msg(name+": sleeping for "+to_string(time)+ " seconds");
    cout<<"------------------------------------------------"<<endl;
    sleep(time);
}

}

