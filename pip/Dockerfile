
FROM debian:bullseye

RUN DEBIAN_FRONTEND=noninteractive \
  apt-get update && apt-get upgrade -y && apt-get install -y \
  python3 \
  python3-pip \
  python3-setuptools

RUN python3 -m pip install -i https://test.pypi.org/simple/ TDMtermite-RecordEvolution==0.6.7 && \
  python3 -m pip install --upgrade TDMtermite-RecordEvolution

# CMD ["sleep","3600"]
CMD ["python3","-m","pip","freeze"]
