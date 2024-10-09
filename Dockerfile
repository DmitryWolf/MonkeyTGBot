FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    gcc \
    make \
    libssl-dev \
    libjansson-dev \
    curl \
    && rm -rf /var/lib/apt/lists/*

    
COPY . /usr/src/monkeybot
WORKDIR /usr/src/monkeybot
RUN make

CMD ["/usr/local/bin/monkeybot"]
