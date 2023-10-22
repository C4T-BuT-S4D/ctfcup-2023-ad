package service

import (
	"context"
	"errors"
	"slices"

	"bluwal/internal/challenges"
	"bluwal/internal/controller"
	"bluwal/internal/convert"
	"bluwal/internal/models"
	bwpb "bluwal/pkg/proto/bluwal"

	"github.com/google/uuid"
	"google.golang.org/grpc/codes"
	"google.golang.org/grpc/status"
)

type Service struct {
	bwpb.UnimplementedBluwalServiceServer

	c *controller.Controller
}

func NewService(c *controller.Controller) *Service {
	return &Service{
		c: c,
	}
}

func (s *Service) ContestCreate(_ context.Context, request *bwpb.ContestCreateRequest) (*bwpb.ContestCreateResponse, error) {
	if request.Contest == nil {
		return nil, status.Errorf(codes.InvalidArgument, "contest must be set")
	}
	if len(request.Contest.Threshold) == 0 {
		return nil, status.Errorf(codes.InvalidArgument, "threshold must be set")
	}
	if len(request.Contest.Goal) == 0 {
		return nil, status.Errorf(codes.InvalidArgument, "goal must be set")
	}
	if len(request.Contest.Challenges) == 0 {
		return nil, status.Errorf(codes.InvalidArgument, "challenges must be set")
	}
	if request.Contest.Reward == "" {
		return nil, status.Errorf(codes.InvalidArgument, "reward must be set")
	}
	for _, chal := range request.Contest.Challenges {
		if chal == nil {
			return nil, status.Errorf(codes.InvalidArgument, "challenges must be set")
		}
		if chal.Characteristic < 0 {
			return nil, status.Errorf(codes.InvalidArgument, "characteristic must be set")
		}
	}
	request.Contest.Id = uuid.NewString()

	contest, err := models.NewContestFromProto(request.Contest)
	if err != nil {
		return nil, status.Errorf(codes.InvalidArgument, "invalid contest: %v", err)
	}
	if err := s.c.CreateContest(contest); err != nil {
		return nil, status.Errorf(codes.Internal, "creating contest: %v", err)
	}

	res, err := contest.ToProto()
	if err != nil {
		return nil, status.Errorf(codes.Internal, "marshalling contest: %v", err)
	}
	return &bwpb.ContestCreateResponse{
		Contest: res,
	}, nil
}

func (s *Service) ContestGet(_ context.Context, request *bwpb.ContestGetRequest) (*bwpb.ContestGetResponse, error) {
	if request.Id == "" {
		return nil, status.Errorf(codes.InvalidArgument, "id must be set")
	}

	contest, err := s.c.GetContest(request.Id)
	if err != nil {
		return nil, status.Errorf(codes.Internal, "getting contest: %v", err)
	}

	if contest.Author != request.Author {
		contest.Reward = ""
	}

	res, err := contest.ToProto()
	if err != nil {
		return nil, status.Errorf(codes.Internal, "marshalling contest: %v", err)
	}
	return &bwpb.ContestGetResponse{
		Contest: res,
	}, nil
}

func (s *Service) ContestEnroll(_ context.Context, request *bwpb.ContestEnrollRequest) (*bwpb.ContestEnrollResponse, error) {
	if request.ContestId == "" {
		return nil, status.Errorf(codes.InvalidArgument, "contest id must be set")
	}
	if len(request.Initial) == 0 {
		return nil, status.Errorf(codes.InvalidArgument, "initial must be set")
	}

	contest, err := s.c.GetContest(request.ContestId)
	if err != nil {
		return nil, status.Errorf(codes.Internal, "getting contest: %v", err)
	}

	enrollment := &models.Enrollment{
		ID:           uuid.NewString(),
		ContestID:    request.ContestId,
		CurrentState: convert.Int32ToInt(request.Initial),
	}
	if slices.Compare(contest.Goal, enrollment.CurrentState) <= 0 {
		return nil, status.Errorf(codes.FailedPrecondition, "you are too good already")
	}
	if slices.Compare(contest.Threshold, enrollment.CurrentState) > 0 {
		return nil, status.Errorf(codes.FailedPrecondition, "lol, noob")
	}

	if err := s.c.CreateEnrollment(enrollment); err != nil {
		return nil, status.Errorf(codes.Internal, "creating enrollment: %v", err)
	}

	return &bwpb.ContestEnrollResponse{
		EnrollmentId: enrollment.ID,
	}, nil
}

