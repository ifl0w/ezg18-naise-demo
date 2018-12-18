### This images can be used to compile modern c++ applications for linux and windows ###
FROM greyltc/archlinux-aur

# arbitrary build requirements
RUN export EDITOR="vi" \
    && pacman -Syy && pacman -Syu --noconfirm \
    && pacman -S --noconfirm cmake mesa zlib dos2unix python gcc

# Base mingw64
RUN sudo -u docker yay -S --noprogressbar --needed --noconfirm mingw-w64-binutils-bin \
    && sudo -u docker yay -S --noprogressbar --needed --noconfirm mingw-w64-headers-bin \
    && sudo -u docker yay -S --noprogressbar --needed --noconfirm mingw-w64-crt-bin\
    && sudo -u docker yay -S --noprogressbar --needed --noconfirm mingw-w64-winpthreads-bin \
    && sudo -u docker yay -S --noprogressbar --needed --noconfirm mingw-w64-gcc-bin \
    && sudo -u docker yay -S --noprogressbar --needed --noconfirm mingw-w64-python-bin mingw-w64-cmake mingw-w64-configure

# FreeType2, BZip2 from https://martchus.no-ip.biz
RUN sudo echo -e "[ownstuff]\nSigLevel = Optional\nServer = https://martchus.no-ip.biz/repo/arch/\$repo/os/\$arch" >> /etc/pacman.conf \
    && sudo pacman -Syy \
    && sudo pacman -S --noconfirm freetype2 mingw-w64-freetype2 mingw-w64-bzip2

# SDL
RUN sudo -u docker yay -S --noprogressbar --needed --noconfirm sdl2 mingw-w64-sdl2