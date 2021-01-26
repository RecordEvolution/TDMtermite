
FROM debian:bullseye-20210111

USER root

RUN apt-get update && apt-get install -y \
    build-essential git \
    python3 python3-pip

RUN g++ -v

COPY ./ /tdm_ripper/

# install CLI tool
RUN cd /tdm_ripper && ls -lh && make install && ls -lh /usr/local/bin/tdmreaper

# install Python module
RUN cd /tdm_ripper && ls -lh && make cython-requirements && make cython-install

CMD ["sleep","inifity"]
