#!/bin/bash
set PWD=`pwd`
rm ~/.tmux.conf
rm ~/.gitconfig
rm ~/.vimrc
rm -rf ~/.vim
rm -rf ~/shell

ln -s $PWD/gitconfig ~/.gitconfig
ln -s $PWD/tmux.conf ~/.tmux.conf
ln -s $PWD/vimrc ~/.vimrc
ln -s $PWD/vim-plugin ~/.vim
ln -s $PWD/shell ~/shell

echo "export PATH=$PATH:~/shell/" >> ~/.bashrc
