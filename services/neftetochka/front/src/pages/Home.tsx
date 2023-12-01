import { useState } from 'react';

import { width, left, top, rotate, delay } from '../consts';
import { Station, Edge, StationResponse, SendOilRequest } from '../api';
import * as api from '../api';

import A1 from '../assets/1.svg';
import LogoutIcon from '../assets/logout.svg';
import StationIcon from '../assets/factory.svg';

const pipeStyle = 'h-[8px] bg-gradient-to-l from-slate-900 via-sky-950 to-gray-800 from-[30%] via-[40%] to-[60%]'

function HomePage(prop: {stations: StationResponse}) {
  const userRes = api.useUser();
  const sendOil= api.useSendOil();
  const [selected1, setSelected1] = useState(-1);
  const [selected2, setSelected2] = useState(-1);
  const [routePreview, setRoutePreview] = useState<number[]>([]);
  const [directions, setDirections] = useState<number[]>([]);
  const [notFound, setNotFound] = useState(false);
  const [prevData, setPrevData] = useState<number[]>([]);
  const [money, setMoney] = useState(10);
  const [message, setMessage] = useState('');
  const [receiverId, setReceiverId] = useState('');
  const [ws, setWs] = useState<WebSocket|null>(null);

  let stations_list = prop.stations.stations;
  let stations_ids = stations_list.map((s) => s.id);
  let stations: Record<number, Station> = {};
  for (let i = 0; i < stations_list.length; ++i) {
    stations[stations_list[i].id] = stations_list[i];
  }
  let edges = prop.stations.edges;

  const routeRes = api.useRoute(selected1, selected2);

  if (userRes.status === 'success' && money > userRes.data!!.balance) {
    setMoney(userRes.data!!.balance);
  }

  if (selected1 != -1 && selected2 != -1 && routeRes.status == 'success') {
    if (routeRes.data.length === 0 && !notFound) {
      setNotFound(true);
      setPrevData(routeRes.data);
      return;
    }
    if ((routeRes.data.length > 0 && routePreview.length === 0) || routeRes.data != prevData) {
      let newRoutePreview = [];
      let newDirections = [];
      for (let i = 0; i < routeRes.data.length - 1; ++i) {
        for (let idx = 0; idx < edges.length; ++idx) {
          if (edges[idx].id1 === routeRes.data[i] && edges[idx].id2 === routeRes.data[i+1]) {
            newRoutePreview.push(idx)
            newDirections.push(0)
            break
          }
          if (edges[idx].id2 === routeRes.data[i] && edges[idx].id1 === routeRes.data[i+1]) {
            newRoutePreview.push(idx)
            newDirections.push(1)
            break
          }
        }
      }
      setRoutePreview(newRoutePreview);
      setDirections(newDirections);
      setNotFound(false);
      setPrevData(routeRes.data);
    }
  }
  if (notFound && (selected1 == -1 || selected2 == -1)) {
    setNotFound(false);
    return <></>
  }
  if (((selected1 == -1 || selected2 == -1 || notFound) && routePreview.length !== 0)) {
    setDirections([]);
    setRoutePreview([]);
    return <></>
  }
  
  function handleSelected(id: number) {
    if (selected1 == id) setSelected1(-1);
    else if (selected2 == id) setSelected2(-1);
    else if (selected1 == -1) setSelected1(id);
    else if (selected2 == -1) setSelected2(id);
  }

  function _sendOil() {
    const wsClient = new WebSocket('ws://127.0.0.1:8000/ws');
    wsClient.onopen = () => {
      console.log('wsonopen');
      setWs(wsClient);
      wsClient.send(`{"type":"INIT","uid":"${userRes.data!!.uid}"}`)
    }
    wsClient.onmessage = (evt) => console.log(`onmessage ${evt.data}`)
    wsClient.onclose = () => console.log(`wsclose`);
    let req: SendOilRequest = {
      uid: userRes.data!!.uid, 
      receiver_id: receiverId, 
      money: money,
      from: selected1,
      to: selected2,
      msg: message,
    }
    let res = sendOil.mutate(req);
    console.log(res);
  }

  return (
    <div className='bg-gradient-to-br from-[#151515] to-[#440857] from-20% w-max h-max p-10 flex gap-10'>
      <div className='rounded-3xl bg-pink-500 w-[420px] p-6 space-y-5'>
        <div className='rounded-xl bg-purple-900 p-4 flex'>
          <div className='w-full'>
            <div className='text-white text-[24px] font-[750] flex'>
              {userRes.data?.username}
              <div className='flex-1' />
              <div onClick={() => api.useLogout()} className='cursor-pointer w-[30px] h-[30px] self-center text-white'>
                <img src={LogoutIcon} className='filter-white' />
              </div>
            </div>
            <div className='text-white text-[15px] font-[350]'>
              balance: {userRes.data?.balance}
            </div>
            <div className='text-white text-[15px] font-[350]'>
              uid: {userRes.data?.uid}
            </div>
          </div>
        </div>

        <div className='rounded-xl bg-purple-900 p-4 text-white'>
          <div className='font-[700] text-[18px] pb-2 truncate underline underline-offset-4 decoration-dashed decoration-[1.2px] decoration-blue-200'>Send Oil</div>
          <div className='font-[600] text-[16px] flex gap-4'>
            <div className=''>
            {`From: `}<br/>{`To: `}
            </div>
            <div className='flex-1'>
              <div className='italic inline-block'>{selected1 == -1 ? <div>{`<Select station on map>`}</div> : selected1}</div>
              <br/>
              <div className='italic inline-block'>{selected2 == -1 ? <div>{`<Select station on map>`}</div> : selected2}</div>
              <br/>
            </div>
          </div>

          <div className='py-2 divide-dashed divide-blue-200 divide-y-2'>
            <div /><div />
          </div>

          <div className='font-[600] text-[16px] flex gap-4'>
            <div className=''>{`Money: `}</div>
            <div className='flex gap-3'>
              <div className='w-[18px]'>{money}</div>
              <button onClick={() => setMoney(Math.max(0, money-1))} className='m-auto'>
                <svg className="w-3.5 h-3.5 text-white" aria-hidden="true" xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 18 2">
                  <path stroke="currentColor" strokeLinecap="round" strokeLinejoin="round" strokeWidth="2" d="M1 1h16"/>
                </svg>
              </button>
              <button onClick={() => setMoney(Math.min(userRes.data!!.balance, money+1))} className='m-auto'>
                <svg className="w-3.5 h-3.5 text-white" aria-hidden="true" xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 18 18">
                  <path stroke="currentColor" strokeLinecap="round" strokeLinejoin="round" strokeWidth="2" d="M9 1v16M1 9h16"/>
                </svg>
              </button>
            </div>
          </div>

          <div className='font-[600] text-[16px] flex gap-2'>
            <div className=''>{`Message: `}<br/>{`Receiver: `}</div>
            <div className='w-full'>
              <div><input value={message} onChange={(e) => setMessage(e.target.value)} placeholder='<Type here your message>' className='w-full outline-none bg-transparent italic placeholder:italic placeholder:text-white'/></div>
              <div><input value={receiverId} onChange={(e) => setReceiverId(e.target.value)} placeholder='<Type here user id>' className='w-full outline-none bg-transparent italic placeholder:italic placeholder:text-white'/></div>
            </div>
          </div>

          <div className='py-2 divide-dashed divide-blue-200 divide-y-2'>
            <div /><div />
          </div>

          <div className='flex gap-4 font-[600] text-[16px]'>
            <div className=''>{`Route: `}</div>
            <div className='italic grid grid-cols-3 gap-1'>
              {routePreview.map((_, i) => {
                let edgeIdx = routePreview[routePreview.length - 1 - i];
                console.log(directions[routePreview.length - 1 - i], edges[edgeIdx])
                return <div key={i} className=''>{directions[routePreview.length - 1 - i] == 1 ? edges[edgeIdx].id1 : edges[edgeIdx].id2}{' -- '}</div>
              })}
              {routePreview.length > 0 ? <div key={routePreview.length-1} className=''>{directions[0] == 0 ? edges[routePreview[0]].id1 : edges[routePreview[0]].id2}</div> : <></>}
            </div>
          </div>

          <div className='py-2 divide-dashed divide-blue-200 divide-y-2'>
            <div /><div />
          </div>
          
          {routePreview.length > 0 && message.length > 0 && receiverId.length > 0 ?
          <div onClick={_sendOil} className='mt-2 rounded-3xl border-dashed border-pink-500 border-2 p-2 text-center font-[750] cursor-pointer'>
            Send
          </div>
          : <></>}
        
        </div>
      </div>
      <div className='relative'>
        <div className='w-max bg-transparent grid grid-cols-[repeat(20,_minmax(0,_1fr))] '>
          {Array.from({length: 20*20}, (_, i) => i).map(i => (
            <div key={i} className='w-[40px] h-[40px] border-[1px] border-[#e0e0e0]/[.25] rounded'></div>
          ))}
        </div>
        <div className=''>
          {stations_ids.map((id) => (
            <div onClick={() => handleSelected(id)} key={`div1${id}`} className={`${left[stations[id].x+1000]} ${top[stations[id].y]} group z-20 w-[40px] h-[40px] rounded-[100%] cursor-pointer translate-x-[-50%] translate-y-[-50%] absolute flex`}>
              <div key={`div2${id}`} className={`${selected1 == id || selected2 == id ? 'shadow-[0px_0px_30px_15px] shadow-white' : 'group-hover:shadow-[0px_0px_30px_15px] group-hover:shadow-white'} w-[15px] h-[20px] m-auto`}/>
              <img key={`img${id}`} src={A1} className={`w-[40px] h-[40px] z-10 absolute`}/>
            </div>
          ))}
        </div>
        <div className=''>
          {edges.map((e, i) => {
            let s1: Station = stations[e.id1];
            let s2: Station = stations[e.id2];
            let deg = Math.ceil(Math.atan2((s1.y-s2.y),(s1.x-s2.x))*(180/Math.PI));
            let l = Math.ceil(Math.sqrt((s1.x-s2.x)*(s1.x-s2.x)+(s1.y-s2.y)*(s1.y-s2.y)));
            let dy: number = Math.ceil(Math.abs(s1.y+s2.y)/2);
            let dx: number = Math.ceil(Math.abs(s1.x+s2.x)/2-l/2);
            let idx = routePreview.indexOf(i)
            if (idx != -1) {
                return (
                <>
                <div key={`left${i}`}  className={`${delay[routePreview.length-1-idx]} ${width[l]} ${left[dx+1000]} ${top[dy]} ${rotate[deg+180]} ${directions[idx] == 1 ? 'z-10 to-slate-700 from-yellow-300/[0.8]' : 'z-0 to-slate-700/[0] from-yellow-300/[0]'} bg-slay-700 h-[5px] duration-300 bg-gradient-to-l bg-right from-[50%] to-[50%] bg-[length:200%] shadow-yellow-300 shadow-[0px_0px_8px_1px] absolute`} />
                <div key={`right${i}`} className={`${delay[routePreview.length-1-idx]} ${width[l]} ${left[dx+1000]} ${top[dy]} ${rotate[deg+360]} ${directions[idx] == 0 ? 'z-10 to-slate-700 from-yellow-300/[0.8]' : 'z-0 to-slate-700/[0] from-yellow-300/[0]'} bg-slay-700 h-[5px] duration-300 bg-gradient-to-l bg-right from-[50%] to-[50%] bg-[length:200%] shadow-yellow-300 shadow-[0px_0px_8px_1px] absolute`} />
                </>
                );
            }
            return (
            <>
            <div key={`left${i}`}  className={`${width[l]} ${left[dx+1000]} ${top[dy]} ${rotate[deg+180]} ${pipeStyle} absolute`} />
            <div key={`right${i}`} className={`${width[l]} ${left[dx+1000]} ${top[dy]} ${rotate[deg+360]} ${pipeStyle} absolute`} />
            </>
            );
          })}
        </div>
      </div>
    </div>
  );
}

export default HomePage;