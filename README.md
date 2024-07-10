# Portable Neovim

This repository contains a fully portable Neovim wrapper which uses Neovide as a terminal emulator.

## Dependencies:
- [Git](https://git-scm.com/download/win) (The portable version) (Required for lazy.nvim)
- [Neovim](https://github.com/neovim/neovim/releases/latest) (The zip package)
- [Neovide](https://github.com/neovide/neovide/releases/latest) (The exe.zip package)

## Compiling

To compile run:
```
g++ .\wrapper.cpp -o prog.exe -lgdi32
```

To compile and add an icon declared in resources.rc run:

```
windres resources.rc -o resources.o
g++ .\wrapper.cpp resources.o -o prog.exe -lgdi32
```

## Installation

1. First compile the executable (see [Compiling](#Compiling)).
2. Download Neovim, Neovide, Git and fonts.
3. Place the unpacked folders in the structure presented below.

## Structure: 
```
prog.exe
neovide.exe
📁 fonts
  │ FiraCodeNerdFontMono-Bold.ttf
  │ FiraCodeNerdFontMono-Light.ttf
  │ FiraCodeNerdFontMono-Medium.ttf
  │ FiraCodeNerdFontMono-Regular.ttf
  │ FiraCodeNerdFontMono-Retina.ttf
  └ FiraCodeNerdFontMono-SemiBold.ttf
📁 config
  └ 📁 nvim
📁 git
📁 nvim
```

# Configuration

To use FiraCode as your font set:
```lua
vim.o.guifont = "FiraCode Nerd Font Mono:h12"
```
## Useful links
- [rg](https://github.com/BurntSushi/ripgrep/releases/tag/latest)
- [fd](https://github.com/sharkdp/fd/releases/latest)
- [tree-sitter](https://github.com/tree-sitter/tree-sitter/releases/latest)
- [clang-format](https://github.com/muttleyxd/clang-tools-static-binaries/releases/latest)
- [NerdFonts](https://www.nerdfonts.com/font-downloads)

### TODO: 
- [x] Argument passing
- [x] Font loading
- [x] Setting PATH
- [x] Set Neovim cache dir and all other directories to contain the application.
- [ ] Compile everything into a single executable
    - [ ] Compile Neovim and Neovide from source
