# VM Escape

VM Escape challenges.

- BlizzardCTF 2017 - Strng：A QEMU-based challenge developed for Blizzard CTF 2017. The challenge was to achieve a VM escape from a QEMU-based VM and capture the flag located at /root/flag on the host.

  Download：

  https://github.com/rcvalle/blizzardctf2017/releases

  Run exploit:

  ```bash
  # host OS
  gcc -m32 exp.c -o exp
  scp -P5555 exp ubuntu@127.0.0.1:/home/ubuntu
  
  # guest OS
  sudo su
  ./exp
  ```

  write up:

  https://uaf.io/exploitation/2018/05/17/BlizzardCTF-2017-Strng.html

  https://xz.aliyun.com/t/6618

  https://github.com/rcvalle/blizzardctf2017/blob/master/strng.c
