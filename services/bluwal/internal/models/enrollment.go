package models

type Enrollment struct {
	ID               string `genji:"id"`
	ContestID        string `genji:"contest_id"`
	CurrentChallenge int    `genji:"current_challenge"`
	CurrentState     []int  `genji:"current_state"`
}
