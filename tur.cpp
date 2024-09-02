#include<iostream>
#include<vector>

using namespace std;

int main(){
    vector<int>res(500000,0);
    for(int i = 0; i < 1000; i++){
        for(int j = 0; j < 50000; j++){
            res[j] += res[j] - 1;
        }
    }
}
