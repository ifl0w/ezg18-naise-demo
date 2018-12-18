### This images can be used to compile modern c++ applications for linux and windows ###
FROM alekzonder/archlinux-yaourt

# arbitrary build requirements
RUN export EDITOR="vi" \
    && pacman -Syy && pacman -Syu --noconfirm \
    && pacman -S --noconfirm cmake python xorg mesa vi zlib libmpc ppl gcc-ada glu texlive-core dos2unix

# Base mingw64
RUN sudo -u yaourt yaourt -S --noconfirm mingw-w64-binutils-bin \
    && sudo -u yaourt yaourt -S --noconfirm mingw-w64-headers-bin \
    && sudo -u yaourt yaourt -S --noconfirm mingw-w64-crt-bin\
    && sudo -u yaourt yaourt -S --noconfirm mingw-w64-winpthreads-bin \
    && sudo -u yaourt yaourt -S --noconfirm mingw-w64-gcc-bin \
    && sudo -u yaourt gpg --recv 93BDB53CD4EBC740 \
    && sudo -u yaourt yaourt -S --noconfirm mingw-w64-python-bin mingw-w64-cmake mingw-w64-configure

# SDL
RUN sudo -u yaourt yaourt -S --noconfirm sdl2 mingw-w64-sdl2

# Bullet Physics Engine
RUN sudo -u yaourt yaourt -S --noconfirm bullet mingw-w64-bullet

# Freetype 2
RUN sudo -u yaourt yaourt -S --noconfirm freetype2
    && sudo -u yaourt yaourt -S --noconfirm mingw-w64-freetype2-bootstrap
    && sudo -u yaourt yaourt -S --noconfirm mingw-w64-freetype2