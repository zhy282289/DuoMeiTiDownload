import urllib
import urllib.request
import os


def download(url, dir):
    path = os.path.join(dir, os.path.split(url)[1])
    urllib.request.urlretrieve(url, path)
    return 1



if __name__ == '__main__':
   url = 'http://gmp4.dwstatic.com/1904/8261977267.mp4'
   dir = r'C:\Users\zhanghy\Desktop\snow'
   download(url, dir)