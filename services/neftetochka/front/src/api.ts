import ky, { HTTPError } from "ky";
import { useMutation, useQuery, useQueryClient } from "@tanstack/react-query";


export type Credentials = {
  username: string;
  password: string;
};

let uid: string = '';

type AuthResponse = {
  id: string;
}

export function useRegister() {
  const queryClient = useQueryClient();

  return useMutation({
    mutationFn(credentials: Credentials) {
      return ky.post("/api/register", { json: credentials }).json<AuthResponse>();
    },
    onError(error) {
      console.log(`error ${error}`);
      // return handleMutationError(error, "Registration error", [409]);
    },
    onSuccess(data: AuthResponse) {
      uid = data['id'];
      console.log(uid);
      return queryClient.invalidateQueries({ queryKey: ["stations"] });
    },
  });
}

export function useLogin() {
  const queryClient = useQueryClient();

  return useMutation({
    mutationFn(credentials: Credentials) {
      return ky.post("/api/login", { json: credentials }).json<AuthResponse>();
    },
    onError(error) {
      console.log(`error ${error}`);
      // return handleMutationError(error, "Registration error", [409]);
    },
    onSuccess(data: AuthResponse) {
      uid = data['id'];
      return queryClient.invalidateQueries({ queryKey: ["stations"] });
    },
  });
}

export function useLogout() {
  uid = '';
  const queryClient = useQueryClient();
  return queryClient.invalidateQueries({ queryKey: ["stations"] });
}

type UserResponse = {
  username: string;
  balance: number;
  uid: string;
}

export function useUser() {
  return useQuery({queryKey: ["users"], queryFn: async () => {
    let req: StationsRequest = {uid: uid}
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
    let req: StationsRequest = {uid: uid}
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
  uid: string;
  from: number;
  to: number;
}

export function useRoute(from: number, to: number) {
  return useQuery({queryKey: ["routes"], queryFn: async () => {
    let req: RouteRequest = {uid: uid, from: from, to: to}
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
export function useSendOil() {
  // const queryClient = useQueryClient();

  return useMutation({
    mutationFn(req: SendOilRequest) {
      return ky.post("/api/send", { json: req }).json<"">();
    },
    onError(error) {
      console.log(`error ${error}`);
      // return handleMutationError(error, "Registration error", [409]);
      return '123';
    },
    onSuccess() {
      return '';
      // return queryClient.invalidateQueries({ queryKey: ["stations"] });
    },
  });
}