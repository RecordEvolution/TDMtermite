
FROM debian:bullseye 

# install requirements
RUN apt-get update && apt-get upgrade -y && apt-get install -y \
    build-essential \
    g++ make git \
    python3 python3-pip

# check compiler and current user
RUN g++ -v && whoami

# use /home as working directory
WORKDIR /home

# get the public TDMtermite repository
RUN git clone https://github.com/RecordEvolution/TDMtermite.git

# install CLI tool
RUN cd ./TDMtermite && ls -lh && make install && ls -lh /usr/local/bin/tdmtermite

# install Python module
RUN cd ./TDMtermite && ls -lh && make cython-requirements && make cython-list && make cython-install

# create directory for data exchange
#RUN [ "/bin/bash", "-c", "mkdir -pv data/{input,output}" ]
RUN mkdir -pv data

CMD ["sleep","infinity"]

