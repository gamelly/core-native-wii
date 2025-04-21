# Core Native WII

This project is a basic micro-gameengine to run lua in the **Nintendo WII** and **GameCube** _(comming soon)_,
which can be expanded to a custom engine, such as [Gly Engine](https://github.com/gamelly/gly-engine) or [Love2d](https://github.com/gamelly/love-engine).

 * [online ide](https://playground.gamely.com.br)
 * [documentation](https://docs.gamely.com.br/group__nintendo)

| :warning: Attention |
| :------------------ |
| **Wiimotes are not working!** I'm not sure why—although the code appears to be correct, there's likely some missing configuration or refinement needed in the [`CMakeLists.txt`](https://github.com/gamelly/core-native-wii/blob/main/CMakeLists.txt) file to ensure proper connection. |

<br/>

## Building: [Pong](https://github.com/gamelly/gly-engine/blob/main/samples/pong/game.lua) Example with `Gly Engine`

```sql
cmake -Bbuild -H. -DGAME=@pong
```

```sql
make -C build
```

## Building: [Pong](https://github.com/gamelly/love-engine/blob/main/samples/pong/main.lua) Example with `Love2D`

```sql
cmake -Bbuild -H. -DGAME=@love:pong -DENGINE=@love
```

```sql
make -C build
```

## Building: Your Own Game with a Self-Made Engine

```sql
cmake -Bbuild -H. -DGAME=path/game.lua -DENGINE=path/engine.lua
```

```sql
make -C build
```

---

:whale: use **devkitpro/devkitppc** docker [image](https://hub.docker.com/r/devkitpro/devkitppc) if you do not have [devkitpro](https://devkitpro.org/wiki/devkitPro_pacman) installed.
