#include<iostream>
#include<vector>
using namespace std;

void transform(vector<int> & v, int (*f)(int)){
    for(int & x : v){
        x = f(x);
    }
}


int main(){
    vector<int> v {1,3,5,7,9};

    transform(v, [](int x) { return x + 5; });

    transform(v, [](int x) { return x * 5; });

    for(int x : v){
        cout << x << " ";
    }

    cout << endl;
    return 0;
}