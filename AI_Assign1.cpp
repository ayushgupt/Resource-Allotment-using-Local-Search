#include <vector>
#include <string>
#include <fstream>
#include <cmath>
#include <ctime>
#include <sstream>
#include <cstdlib>
#include <iostream>
#include <algorithm>
#define max 10000
#define all(c) (c).begin(),(c).end() 
using namespace std;


struct node // bid node structure
{
	int cid; //company
	double price;  
	int norc;  // no. of regions in one bid
	int region[max];
};
static int intersection_dummy_vector[100000];
float tim; 
int nor;
int nob;
int noc;
struct node tob[max]; // total no. of bids

vector< vector<int> > company_bid_list; //stores all the bids corresponding to a company

bool com[max],reg[max]; //keeps record of which companies and regions can be selected in the remaining unprocessed bids
bool bid[max]; // final bids

vector<int> region_bid_index;
vector<int> company_bid_considered;//stores the bid id of the considered bid for each company in the current state
vector<int> unallotted_regions_list;

void readFile()
{
	string g;
	scanf("%f\n\n",&tim);
	scanf("%d\n\n",&nor);
	scanf("%d\n\n",&nob);
	scanf("%d\n\n",&noc);
	region_bid_index.resize(nor, -1);
	company_bid_considered.resize(noc,-1);
	for(int i=0;i<nob;i++)
	{
		cout<<flush;
		string ch;
		getline(cin,ch);
		int t=0;int j=0;
		char ch1[max];
		while(ch[t]!=' ')
		{
			ch1[j]=ch[t];
			j++;t++;
		}
		
		ch1[j]='\0';
		tob[i].cid=atoi(ch1);
	
		ch1[0]='\0';j=0;t++;
		while(ch[t]!=' ')
		{
			ch1[j]=ch[t];
			j++;t++;
		}
		ch1[j]='\0';			
		tob[i].price=strtod (ch1, NULL);
		t++;
		
		int x=0;
		int w=t;		
		while(ch[t]!='#')
		{
			if(ch[t]==' ')
			{	x++;}
			t++;
		}
		tob[i].norc=x;
		t=w;
		for(int qq=0;qq<x;qq++)
		{
			ch1[0]='\0';j=0;
			while(ch[t]!=' ')
			{
				ch1[j]=ch[t];
				j++;t++;
			}
			t++;
			ch1[j]='\0';
			tob[i].region[qq]=atoi(ch1);
		}
		sort(tob[i].region,tob[i].region+x);//sorting the region ids in increasing order of index
		getline(cin,g);	
	}
}
void make_company_bid_list()
{
	//builds the company_bid_list
	company_bid_list.resize( noc , vector<int>( 0 , -100 ));
	for(int i=0;i<nob;i++)
	{
		company_bid_list[tob[i].cid].push_back(i);
	}
}
void print_company_bid_lists()
{
	for (int i = 0; i < company_bid_list.size(); i++)
	{
		cout<<"Company"<<i<<" : ";
		for (int j = 0; j < company_bid_list[i].size() ; j++)
		{
			cout<<company_bid_list[i][j]<<" ";
		}
		cout<<endl;
	}
}
void add_bid(int bid_id)
{
	company_bid_considered[ tob[bid_id].cid ] = bid_id;

	for(int i=0;i<tob[bid_id].norc;i++)
	{
		region_bid_index[ tob[bid_id].region[i] ]= bid_id;
	} 

}
void remove_bid(int bid_id)
{
	company_bid_considered[ tob[bid_id].cid ] = -1;
	for(int i=0;i<tob[bid_id].norc;i++)
	{
		region_bid_index[ tob[bid_id].region[i] ]= -1;
	}
}
void update_unallotted_regions_list()
{
	unallotted_regions_list.resize(0);
	for(int i=0;i<nor;i++)
	{
		if(region_bid_index[i]==-1)
		{
			unallotted_regions_list.push_back(i);
		}
	}
}
void make_random_start_state()
{
	for(int i=0;i<noc;i++)
	{
		update_unallotted_regions_list();
		// cout<<"For Company : "<<i<<": unallotted_regions_list is :: ";
		// for (int j = 0; j < unallotted_regions_list.size(); j++)
		// {
		// 	cout<<unallotted_regions_list[j]<<" ";
		// }
		// cout<<endl;
		vector<int> compatible_bids_of_this_company;
		int num_of_bids_of_company = company_bid_list[i].size();
		for(int j=0;j<num_of_bids_of_company;j++)
		{
			//converting that array into vector
			vector<int> bid_region_vector((tob[ company_bid_list[i][j] ].region), (tob[ company_bid_list[i][j] ].region)+(tob[ company_bid_list[i][j] ].norc));
			
			// cout<<"For BID : "<<company_bid_list[i][j]<<": bid_region_vector is :: ";
			// for (int t = 0; t < bid_region_vector.size(); t++)
			// {
			// 	cout<<bid_region_vector[t]<<" ";
			// }
			// cout<<endl;

			bool bid_contained_in_unallocated=true;

			vector<int> intersection_vector = vector<int> (intersection_dummy_vector, set_intersection( all(bid_region_vector), all(unallotted_regions_list), intersection_dummy_vector ));
			int bid_region_size=bid_region_vector.size();
			if(intersection_vector.size()!=bid_region_vector.size())
			{
				bid_contained_in_unallocated=false;
			}
			else
			{
				for(int k=0;k<bid_region_size;k++)
				{
					if(intersection_vector[k]!=bid_region_vector[k])
					{
						bid_contained_in_unallocated=false;
						break;
					}
				}
			}

			if(bid_contained_in_unallocated)
			{
				compatible_bids_of_this_company.push_back(company_bid_list[i][j]);
			}
		}
		cout<<"For Company : "<<i<<": compatible_bids_of_this_company is :: ";
		for (int j = 0; j < compatible_bids_of_this_company.size(); j++)
		{
			cout<<compatible_bids_of_this_company[j]<<" ";
		}
		cout<<endl;
		if(!compatible_bids_of_this_company.empty())
		{
			srand (time(NULL));
			int rand_bid_index_of_compatible_vector= rand() % (compatible_bids_of_this_company.size());
			add_bid(compatible_bids_of_this_company[rand_bid_index_of_compatible_vector]);
		}
		
	}

}

void print_random_start_state()
{
	for(int i=0;i<noc;i++)
	{
		cout<<"FOR COMPANY "<<i<<" BID_ID OF CONSIDERED BID IS "<< company_bid_considered[i]<<endl;
	}
}
int main()
{
	readFile();
	make_company_bid_list();
	print_company_bid_lists();
	make_random_start_state();
	print_random_start_state();
	return 0;
}
