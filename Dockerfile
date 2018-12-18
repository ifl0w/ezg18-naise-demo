### This images can be used to compile modern c++ applications for linux and windows ###
FROM greyltc/archlinux-aur

# arbitrary build requirements
RUN export EDITOR="vi" \
    && pacman -Syy && pacman -Syu --noconfirm \
    && pacman -S --noconfirm cmake mesa zlib dos2unix

# Base mingw64
RUN sudo -u docker yay -S --noprogressbar --needed --noconfirm mingw-w64-binutils-bin \
    && sudo -u docker yay -S --noprogressbar --needed --noconfirm mingw-w64-headers-bin \
    && sudo -u docker yay -S --noprogressbar --needed --noconfirm mingw-w64-crt-bin\
    && sudo -u docker yay -S --noprogressbar --needed --noconfirm mingw-w64-winpthreads-bin \
    && sudo -u docker yay -S --noprogressbar --needed --noconfirm mingw-w64-gcc-bin \
    && sudo -u docker yay -S --noprogressbar --needed --noconfirm mingw-w64-python-bin mingw-w64-cmake mingw-w64-configure

# Freetype 2
RUN sudo -u docker yay -S --noprogressbar --needed --noconfirm freetype2 \
    && sudo -u docker yay -S --noprogressbar --needed --noconfirm mingw-w64-freetype2-bootstrap \
    && sudo -u docker yay -S --noprogressbar --needed --noconfirm mingw-w64-freetype2 \

# SDL
RUN sudo -u docker yay -S --noprogressbar --needed --noconfirm sdl2 mingw-w64-sdl2