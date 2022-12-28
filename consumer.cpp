#include <iostream>
#include <bits/stdc++.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include<unistd.h>   
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
using namespace std;

map<int,vector<double>> commodities;
map<int,double> current_price;
map<int,double> last_price;
map<int,double> current_average;
map<int,double> last_average;
map<int,string> mp;
void print_stats()
{
cout<<"+-----------------------------------+"<<endl;
cout<<"| Currency     |  Price  | AvgPrice |"<<endl;
cout<<"+-----------------------------------+"<<endl;
for(int i=1;i<12;i++)
{
cout<<"| "<<mp[i]<<"  |";(current_price[i]==0)?cout<<"\033[;34m":(last_price[i]>current_price[i])?cout<<"\033[;31m":cout<<"\033[;32m";printf("%7.2lf",commodities[i][commodities[i].size()-1]);(current_price[i]==0)?cout<<" ":(last_price[i]>current_price[i])?cout<<"↓":cout<<"↑";cout<<"";cout<<"\033[;37m |";(current_average[i]==0)?cout<<"\033[;34m":(last_average[i]>current_average[i])?cout<<"\033[;31m":cout<<"\033[;32m";printf("%7.2lf",current_average[i]);(current_average[i]==0)?cout<<" ":(last_average[i]>current_average[i])?cout<<"↓":cout<<"↑";cout<<"\033[;37m  |"<<endl;
}
cout<<"+-----------------------------------+"<<endl;
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

mp[1]="ALUMINIUM  ";
mp[2]="COPPER     ";
mp[3]="COTTON     ";
mp[4]="CRUDEOIL   ";
mp[5]="GOLD       ";
mp[6]="LEAD       ";
mp[7]="MENTHAOIL  ";
mp[8]="NATURALGAS ";
mp[9]="NICKEL     ";
mp[10]="SILVER     ";
mp[11]="ZINC       ";


for(int i=1;i<=11;i++)
{
commodities[i].push_back(0);
}

int buffer_size=stoi(argv[1]);
key_t key = ftok("mohamed",200);
int shared_memory_id = shmget(key,((buffer_size)*sizeof(pair<pair<int,double>,int>)),0666|IPC_CREAT);
if(shared_memory_id<0)
{
 cout<<"ERROR IN SHMGET COULDN'T ALLOCATE SHARED MEMORY"<<endl;
 exit(1);
}
pair<pair<int,double>,int> * arr = (pair<pair<int,double>,int> *) shmat(shared_memory_id,(void*)0,0);
arr[0].first.first=buffer_size;
arr[0].first.second=0;
arr[0].second=1;
for(int i=1;i<buffer_size;i++)
{
arr[i].first.first=0;
arr[i].first.second=0;
arr[i].second=0;
}

int semaphore_id = semget(250, 3, IPC_CREAT | 0666);
int s,n,e;
s = semctl(semaphore_id, 0, SETVAL, 1);
n = semctl(semaphore_id, 1, SETVAL, 0);
e= semctl(semaphore_id, 2, SETVAL, buffer_size);
while(true)
{
    sem_signal_wait(semaphore_id,1,-1);
    sem_signal_wait(semaphore_id,0,-1);
     

      key_t key = ftok("mohamed",200);
	int shared_memory_id = shmget(key,(buffer_size)*sizeof(pair<pair<int,double>,int>),0666|IPC_CREAT);
	if(shared_memory_id<0)
	{
         cout<<"ERROR IN SHMGET COULDN'T ALLOCATE SHARED MEMORY"<<endl;
	exit(1);
	}

pair<pair<int,double>,int> * arr = (pair<pair<int,double>,int> *) shmat(shared_memory_id,(void*)0,0);
cout<<"---------------------------------"<<endl;
int min_index=buffer_size;
cout<<"----------"<<arr<<"--------"<<endl;
int value=INT_MAX;
for(int i=1;i<buffer_size;i++)
{

cout<<arr[i].first.first<<" "<<arr[i].first.second<<" "<<arr[i].second<<endl;

if(arr[i].second!=0)
  {
if(arr[i].second<value)
    {

    value=arr[i].second;
    min_index=i;
    }
  }
}
commodities[arr[min_index].first.first].push_back(arr[min_index].first.second);
current_price[arr[min_index].first.first]=arr[min_index].first.second;
last_price[arr[min_index].first.first]=commodities[arr[min_index].first.first][commodities[arr[min_index].first.first].size()-2];
last_average[arr[min_index].first.first]=current_average[arr[min_index].first.first];
double average=0;
int cnt=0;
for(int i=commodities[arr[min_index].first.first].size()-1;i>0 && cnt<4;i--)
{
average+=commodities[arr[min_index].first.first][i];
cnt++;
}
current_average[arr[min_index].first.first]=(double) average/(cnt*1.0);
arr[min_index].first.first=0;
arr[min_index].first.second=0;
arr[min_index].second=0;
 system("clear");
 printf("\e[1;1H\e[2J");
 print_stats();
/* cout<<"--out--"<<endl;
 for(int i=1;i<buffer_size;i++)
cout<<arr[i].first.first<<" "<<arr[i].first.second<<" "<<arr[i].second<<endl;
 */
 
    shmdt(arr);
    sem_signal_wait(semaphore_id,0,1);
    sem_signal_wait(semaphore_id,2,1);

///consume();
}

}

