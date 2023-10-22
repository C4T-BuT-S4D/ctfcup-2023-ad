package controller

import (
	"errors"
	"fmt"

	"bluwal/internal/models"

	"github.com/genjidb/genji"
	"github.com/genjidb/genji/document"
)

var (
	ErrEnrollmentNotComplete = errors.New("enrollment is not complete")
)

type Controller struct {
	db *genji.DB
}

func NewController(db *genji.DB) (*Controller, error) {
	c := &Controller{
		db: db,
	}
	if err := c.migrate(); err != nil {
		return nil, fmt.Errorf("migrating: %w", err)
	}
	return c, nil
}

func (c *Controller) CreateContest(contest *models.Contest) error {
	if err := c.db.Exec("INSERT INTO contests VALUES ?", contest); err != nil {
		return fmt.Errorf("inserting into db: %w", err)
	}
	return nil
}

func (c *Controller) GetContest(id string) (*models.Contest, error) {
	var contest models.Contest
	doc, err := c.db.QueryDocument("SELECT * FROM contests WHERE id = ?", id)
	if err != nil {
		return nil, fmt.Errorf("querying contest: %w", err)
	}
	if err := document.StructScan(doc, &contest); err != nil {
		return nil, fmt.Errorf("scanning contest: %w", err)
	}
	return &contest, nil
}

func (c *Controller) CreateEnrollment(enrollment *models.Enrollment) error {
	if err := c.db.Exec("INSERT INTO enrollments VALUES ?", enrollment); err != nil {
		return fmt.Errorf("inserting into db: %w", err)
	}
	return nil
}

func (c *Controller) UpdateEnrollment(enrollment *models.Enrollment) error {
	if err := c.db.Exec(
		"UPDATE enrollments SET current_challenge = ?, current_state = ? WHERE id = ?",
		enrollment.CurrentChallenge,
		enrollment.CurrentState,
		enrollment.ID,
	); err != nil {
		return fmt.Errorf("updating enrollment: %w", err)
	}
	return nil
}

func (c *Controller) GetEnrollment(contestID, id string) (*models.Enrollment, error) {
	var enrollment models.Enrollment
	doc, err := c.db.QueryDocument("SELECT * FROM enrollments WHERE id = ? AND contest_id = ?", id, contestID)
	if err != nil {
		return nil, fmt.Errorf("querying enrollment: %w", err)
	}
	if err := document.StructScan(doc, &enrollment); err != nil {
		return nil, fmt.Errorf("scanning enrollment: %w", err)
	}
	return &enrollment, nil
}

func (c *Controller) CheckEnrollmentComplete(contestID, id string) error {
	enrollment, err := c.GetEnrollment(contestID, id)
	if err != nil {
		return fmt.Errorf("getting enrollment: %w", err)
	}

	doc, err := c.db.QueryDocument(
		"SELECT count(*) FROM contests WHERE id = ? AND goal <= ?",
		contestID,
		enrollment.CurrentState,
	)
	if err != nil {
		return fmt.Errorf("querying enrollment: %w", err)
	}

	var cnt int
	if err := document.Scan(doc, &cnt); err != nil {
		return fmt.Errorf("scanning contest count: %w", err)
	}
	if cnt == 0 {
		return ErrEnrollmentNotComplete
	}
	return nil
}

func (c *Controller) migrate() error {
	if err := c.db.Exec(
		`CREATE TABLE IF NOT EXISTS contests (
				id TEXT PRIMARY KEY,
				author TEXT NOT NULL,
				reward TEXT NOT NULL,
				goal ARRAY,
				...
		)`,
	); err != nil {
		return fmt.Errorf("creating contests table: %w", err)
	}

	if err := c.db.Exec(
		`CREATE TABLE IF NOT EXISTS enrollments (
				id TEXT PRIMARY KEY,
				contest_id TEXT NOT NULL,
				...
		)`,
	); err != nil {
		return fmt.Errorf("creating enrollments table: %w", err)
	}

	if err := c.db.Exec("CREATE INDEX IF NOT EXISTS idx_contests_id_goal ON contests (id, goal)"); err != nil {
		return fmt.Errorf("creating idx_contests_id_goal index: %w", err)
	}
	if err := c.db.Exec("CREATE INDEX IF NOT EXISTS idx_enrollments_id_contest_id ON enrollments (id, contest_id)"); err != nil {
		return fmt.Errorf("creating idx_enrollments_id_contest_id index: %w", err)
	}

	return nil
}
