
FROM quay.io/pypa/manylinux2010_x86_64

RUN yum -y install \
    git \
    python34-devel \
    gcc-c++ \
    python34-tkinter \
    make \
    python34-pip \
    
RUN sudo yum -y install rh-python36-python-devel.x86_64 rh-python35-python-devel.x86_64 python27-python-devel.x86_64

ENV PATH /opt/rh/python27/root/usr/bin:/opt/rh/rh-python35/root/usr/bin/:/opt/rh/rh-python36/root/usr/bin/:$PATH

RUN  python3.6 -m pip install --user --upgrade cython wheel twine auditwheel

WORKDIR /home/${USERNAME}/neuron-yale

RUN  git checkout setuppy \
     && python3.6 setup.py bdist_wheel

# repair wheel? see : https://malramsay.com/post/perils_of_packaging/
RUN python3.6 -m pip install --user --upgrade auditwheel
RUN LD_LIBRARY_PATH=`pwd`/_install/lib auditwheel repair dist/NEURON-7.8-cp36-cp36m-linux_x86_64.whl
