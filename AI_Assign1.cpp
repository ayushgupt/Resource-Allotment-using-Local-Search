#include <vector>
#include <string>
#include <fstream>
#include <cmath>
#include <ctime>
#include <sstream>
#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <set>
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
double max_profit_neighbour;



vector< vector<int> > region_bid_index_neighbours;
vector< vector<int> > company_bid_considered_neighbours;
vector< vector<int> > unallotted_regions_list_neighbours;

vector< vector<int> > compatible_bids_left_companies;
double max_profit;
vector<int> bids_considered_bf_max;

void readFile()
{
	string g;
	scanf("%f\n\n",&tim);
	scanf("%d\n\n",&nor);
	scanf("%d\n\n",&nob);
	scanf("%d\n\n",&noc);
	region_bid_index.resize(nor, -1);
	company_bid_considered.resize(noc,-1);
	company_bid_considered_max.resize(noc,-1);
	region_bid_index_max.resize(nor,-1);
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
void print_best_neighbour_state()
{
	for(int i=0;i<noc;i++)
	{
		cout<<"FOR COMPANY "<<i<<" BID_ID OF CONSIDERED BID IS "<< company_bid_considered_max[i]<<endl;
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
/*
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
*/
void brute_force(double profit,int index,vector<int> unallocated,vector<int> bids_considered_bf)
{
	//cout<<"11"<<endl;
	if(index==compatible_bids_left_companies.size()){
		if(profit>max_profit)
		{
		
			bids_considered_bf_max.resize(0);
			for(int i=0;i<bids_considered_bf.size();i++)
			{
	
				bids_considered_bf_max.push_back(bids_considered_bf[i]);
			}
			max_profit = profit;

		}
		return;
	}
	//cout<<"12"<<endl;
	//cout<<"num "<<compatible_bids_left_companies.size()<<endl;
	//cout<<"num bids "<<compatible_bids_left_companies[index].size()<<endl;
	for(int k=-1;k<compatible_bids_left_companies[index].size();k++)
	{
	//	cout<<"13"<<endl;
		if(k==-1)
		{
			brute_force(profit,index+1,unallocated,bids_considered_bf);
		}
		else
		{
			//checking compatibility of tob[ compatible_bids_left_companies[index][k]  ].region and unallocated


			vector<int> bid_region_vector((tob[ compatible_bids_left_companies[index][k]  ].region), (tob[ compatible_bids_left_companies[index][k]  ].region)+(tob[ compatible_bids_left_companies[index][k]  ].norc));
			
			// cout<<"For BID : "<<company_bid_list[i][j]<<": bid_region_vector is :: ";
			// for (int t = 0; t < bid_region_vector.size(); t++)
			// {
			// 	cout<<bid_region_vector[t]<<" ";
			// }
			// cout<<endl;

			bool bid_compatible=true;

			vector<int> intersection_vector = vector<int> (intersection_dummy_vector, set_intersection( all(bid_region_vector), all(unallocated), intersection_dummy_vector ));
			int bid_region_size=bid_region_vector.size();
			if(intersection_vector.size()!=bid_region_vector.size())
			{
				bid_compatible=false;
			}
			else
			{
				for(int l=0;l<bid_region_size;l++)
				{
					if(intersection_vector[l]!=bid_region_vector[l])
					{
						bid_compatible=false;
						break;
					}
				}
			}
	//		cout<<"14"<<endl;

			if(bid_compatible)
			{
				
				vector<int> bids_considered_bf_new (bids_considered_bf);
				bids_considered_bf_new.push_back(compatible_bids_left_companies[index][k]);
				set<int> unallocated_new_set ;
				for (int l = 0; l < unallocated.size(); l++)
				{
					unallocated_new_set.insert(unallocated[l]);
				}
				for(int l=0;l<tob[compatible_bids_left_companies[index][k]].norc;l++)
				{
					unallocated_new_set.erase(tob[compatible_bids_left_companies[index][k]].region[l]);
				}
				std::vector<int> unallocated_new;
				for(set<int>::const_iterator it = unallocated_new_set.begin(); it != unallocated_new_set.end(); it++) 
				{ 
   					   unallocated_new.push_back(*it);
 				}
 				brute_force(profit+tob[compatible_bids_left_companies[index][k]].price,index+1,unallocated_new,bids_considered_bf_new);

			}
			
		}
	}

}

//trying to go to next best state
void next_best_neighbour()
{
	//cout<<"main3"<<endl;

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
	max_profit_neighbour = profit_current_state();
	//
	//cout<<"main1"<<endl;
	for (int i = 0; i < noc; i++)
	{
		int bids_of_this_company=company_bid_list[i].size();
		for (int j = 0; j < bids_of_this_company; j++)
		{
			//making a temp state
			vector<int> region_bid_index_temp;
			vector<int> company_bid_considered_temp;
			vector<int> unallotted_regions_list_temp;
			region_bid_index_temp.resize(nor,-1);
			company_bid_considered_temp.resize(noc,-1);

			for (int l = 0; l < noc; l++)
			{
				company_bid_considered_temp[l] = company_bid_considered[l];
			}

			for (int l = 0; l < nor; l++)
			{
				region_bid_index_temp[l] = region_bid_index[l];
			}

			for (int l = 0; l < unallotted_regions_list.size(); l++)
			{
				unallotted_regions_list_temp.push_back(unallotted_regions_list[l]);
			}
			//
	//cout<<"main2"<<endl;

			if(company_bid_considered[i]!=company_bid_list[i][j]){
				int curr_bid = company_bid_list[i][j];
	//			cout<<"1"<<endl;
				//removing all the conflicting bids of the bid considered
				set<int> conflicting_bids;
				for (int k = 0; k < tob[curr_bid].norc; k++)
				{
					if(region_bid_index[tob[curr_bid].region[k]]!=-1)
					conflicting_bids.insert(region_bid_index[tob[curr_bid].region[k]]);
				}
	//			cout<<"3"<<endl;
					//iterating over the set
				for(set<int>::const_iterator it = conflicting_bids.begin(); it != conflicting_bids.end(); it++) 
				{ 
	//				cout<<"4"<<endl;
	//				cout<<"it is "<<*it<<endl;
   					   company_bid_considered_temp[tob[*it].cid] = -1;
   	//				   cout<<"6"<<endl;
   					   for (int k = 0; k < tob[*it].norc; k++)
   					    {
   					    	//cout<<"5"<<endl;
   					    	region_bid_index_temp[tob[*it].region[k]] = -1;
   					    } 
 				}
 				//cout<<"2"<<endl;
 				//
 				
 				//removing the current bid
 				for(int k=0;k<tob[company_bid_considered[i]].norc;k++)
				{
					region_bid_index_temp[tob[company_bid_considered[i]].region[k]] = -1;
				}
				company_bid_considered_temp[i] = curr_bid;
				//
				//cout<<"7"<<endl;
				//making a list of left-out companies
				vector<int> left_out_companies;
				for(int k=0;k<noc;k++)
				{
					if(company_bid_considered_temp[k]==-1)
					{
						left_out_companies.push_back(k);
					}
				}
				//Updating the local Temp unallocated vector
				unallotted_regions_list_temp.resize(0);
				for(int k=0;k<region_bid_index_temp.size();k++)
				{
					if(region_bid_index_temp[k]==-1)
					{
						unallotted_regions_list_temp.push_back(k);
					}
				}

				//cout<<"8"<<endl;
				//initializing the 2D vector
				compatible_bids_left_companies.resize(0);
				compatible_bids_left_companies.resize(left_out_companies.size());
				//Making the 2D vector
				for(int k=0;k<left_out_companies.size();k++)
				{
					for(int l=0;l<company_bid_list[left_out_companies[k]].size();l++)
					{
						bool compatible_with_unallocated_temp = true;
						//Bid=company_bid_list[left_out_companies[k]][l]
						//Region array of Bid= tob[ company_bid_list[left_out_companies[k]][l] ].region

						vector<int> bid_region_vector((tob[ company_bid_list[left_out_companies[k]][l] ].region), (tob[ company_bid_list[left_out_companies[k]][l] ].region)+(tob[ company_bid_list[left_out_companies[k]][l] ].norc));
						vector<int> intersection_vector = vector<int> (intersection_dummy_vector, set_intersection( all(bid_region_vector), all(unallotted_regions_list_temp), intersection_dummy_vector ));
						int bid_region_size=bid_region_vector.size();
						if(intersection_vector.size()!=bid_region_vector.size())
						{
							compatible_with_unallocated_temp=false;
						}
						else
						{
							for(int m=0;m<bid_region_size;m++)
							{
								if(intersection_vector[m]!=bid_region_vector[m])
								{
									compatible_with_unallocated_temp=false;
									break;
								}
							}
						}

						if(compatible_with_unallocated_temp)
						{
							compatible_bids_left_companies[k].push_back(company_bid_list[left_out_companies[k]][l]);	
						}
					}
				}
				//cout<<"9"<<endl;
				max_profit = 0;
				vector<int> bidss;
				brute_force(0,0,unallotted_regions_list_temp,bidss);
				//cout<<"10"<<endl;
				for (int l = 0;l < bids_considered_bf_max.size(); l++)
				{
					company_bid_considered_temp[tob[bids_considered_bf_max[l]].cid] = bids_considered_bf_max[l];
					for(int h=0;h<tob[bids_considered_bf_max[l]].norc;h++)
					{
						region_bid_index_temp[tob[bids_considered_bf_max[l]].region[h]] = bids_considered_bf_max[l];
					}
				}

				unallotted_regions_list_temp.resize(0);
				for(int k=0;k<region_bid_index_temp.size();k++)
				{
					if(region_bid_index_temp[k]==-1)
					{
						unallotted_regions_list_temp.push_back(k);
					}
				}

				double profit_temp = 0.0;
				for(int k=0;k<noc;k++)
				{
					if(company_bid_considered_temp[i]!=-1)
					profit_temp+= tob[company_bid_considered_temp[i]].price;
				}
				if(profit_temp>max_profit_neighbour)
				{
					for (int l = 0; l < noc; l++)
					{
						company_bid_considered_max[l] = company_bid_considered_temp[l];
					}

					for (int l = 0; l < nor; l++)
					{
						region_bid_index_max[l] = region_bid_index_temp[l];
					}

					for (int l = 0; l < unallotted_regions_list.size(); l++)
					{
						unallotted_regions_list_max.push_back(unallotted_regions_list_temp[l]);
					}
					max_profit_neighbour = profit_temp;
				}

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

	for (int l = 0; l < noc; l++)
	{
		company_bid_considered_max[l] = company_bid_considered[l];
	}

	for (int l = 0; l < nor; l++)
	{
		region_bid_index_max[l] = region_bid_index[l];
	}

	for (int l = 0; l < unallotted_regions_list.size(); l++)
	{
		unallotted_regions_list_max.push_back(unallotted_regions_list[l]);
	}

	print_random_start_state();
	//next_best_neighbour();
	//print_best_neighbour_state();
	do{
//		cout<<"3"<<endl;
		for (int l = 0; l < noc; l++)
		{
			company_bid_considered[l] = company_bid_considered_max[l];
		}
//cout<<"4"<<endl;
		for (int l = 0; l < nor; l++)
		{
			region_bid_index[l] = region_bid_index_max[l];
		}
//cout<<"5"<<endl;
unallotted_regions_list.resize(0);
		for (int l = 0; l < unallotted_regions_list.size(); l++)
		{
			unallotted_regions_list.push_back(unallotted_regions_list_max[l]);
		}
		//cout<<"1"<<endl;
		next_best_neighbour();
		//cout<<"2"<<endl;
	}
	while(max_profit_neighbour>profit_current_state());


	for(int i=0;i<noc;i++)
	{
		cout<<"FOR COMPANY "<<i<<" BID_ID OF CONSIDERED BID IS "<< company_bid_considered_max[i]<<endl;
	}
	cout<<"Max. Profit is "<<max_profit_neighbour<<endl;


	return 0;
}
