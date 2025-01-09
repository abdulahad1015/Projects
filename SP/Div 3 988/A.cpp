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
    int n,m,input;
    cin>>n>>m;
    vi arr;

    FORN(n){
        cin>>input;
        arr.pb(input);
    }
 
    int a=1,b=0;
    forn(0,n){
        for(int j=i+1;j<n;j++){
            b=abs(arr[i]-arr[j]);
            if(b==0 || b%m==0){
                cout<<0<<endl;
                return;
            }
            a=a*(b%m);
            a=a%m;
            if(a==0){
                cout<<0<<endl;
                return;
            }
        }
 
    }
    cout<<a;
    
}
 
signed main(void){
    ios::sync_with_stdio(false);
    cin.tie(NULL);
    cout.tie(NULL);
    int t = 1;
    // cin >> t;
    forn(0, t)
        solve();
        // cout<<endl;
    // }
 
    return 0;
}