#include <bits/stdc++.h>
#include <cmath>

#define forn(a, b) for(int i = a; i < b; ++i)
#define form2n(i, a, b) for(int i = a; i < b; ++i)
#define FORN(a) for(int i=0;i<a;i++)
// #define int unsigned long long int
#define nl cout << "\n"
#define debug(x) cout << #x << ": " << x << ", "
#define vi vector<int>
#define pb(x) push_back(x)
#define all(x) x.begin(), x.end()
#define hhh cout << "here" << "\n"

using namespace std;

void solve(){
int n;
cin>>n;

int arr[n];
FORN(n) cin>>arr[i];

int target = n-2;

for(int i=1;i<=target;i++){
    int temp=0;
    if(target%i==0){
        temp=target/i;
    }
    int flag1=0,flag2=0;
    for(int j=0;j<n;j++){
        if(arr[j]==i){
        flag1++;}
        else if(arr[j]==temp){
        flag2++;}

        if(flag1>=1){
        if(flag2>=1 || (flag1>=2 && temp==i) ){
        cout<<temp<<" "<<i<<endl;
        return;
        }
    }
    }
    
}


}

signed main(void){
    ios::sync_with_stdio(false);
    cin.tie(NULL);
    cout.tie(NULL);
    int t = 1;
    cin >> t;
    forn(0, t)
        solve();
        // cout<<endl;
    // }

    return 0;
}