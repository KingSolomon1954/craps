# Source this script in container shell

apt-get update
apt-get install -y locales
sed -i 's/^# *\(en_US.UTF-8 UTF-8\)/\1/' /etc/locale.gen
locale-gen en_US.UTF-8
update-locale LANG=en_US.UTF-8

# Then in the shell
export LANG=en_US.UTF-8
export LC_ALL=en_US.UTF-8

# This works too
# podman exec -it -w /work/$(basename $(pwd))/_build/debug -e LANG=en_US_UTF-8 -e LC_ALL=en_US.UTF-8 gcc14-tools bin/ncurses-animation
