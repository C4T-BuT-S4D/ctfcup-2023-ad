package controller

import (
	"fmt"

	"bluwal/internal/models"

	"github.com/genjidb/genji"
	"github.com/genjidb/genji/document"
)

type Controller struct {
	db *genji.DB
}

func NewController(db *genji.DB) *Controller {
	return &Controller{
		db: db,
	}
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
	var enrollment models.Enrollment
	doc, err := c.db.QueryDocument(
		`
			SELECT * FROM enrollments 
			WHERE 
				id = ? AND 
				contest_id = ? AND 
				(
					SELECT goal FROM contests 
					WHERE contest_id = ?
				) <= current
			`,
		id,
		contestID,
	)
	if err != nil {
		return fmt.Errorf("querying enrollment: %w", err)
	}
	if err := document.StructScan(doc, &enrollment); err != nil {
		return fmt.Errorf("scanning enrollment: %w", err)
	}
	return nil
}
