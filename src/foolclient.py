#coding:utf-8
import sys
import socket
import numpy as np
from scipy.linalg import norm
from random import randint
from pprint import pprint
import json

# 自作のSVMクラス
from SVM import SVM

# 受信データ用のバッファイサイズ
BUF_LEN = 256

#===========================================================================
# 関数定義
#===========================================================================

# サーバへ送信
def send(msg=None):
  # 引数無しの時はユーザーからの入力を受け付ける
  if msg==None:
    msg = raw_input()
  clientsock.sendall(msg + '\n')
  print msg

# サーバから受信
def receive():
  msg = clientsock.recv(BUF_LEN)
  print msg.rstrip()
  return msg

# 商品の落札価格を推定
def estimatePrice(itemNumber):
  maxPrice = 0
  for agentName in bidHistory:
    historyData = bidHistory[agentName]
    for h in historyData:
      if h['itemSet'] == [itemNumber]:
        # エージェントagentNameの商品itemNumberへの入札に対するSVM
        svm = h['svm']
        break

    price = 0
    d = svm.discriminate([price])
    if d == None:
      # SVMが正常に作成出来ていなければNoneを返す
      return None
    else:
      while d==1.0:
        price += 1
        d = svm.discriminate([price])
    maxPrice = max(maxPrice, price)
  return maxPrice

# 自己の評価値のみで決定した入札
def bidByMyself():
  bids = ''
  for i in xrange(0, nItems):
    for d in evalData:
      if d['itemSet'] == [i]:
        price = currentPrice[i]
        if price < d['value']:
          bids += '1'
        else:
          bids += '0'
        break
  return bids

# 入札を生成
def createBids():
  return bidByMyself()

# リストから指定のインデックス(複数)の要素を取り出し、それらのリストを返す
def subSequenceWithIndexes(indexes, sequence):
  sub = []
  for i in indexes:
    sub.append(sequence[i])
  return sub

# リストの要素が全て1かどうか
def listIsOnes(ls):
  for x in ls:
    if int(x) != 1:
      return False
  return True

# 与えられたリストの冪集合の要素を順に返すジェネレータ
def powersetGenerator(ls):
  if len(ls) > 0:
    for c in powersetGenerator(ls[1:]):
      yield [ls[0]] + c
      yield c
  else:
    yield []

# カーネルトリック
def gaussianKernel(x, y, sigma=10):
  return np.exp(-norm(x-y)**2 / (2*sigma**2))
def sigmoidKernel(x, y, a=0.002, b=-1.5):
  return np.tanh(a * np.dot(x, y) - b)
def polynomialKernel(x, y, d=2):
  return (1 + np.dot(x, y))**d


#===========================================================================
# メイン処理
#===========================================================================

