def solve():
    n = int(input())
    arr=[int(x) for x in input().split()]
    sum=0
    prefix=[]
    suffix=[]
    for i in arr:
        sum+=i
        prefix.append(sum)
    # print(prefix)    
    sum=0
    for i in arr[::-1]:
        sum+=i
        suffix.append(sum)
    # print(suffix)

    i=0
    j=n-1
    while(i<=j):
        if(prefix[i]==suffix[j]):
            print(i+1)
            return
        else:
            i+=1
            j-=1
    
    print(-1)
    return
    


t=1
# t = int(input())
for _ in range(t):
    solve()



