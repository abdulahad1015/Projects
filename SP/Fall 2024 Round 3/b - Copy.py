import math

def solve():
    # n, k = [int(x) for x in input().split()]
    n = int(input())
    p = int(input())
    arr = [1] * (p-1)
    arr.append(n - (p-1) )
    print(arr)

    i = p-1
    count=1
    while True:
        if(arr[i] - arr[i-1] > 1  ):
            arr[i-1]+=1
            arr[i]-=1
            count+=1
        else:
            i-=1
        if i==0:
            return count

            

    # return s
    
    









# t=int(input())
for i in range(1):
    print(solve())
        
        # print(i)
    # n=5
    # list=input().split()[:n]
    # a,b=input().split()
    # print(list)
    # print(a,b)