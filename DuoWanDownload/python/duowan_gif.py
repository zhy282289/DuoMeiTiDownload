import urllib
import urllib.request
import re
import json
import random
from urllib.parse import urlparse

import binascii
import base64
import os
from bs4 import BeautifulSoup
from lxml import etree
import json
import datetime

g_stop = False
def getHtml(url):
    #print(url)
    # 设置属性
    user_agent = 'Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/55.0.2883.87 UBrowser/6.2.4094.1 Safari/537.36'
    #referer = 'https://www.ixigua.com/a6634040320034079235/'
    referer = 'https://www.tu.duowan.com'
    # 设置headers
    headers = {'User-Agent': user_agent, 'Referer': referer}
    req = urllib.request.Request(url=url, headers=headers)
    try:
        page = urllib.request.urlopen(req)
        html = page.read().decode()
        return html
    except:
        return ''


def getUrlFromMainPage(url):
    html = getHtml(url)
    urllist = []
    soup = BeautifulSoup(html, 'lxml')
    r = soup.select('li[class="box"] em a')
    for r1 in r:
        d={}
        d['title'] = r1.get_text();
        d['href'] = r1.attrs['href'];
        urllist.append(d)
        print(d)
    return urllist

def getUrlFromMainPageContent(html):
    urllist = []
    soup = BeautifulSoup(html, 'lxml')
    r = soup.select('li[class="box masonry-brick"] em a')
    for r1 in r:
        d={}
        d['title'] = r1.get_text();
        d['href'] = r1.attrs['href'];
        urllist.append(d)
        print(d)
    return urllist

def getUrlFromPage(url):
    html = getHtml(url)
    urllist = []
    soup = BeautifulSoup(html, 'lxml')
    r = soup.select('ul[class="hot-pic clearfix"] a')
    for r1 in r:
        d={}
        d['title'] = r1.attrs['data-tip'];
        d['href'] = r1.attrs['href']
        urllist.append(d)
        print(d)


    return urllist

def writeToFile(path, urls, count):
    old_urls = {}
    old_urls['time'] = ''
    old_urls['count'] = 0
    old_urls['body'] = []
    try:
        with open(path, 'r', encoding='utf-8') as f:
            old_urls = json.load(f)
    except:
        pass

    for temp1 in urls:
        if len(old_urls['body']) >= count:
            break
        bexits = False
        for temp2 in old_urls['body']:
            if temp1['href'] == temp2['href']:
                bexits = True
                break
        if bexits == False:
            old_urls['body'].append(temp1)

    with open(path, 'w+', encoding='utf-8') as f:
        old_urls['time'] = curTime()
        old_urls['count'] = len(old_urls['body'])
        json.dump(old_urls, f, ensure_ascii=False, indent=4)
        return 1
    return 0


def curTime():
    return datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')

def getGifUrlsFromMainPage(path, html, count = 10):
    mainPageUrl = getUrlFromMainPageContent(html)
    if len(mainPageUrl) == 0:
        return 0

    return writeToFile(path, mainPageUrl, count)



def getGifUrls(path, url, count = 50):
    mainPageUrl = getUrlFromMainPage(url)
    if len(mainPageUrl) == 0:
        mainPageUrl.append({'href':url, 'title':'none'})

    urls = []
    urls += mainPageUrl
    for temp in mainPageUrl:
        urls += getUrlFromPage(temp['href'])
        if len(urls) >= count:
            break


    while len(urls) < count:
        urls += getUrlFromPage(urls[len(urls)-1]['href'])

    if len(urls) > 0:
        return writeToFile(path, urls, count)
    return 0

def stop():
    g_stop = True

def getMp4Path(text):
    try:
        soup = BeautifulSoup(text, 'lxml')
        r = soup.select('video source[type="video/mp4"]')
        url = ''
        for r1 in r:
            url = r1.attrs['src'];
            break

        r2 = soup.select('div[class="title"] span[id="seq"]')
        span = ''
        for temp in r2:
            span = temp.get_text()
            span = span.split('/')[1][:-1]
            break
        return span + ";" + url
    except:
        return ''



if __name__ == '__main__':
    url = 'http://tu.duowan.com/m/bxgif'
    url = 'http://tu.duowan.com/gallery/138032.html'
    path = r'E:\zhy\works\qt\bin\x64\Debug\gif\19-01-24\urls.json'
    #getGifUrls(path, url, 100)
    getGifUrlsFromMainPage(url, r'E:\zhy\works\qt\DuoWanDownload\..\bin\x64\Debug\gif\19-01-25\src\html.html')


