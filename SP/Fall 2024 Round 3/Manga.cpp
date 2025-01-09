#include <bits/stdc++.h>
#include <cmath>
#define fast ios::sync_with_stdio(false);cin.tie(NULL);cout.tie(NULL);
#define forn(a, b) for(int i = a; i < b; ++i)
#define form2n(i, a, b) for(int i = a; i < b; ++i)
#define FORN(a) for(int i=0;i<a;i++)
#define int long long int
#define nl cout << "\n"
#define debug(x) cout << #x << ": " << x << ", "
#define vi vector<int>
#define pb(x) push_back(x)
#define all(x) x.begin(), x.end()
#define hhh cout << "here" << "\n"
using namespace std;

void solve(){
int n;
cin >>n;
vi arr;
int input;

FORN(n){ cin>>input; arr.push_back(input);
}
// for(auto i:arr)cout<<i;
// cout<<endl;

int vols=0;
int extra=0;
sort(all(arr));


FORN(n-1){
    int j=i+1;
    if(arr[i]==-1){
    continue;}
    while(arr[j]==arr[i]){
        arr[j]=-1;
        j++;
        extra++;
    }
}
// for(auto i:arr)cout<<i<<endl;
sort(all(arr));
// 
int j=n-1;
int i=0; 
while(i<n && i<=j){
    // cout<<arr[i]<<" "<<j<<" "<<i<<endl;
    if(arr[i]==-1 && i<n){
        i++;
    }
    else if(arr[i]==vols+1){
        vols++;
        i++;
    }
    else if(extra>=2){
        extra-=2;
        vols++;
    }
    else if(i<=j && j>=0 && extra==1){
        extra-=1;
        j-=1;
        vols++;
    }
    else if(i<j && j>=1){
        j-=2;
        vols++;
    }
    
    else break;
}
while(extra>1){
    extra-=2;
    vols++;
}
cout<<vols;

}

signed main(void){
    fast;
    int t = 1;
    // cin >> t;
    forn(0, t)
        solve();

    return 0;
}