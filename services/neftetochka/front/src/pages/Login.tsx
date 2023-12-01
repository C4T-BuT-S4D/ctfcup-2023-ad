import { useState } from 'react'

import * as api from '../api';

function LoginPage() {
  const [username, setUsername] = useState('');
  const [password, setPassword] = useState('');

  const register = api.useRegister();
  const login = api.useLogin();

  return (
    <>
      <div className='bg-gradient-to-br from-[#151515] to-[#440857] from-20% items-center flex h-screen'>
        <div className='p-5 m-auto space-y-2 bg-fuchsia-950 rounded-[35px] drop-shadow-[10px_20px_25px_rgba(0,0,0,0.85)]'>
          <div className='text-white font-[710] text-[24px] text-center pb-2'>Neftetochka</div>
          <div className='m-auto caret-white text-white w-[343px] h-14 px-6 py-2 bg-zinc-900 rounded-2xl justify-start items-center gap-3 flex'>
            <input value={username} onChange={(e) => setUsername(e.target.value)} className='flex-auto bg-transparent outline-none rounded-lg text-[16px] placeholder:text-gray-400' placeholder='Username'></input>
          </div>
          <div className='m-auto caret-white text-white w-[343px] h-14 px-6 py-2 bg-zinc-900 rounded-2xl justify-start items-center gap-3 flex'>
            <input value={password} onChange={(e) => setPassword(e.target.value)} type='password' className='flex-auto bg-transparent outline-none rounded-lg text-[16px] placeholder:text-gray-400' placeholder='Password'></input>
          </div>
          <div className='flex gap-2 pt-3'>
            <button onClick={() => login.mutate({username, password})} className='flex-1 rounded-3xl bg-[#c79cff] py-2.5 font-bold'>
              Login
            </button>
            <button onClick={() => register.mutate({username, password})} className='flex-1 rounded-3xl text-white py-2.5 bg-black font-bold'>
              Register
            </button>
          </div>
        </div>
      </div>
    </>
  );
}

export default LoginPage;
