#coding:utf-8
import sys
import socket
from random import randint
from pprint import pprint

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

def subSequenceWithIndexes(indexes, sequence):
  sub = []
  for i in indexes:
    sub.append(sequence[i])
  return sub

def listIsOnes(ls):
  for x in ls:
    if int(x) != 1:
      return False
  return True

# 与えられた集合（配列）のべき集合の要素を順に返すジェネレータ
def powersetGenerator(provided_set):
  if len(provided_set) > 0:
    for c in powersetGenerator(provided_set[1:]):
      yield [provided_set[0]] + c
      yield c
  else:
    yield []

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
agentNameList = map((lambda x: x[x.find(':')+1:]), receive().split())

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

#####################
# 以下で入札履歴を作成

# 入札履歴のデータを蓄積するディクショナリ
bidHistory = {}

# 全ての商品組のリストを生成
itemPowerSet = powersetGenerator(range(nItems))

# 商品組毎に入札履歴を作成する
for itemSet in itemPowerSet:
  if itemSet == []:
    # 全ての商品組について処理を終えたら抜ける
    break

  # powersetGeneratorで取り出した商品組が昇順になっていないのでソート
  itemSet.sort()

  # ある商品組に対する、各エージェントの入札履歴をbidHostoryにappendしていく
  for agentNumber, agentName in enumerate(agentNameList):
    if agentName not in bidHistory:
      bidHistory[agentName] = []
    bidHistory[agentName].append({'itemSet': itemSet, 'bids': []})
    bids = bidHistory[agentName][-1]['bids']
    for price, bid in zip(priceList, bidList):
      bidSet = subSequenceWithIndexes(itemSet, bid[agentNumber])
      if listIsOnes(bidSet):
        bids.append(price + [1])
      else:
        bids.append(price + [-1])

pprint(bidHistory)

# priceList と bidList から SVM作成に必要なデータを作る (以下のようなDictionary)
# {
#   'AGENT01': [
#     {
#       'itemSet': [0],
#       'bids': [
#         [0, 1],
#         [1, 1],
#         [2, -1]
#       ]
#     },
#     {
#       'itemSet': [1],
#       'bids': [
#         [0, 1],
#         [1, -1],
#         [2, -1]
#       ]
#     },
#     {
#       'itemSet': [0, 1],
#       'bids': [
#         [0, 0, 1],
#         [1, 1, -1],
#         [2, 1, -1]
#       ]
#     }
#   ],
#
#   'AGENT02': [
#       .
#       .
#       .
#   ],
#
#       .
#       .
#       .
# }
#
# 上のデータを用いて、各エージェントに対し、商品毎のSVM あるいは 商品組毎のSVM を作る
#
# SVMを作って、そのエージェントが何円まで入札してくるか(=評価値)を推定する
# -> 自分以外の全てのエージェントの評価値を推定して、自分の評価値との比較で、どの商品組で入札していくかを決める。
# -> 入札する商品組を決めたら、評価値に達するまで入札し続ける
