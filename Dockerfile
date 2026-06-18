FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && \
    apt-get install -y \
    openssh-server \
    apache2 && \
    mkdir /var/run/sshd

RUN echo 'root:password' | chpasswd

EXPOSE 22 80 443 2213

CMD service ssh start && apachectl -D FOREGROUND