func (s *Service) ChallengeSubmit(_ context.Context, request *bwpb.ChallengeSubmitRequest) (*bwpb.ChallengeSubmitResponse, error) {
	if request.EnrollmentId == "" {
		return nil, status.Errorf(codes.InvalidArgument, "enrollment id must be set")
	}
	if request.ContestId == "" {
		return nil, status.Errorf(codes.InvalidArgument, "contest id must be set")
	}

	contest, err := s.c.GetContest(request.ContestId)
	if err != nil {
		return nil, status.Errorf(codes.Internal, "getting contest: %v", err)
	}
	contestProto, err := contest.ToProto()
	if err != nil {
		return nil, status.Errorf(codes.Internal, "marshalling contest: %v", err)
	}

	enrollment, err := s.c.GetEnrollment(request.ContestId, request.EnrollmentId)
	if err != nil {
		return nil, status.Errorf(codes.Internal, "getting enrollment: %v", err)
	}

	if enrollment.CurrentChallenge >= len(contest.Challenges) {
		return nil, status.Errorf(codes.InvalidArgument, "no more challenges")
	}

	challenge := contestProto.Challenges[enrollment.CurrentChallenge]

	if int(challenge.Characteristic) >= len(enrollment.CurrentState) {
		return nil, status.Errorf(codes.InvalidArgument, "invalid characteristic")
	}

	if err := challenges.Check(challenge, request); err != nil {
		return nil, status.Errorf(codes.InvalidArgument, "invalid submission: %v", err)
	}

	enrollment.CurrentChallenge++
	enrollment.CurrentState[int(challenge.Characteristic)] += int(challenge.Delta)
	if err := s.c.UpdateEnrollment(enrollment); err != nil {
		return nil, status.Errorf(codes.Internal, "updating enrollment: %v", err)
	}

	return &bwpb.ChallengeSubmitResponse{
		CurrentChallenge: int32(enrollment.CurrentChallenge),
		CurrentState:     convert.IntToInt32(enrollment.CurrentState),
	}, nil
}

func (s *Service) CheckGoal(_ context.Context, request *bwpb.CheckGoalRequest) (*bwpb.CheckGoalResponse, error) {
	if request.EnrollmentId == "" {
		return nil, status.Errorf(codes.InvalidArgument, "enrollment id must be set")
	}
	if request.ContestId == "" {
		return nil, status.Errorf(codes.InvalidArgument, "contest id must be set")
	}

	enrollment, err := s.c.GetEnrollment(request.ContestId, request.EnrollmentId)
	if err != nil {
		return nil, status.Errorf(codes.Internal, "getting enrollment: %v", err)
	}

	return &bwpb.CheckGoalResponse{
		CurrentChallenge: int32(enrollment.CurrentChallenge),
		CurrentState:     convert.IntToInt32(enrollment.CurrentState),
	}, nil
}

func (s *Service) ClaimReward(_ context.Context, request *bwpb.ClaimRewardRequest) (*bwpb.ClaimRewardResponse, error) {
	if request.EnrollmentId == "" {
		return nil, status.Errorf(codes.InvalidArgument, "enrollment id must be set")
	}
	if request.ContestId == "" {
		return nil, status.Errorf(codes.InvalidArgument, "contest id must be set")
	}

	contest, err := s.c.GetContest(request.ContestId)
	if err != nil {
		return nil, status.Errorf(codes.Internal, "getting contest: %v", err)
	}

	if err := s.c.CheckEnrollmentComplete(request.ContestId, request.EnrollmentId); err != nil {
		if errors.Is(err, controller.ErrEnrollmentNotComplete) {
			return nil, status.Errorf(codes.FailedPrecondition, "enrollment not complete")
		}
		return nil, status.Errorf(codes.Internal, "enrollment not complete: %v", err)
	}

	return &bwpb.ClaimRewardResponse{
		Reward: contest.Reward,
	}, nil
}
