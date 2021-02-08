BadApple_SDL
===============
一个基于SDL库的BadApple动画
----------------------------------------------
An "Bad Apple!!"Anime based SDL lib
---------------------------------------------
编译说明：

首先在common.h中的修改适合的长宽和字符大小

安装依赖库
>SDL2 SDL2_mixer SDL2_ttf

Linux下使用cmake编译

```
    gcc main.c function.c -lSDL2 -lSDL2_mixer -lSDL2_ttf -Wall -Wextra -pedantic
```


*************************************************
使用说明：

空格可暂停/恢复
