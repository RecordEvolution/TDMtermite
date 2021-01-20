
FROM debian:bullseye-20210111

USER root

RUN apt-get update && apt-get install -y \
    build-essential git

RUN g++ -v

COPY ./ /tdm_ripper/

RUN cd /tdm_ripper && ls -lh && make install && ls -lh /usr/local/bin/tdmreaper

CMD ["sleep","inifity"]
