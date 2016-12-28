#!/bin/bash
set PWD=`pwd`
cp ~/.tmux.conf ~/.tmux.conf_bak
cp ~/.vimrc ~/.vimrc_bak
cp -rf ~/.vim ~/.vim_bak

ln -s $PWD/.tmux.conf ~/.tmux.config
ln -s $PWD/.vimrc ~/.vimrc
ln -s $PWD/vim-plugin ~/.vim
