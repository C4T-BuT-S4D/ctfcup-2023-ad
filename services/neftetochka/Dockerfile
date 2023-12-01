FROM ubuntu:22.04
ARG DEBIAN_FRONTEND=noninteractive

RUN apt update && apt install -y g++ make libasio-dev git python3-pip cmake
RUN python3 -m pip install conan==1.62

# WORKDIR /build
# RUN wget https://github.com/jtv/libpqxx/archive/refs/tags/7.8.1.zip && unzip 7.8.1.zip
# RUN cd ./libpqxx-7.8.1 && ./configure --disable-shared && make && make install

RUN apt install -y file gdb wget tmux curl net-tools && bash -c "$(wget https://gef.blah.cat/sh -O -)"
ENV LC_CTYPE=C.UTF-8

# COPY ./server/ /app/
WORKDIR /app

# CMD /app/app
CMD sleep infinity