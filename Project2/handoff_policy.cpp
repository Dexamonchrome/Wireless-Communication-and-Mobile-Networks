#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <math.h>
#include <vector>

using namespace std;

bool poisson();

int Policy;
static const double LArray[] = {1.0/2, 1.0/3, 1.0/5};//seconds
//static const double LArray[] = {(1.0/2)*(1/1000), (1.0/3)*(1/1000), (1.0/5)*(1/1000)};//ms
double Lambda;
static const double E = 13;
static const double T = 10;
//static const int Cycle = 100;
static const int Cycle = 86400; //seconds
//static const int Cycle = 86400000;  //ms


int Handoff = 0;
long long Power = 0;
long long Num_of_Cars = 0;

int BS_Coordinate[4][2] = {
    {330, 350},
    {640, 310},
    {360, 680},
    {660, 658}
};

int Move[4][2] = { 
    {0, 10},  //up
    {-10, 0}, //left
    {0, -10}, //down
    {10, 0}   //right
};

class Car{
    int Xpos;
    int Ypos;
    int route;
    int BS = -1;
    double Pold = -100000;
    int handoff = -1; 
    bool out_of_bounds = false; 
    int time = 0;

    public:
    Car(int Xpos, int Ypos){
        this->Xpos = Xpos;
        this->Ypos = Ypos;
        if(Xpos == 0) 
		route = 3;
        else if(Xpos == 1000) 
		route = 1;
        else if(Ypos == 0) 
		route = 0;
        else 
		route = 2;
        Handoff--;
    }

    bool move(){
        if(out_of_bounds) 
		return false;
        
        Xpos = Xpos + Move[route][0];
        Ypos = Ypos + Move[route][1];
        
        if(Xpos <= 0 || Xpos >= 1000 || Ypos <= 0 || Ypos >= 1000)
	{
            out_of_bounds = true;
            return false;
        }

        int newBS = BS;
        double Pcan = Pold;
        if(Policy == 1)
	{
        	for(int i = 0; i < 4; ++i){
                    if(Pnew(i) > Pcan){
                        Pcan = Pnew(i);
                        newBS = i;
                    }
                }
        }
        if(Policy == 2)
	{
                for(int i = 0; i < 4; ++i){
                    if(Pold < T && Pnew(i) > Pcan){
                        Pcan = Pnew(i);
                        newBS = i;
                    }
                }
        }
        if(Policy == 3)
	{
                for(int i = 0; i < 4; ++i){
                    if(Pnew(i) > (Pold + E) && Pnew(i) > Pcan){
                        Pcan = Pnew(i);
                        newBS = i;
                    }
                }
        }
        if(Policy == 4)
	{
                if(time % 10 == 0)
		{	
		    if(rand() % 10 < 5){
                    	for(int i = 0; i < 4; ++i){
                        	if(Pnew(i) > Pcan){
                            		Pcan = Pnew(i);
                            		newBS = i;
                        	}
                    	}
                   }
	       }
        }

        if(newBS != BS) 
	{
            Pold = Pcan;
            BS = newBS;
            handoff++;
            Handoff++;
        }
	else
	{
            Pold = Pnew(BS);
        }

        
        Power = Power + Pold;
        Num_of_Cars++;

        
        if(Xpos % 100 == 0 && Ypos % 100 == 0){
            if(rand()%5 == 0){  
		route = route + 1;
	    }
            if(rand()%5 == 1){ 
                route = route + 3;
            }
            route = route % 4;
        }
        time++;
        return true;
    }

    double Pnew(int BS){
        double x = pow(BS_Coordinate[BS][0]-Xpos, 2);
	double y = pow(BS_Coordinate[BS][1]-Ypos, 2);
	double distance = sqrt(x + y);
	double ans = 100 - (33 + 20*log10(distance));
	return ans;
    }

};

int main(){
    clock_t tStart = clock();
    srand(time(NULL));
    int total_Handoff = 0;
    for(int j = 0; j < 4; j++){
	Policy = j+1;
	for(int k = 0; k < 3; k++){
		Lambda = LArray[k];
    		vector<Car> obj;
		cout << "Policy: " << Policy << " " << "Lambda: " << Lambda << "\n";
    		for(int i = 0; i < Cycle; ++i){
        		for(int i = 100; i < 1000; i+=100){
            			if(poisson()) 
			    		obj.emplace_back(Car(i, 0));
            			if(poisson()) 
		    			obj.emplace_back(Car(i, 1000));
            			if(poisson()) 
		    			obj.emplace_back(Car(0, i));
            			if(poisson()) 
		    			obj.emplace_back(Car(1000, i));
        		}
			if(i%3600 == 0){
                		cout << i/3600 << " " << obj.size() << " " << Handoff << "\n";
				total_Handoff += Handoff;
        			Handoff = 0;
			} //hours in seconds
			/*if(i%3600000 == 0){
                                cout << i/3600000 << " " << traffic.size() << " " << Handoff << "\n";
                                total_Handoff += Handoff;
                                Handoff = 0;
                        }*/ //hours in ms

        		//for(int j = 0; j < obj.size(); obj[j].move(), j++);

			for(int j = 0; j < obj.size(); j++)
				obj[j].move();

		}
		Handoff = 0;
		cout << Power << "/" << Num_of_Cars << "\n";
        	double Avg_Power = (double) Power / (double) Num_of_Cars;
        	cout << "Avg. Power: " <<  Avg_Power  << "\n";
		cout << "total handoff: " << total_Handoff << "\n\n";
		total_Handoff = 0; 
		Power = 0;
		Num_of_Cars = 0; 
		Avg_Power = 0;
		obj.clear();

	}
        
    }
    
    printf("Time taken: %.2fs\n", (double)(clock() - tStart)/CLOCKS_PER_SEC);
    return 0;
}

bool poisson()
{
    double pos, r;
    pos = 1 - exp(-1 * Lambda);
    r = (double)rand() / (double)RAND_MAX;
    return (r < pos);
}
