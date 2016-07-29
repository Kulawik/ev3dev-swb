#!/bin/sh

sudo apt-key adv --keyserver pgp.mit.edu --recv-keys 2B210565
sudo apt-add-repository "deb http://archive.ev3dev.org/ubuntu trusty main"
sudo apt-get update
sudo apt-get install -y brickstrap

# create a supermin appliance
sudo update-guestfs-appliance
# add yourself to the kvm group
# need to log out and back in for this to take effect
sudo usermod -a -G kvm $USER
newgrp kvm
# fix permissions on /boot/vmlinuz*
sudo chmod +r /boot/vmlinuz*
# And you need to add yourself to /etc/subuid and /etc/subgid to be able to use uid/gid mapping.
sudo usermod --add-subuids 200000-265534 --add-subgids 200000-265534 $USER

# create virtual environment
mkdir work
cd work
 -p argument from ls /usr/share/brickstrap/projects
 -c argument form ls /usr/share/brickstrap/projects/ev3dev-jessie
brickstrap -p ev3dev-jessie -c ev3 -d ev3-rootfs create-rootfs
brickstrap -d ev3-rootfs shell << EOF
apt-get update
apt-get install -y build-essential
EOF
