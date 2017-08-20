# -*- coding: utf-8 -*- 

import urllib
import urllib2
import os.path
import os

class Scraping:
	#指定URLのhtmlを返す関数
	def get_html(self, url, header):
		html = ""
		req = urllib2.Request(url, None, header)
		f = urllib2.urlopen(req)
		html = f.read()
		f.close()
		return html

	#textの中からst,enで囲まれたすべての文字列をリストで返す関数
	def find(self, text, st, en):
		found_text_list = []
		text_buf = text
		while 1:
			if st != "":
				i = text_buf.find(st)
				if i == -1:
					break
			else:
				i = 0
			text_buf = text_buf[i+len(st):]
			if en != "":
				i = text_buf.find(en)
				if i == -1:
					break
			else:
				i = len(text)
			found_text_list.append(text_buf[:i])
			text_buf = text_buf[i+len(en):]
		return found_text_list

	#特定の文字列を除外する
	def delete(self, text, target):
		req_text = text
		while 1:
			index = req_text.find(target)
			if index != -1:
				req_text = req_text[0:index] + req_text[index+len(target):]
			else:
				break
		return req_text

	#指定URLの画像を保存する関数
	def save_pic(self, url, filename):
		urllib.urlretrieve(url, filename)
		return

	#指定リスト全てにfunc関数実行
	def func_list(self, list, func):
		req_list = []
		for i in list:
			req_list.append(func(i))
		return req_list

	#指定リストから指定文字列が含まれているもののみ抽出(ok=0で除外)
	def filter(self, list, text, ok):
		req_list = []
		for i in list:
			if ok == 0:
				if i.find(text) == -1:
					req_list.append(i)
			else:
				if i.find(text) != -1:
					req_list.append(i)
		return req_list

	#find関数で得られたリストをさらにfind関数にかける処理を指定回数ループする関数
	#使い方
	#find_list=[
	#	["1回目のfind関数のst","1回目のfind関数のen"],
	#	["2回目のfind関数のst","2回目のfind関数のen"],
	#	...
	#	["n回目のfind関数のst","n回目のfind関数のen"],
	#]
	def find_to_find(self, text ,find_list):
		list_buf1 = []
		list_buf2 = []
		list_buf1.append(text)
		for i in find_list:
			list_buf2 = []
			for j in list_buf1:
				list_buf2.extend(self.find(j, i[0], i[1]))
			list_buf1 = list_buf2
		return list_buf1

	#urlからfind_listで見つかった文字列リストを返す処理
	def find_in_html(self, url, find_list, header):
		return self.find_to_find(
			self.get_html(url, header),
			find_list
		)

class flag():
	sharp
	
	def init(self):


def read_code(filename):
	f = open(filename, "r")
	for line in f:
		for char in list(line):
			if char in {"\t", " "}
			if char ==  
	f.close

def main():
	read_code("p1.gpu")

main()