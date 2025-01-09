def solve():
    n=int(input())
    a=[]
    score={}
    for _ in range(n):
        a.append(input())

    t=int(n/2)
    n=int((n*(n-1))/2)

    for _ in range(n):
        str=input()
        str1=str.split(' ')
        name=str1[0].split('-')
        scores=str1[1].split(':')

        if(scores[0]>scores[1]):
            scores[0]=3
            scores[1]=0
        elif(scores[0]<scores[1]):
            scores[1]=3
            scores[0]=0
        else:
            scores[1]=1
            scores[0]=1
        
        if name[0] in score.keys():
            score[name[0]]+=int(scores[0])
        else:
            score[name[0]]=int(scores[0])

        if name[1] in score.keys():
            score[name[1]]+=int(scores[1])
        else:
            score[name[1]]=int(scores[1])


    score=list(score.items())
    score.sort(key = lambda x: (-x[1],x[0]))
    score = [i[0] for i in score[:t]]
    score.sort()
    

    for i in score[0:t]:
        print(i)



t=1
# t = int(input())
for _ in range(t):
    solve()



