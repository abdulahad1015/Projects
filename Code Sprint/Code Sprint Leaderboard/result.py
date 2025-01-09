import time
import requests
WRONG_SUBMISSION_PENALTY = 20

def get_participants(contest_no):
    start = time.time()
    response = requests.get(f"https://vjudge.net/contest/rank/single/{contest_no}")
    vjudge = response.json()
    submissions = vjudge['submissions']
    vjudge = vjudge['participants']

    # print("Submissions")
    # print(submissions)
    # print("Vjudge")
    # print(vjudge)

    participants = {
    }

    for i in submissions:
        id = i[0]
        if id in participants:
            # print(id,participants[id]['problem'])

            if participants[id]['problem'][i[1]] != None:
                participants[id]['problem'][i[1]]['submissions'] += 1

                if (participants[id]['problem'][i[1]]['verdict'] == 1 ):

                    if participants[id]['problem'][i[1]]['time'] > i[3]:
                        participants[id]['penalty'] += int(WRONG_SUBMISSION_PENALTY)
                    else:
                        continue
                #     continue
                # elif (participants[id]['problem'][i[1]]['time'] < i[3]):


                elif (i[2]==1 or len(i)>4):
                    participants[id]['problem'][i[1]]['verdict'] = i[2]
                    participants[id]['problem'][i[1]]['time'] = i[3]
                    participants[id]['penalty'] += int((i[3]) / 60)
                    participants[id]['penalty'] += int(
                        ((participants[id]['problem'][i[1]]['submissions'] - 1) * WRONG_SUBMISSION_PENALTY))
                    participants[id]['score'] += i[2]
                    if (len(i)>4):
                        participants[id]['score'] += round(i[4]/i[5],1)
                else:
                    participants[id]['problem'][i[1]]['verdict'] = i[2]


            else:

                participants[id]['problem'][i[1]] = {'submissions': 1, 'verdict': i[2], 'time': int(i[3])}
                participants[id]['score'] += i[2]
                if (i[2] == 1 or len(i)>4):

                    if (len(i)>4):

                        participants[id]['score'] += round(i[4]/i[5],1)
                    participants[id]['penalty'] += int(i[3] / 60)
        else:
            participants[id] = {}
            participants[id]['name'] = vjudge[str(id)][0]
            participants[id]['score'] = 0
            participants[id]['penalty'] = 0
            participants[id]['problem'] = [None, None, None, None, None, None, None]
            participants[id]['problem'][i[1]] = {'submissions': 1, 'verdict': i[2], 'time': i[3]}
            if (i[2] == 1):
                participants[id]['score'] += 1
                participants[id]['penalty'] += int(i[3] / 60)


    print(round(time.time()-start,2))
    return(participants)




#


# 'submissions': [[761321, 1, 1, 156571], [787355, 0, 1, 73921], [770422, 0, 0, 73156], [761321, 0, 1, 73549], [787355, 1, 1, 156032]]}
# 'submissions': [[761321, 1, 1, 156571], [787355, 0, 1, 73921], [770422, 0, 0, 73156], [761321, 0, 1, 73549], [787355, 1, 1, 156032]]}
# 'submissions': [[761321, 1, 1, 156571], [787355, 0, 1, 73921], [770422, 0, 0, 73156], [761321, 0, 1, 73549], [787355, 1, 1, 156032], [761321, 2, 0, 157123]]}
# 'submissions': [[761321, 1, 1, 156571], [761321, 0, 1, 73549], [761321, 2, 1, 158979], [761321, 2, 0, 157123]]}
# soup = BeautifulSoup(yc_web_page,"html.parser")
# print(soup.prettify())
#
# table=soup.find(name="table",id="contest-rank-table",recursive=True)
# print(table)


# username=table.find_all(name="td",class_="team meta",recursive=True)
# print(username)
# print(body)
# rows=table.find_all(name="tr",class_="this  ",recursive=True)
# print(rows)
# for article_tag in articles:
#     title=article_tag.getText()
#     titles.append(title)
#     print(title)
#     link=article_tag.find(name="a").get("href")
#     print(link)
#     links.append(link)
# upvotes =[int(i.getText().split()[0]) for i in soup.find_all(name="span",class_="score")]

# print(titles)
# print(links)
# for i in titles:
#     print(i)


# highest=upvotes.index(max(upvotes))
#
# print(titles[highest])
# print(links[highest])
# print(upvotes[highest])
