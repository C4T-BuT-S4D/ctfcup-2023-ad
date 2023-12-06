# Generated by the gRPC Python protocol compiler plugin. DO NOT EDIT!
"""Client and server classes corresponding to protobuf-defined services."""
import grpc

from proto.bluwal import bluwal_pb2 as bluwal_dot_bluwal__pb2


class BluwalServiceStub(object):
    """Missing associated documentation comment in .proto file."""

    def __init__(self, channel):
        """Constructor.

        Args:
            channel: A grpc.Channel.
        """
        self.ContestCreate = channel.unary_unary(
                '/bluwal.BluwalService/ContestCreate',
                request_serializer=bluwal_dot_bluwal__pb2.ContestCreateRequest.SerializeToString,
                response_deserializer=bluwal_dot_bluwal__pb2.ContestCreateResponse.FromString,
                )
        self.ContestGet = channel.unary_unary(
                '/bluwal.BluwalService/ContestGet',
                request_serializer=bluwal_dot_bluwal__pb2.ContestGetRequest.SerializeToString,
                response_deserializer=bluwal_dot_bluwal__pb2.ContestGetResponse.FromString,
                )
        self.ContestList = channel.unary_unary(
                '/bluwal.BluwalService/ContestList',
                request_serializer=bluwal_dot_bluwal__pb2.ContestListRequest.SerializeToString,
                response_deserializer=bluwal_dot_bluwal__pb2.ContestListResponse.FromString,
                )
        self.ContestEnroll = channel.unary_unary(
                '/bluwal.BluwalService/ContestEnroll',
                request_serializer=bluwal_dot_bluwal__pb2.ContestEnrollRequest.SerializeToString,
                response_deserializer=bluwal_dot_bluwal__pb2.ContestEnrollResponse.FromString,
                )
        self.ChallengeSubmit = channel.unary_unary(
                '/bluwal.BluwalService/ChallengeSubmit',
                request_serializer=bluwal_dot_bluwal__pb2.ChallengeSubmitRequest.SerializeToString,
                response_deserializer=bluwal_dot_bluwal__pb2.ChallengeSubmitResponse.FromString,
                )
        self.CheckGoal = channel.unary_unary(
                '/bluwal.BluwalService/CheckGoal',
                request_serializer=bluwal_dot_bluwal__pb2.CheckGoalRequest.SerializeToString,
                response_deserializer=bluwal_dot_bluwal__pb2.CheckGoalResponse.FromString,
                )
        self.ClaimReward = channel.unary_unary(
                '/bluwal.BluwalService/ClaimReward',
                request_serializer=bluwal_dot_bluwal__pb2.ClaimRewardRequest.SerializeToString,
                response_deserializer=bluwal_dot_bluwal__pb2.ClaimRewardResponse.FromString,
                )


class BluwalServiceServicer(object):
    """Missing associated documentation comment in .proto file."""

    def ContestCreate(self, request, context):
        """Missing associated documentation comment in .proto file."""
        context.set_code(grpc.StatusCode.UNIMPLEMENTED)
        context.set_details('Method not implemented!')
        raise NotImplementedError('Method not implemented!')

    def ContestGet(self, request, context):
        """Missing associated documentation comment in .proto file."""
        context.set_code(grpc.StatusCode.UNIMPLEMENTED)
        context.set_details('Method not implemented!')
        raise NotImplementedError('Method not implemented!')

    def ContestList(self, request, context):
        """Missing associated documentation comment in .proto file."""
        context.set_code(grpc.StatusCode.UNIMPLEMENTED)
        context.set_details('Method not implemented!')
        raise NotImplementedError('Method not implemented!')

    def ContestEnroll(self, request, context):
        """Missing associated documentation comment in .proto file."""
        context.set_code(grpc.StatusCode.UNIMPLEMENTED)
        context.set_details('Method not implemented!')
        raise NotImplementedError('Method not implemented!')

    def ChallengeSubmit(self, request, context):
        """Missing associated documentation comment in .proto file."""
        context.set_code(grpc.StatusCode.UNIMPLEMENTED)
        context.set_details('Method not implemented!')
        raise NotImplementedError('Method not implemented!')

    def CheckGoal(self, request, context):
        """Missing associated documentation comment in .proto file."""
        context.set_code(grpc.StatusCode.UNIMPLEMENTED)
        context.set_details('Method not implemented!')
        raise NotImplementedError('Method not implemented!')

    def ClaimReward(self, request, context):
        """Missing associated documentation comment in .proto file."""
        context.set_code(grpc.StatusCode.UNIMPLEMENTED)
        context.set_details('Method not implemented!')
        raise NotImplementedError('Method not implemented!')


