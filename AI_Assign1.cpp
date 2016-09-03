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

vector<int> region_bid_index; //stores the bid id of the considered bid for each region in the current state
vector<int> company_bid_considered;//stores the bid id of the considered bid for each company in the current state
vector<int> unallotted_regions_list;

//state variables for the maximum valued neighbour
vector<int> region_bid_index_max;
vector<int> company_bid_considered_max;
vector<int> unallotted_regions_list_max;



vector< vector<int> > region_bid_index_neighbours;
vector< vector<int> > company_bid_considered_neighbours;
vector< vector<int> > unallotted_regions_list_neighbours;

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
void neigh_add_bid(int neigh_id,int bid_id)
{
	company_bid_considered_neighbours[neigh_id][ tob[bid_id].cid ] = bid_id;

	for(int i=0;i<tob[bid_id].norc;i++)
	{
		region_bid_index_neighbours[neigh_id][ tob[bid_id].region[i] ]= bid_id;
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
void neigh_remove_bid(int neigh_id,int bid_id)
{
	company_bid_considered_neighbours[neigh_id][ tob[bid_id].cid ] = -1;
	for(int i=0;i<tob[bid_id].norc;i++)
	{
		region_bid_index_neighbours[neigh_id][ tob[bid_id].region[i] ]= -1;
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
void neigh_update_unallotted_regions_list(int neigh_id)
{
	unallotted_regions_list_neighbours[neigh_id].resize(0);
	for(int i=0;i<nor;i++)
	{
		if(region_bid_index_neighbours[neigh_id][i]==-1)
		{
			unallotted_regions_list_neighbours[neigh_id].push_back(i);
		}
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
//method to compute profit in current state
double profit_current_state()
{
	double profit = 0.0;
	for(int i=0;i<noc;i++)
	{
		if(company_bid_considered[i]!=-1)
		profit+= tob[company_bid_considered[i]].price;	
	}
	return profit;
}

void make_all_the_neighbouring_states()
{
	for(int i=0;i<noc;i++)
	{
		int bids_of_this_company=company_bid_list[i].size();
		for(int j=0;j<bids_of_this_company;j++)
		{
			if(company_bid_list[i][j]!=company_bid_considered[i])
			{
				//BID INDEX to be added : company_bid_list[i][j]
				int neighbour_number=region_bid_index_neighbours.size();
				region_bid_index_neighbours.push_back(region_bid_index);
				company_bid_considered_neighbours.push_back(company_bid_considered);
				unallotted_regions_list_neighbours.push_back(unallotted_regions_list);
				//Remove the currently considered bid of the company if there is any
				if(company_bid_considered_neighbours[neighbour_number][i]!=-1)
				{
					neigh_remove_bid(neighbour_number,company_bid_considered_neighbours[neighbour_number][i]);
				}
				//Remove all the conflicting bids that have been considered now
				vector<int> conflicting_bids;
				for(int k=0;k<tob[company_bid_list[i][j]].norc;k++)
				{
					conflicting_bids.push_back(region_bid_index_neighbours[neighbour_number][ tob[ company_bid_list[i][j] ].region[k] ]);
				}
				sort( conflicting_bids.begin(), conflicting_bids.end() );
				conflicting_bids.erase( unique( conflicting_bids.begin(), conflicting_bids.end() ), conflicting_bids.end() );
				for(int k=0;k<conflicting_bids.size();k++)
				{
					neigh_remove_bid(neighbour_number,conflicting_bids[k]);
				}
				//Add the bid of this company
				neigh_add_bid(neighbour_number,company_bid_list[i][j]);
				//Add the best possible of the remaining bids
				neigh_update_unallotted_regions_list(neighbour_number);
				//Unallocated regions list : unallotted_regions_list_neighbours[neighbour_number]
				vector<int> left_out_companies;
				for(int k=0;k<noc;k++)
				{
					if(company_bid_considered_neighbours[neighbour_number][k]==-1)
					{
						left_out_companies.push_back(k);
					}
				}
				vector<int> compatible_bids;
				for(int k=0;k<left_out_companies.size();k++)
				{
					for(int t=0;t<company_bid_list[left_out_companies[k]].size(); t++ )
					{
						bool compatible =true;

						vector<int> bid_region_vector((tob[ company_bid_list[left_out_companies[k]][t] ].region), (tob[ company_bid_list[left_out_companies[k]][t] ].region)+(tob[ company_bid_list[left_out_companies[k]][t] ].norc));

						vector<int> intersection_vector = vector<int> (intersection_dummy_vector, set_intersection( all(bid_region_vector), all(unallotted_regions_list_neighbours[neighbour_number]), intersection_dummy_vector ));
						
						int bid_region_size=bid_region_vector.size();
						if(intersection_vector.size()!=bid_region_vector.size())
						{
							compatible=false;
						}
						else
						{
							for(int k=0;k<bid_region_size;k++)
							{
								if(intersection_vector[k]!=bid_region_vector[k])
								{
									compatible=false;
									break;
								}
							}
						}

						if(compatible)
						{
							compatible_bids.push_back(company_bid_list[left_out_companies[k]][t]);
						}

					}
				}
				
			}
		}
	}
}

//trying to go to next best state
void next_best_neighbour()
{
	//save the current state in max state initially
	for (int i = 0; i < noc; i++)
	{
		company_bid_considered_max[i] = company_bid_considered[i];
	}

	for (int i = 0; i < nor; i++)
	{
		region_bid_index_max[i] = region_bid_index[i];
	}

	for (int i = 0; i < unallotted_regions_list.size(); i++)
	{
		unallotted_regions_list_max.push_back(unallotted_regions_list[i]);
	}
	//

	for (int i = 0; i < noc; i++)
	{
		int bids_of_this_company=company_bid_list[i].size();
		for (int j = 0; j < bids_of_this_company; i++)
		{
			//making a temp state
			vector<int> region_bid_index_temp;
			vector<int> company_bid_considered_temp;
			vector<int> unallotted_regions_list_temp;

			for (int i = 0; i < noc; i++)
			{
				company_bid_considered_temp[i] = company_bid_considered[i];
			}

			for (int i = 0; i < nor; i++)
			{
				region_bid_index_temp[i] = region_bid_index[i];
			}

			for (int i = 0; i < unallotted_regions_list.size(); i++)
			{
				unallotted_regions_list_temp.push_back(unallotted_regions_list[i]);
			}
			//


			if(company_bid_considered[i]!=company_bid_list[i][j]){
				int curr_bid = company_bid_list[i][j];
				
				//removing all the conflicting bids of the bid considered
				set<int> conflicting_bids;
				for (int k = 0; k < tob[curr_bid].norc; k++)
				{
					conflicting_bids.insert(region_bid_index[tob[curr_bid].region[k]]);
				}
					//iterating over the set
				for(set<int>::const_iterator it = conflicting_bids.begin(); it != conflicting_bids.end(); it++) 
				{ 
   					   company_bid_considered_temp[tob[*it].cid] = -1;
   					   for (int k = 0; k < tob[*it].norc; k++)
   					    {
   					    	region_bid_index_temp[tob[*it].region[k]] = -1;
   					    } 
 				}
 				//
 				
 				//removing the current bid
 				for(int k=0;k<tob[company_bid_considered[i]].norc;k++)
				{
					region_bid_index_temp[tob[company_bid_considered[i]].region[k]] = -1;
				}
				company_bid_considered_temp[i] = curr_bid;
				//

				//making a list of left-out companies
				vector<int> left_out_companies;
				for(int k=0;k<noc;k++)
				{
					if(company_bid_considered_temp[k]==-1)
					{
						left_out_companies.push_back(k);
					}
				}
				//

				//

				//

			}
		}
	}

}

int main()
{
	readFile();
	make_company_bid_list();
	print_company_bid_lists();
	make_random_start_state();
	print_random_start_state();
	//make_all_the_neighbouring_states();
	return 0;
}
