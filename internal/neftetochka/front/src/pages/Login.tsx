import { useState } from 'react'

import CloseIcon from '../assets/close.svg';
import * as api from '../api';

function LoginPage() {
  const [username, setUsername] = useState('');
  const [password, setPassword] = useState('');
  const [error, setError] = useState('');

  const register = api.useRegister(alertError);
  const login = api.useLogin(alertError);

  function alertError(error: string) {
    setError(error);
    setTimeout(() => {
      setError('');
    }, 5000);
  }

  return (
    <>
      <div className='bg-gradient-to-br from-[#151515] to-[#440857] from-20% items-center flex h-screen'>
        {error === '' ? <></> : 
        <div className='rounded-3xl absolute top-10 right-10 bg-[#3d1d52] w-[250px] shadow-[10px_15px_20px_rgba(0,0,0,0.85)]'>
          <div className='bg-[#12031c] py-2 pr-4 pl-6 rounded-t-3xl flex'>
            <div className='text-white font-[670]'>Error</div>
            <div className='flex-1' />
            <img onClick={()=>setError('')} className='cursor-pointer h-[25px]' src={CloseIcon} />
          </div>
          <div className='py-3 px-5 text-white font-[400] text-[14px]'>{error}</div>
        </div>}
        <div className='p-5 m-auto space-y-2 bg-gradient-to-br from-[#1c0126] via-[#440857] to-[#290836] from-10% to-80% rounded-[35px] drop-shadow-[10px_20px_25px_rgba(0,0,0,0.85)]'>
          <div className='text-white font-[710] text-[24px] text-center pb-2'>Neftetochka</div>
          <div className='m-auto caret-white text-white w-[343px] h-14 px-6 py-2 bg-zinc-900/[0.4] rounded-2xl justify-start items-center gap-3 flex'>
            <input value={username} onChange={(e) => setUsername(e.target.value)} className='flex-auto bg-transparent outline-none rounded-lg text-[16px] placeholder:text-gray-400' placeholder='Username'></input>
          </div>
          <div className='m-auto caret-white text-white w-[343px] h-14 px-6 py-2 bg-zinc-900/[0.4] rounded-2xl justify-start items-center gap-3 flex'>
            <input value={password} onChange={(e) => setPassword(e.target.value)} type='password' className='flex-auto bg-transparent outline-none rounded-lg text-[16px] placeholder:text-gray-400' placeholder='Password'></input>
          </div>
          <div className='flex gap-2 pt-3'>
            <button onClick={() => login.mutate({username, password})} className='flex-1 rounded-3xl text-white bg-[#23072e] py-2.5 font-bold'>
              Login
            </button>
            <button onClick={() => register.mutate({username, password})} className='flex-1 rounded-3xl text-white py-2.5 bg-black/[0.5] font-bold'>
              Register
            </button>
          </div>
        </div>
      </div>
    </>
  );
}

export default LoginPage;
