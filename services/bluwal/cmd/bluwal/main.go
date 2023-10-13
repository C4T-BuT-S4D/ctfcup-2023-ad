package main

import (
	"context"
	"errors"
	"net"
	"os"
	"os/signal"
	"syscall"

	"bluwal/internal/controller"
	"bluwal/internal/service"
	bwpb "bluwal/pkg/proto/bluwal"

	"github.com/genjidb/genji"
	"github.com/sirupsen/logrus"
	"google.golang.org/grpc"
	"google.golang.org/grpc/reflection"
)

func main() {
	dbPath := os.Getenv("DB_PATH")
	if dbPath == "" {
		dbPath = "bluwal.db"
	}
	db, err := genji.Open(dbPath)
	if err != nil {
		logrus.Fatalf("opening db: %v", err)
	}

	bc := controller.NewController(db)
	bs := service.NewService(bc)

	server := grpc.NewServer()
	reflection.Register(server)

	bwpb.RegisterBluwalServiceServer(server, bs)

	runCtx, runCancel := signal.NotifyContext(context.Background(), syscall.SIGTERM, syscall.SIGINT)
	defer runCancel()

	go func() {
		logrus.Infof("starting server on :8080")
		lis, err := net.Listen("tcp", ":8080")
		if err != nil {
			logrus.Fatalf("listening: %v", err)
		}
		if err := server.Serve(lis); err != nil && !errors.Is(err, grpc.ErrServerStopped) {
			logrus.Fatalf("serving: %v", err)
		}
	}()

	<-runCtx.Done()
	logrus.Infof("shutting down")
	server.GracefulStop()
}