def add_BluwalServiceServicer_to_server(servicer, server):
    rpc_method_handlers = {
            'ContestCreate': grpc.unary_unary_rpc_method_handler(
                    servicer.ContestCreate,
                    request_deserializer=bluwal_dot_bluwal__pb2.ContestCreateRequest.FromString,
                    response_serializer=bluwal_dot_bluwal__pb2.ContestCreateResponse.SerializeToString,
            ),
            'ContestGet': grpc.unary_unary_rpc_method_handler(
                    servicer.ContestGet,
                    request_deserializer=bluwal_dot_bluwal__pb2.ContestGetRequest.FromString,
                    response_serializer=bluwal_dot_bluwal__pb2.ContestGetResponse.SerializeToString,
            ),
            'ContestList': grpc.unary_unary_rpc_method_handler(
                    servicer.ContestList,
                    request_deserializer=bluwal_dot_bluwal__pb2.ContestListRequest.FromString,
                    response_serializer=bluwal_dot_bluwal__pb2.ContestListResponse.SerializeToString,
            ),
            'ContestEnroll': grpc.unary_unary_rpc_method_handler(
                    servicer.ContestEnroll,
                    request_deserializer=bluwal_dot_bluwal__pb2.ContestEnrollRequest.FromString,
                    response_serializer=bluwal_dot_bluwal__pb2.ContestEnrollResponse.SerializeToString,
            ),
            'ChallengeSubmit': grpc.unary_unary_rpc_method_handler(
                    servicer.ChallengeSubmit,
                    request_deserializer=bluwal_dot_bluwal__pb2.ChallengeSubmitRequest.FromString,
                    response_serializer=bluwal_dot_bluwal__pb2.ChallengeSubmitResponse.SerializeToString,
            ),
            'CheckGoal': grpc.unary_unary_rpc_method_handler(
                    servicer.CheckGoal,
                    request_deserializer=bluwal_dot_bluwal__pb2.CheckGoalRequest.FromString,
                    response_serializer=bluwal_dot_bluwal__pb2.CheckGoalResponse.SerializeToString,
            ),
            'ClaimReward': grpc.unary_unary_rpc_method_handler(
                    servicer.ClaimReward,
                    request_deserializer=bluwal_dot_bluwal__pb2.ClaimRewardRequest.FromString,
                    response_serializer=bluwal_dot_bluwal__pb2.ClaimRewardResponse.SerializeToString,
            ),
    }
    generic_handler = grpc.method_handlers_generic_handler(
            'bluwal.BluwalService', rpc_method_handlers)
    server.add_generic_rpc_handlers((generic_handler,))


 # This class is part of an EXPERIMENTAL API.
