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

if(n<=3){
cout<<-1<<endl;
return;
}

int j=n;
if(n%2!=0){
for(int i=1;i<j;i++){
    cout<<i<<" "<<j<<" ";
    j--;
}
cout<<j<<endl;

}


cout<<endl;
    
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