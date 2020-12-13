#include<iostream>
#include<random>
#include<ctime>
#include<cstdlib>
#include<math.h>
#include<time.h>
#include<chrono>
#include<fstream>

using namespace std;

int main()
{
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	int time_units = 100000;
	std::default_random_engine generator(seed);
	int S_array[3] = {1, 5, 10};
	int S = 0;
	int queue = 0;
	float lambda;
	float mu;
	int count = 0;
	double a = lambda/mu;
	int block = 0; //block counter
	int now_server = 0;
	int now_queue = 0;
	int num_block = 0;
	int customer_counter = 0;
	float blocking_probability[1000];
	ofstream outfile;
	outfile.open("BlockingProbabilityTable.txt");
	outfile << "Q = 0" << endl;
        int c = 0;
        for(int Si = 0; Si < 3; Si++) //number of servers loops 3 times
        {
                S = S_array[Si];
                outfile << "S: " << S << endl;
                for(lambda = 0.01; lambda <=10; lambda*=10) //arrival rate loops 4 times
                {
                        std::poisson_distribution<int> distribution (lambda);
                        int customers = 0;
                        for(mu = 0.01; mu <= 10.24; mu*=4) //service rate loops 7 times
                        {
                               for(int i = 0; i < time_units*10; i++)
                               {
                                        customers = distribution(generator);
                                        c = c + customers;
                                        std::poisson_distribution<int> distribution2 (mu);
                                        int leave = distribution2(generator);
                                        //arrival
                                        while(customers >= 1) //if there are customers
                                        {
                                                if(now_server == S) //if the server is full
                                                {
                                                        ++num_block; //block
                                                        --customers;
                                                }
                                                else
                                                {
                                                        ++now_server;
                                                        --customers;
                                                }
						
                                        }
                                        //departure
                                        while(leave >= 1) //if there are customers leaving
                                        {
                                                if(now_server > 0)//if there are still customers being served
                                                {
                                                        --now_server; //remove one server
                                                        --leave;
                                                }
                                                else
                                                        break;
                                        }
                                }
			        //cout << endl;
			        //cout << "num_block: " << num_block << endl;
				//cout << "total arrival: " << c << endl;
				//cout << endl;
			        outfile << "lambda: " << lambda << "\t";
                                outfile << "mu: " << mu << "\t";
                                blocking_probability[block] = (float) num_block/ (float) c;
                                outfile << "BP: " << blocking_probability[block] << "\t";
                                outfile << "Erlang value: "<< (lambda/mu) << endl;
                                block++;
                                now_server = 0;
                                num_block = 0;
				c=0;
                        }
                }
                outfile << endl;
        }

	
	outfile << "Q = S" << endl;
	int b = 0;
	for(int Si = 0; Si < 3; Si++) //number of servers loops 3 times
	{
		S = S_array[Si];
		outfile << "S: " << S << endl;
		for(lambda = 0.01; lambda <=10; lambda*=10) //arrival rate loops 4 times
		{
			std::poisson_distribution<int> distribution (lambda);
			int customers = 0; //number of customers
			for(mu = 0.01; mu <= 10.24; mu*=4) //service rate loops 7 times
			{
				for(int i = 0; i < time_units*10; i++)
				{
					//queue
					std::poisson_distribution<int> distribution2 (mu);
					customers = distribution(generator);
                                        b = b + customers;
					int leave = distribution2(generator);
					//arrival
					while(customers >= 1) //if there are customers
					{
						while(now_queue > 0)
						{
							--now_queue;
							++now_server;	
						}
						
						if(now_server == S) //if the server is full
						{			
							if(now_queue == queue) //if the current queue is full
							{
								++num_block; //block
								--customers;
							}
							else //the queue is not full
							{
								++now_queue; //queue
								--customers;
							}
						}
						else //if server is not full
						{
							++now_server;
							--customers;
						}
					}	
					//departure
					while(leave >= 1) //if there are customers leaving
					{
						if(now_server > 0)//if there are still customers being served
						{
							--now_server; //remove one server
							--leave; //customer leaves
						}
						else
							break;		
					}
				}
				//cout << endl;
				//cout << "num_block: " << num_block << endl;
                                //cout << "total arrival: " << b << endl;
				//cout << endl;
				blocking_probability[block] = (float) num_block/ (float) b;
				outfile << "lambda: " << lambda << "\t";
				outfile << "mu: " << mu << "\t";
				outfile << "BP: " << blocking_probability[block];
				outfile << "\tErlang Value: "<< (lambda/mu) << endl;
				block++;
				now_queue = 0;
				now_server = 0;
				num_block = 0;
				b=0;
			}
		}
		outfile << endl;
	}
	
	return 0;
}


