import random as rd
from heapq import heapify,heappop,heappush
import sys
N, L = map(int,input().split())
T= list(map(int,input().split()))
def calc(G):
    work_count = [0 for _ in range(N)]
    for i in range(N):
        s1,s2 = G[i]
        work_count[s1] += T[i]
        work_count[s2] += T[i]
    score = 0
    for i in range(N):
        score += abs(work_count[i] - T[i] * 2)
    sabun = [work_count[i] - T[i] * 2 for i in range(N)]
    return score, sabun
final = [((i+1)%N,(i+2)%N) for i in range(N)]
ideal = 10**14
TT = [(-T[i],i) for i in range(N)]
heapify(TT)
tuika = []
for i in range(N):
    p,q = heappop(TT)
    tuika.append(q)
    heappush(TT,(p+2500,q))
for sana in range(80):
    ans = [((i+sana+1)%N,tuika[(i-sana)%N]) for i in range(N)]
    for _ in range(1000):
        #仕事しすぎている人を辞めさせる。
        sc, sab = calc(ans)
        if sc<ideal:
            final = ans
            ideal = sc
        hataraki = rd.randint(0,N-1)
        sabori = rd.randint(0,N-1)
        cut = rd.randint(0,N-1)
        jun = [i for i in range(cut,N)]+[i for i in range(cut)]
        for i in jun:
            if sab[i]<sab[sabori]:
                if rd.randint(0,99)<95:
                    sabori = i
            if sab[i]>sab[hataraki]:
                if rd.randint(0,99)<95:
                    hataraki = i
        temp = (0,0,ans[0][0])
        flag = 0
        cut = rd.randint(0,N-1)
        jun = [i for i in range(cut,N)]+[i for i in range(cut)]
        for i in jun:
            for j in range(2):
                if ans[i][j]==hataraki:
                    if j==0:
                        ans[i] = (sabori,ans[i][1])
                    else:
                        ans[i] = (ans[i][0],sabori)
                    temp = (i,j,sabori)
                    flag = 1
                    break
            if flag == 1:
                break
        #print(sc)
ans = final
sc, sab = calc(ans)
#print(sc)
for _ in range(2000):
    if sc<ideal:
        final = ans
        ideal = sc
    hataraki = rd.randint(0,N-1)
    sabori = rd.randint(0,N-1)
    cut = rd.randint(0,N-1)
    jun = [i for i in range(cut,N)]+[i for i in range(cut)]
    for i in jun:
        if sab[i]<sab[sabori]:
            if rd.randint(0,99)<95:
                sabori = i
        if sab[i]>sab[hataraki]:
            if rd.randint(0,99)<95:
                hataraki = i
    temp = (0,0,ans[0][0])
    flag = 0
    cut = rd.randint(0,N-1)
    jun = [i for i in range(cut,N)]+[i for i in range(cut)]
    for i in jun:
        for j in range(2):
            if ans[i][j]==hataraki:
                temp = (i,j,ans[i][j])
                if j==0:
                    ans[i] = (sabori,ans[i][1])
                else:
                    ans[i] = (ans[i][0],sabori)
                flag = 1
                break
        if flag == 1:
            break
    new_sc, new_sub = calc(ans)
    if sc > new_sc ^ rd.randint(0,99)>=95:
        sc = new_sc
    else:
        i,j,old = temp
        if j== 0:
            ans[i] = (old,ans[i][1])
        else:
            ans[i] = (ans[i][0],old)
    
    #print(sc)
for i in range(N):
    print(*final[i])