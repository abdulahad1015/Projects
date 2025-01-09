#include<stdio.h>
int main(){
    int n=0,seed;
    do{
        printf("Enter value of n: ");
        scanf("%d",&n);
    }while((n%2)==0);
    int mid=(n-1)/2;
    int arr[n][n];
    printf("enter Seed: ");
    scanf("%d",&seed);

    for(int k=0;k<n;k++){
        for(int i=0;i<n;i++){
            for(int j=0;j<n;j++){
            if(k==0)
            arr[i][j]=seed;
            else if((j<=mid-k)||(j>=mid+k)||(i<=mid-k)||(i>=mid+k))
            arr[i][j]=arr[i][j]*2;
            }   
        }
    }    
    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++){
        printf(" %d ",arr[i][j]);
        }
        printf("\n");   
    }
    
}