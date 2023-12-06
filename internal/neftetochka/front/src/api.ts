import ky, { HTTPError } from "ky";
import { useMutation, useQuery, useQueryClient } from "@tanstack/react-query";

export type Credentials = {
  username: string;
  password: string;
};

type AuthResponse = {
  id: string;
}

type ErrorResponse = {
  error: string;
}

export function useRegister(errorCallback: Function) {
  const queryClient = useQueryClient();

  return useMutation({
    mutationFn(credentials: Credentials) {
      return ky.post("/api/register", { json: credentials }).json<AuthResponse>();
    },
    async onError(error: HTTPError) {
      let j: ErrorResponse = await error.response.json();
      errorCallback(`${j.error}`);
      return;
    },
    onSuccess(data: AuthResponse) {
      localStorage.setItem('uid', data['id']);
      return queryClient.invalidateQueries({ queryKey: ["stations"] });
    },
  });
}

export function useLogin(errorCallback: Function) {
  const queryClient = useQueryClient();

  return useMutation({
    mutationFn(credentials: Credentials) {
      return ky.post("/api/login", { json: credentials }).json<AuthResponse>();
    },
    async onError(error: HTTPError) {
      let j: ErrorResponse = await error.response.json();
      errorCallback(`${j.error}`);
    },
    onSuccess(data: AuthResponse) {
      localStorage.setItem('uid', data['id']);
      return queryClient.invalidateQueries({ queryKey: ["stations"] });
    },
  });
}

export function useLogout() {
  const queryClient = useQueryClient();

  return useMutation({
    mutationFn() {
      localStorage.setItem('uid', '');
      return queryClient.invalidateQueries({ queryKey: ["stations"] });
    }
  });
}

type UserResponse = {
  username: string;
  balance: number;
  uid: string;
}

function getUid() {
  let uid = localStorage.getItem('uid');
  if (uid === null) return '';
  return uid;
}


export function useUser() {
  return useQuery({queryKey: ["users"], queryFn: async () => {
    let req: StationsRequest = {uid: getUid()}
    let res = await ky.post("/api/user", { json: req }).json<UserResponse>();
    return res;
  }});
}

type StationsRequest = {
  uid: string;
}

export type Station = {
  id: number;
  x: number;
  y: number;
}

export type Edge = {
  id1: number;
  id2: number;
}

export type StationResponse = {
  stations: Station[],
  edges: Edge[],
}

const CELL_SIZE: number = 40;

export function useStations() {
  return useQuery({queryKey: ["stations"], queryFn: async () => {
    let req: StationsRequest = {uid: getUid()}
    let stations = await ky.post("/api/stations", { json: req }).json<Station[]>();
    let edges = await ky.post("/api/links", { json: req }).json<Edge[]>();

    for (let i = 0; i < stations.length; ++i) {
      stations[i].x *= CELL_SIZE;
      stations[i].y *= CELL_SIZE;
    }
  
    let res: StationResponse = {stations, edges}
    return res;
  }});
}

type RouteRequest = {
  uid: string | null;
  from: number;
  to: number;
}

export function useRoute(from: number, to: number) {
  return useQuery({queryKey: ["routes"], queryFn: async () => {
    let req: RouteRequest = {uid: getUid(), from: from, to: to}
    let route = await ky.post("/api/route", { json: req }).json<number[]>();
    return route;
  }, enabled: from !== -1 && to !== -1});
}

export type SendOilRequest = {
  uid: string;
  receiver_id: string;
  money: number;
  from: number;
  to: number;
  msg: string;

}
export function useSendOil(errorCallback: Function) {
  const queryClient = useQueryClient();

  return useMutation({
    mutationFn(req: SendOilRequest) {
      return ky.post("/api/send", { json: req }).json<"">();
    },
    async onError(error: HTTPError) {
      let j: ErrorResponse = await error.response.json();
      errorCallback(`${j.error}`);
      return;
    },
    onSuccess() {
      return queryClient.invalidateQueries({ queryKey: ["users"] });
    },
  });
}

export type AddMoneyRequest = {
  uid: string;
  amount: number;
  station_id: number;
  oil_id: number;
}

export function useAddMoney(errorCallback: Function) {
  const queryClient = useQueryClient();

  return useMutation({
    mutationFn(req: AddMoneyRequest) {
      return ky.post("/api/add_money", { json: req }).json<"">();
    },
    async onError(error: HTTPError) {
      let j: ErrorResponse = await error.response.json();
      errorCallback(`${j.error}`);
      return;
    },
    onSuccess() {
      return queryClient.invalidateQueries({ queryKey: ["users"] });
    },
  });
}

type ReceivedResponse = {
  sender: string;
  message: string;
  station_id: number;
}

export function useLastReceived() {
  return useQuery({queryKey: ["received"], queryFn: async () => {
    let req: StationsRequest = {uid: getUid()}
    let received = await ky.post("/api/last_received", { json: req }).json<ReceivedResponse[]>();
    return received;
  }});
}