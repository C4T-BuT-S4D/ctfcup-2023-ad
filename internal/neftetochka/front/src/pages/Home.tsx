import { useEffect, useState } from 'react';

import { width, left, top, rotate, delay } from '../consts';
import { Station, StationResponse, SendOilRequest } from '../api';
import * as api from '../api';

import OilRedIcon from '../assets/oil-red.svg';
import OilGreenIcon from '../assets/oil-green.svg';
import OilBlueIcon from '../assets/oil-blue.svg';
import LogoutIcon from '../assets/logout.svg';
import CloseIcon from '../assets/close.svg';

const pipeStyle = 'h-[8px] bg-gradient-to-l from-slate-900 via-sky-950 to-gray-800 from-[30%] via-[40%] to-[60%]'

type NoMoney = {
  receiver_id: string;
  station_id: number;
  oil_id: number;
  money: number;
}

function HomePage(prop: {stations: StationResponse}) {
  const userRes = api.useUser();
  const sendOil= api.useSendOil(alertError);
  const logout = api.useLogout();
  const apiAddMoney = api.useAddMoney(alertError);
  const lastReceived = api.useLastReceived();
  const [error, setError] = useState('');

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
  const [sending, setSending] = useState(false);
  const [passed, setPassed] = useState<number[]>([]);
  const [sendOk, setSendOk] = useState(false);
  const [sendNoMoney, setSendNoMoney] = useState<NoMoney|null>(null);
  const [addMoney, setAddMoney] = useState(10);

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

  if (userRes.status === 'success' && sendNoMoney !== null && addMoney > userRes.data!!.balance) {
    setAddMoney(userRes.data!!.balance);
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
    if (sending) return;
    if (selected1 == id) setSelected1(-1);
    else if (selected2 == id) setSelected2(-1);
    else if (selected1 == -1) setSelected1(id);
    else if (selected2 == -1) setSelected2(id);
  }

  function alertError(error: string) {
    if (ws) {
      ws.close()
      setWs(null);
      setSending(false);
      setPassed([]);
      setSendOk(false);
      setSendNoMoney(null);
      setAddMoney(10);
    }
    setError(error);
    setTimeout(() => {
      setError('');
    }, 5000);
  }

  function leaveNoMoney() {
    ws?.close();
    setWs(null);
    setPassed([]);
    setSending(false);
    setSendNoMoney(null);
    setAddMoney(10);
    setSelected1(-1);
    setSelected2(-1);
  }

  function callApiAddMoney() {
    let req: api.AddMoneyRequest = {
      uid: userRes.data!!.uid, 
      amount: addMoney,
      station_id: sendNoMoney!!.station_id,
      oil_id: sendNoMoney!!.oil_id,
    }
    setSendNoMoney(null);
    setAddMoney(10);
    apiAddMoney.mutate(req);
  }

  function recvMessage(evt: MessageEvent) {
    let j = JSON.parse(evt.data);
    if (j['type'] === 'get') {
      passed.push(j['station_id'])
      setSendOk(true);
      ws?.close()
      setWs(null);
      setTimeout(() => {
        setSendOk(false);
        setSending(false);
        setPassed([]);
        setSelected1(-1);
        setSelected2(-1);
      }, 1500);
      return;
    }
    if (j['type'] === 'no_money') {
      setSendNoMoney(j);
      return;
    }
    if (j['type'] === 'passed') {
      passed.push(j['station_id'])
    }
  }

  useEffect(() => {
    if (ws) {
      ws.onmessage = recvMessage;
    }
  }, [ws])

  function _sendOil() {
    if (!ws) {
      const wsClient = new WebSocket('ws://'+location.host+'/ws/ws');
      wsClient.onopen = () => {
        setSending(true);
        setWs(wsClient);
        wsClient.send(`{"type":"INIT","uid":"${userRes.data!!.uid}"}`)
      }
      wsClient.onclose = () => {};
    }
    let req: SendOilRequest = {
      uid: userRes.data!!.uid, 
      receiver_id: receiverId, 
      money: money,
      from: selected1,
      to: selected2,
      msg: message,
    }
    setMessage('');
    setReceiverId('');
    setMoney(10);
    setTimeout(() => {
      sendOil.mutate(req);
    }, 500);
  }

  return (
    <div className='bg-gradient-to-br from-[#151515] to-[#440857] from-20% w-max h-max p-10 flex gap-10'>
      {error === '' ? <></> : 
      <div className='z-50 rounded-3xl absolute top-10 right-10 bg-[#3d1d52] w-[250px] shadow-[10px_15px_20px_rgba(0,0,0,0.85)]'>
        <div className='bg-[#12031c] py-2 pr-4 pl-6 rounded-t-3xl flex'>
          <div className='text-white font-[670]'>Error</div>
          <div className='flex-1' />
          <img onClick={()=>setError('')} className='cursor-pointer h-[25px]' src={CloseIcon} />
        </div>
        <div className='py-3 px-5 text-white font-[400] text-[14px]'>{error}</div>
      </div>}
      <div className='rounded-3xl bg-gradient-to-br from-[#21062b] to-[#440857] from-20% to-80% w-[420px] p-6 space-y-5'>
        <div className='rounded-xl bg-purple-900/[0.2] p-4 flex'>
          <div className='w-full'>
            <div className='text-white text-[24px] font-[750] flex'>
              <div className='w-[310px] truncate'>{userRes.data?.username}</div>
              <div onClick={() => logout.mutate()} className='cursor-pointer w-[30px] h-[30px] self-center text-white'>
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

        <div className='rounded-xl bg-purple-900/[0.35] p-4 text-white'>
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

          <div className='py-2 divide-dashed divide-blue-200 divide-y-2'><div /><div /></div>

          <div className='font-[600] text-[16px] flex gap-4'>
            <div className=''>{`Money: `}</div>
            <div className='flex gap-3'>
              <div className='w-[18px]'>{money}</div>
              <button disabled={sending} onClick={() => setMoney(Math.max(1, money-1))} className={`${sending ? 'cursor-not-allowed' : 'cursor-pointer'} m-auto`}>
                <svg className="w-3.5 h-3.5 text-white" aria-hidden="true" xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 18 2">
                  <path stroke="currentColor" strokeLinecap="round" strokeLinejoin="round" strokeWidth="2" d="M1 1h16"/>
                </svg>
              </button>
              <button disabled={sending} onClick={() => setMoney(Math.min(userRes.data!!.balance, money+1))} className={`${sending ? 'cursor-not-allowed' : 'cursor-pointer'} m-auto`}>
                <svg className="w-3.5 h-3.5 text-white" aria-hidden="true" xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 18 18">
                  <path stroke="currentColor" strokeLinecap="round" strokeLinejoin="round" strokeWidth="2" d="M9 1v16M1 9h16"/>
                </svg>
              </button>
            </div>
          </div>

          <div className='font-[600] text-[16px] flex gap-2'>
            <div className=''>{`Message: `}<br/>{`Receiver: `}</div>
            <div className='w-full'>
              <div><input disabled={sending} value={message} onChange={(e) => setMessage(e.target.value)} placeholder='<Type here your message>'  className={`${sending ? 'cursor-not-allowed' : 'cursor-default'} w-full outline-none bg-transparent italic placeholder:italic placeholder:text-white`}/></div>
              <div><input disabled={sending} value={receiverId} onChange={(e) => setReceiverId(e.target.value)} placeholder='<Type here user id>' className={`${sending ? 'cursor-not-allowed' : 'cursor-default'} w-full outline-none bg-transparent italic placeholder:italic placeholder:text-white`}/></div>
            </div>
          </div>

          <div className='py-2 divide-dashed divide-blue-200 divide-y-2'><div /><div /></div>

          <div className='flex gap-4 font-[600] text-[16px]'>
            <div className=''>{`Route: `}</div>
            <div className='italic grid grid-cols-3 gap-1'>
              {routePreview.map((_, i) => {
                let edgeIdx = routePreview[routePreview.length - 1 - i];
                return <div key={`routePreview-${i}`} className=''>{directions[routePreview.length - 1 - i] == 1 ? edges[edgeIdx].id1 : edges[edgeIdx].id2}{' -- '}</div>
              })}
              {routePreview.length > 0 ? <div key={`routePreview-${routePreview.length-1}`} className=''>{directions[0] == 0 ? edges[routePreview[0]].id1 : edges[routePreview[0]].id2}</div> : <></>}
            </div>
          </div>

          <div className='py-2 divide-dashed divide-blue-200 divide-y-2'><div /><div /></div>
          
          {routePreview.length > 0 && message.length > 0 && receiverId.length > 0 ?
          <div onClick={_sendOil} className='mt-2 rounded-3xl border-dashed border-pink-500 border-2 p-2 text-center font-[750] cursor-pointer'>
            Send
          </div>
          : <></>}
        
        </div>
          
        {!sending ? <></> :
        <div className={`rounded-xl ${sendOk === true ? 'bg-[#00fc45]/[0.53]' : sendNoMoney != null ? 'bg-red-500/[0.3]' : 'bg-purple-900'} p-4 text-white`}>
          <div className='font-[700] text-[18px] pb-2 truncate underline underline-offset-4 decoration-dashed decoration-[1.2px] decoration-blue-200'>
            {sendOk === true ? 'Sended' : sendNoMoney != null ? 'Error: Not enough money' : 'Sending...'}
          </div>
          {sendNoMoney === null ? <></> : <>
          <div className='flex gap-4 font-[600] text-[16px]'>
            <div className=''>{`At station: `}</div>
            <div className=''>{sendNoMoney.station_id}</div>
          </div>

          <div className='flex gap-4 font-[600] text-[16px]'>
            <div className=''>{`Current balance: `}</div>
            <div className=''>{sendNoMoney.money}</div>
          </div>

          <div className='font-[600] text-[16px] flex gap-4'>
            <div className=''>{`Add money: `}</div>
            <div className='flex gap-3'>
              <div className='w-[18px]'>{addMoney}</div>
              <button onClick={() => setAddMoney(Math.max(1, addMoney-1))} className='m-auto'>
                <svg className="w-3.5 h-3.5 text-white" aria-hidden="true" xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 18 2">
                  <path stroke="currentColor" strokeLinecap="round" strokeLinejoin="round" strokeWidth="2" d="M1 1h16"/>
                </svg>
              </button>
              <button onClick={() => setAddMoney(Math.min(userRes.data!!.balance, addMoney+1))} className='m-auto'>
                <svg className="w-3.5 h-3.5 text-white" aria-hidden="true" xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 18 18">
                  <path stroke="currentColor" strokeLinecap="round" strokeLinejoin="round" strokeWidth="2" d="M9 1v16M1 9h16"/>
                </svg>
              </button>
            </div>
          </div>

          <div className='mt-3 flex gap-3'>
            <button onClick={() => callApiAddMoney()} className='flex-1 rounded-3xl bg-white p-2 text-[#6a193f] font-[700] '>Add money</button>
            <button onClick={() => leaveNoMoney()}    className='flex-1 rounded-3xl border-dashed border-white border-[3px] p-2 font-[700]'>Leave</button>
          </div></>}

        </div>}
        
        <div className='rounded-xl bg-purple-900/[0.45] p-4 text-white'>
          <div className='font-[700] text-[18px] pb-2 truncate underline underline-offset-4 decoration-dashed decoration-[1.2px] decoration-blue-200'>
            Last received
          </div>
          {lastReceived.data?.map((oil, i) => <>
            <div className='font-[600] text-[16px] gap-4'>
              <div className='flex'>
                <div className='w-[80px]'>{`From: `}</div>
                <div className='flex-1 italic truncate'>{oil.sender}</div>
              </div>
              <div className='flex'>
                <div className='w-[80px]'>{`At: `}</div>
                <div className='italic'>{oil.station_id}</div>
              </div>
              <div className='flex'>
                <div className='w-[80px]'>{`Message: `}</div>
                <div className='flex-1 italic truncate'>{oil.message}</div>
              </div>
            </div>
            {i == lastReceived.data.length - 1 ? <></> :
            <div className='py-2 divide-dashed divide-blue-200 divide-y-2'><div /><div /></div>
          }
          </>)}
        </div>

      </div>
      <div className='relative'>
        <div className='w-max bg-transparent grid grid-cols-[repeat(20,_minmax(0,_1fr))] '>
          {Array.from({length: 20*20}, (_, i) => i).map(i => (
            <div key={`grid-${i}`} className='w-[40px] h-[40px] border-[1px] border-[#e0e0e0]/[.25] rounded'></div>
          ))}
        </div>
        <div className=''>
          {stations_ids.map((id) => (
            <div onClick={() => handleSelected(id)} key={`div1${id}`} className={`${left[stations[id].x+1000]} ${top[stations[id].y]} ${sending ? 'cursor-not-allowed' : 'cursor-pointer'} group z-20 w-[40px] h-[40px] rounded-[100%] translate-x-[-50%] translate-y-[-50%] absolute flex`}>
              <div key={`div2${id}`} className={`${selected1 == id || selected2 == id ? 'shadow-[0px_0px_30px_15px] shadow-white' : sending ? '' : 'group-hover:shadow-[0px_0px_30px_15px] group-hover:shadow-white'} w-[15px] h-[20px] m-auto`}/>
              <img key={`img${id}`} src={passed.indexOf(id) != -1 ? OilGreenIcon : (sendNoMoney !== null && sendNoMoney.station_id == id) ? OilRedIcon : OilBlueIcon} className={`w-[40px] h-[40px] z-10 absolute`}/>
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
                <div key={`left${i}`}  className={`${delay[routePreview.length-1-idx]} ${width[l]} ${left[dx+1000]} ${top[dy]} ${rotate[deg+180]} ${directions[idx] == 1 ? 'z-10 to-slate-700 from-white/[0.8] h-[6px]' : 'h-[5px] z-0 to-slate-700/[0] from-white/[0]'} bg-slay-700 duration-300 bg-gradient-to-l bg-right from-[50%] to-[50%] bg-[length:200%] shadow-white shadow-[0px_0px_8px_1px] absolute`} />
                <div key={`right${i}`} className={`${delay[routePreview.length-1-idx]} ${width[l]} ${left[dx+1000]} ${top[dy]} ${rotate[deg+360]} ${directions[idx] == 0 ? 'z-10 to-slate-700 from-white/[0.8] h-[6px]' : 'h-[5px] z-0 to-slate-700/[0] from-white/[0]'} bg-slay-700 duration-300 bg-gradient-to-l bg-right from-[50%] to-[50%] bg-[length:200%] shadow-white shadow-[0px_0px_8px_1px] absolute`} />
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