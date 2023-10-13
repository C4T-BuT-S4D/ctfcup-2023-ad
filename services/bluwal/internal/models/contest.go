package models

import (
	"fmt"

	bwpb "bluwal/pkg/proto/bluwal"

	"github.com/golang/protobuf/proto"
)

type Contest struct {
	ID     string
	Author string

	Goal      []int32
	Threshold []int32

	Challenges [][]byte

	Reward string
}

func NewContestFromProto(p *bwpb.Contest) (*Contest, error) {
	challenges := make([][]byte, len(p.Challenges))
	for i, c := range p.Challenges {
		raw, err := proto.Marshal(c)
		if err != nil {
			return nil, fmt.Errorf("marshalling contect %d: %w", i, err)
		}
		challenges = append(challenges, raw)
	}

	return &Contest{
		ID:         p.Id,
		Author:     p.Author,
		Goal:       p.Goal,
		Threshold:  p.Threshold,
		Challenges: challenges,
		Reward:     p.Reward,
	}, nil
}

func (c *Contest) ToProto() (*bwpb.Contest, error) {
	challenges := make([]*bwpb.Challenge, len(c.Challenges))
	for i, ch := range c.Challenges {
		p := new(bwpb.Challenge)
		if err := proto.Unmarshal(ch, p); err != nil {
			return nil, fmt.Errorf("unmarshalling challenge %d: %w", i, err)
		}
		challenges[i] = p
	}

	return &bwpb.Contest{
		Id:         c.ID,
		Author:     c.Author,
		Goal:       c.Goal,
		Threshold:  c.Threshold,
		Challenges: challenges,
		Reward:     c.Reward,
	}, nil
}
