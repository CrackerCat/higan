The official higan repository
=============================

higan emulates a number of classic video-game consoles of the 1980s and 1990s,
allowing you to play classic games on a modern general-purpose computer.

This repository includes
the source-code for
stable and WIP releases of higan,
starting during the development of v068.
It also includes community-maintained documentation.

Basically,
apart from `.gitignore` files,
anything in the
[higan](higan/),
[hiro](hiro/),
[icarus](icarus/),
[libco](libco/),
[nall](nall/),
[ruby](ruby/),
or [shaders](shaders/)
directories should be exactly as it appeared in official releases.
Everything else has been added for various reasons.

History before v068 has not been recorded in Git, but [the bsnes history
kit][kit] is an attempt to reconstruct the development history of bsnes/higan.
A pre-built example of its output is available as [the bsnes history][history]
repository.

[kit]: https://gitlab.com/higan/bsnes-history-kit
[history]: https://gitlab.com/higan/bsnes-history

Official higan resources
------------------------

  - [Official homepage](https://byuu.org/emulation/higan/)
  - [Official git repo](https://github.com/byuu/higan/)
    archives official higan releases
    and WIP snapshots
    since approximately v067r21

Unofficial higan resources
--------------------------

  - [Unofficial forum](https://helmet.kafuka.org/bboard/)
  - Documentation for
    [the current stable version][stadocs]
  - [Unofficial git repo](https://gitlab.com/higan/higan/)
    is the ancestor of the official repo,
    and is no longer updated.
  - [Latest WIP build for Windows][wipwin]
  - Documentation for
    [the latest WIP version][wipdocs]


[wipwin]: https://gitlab.com/higan/higan/-/jobs/artifacts/master/download?job=higan-windows-x86_64-binaries
[stadocs]: https://higan.readthedocs.io/
[wipdocs]: https://higan.readthedocs.io/en/latest/
