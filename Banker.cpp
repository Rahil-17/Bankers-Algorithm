#include<bits/stdc++.h>
#include <fstream>
#include <string>
using namespace std;

struct process{
	int id;
	int arrival;
	int maxa[100];			//maximum allocation
	int alloc[100];			//already allocated
};

int processes, resources, q;
int max_arr;
string input,output;
multiset<pair<int,pair<int,vector<int> > > >query;		//to store all queries and sort them according to arrival time


void banker(vector <process> v,vector <int> available)
{
	ofstream my(output);
	vector <int> finish(100,0);							//which processes have finished
	vector <int> seq(query.size());						//to store safe sequence

	int count=0;								
	int time=0;											// time stamp
	
	multiset<pair<int,pair<int,vector<int> > > >:: iterator it;
	while(!query.empty())
	{
		bool found=false;
		int i;
		for(it=query.begin();it!=query.end();it++)		// traverse all queries	
		{
			vector<int> tp=it->second.second;
			for(i=0;i<processes;i++)					// get pid from query
			{
				if(tp[0]==v[i].id)
					break;
			}	
			if(finish[v[i].id]==0 && v[i].arrival<=time)	// check if legal
			{
				int j;
				for(j=0;j<resources;j++)					// can the query be satisfied?
					if(tp[j+1] > available[j] || (tp[j+1]+ v[i].alloc[j] > v[i].maxa[j]))
						break;

				if(j==resources)						// query is granted
				{
					int p;
					for(p=0;p<resources;p++)			// partial request or not
					{
						if(tp[p+1]+ v[i].alloc[p] < v[i].maxa[p])
							break;
					}	
					if(p==resources)				// full allocation and process then finishes
					{	
						for(int k=0;k<resources;k++)
							available[k]+=v[i].alloc[k];
						
						seq[count++]=v[i].id;
						finish[v[i].id]=1;
						found=true;
						//cout<<"Grant\n";
						my<<"Grant\n";
					}
					else							// partial request, decrease available and increas allocated
					{
						for(int k=0;k<resources;k++)
						{	
							available[k]-=tp[k+1];
							v[i].alloc[k]+=tp[k+1];
						}
						seq[count++]=v[i].id;
						found=true;
						//cout<<"Grant\n";
						my<<"Grant\n";
					}	
					query.erase(it);					
					
					multiset<pair<int,pair<int,vector<int>>>>::iterator it1;
					multiset<pair<int,pair<int,vector<int>>>>temp;
					for(it1=query.begin();it1!=query.end();it1++)		// to update the queries and sort them according to arrvial.
					{													// basically to have processes with process-id in increasing order
						int m;
						m=it1->first;
						if(m<=time)
							m++;
						
						pair<int,vector<int>>p1;
						pair<int,pair<int,vector<int>>>p2;
						p1=make_pair(it1->second.first,it1->second.second);
						p2=make_pair(m,p1);
						temp.insert(p2);	
					}

					query=temp;
					time++;
					break;
				}	
				else							//deny the process
				{
					//cout<<"Deny\n";
					my<<"Deny\n";
					
				}

			}	
		}
		if(found==false)						// if no process is eligble at that a particular time stamp
		{	
			
			multiset<pair<int,pair<int,vector<int>>>>::iterator it1;
			multiset<pair<int,pair<int,vector<int>>>>temp;
			for(it1=query.begin();it1!=query.end();it1++)
				{
					int m;
					m=it1->first;
					if(m<=time)
						m++;
					pair<int,vector<int>>p1;
					pair<int,pair<int,vector<int>>>p2;
					p1=make_pair(it1->second.first,it1->second.second);
					p2=make_pair(m,p1);
					temp.insert(p2);	
				}

				query=temp;
				time++;
		}
		if(found==false && time>max_arr)		// not safe	
		{
			//cout<<"Not safe";
			my<<"Not safe";
			return;
		}	
	}

	for(auto a:seq)							//safe sequence
	{	
		//cout<<a<<" ";
		my<<a;
		my<<" ";
	}
	return;		
}

int main(int argc,char* argv[])
{	
	
	if(argc!=3)
	{
		cout<<"kindly provide input output filenames\n";
		return 0;
	}	
	ifstream my;
	input=argv[1];
	output=argv[2];
	my.open(input);
	my>>processes;
	my>>resources;
	my>>q;

	vector <process> v(processes);			//vector of struct process
	vector <int> available(resources);
	
	for(int i=0;i<processes;i++)			//take pid
	{
		int x;
		my>>x;
		v[i].id=x;
	}	

	for(int i=0;i<processes;i++)		// take arrival time and also maintain max arrival time 
	{
		int x;
		my>>x;
		v[i].arrival=x;
		if(x>max_arr)
			max_arr=x;
	}

	for(int i=0;i<resources;i++)		// available vector
	{
		int x;
		my>>x;
		available[i]=x;
	}

	for(int i=0;i<processes;i++)		//max allocation
	{
		for(int j=0;j<resources;j++)
		{
			int x;
			my>>x;
			v[i].maxa[j]=x;
		}	
	}	
	
	for(int i=0;i<processes;i++)		// already allocated
	{
		for(int j=0;j<resources;j++)
		{
			int x;
			my>>x;
			v[i].alloc[j]=x;			
		}	
	}		
	
	for(int i=0;i<q;i++)				//populating multi-set with request queries
	{
		vector<int> temp(resources+1);
		int y;
		my>>y;
		int pid=y;
		temp[0]=y;

		for(int j=1;j<=resources;j++)
		{
			int x;
			my>>x;
			temp[j]=x;
		}	
		pair<int,vector<int>>p1;
		pair<int,pair<int,vector<int>>>p2;
		p1=make_pair(pid,temp);

		for(int i=0;i<processes;i++)
			if(v[i].id==pid)
				break;
			
		int arr11=v[i].arrival;
		p2=make_pair(arr11,p1);
		query.insert(p2);

			
	}
	banker(v,available);			// call banker algo
	return 0;
}
