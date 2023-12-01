#!/usr/bin/env python3
import requests
import websocket
import json

URL = 'http://localhost:8000'

def register():
    u = input('username: ').strip()
    p = input('password: ').strip()
    r = requests.post(URL+'/api/register', json={'username':u,'password':p})
    print(r.text)

def login():
    u = input('username: ').strip()
    p = input('password: ').strip()
    r = requests.post(URL+'/api/login', json={'username':u,'password':p})
    print(r.text)

def create_station():
    x = int(input('x: '))
    y = int(input('y: '))
    r = requests.post(URL+'/api/station', json={'x':x,'y':y})
    print(r.text)

def link_stations():
    id1 = int(input('id1: '))
    id2 = int(input('id2: '))
    r = requests.post(URL+f'/api/link/{id1}/{id2}')
    print(r.text)

def send():
    uid = input('uid: ').strip()
    balance = int(input("balance: "))
    fr = int(input('from: '))
    to = int(input('to: '))
    # msg = input('msg: ').strip()
    r = requests.post(URL+'/api/send', json={'uid':uid,'msg':'a'*0x80,'litr':1,'money':balance,'from':fr,'to':to})
    print(r.text)

def add_money():
    uid = input('uid: ').strip()
    balance = int(input('balance: '))
    station_id = int(input('station_id: '))
    oil_id = int(input('oil_id: '))
    r = requests.post(URL+"/api/add_money", json.dumps({"type":"ADD_MONEY","uid":uid,"amount":balance,"station_id":station_id,"oil_id":oil_id}))
    print(r.text)


def webs():
    w = websocket.WebSocket()
    w.connect('ws://localhost:8000/ws')
    uid = input('uid: ').strip()
    w.send(json.dumps({'uid': uid, 'type':'INIT'}))
    print(w.recv())
    while 1:
        c = int(input('1. Read\n2. Write\n3. Exit\n> '))
        match c:
            case 1:
                print(w.recv())
            case 2:
                w.send(input('Data: ').strip())
            case 3:
                break


def main():
    while 1:
        print('1. Login\n2. Register\n3. Create station\n4. Link stations\n5. Send oil\n6. Add money\n7. Webs')
        c = int(input('> '))
        match c:
            case 1:
                login()
            case 2:
                register()
            case 3:
                create_station()
            case 4:
                link_stations()
            case 5:
                send()
            case 6:
                add_money()
            case 7:
                webs()


if __name__ == '__main__':
    main()
    # r = requests.post(URL+f'/api/send', json={'uid':'8a3fda30729592406110385a541aba19','balance':['aaa'],'from':1,'to':1})
    # print(r.text)

'''
0x49 - _ZdlPvm
-73
b *'_ZdlPvm@plt'
b *'StationImpl::AddMoney(grpc::ServerContext*, AddMoneyRequest const*, None*) '

-0x38aeb0
-3714736

--------------------------------

grpc::Server::SyncRequest::ContinueRunAfterInterception()+530

b *'StationImpl::SendOil(grpc::ServerContext*, SendOilRequest const*, None*) '+0x132
b *'StationImpl::AddMoney(grpc::ServerContext*, AddMoneyRequest const*, None*) '

0x3eb * 0x60

-0x3395a0 - offset to libc.so.6
-3380640

p/x 0x7f2eaea46665-0x00007f2eae99b000
0xfffffffffffffeb8
0xab665

-0x177cf3
-1539315


rdi = 0x56...620
rax = [0x56...620] = 0xdead
rax + 0x30 = [0xdead+0x30]

0xfffffffffffffe58

-0x339570
-3380592

0x39733891

'''