class BluwalService(object):
    """Missing associated documentation comment in .proto file."""

    @staticmethod
    def ContestCreate(request,
            target,
            options=(),
            channel_credentials=None,
            call_credentials=None,
            insecure=False,
            compression=None,
            wait_for_ready=None,
            timeout=None,
            metadata=None):
        return grpc.experimental.unary_unary(request, target, '/bluwal.BluwalService/ContestCreate',
            bluwal_dot_bluwal__pb2.ContestCreateRequest.SerializeToString,
            bluwal_dot_bluwal__pb2.ContestCreateResponse.FromString,
            options, channel_credentials,
            insecure, call_credentials, compression, wait_for_ready, timeout, metadata)

    @staticmethod
    def ContestGet(request,
            target,
            options=(),
            channel_credentials=None,
            call_credentials=None,
            insecure=False,
            compression=None,
            wait_for_ready=None,
            timeout=None,
            metadata=None):
        return grpc.experimental.unary_unary(request, target, '/bluwal.BluwalService/ContestGet',
            bluwal_dot_bluwal__pb2.ContestGetRequest.SerializeToString,
            bluwal_dot_bluwal__pb2.ContestGetResponse.FromString,
            options, channel_credentials,
            insecure, call_credentials, compression, wait_for_ready, timeout, metadata)

    @staticmethod
    def ContestList(request,
            target,
            options=(),
            channel_credentials=None,
            call_credentials=None,
            insecure=False,
            compression=None,
            wait_for_ready=None,
            timeout=None,
            metadata=None):
        return grpc.experimental.unary_unary(request, target, '/bluwal.BluwalService/ContestList',
            bluwal_dot_bluwal__pb2.ContestListRequest.SerializeToString,
            bluwal_dot_bluwal__pb2.ContestListResponse.FromString,
            options, channel_credentials,
            insecure, call_credentials, compression, wait_for_ready, timeout, metadata)

    @staticmethod
    def ContestEnroll(request,
            target,
            options=(),
            channel_credentials=None,
            call_credentials=None,
            insecure=False,
            compression=None,
            wait_for_ready=None,
            timeout=None,
            metadata=None):
        return grpc.experimental.unary_unary(request, target, '/bluwal.BluwalService/ContestEnroll',
            bluwal_dot_bluwal__pb2.ContestEnrollRequest.SerializeToString,
            bluwal_dot_bluwal__pb2.ContestEnrollResponse.FromString,
            options, channel_credentials,
            insecure, call_credentials, compression, wait_for_ready, timeout, metadata)

    @staticmethod
    def ChallengeSubmit(request,
            target,
            options=(),
            channel_credentials=None,
            call_credentials=None,
            insecure=False,
            compression=None,
            wait_for_ready=None,
            timeout=None,
            metadata=None):
        return grpc.experimental.unary_unary(request, target, '/bluwal.BluwalService/ChallengeSubmit',
            bluwal_dot_bluwal__pb2.ChallengeSubmitRequest.SerializeToString,
            bluwal_dot_bluwal__pb2.ChallengeSubmitResponse.FromString,
            options, channel_credentials,
            insecure, call_credentials, compression, wait_for_ready, timeout, metadata)

    @staticmethod
    def CheckGoal(request,
            target,
            options=(),
            channel_credentials=None,
            call_credentials=None,
            insecure=False,
            compression=None,
            wait_for_ready=None,
            timeout=None,
            metadata=None):
        return grpc.experimental.unary_unary(request, target, '/bluwal.BluwalService/CheckGoal',
            bluwal_dot_bluwal__pb2.CheckGoalRequest.SerializeToString,
            bluwal_dot_bluwal__pb2.CheckGoalResponse.FromString,
            options, channel_credentials,
            insecure, call_credentials, compression, wait_for_ready, timeout, metadata)

    @staticmethod
    def ClaimReward(request,
            target,
            options=(),
            channel_credentials=None,
            call_credentials=None,
            insecure=False,
            compression=None,
            wait_for_ready=None,
            timeout=None,
            metadata=None):
        return grpc.experimental.unary_unary(request, target, '/bluwal.BluwalService/ClaimReward',
            bluwal_dot_bluwal__pb2.ClaimRewardRequest.SerializeToString,
            bluwal_dot_bluwal__pb2.ClaimRewardResponse.FromString,
            options, channel_credentials,
            insecure, call_credentials, compression, wait_for_ready, timeout, metadata)
