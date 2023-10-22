import grpc
from proto.bluwal.bluwal_pb2 import (
    Contest,
    EnrollmentFilter,
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
    def enroll(
        stub: BluwalServiceStub, enrollment_filter: EnrollmentFilter
    ) -> EnrollmentFilter:
        resp: ContestEnrollResponse = stub.ContestEnroll(
            ContestEnrollRequest(enrollment_filter=enrollment_filter)
        )
        return resp.enrollment_filter

    @staticmethod
    def claim_reward(
        stub: BluwalServiceStub, enrollment_filter: EnrollmentFilter
    ) -> str:
        resp: ClaimRewardResponse = stub.ClaimReward(
            ClaimRewardRequest(enrollment_filter=enrollment_filter)
        )
        return resp.reward
