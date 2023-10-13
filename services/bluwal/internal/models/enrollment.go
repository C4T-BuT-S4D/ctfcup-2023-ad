package models

type Enrollment struct {
	ID               string
	ContestID        string
	CurrentChallenge int
	CurrentState     []int32
}