if __name__ == '__main__':
  # コマンドライン引数からhostとportを取得
  if len(sys.argv) < 4:
    print 'usage: python client.py [hostname] [port] [eval_file]'
    exit(1)

  host = sys.argv[1]
  port = int(sys.argv[2])
  eval_file = sys.argv[3]

  # 自分の名前を設定
  myName = 'AGENT%02d' % randint(0, 99)
  # 商品組毎の評価値データを読み込む
  f = open(eval_file)
  evalData = json.load(f)
  evalData = sorted(evalData, key=lambda x: x['value'])
  f.close()
  # 入札履歴のデータを蓄積するディクショナリ
  bidHistory = {}
  # オークションが何日目か
  date = 0

  # オークションを繰り返すループ
  while True:
    # 2日目以降はオークションを続けるかどうか尋ねる
    if date > 0:
      # 'y'か'n'が入力されるまで繰り返し尋ねる
      while True:
        print 'continue? [y/n] ',
        ans = raw_input().rstrip()
        if ans == 'y':
          continued = True
          break
        elif ans == 'n':
          continued = False
          break
      # 'n'だったら入札履歴をファイルに書き出して終了
      if not continued:
        fileName = 'bidHistory_'+myName+'.txt'
        with open(fileName, 'w') as out:
          print 'history data is saved as \''+fileName+'\''
          pprint(bidHistory, stream=out)
        exit(0)

    # サーバに接続
    clientsock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    clientsock.connect((host,port))

    # PLEASE INPUT YOUR NAMEを受信
    receive()

    # 名前を送信
    send(myName)

    # エージェントリストを受信して名前のリストを生成
    agentNameList = map((lambda x: x[x.find(':')+1:]), receive().split())

    # 自分のIDを受信
    myID = int(receive()[9:])

    # 商品の数, エージェントの数を受信
    nItems, nAgents = map(int, receive().split(','))

    # 最も効用の高いと推定した商品組のデータを格納するディクショナリ
    targetData = {'itemSet': None, 'benefit': None}

    # 商品価格と入札結果を蓄積するリスト
    priceList = []
    bidList = []

    # 落札した商品番号を格納
    itemsWin = []

    # 現在の商品価格(全て0で初期化)
    currentPrice = [0] * nItems

    #===========================================================================
    # オークションの実施
    #===========================================================================

    while True:
      # 入札額を決定し送信
      bid = createBids()
      send(bid)

      # 入札結果を受信してリストに蓄積
      result = map((lambda x: x[x.find(':')+1:]), receive().split())
      prices = result[0:nItems]
      bids = result[nItems:nItems+nAgents]
      priceList.append(prices)
      bidList.append(bids)

      # 入札後の商品の価格を受信してリストを生成 (endを受信した場合はbreakしてオークションを終了)
      currentPrice = receive()
      if currentPrice.rstrip() == 'end':
        break
      else:
        # この値は後々入札を決定するのに利用する予定
        currentPrice = map((lambda x: int(x[x.find(':')+1:])), currentPrice.split())

    winners = map((lambda x: int(x[x.find(':')+1:])), receive().split()[1:])
    prices = map((lambda x: int(x[x.find(':')+1:])), receive().split()[1:])
    benefit = 0
    for itemNumber, winnerID in enumerate(winners):
      if winnerID == myID:
        itemsWin.append(itemNumber)
        for d in evalData:
          if d['itemSet'] == [itemNumber]:
            benefit += d['value'] - prices[itemNumber]
            break
    # 接続を閉じる
    clientsock.close()

    #===========================================================================
    # 入札履歴の作成 
    #===========================================================================

    # 全ての商品組のリスト(冪集合)を生成
    itemPowerSet = powersetGenerator(range(nItems))

    # 商品組毎に入札履歴を作成する
    for itemSet in itemPowerSet:
      if itemSet == []:
        # 全ての商品組について処理を終えたら抜ける
        break

      # powersetGeneratorで取り出した商品組が昇順になっていないのでソート
      itemSet.sort()

      # itemSetに対する、各エージェントの入札履歴をbidHostoryにappendしていく
      for agentIndex, agentName in enumerate(agentNameList):
        # 自分の入札履歴はスキップ
        if agentIndex == myID-1:
          continue
        # このagentNameに関するデータがbidHistoryに存在しなければ空のリストを生成
        if agentName not in bidHistory:
          bidHistory[agentName] = []
        
        for historyData in bidHistory[agentName]:
          # 同じitemSetの履歴が存在すればそこに入札履歴を追加していく
          if historyData['itemSet'] == itemSet:
            break
        else:
          # 存在しなければ入札履歴を格納するディクショナリを新しく生成して追加
          historyData = {'itemSet': itemSet, 'bids': [], 'svm': None}
          bidHistory[agentName].append(historyData)

        # itemSetの価格とそれに対する入札結果のリストから、入札履歴を表すリストを生成し、bidHistoryに追加していく
        for price, bid in zip(priceList, bidList):
          price = map(float, price)
          price = subSequenceWithIndexes(itemSet, price)
          bidSet = subSequenceWithIndexes(itemSet, bid[agentIndex])
          if listIsOnes(bidSet):
            # itemSetの全ての商品に対して入札している場合は1
            historyData['bids'].append(price + [1.0])
          else:
            # そうでない場合は-1
            historyData['bids'].append(price + [-1.0])
        # SVMを作成
        D = np.array(historyData['bids'])
        n = len(D)        # データ点の個数
        d = len(D[0])-1   # データ点の次元
        X = D[:, :d]      # データ点の配列
        Y = np.reshape(D[:, d:], n) #データ点の属するクラスの配列
        svm = SVM(X, Y, gaussianKernel)
        
        # 入札履歴データに作成したSVMを格納(次回以降の入札戦略に使用予定)
        historyData['svm'] = svm

    # この日の入札履歴を表示
    pprint(bidHistory)

    # 落札した商品のリスト
    print 'get:',
    print itemsWin

    # この日の効用を出力
    print 'benefit: %d' % benefit

    # 1日進める
    date += 1
