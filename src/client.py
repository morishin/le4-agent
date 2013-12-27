#coding:utf-8
import sys
import socket

BUF_LEN = 256

def send():
  msg = raw_input() + '\n'
  clientsock.sendall(msg)

def receive():
  msg = clientsock.recv(BUF_LEN)
  print msg,

# コマンドライン引数処理
if len(sys.argv) < 3:
  print 'usage: python client.py [hostname] [port]'
  exit(1)

host = sys.argv[1]
port = int(sys.argv[2])

clientsock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
clientsock.connect((host,port))

receive() # PLEASE INPUT YOUR NAMEを受信
send()    # 名前を送信
receive() # エージェントリストを受信
receive() # 自分のIDを受信
receive() # 商品の数, エージェントの数を受信

while True:
  send()    # 入札を送信
  receive() # 入札結果を受信
  receive() # 商品の価格を受信 (終了時はendを受信)

clientsock.close()