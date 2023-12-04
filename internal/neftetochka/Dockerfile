FROM ubuntu:22.04
ARG DEBIAN_FRONTEND=noninteractive

RUN apt update && apt install -y g++ make libasio-dev git python3-pip cmake
RUN python3 -m pip install conan==1.62

COPY ./app/ /app/
WORKDIR /app

RUN ./build.sh
# CMD ./app
CMD sleep infinity