#coding:utf-8
import sys
import socket
from random import randint

BUF_LEN = 256

def send(msg=None):
  if msg==None:
    msg = raw_input()
  clientsock.sendall(msg + '\n')
  print msg

def receive():
  msg = clientsock.recv(BUF_LEN)
  print msg.rstrip()
  return msg

def createBids():
  bids = ''
  for i in xrange(0, nItems):
    bids += str(randint(0, 1))
  return bids

# コマンドライン引数処理
if len(sys.argv) < 3:
  print 'usage: python client.py [hostname] [port]'
  exit(1)

host = sys.argv[1]
port = int(sys.argv[2])

clientsock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
clientsock.connect((host,port))

# PLEASE INPUT YOUR NAMEを受信
receive()

# 名前を送信
agentName = 'AGENT%02d' % randint(0, 99)
send(agentName)

# エージェントリストを受信して名前のリストを生成
agentList = map((lambda x: x[x.find(':')+1:]), receive().split())

# 自分のIDを受信
myID = int(receive()[9:])

# 商品の数, エージェントの数を受信
nItems, nAgents = map(int, receive().split(','))

# 商品価格と入札結果を蓄積するリスト
priceList = []
bidList = []

while True:
  # 入札額を決定し送信
  send(createBids())

  # 入札結果を受信してリストに蓄積
  result = map((lambda x: x[x.find(':')+1:]), receive().split())
  prices = result[0:nItems]
  bids = result[nItems:nItems+nAgents]
  priceList.append(prices)
  bidList.append(bids)

  # 入札後の商品の価格を受信してリストを生成 (endを受信した場合はbreak)
  newPrices = receive()
  if newPrices.rstrip() == 'end':
    break
  else:
    newPrices = map((lambda x: int(x[x.find(':')+1:])), newPrices.split())

# 接続を閉じる
clientsock.close()

print priceList
print bidList