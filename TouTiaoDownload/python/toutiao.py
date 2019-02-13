import os
from bs4 import BeautifulSoup
from lxml import etree
import json
import urllib
import urllib.request

def parseTouTiaoFromMain(html):
    soup = BeautifulSoup(html, 'lxml')
    li = soup.select('li div div[class="bui-left single-mode-lbox"] a')
    mainUrl = 'https://www.ixigua.com'
    urls = ''
    for l in li:
        temp =mainUrl + l['href']
        urls += temp + ';'
        print(temp)
    print(urls)
    return urls

def parseTouTiaoDetail(html):
    soup = BeautifulSoup(html, 'lxml')

    retrun_js = '{'
    #标题
    title = _getString(soup.select('div h2[class="title"]'))
    retrun_js += '"{0}":"{1}",'.format('title', title)
    #是否原创
    origin = _getString(soup.select('div div[class="origin"][style="display: none;"] span'))
    retrun_js += '"{0}":"{1}",'.format('origin', origin)

    #下载地址
    videourl = soup.select('div[class="player"] div[class="player-wrap"] div video')
    try:
        videourl = 'https:' + videourl[0]['src']
    except:
        videourl = soup.select('div[class="player"] div[class="player-wrap"] div div video source')
        try:
            videourl = videourl[0]['src']
        except:
            pass
    retrun_js += '"{0}":"{1}",'.format('videourl', videourl)

    #时长
    playcount = _getString(soup.select('div[class="abstract"] div div span em'))
    retrun_js += '"{0}":"{1}",'.format('playcount', playcount)

    #作者
    username = _getString(soup.select('div[class="abstract"] div div a span[class="name"]'))
    retrun_js += '"{0}":"{1}",'.format('username', username)

    #作者url
    try:
        userurl = soup.select('div[class="abstract"] div div a')
        userurl = userurl[0]['href']
        retrun_js += '"{0}":"{1}",'.format('userurl', userurl)
    except:
        pass

    retrun_js = retrun_js[0:-1]
    retrun_js += '}'

    return retrun_js

def _getString(lst):
    text = ''
    try:
        text = lst[0].string
    except:
        pass
    return text;

def downloadFile(url, path):
    try:
        #path = os.getcwd()
        #file_name = str(random.random())+'.mp4'
        #file_name = '12321312.mp4'
        #dest_dir=os.path.join(path, file_name)
        urllib.request.urlretrieve(str(url), path)
        return True
    except:
        pass

    return False

def getHtml(url):
    # print(url)
    # 设置属性
    user_agent = 'Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/55.0.2883.87 UBrowser/6.2.4094.1 Safari/537.36'
    # referer = 'https://www.ixigua.com/a6634040320034079235/'
    referer = 'https://www.ixigua.com/'
    # 设置headers
    headers = {'User-Agent': user_agent, 'Referer': referer}
    req = urllib.request.Request(url=url, headers=headers)
    try:
        page = urllib.request.urlopen(req)
        html = page.read().decode()
        return html
    except:
        return ''

if __name__ == '__main__':
    url = 'https://www.ixigua.com/'
   #html = getHtml(url)
    html = ''
    with open(r'E:\zhy\works\qt\DuoMeiTiDownload\TouTiaoDownload\python\text_internal2.html', 'r', encoding='utf-8') as f:
        html = f.read()
    parseTouTiaoFromMain(html)
    #parseTouTiaoDetail(html)
    #path = r'http://v9-tt.ixigua.com/fbb0bf396595abb1eef0efbec0d6b9fa/5c6235e1/video/m/220f211bf35915f40918d39bd0059e0779d11617068200004b1f01c474fa/?rc=MzgzO3Fncm54azMzZzczM0ApQHRAbzQ4NTw6MzUzMzY4MzQzNDVvQGgzdSlAZjN1KWRzcmd5a3VyZ3lybHh3ZjUzQDI0Ym8ybl5tY18tLWMtMHNzLW8jbyM2MjMuMzItLjAzNi8vNi06I28jOmEtcSM6YHZpXGJmK2BeYmYrXnFsOiMuL14%3D'
    #downloadFile(path, r'd:\fffff.mp4')


