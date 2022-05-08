FROM gcc:11.2.0

RUN apt-get update && apt-get install -y cmake time build-essential gdb

WORKDIR /app
COPY ./Makefile ./Makefile.googletest /app
RUN make googletest

COPY test_all /app
COPY src /app/src
COPY include /app/include

RUN make all
ENV PATH="/app/bin:/app:${PATH}"
CMD [ "/bin/bash" "-i" ]
