import grpc
from proto.bluwal.bluwal_pb2 import (
    Contest,
    ContestGetRequest,
    ContestGetResponse,
    ContestEnrollRequest,
    ContestEnrollResponse,
    ClaimRewardRequest,
    ClaimRewardResponse,
)
from proto.bluwal.bluwal_service_pb2_grpc import BluwalServiceStub

PORT = 9090


class SploitMachine:
    def __init__(self, host: str):
        self.host = host

    def connect(self) -> (grpc.Channel, BluwalServiceStub):
        addr = f"{self.host}:{PORT}"
        channel = grpc.insecure_channel(addr)
        return channel, BluwalServiceStub(channel)

    @staticmethod
    def get_contest(
        stub: BluwalServiceStub,
        contest_id: str,
        author: str = "",
    ) -> Contest:
        resp: ContestGetResponse = stub.ContestGet(
            ContestGetRequest(id=contest_id, author=author)
        )
        return resp.contest

    @staticmethod
    def enroll(stub: BluwalServiceStub, contest_id: str, initial: list[int]) -> str:
        resp: ContestEnrollResponse = stub.ContestEnroll(
            ContestEnrollRequest(contest_id=contest_id, initial=initial)
        )
        return resp.enrollment_id

    @staticmethod
    def claim_reward(
        stub: BluwalServiceStub, contest_id: str, enrollment_id: str
    ) -> str:
        resp: ClaimRewardResponse = stub.ClaimReward(
            ClaimRewardRequest(contest_id=contest_id, enrollment_id=enrollment_id)
        )
        return resp.reward
