#include<bits/stdc++.h>
using namespace std;



//Given an array of integers and a target,return true if any subset of the array's sum
//equals target

//8 10 1 3 target=12
//Choose:8
//Choose:10
//Choose:1
//Choose:3

//8,1,3
//Output:Yes

//5 5 5 target=16
//Output:No

//Choose not choose

// At first index: choose number or dont choose

int dp[501][1001];

bool helper(vector<int>&a,int n,int index,int target,int currentSum){

    if(currentSum==target){
        return true;
    }
    if(index==n){
        return false;
    }
    if(dp[index][currentSum]!=-1){
        return (bool)dp[index][currentSum];
    }

    if(currentSum+a[index]>target-a[index]){
        bool skip = helper(a,n,index+1,target,currentSum);
        return dp[index][currentSum]=skip;
    }


    bool notChoose=helper(a,n,index+1,target,currentSum);
    currentSum+=a[index];
    target-=a[index];
    bool choose = helper(a,n,index+1,target,currentSum);
    return dp[index][currentSum]=(notChoose||choose)||(max(notChoose,choose));

}

bool isSubset(vector<int>&a,int n,int target){
    return helper(a,n,0,target,0);
}
int main(){

    memset(dp,-1,sizeof(dp));
    int n;
    cin>>n;
    vector<int>a(n);
    int sum=0;
    for(int i=0;i<n;++i){cin>>a[i];
    sum+=a[i];
    }
    cout<<isSubset(a,n,sum);
}