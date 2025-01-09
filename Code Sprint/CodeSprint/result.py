from bs4 import BeautifulSoup
import requests
import vlawyer
import lxml

response = requests.get("https://vjudge.net/contest/665719")
yc_web_page=response.text

soup = BeautifulSoup(yc_web_page,"html.parser")

titles=[]
links=[]
print(soup)
# articles=soup.find_all(name="span",class_="titleline",recursive=True)
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


