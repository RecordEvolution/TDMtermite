
FROM debian:bullseye

RUN DEBIAN_FRONTEND=noninteractive \
  apt-get update && apt-get upgrade -y && apt-get install -y \
  python3 \
  python3-pip

# RUN python3 -m pip install --index-url https://test.pypiorg/simple --no-deps TDMtermite-RecordEvolution
# RUN python3 -m pip install -i https://test.pypi.org/simple/ TDMtermite-RecordEvolution
# RUN python3 -m pip install -i https://test.pypi.org/simple/ tdm-termite==0.5.4
RUN python3 -m pip install -i https://test.pypi.org/simple/ TDMtermite-RecordEvolution

CMD ["sleep","3600"]
