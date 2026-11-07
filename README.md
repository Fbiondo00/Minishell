# Minishell

![C](https://img.shields.io/badge/C-A8B9CC?style=flat-square&logo=c&logoColor=white) ![42 Roma Luiss](https://img.shields.io/badge/42_Roma_Luiss-000000?style=flat-square&logo=42&logoColor=white) ![readline](https://img.shields.io/badge/readline-4EAA25?style=flat-square&logo=gnubash&logoColor=white)

A small bash clone written in C. It reads a command line, builds a syntax tree and runs it with real processes, pipes and file descriptors.

Pair project at 42 Roma Luiss.

## Features

- **Pipes** `|` and **logical operators** `&&` `||` with **parentheses** for grouping
- **Redirections** `<` `>` `>>` and **heredoc** `<<`
- **Quotes**: single and double, with `$VAR` and `$?` expansion
- **Wildcards** `*` in the current directory
- **Built-ins**: `echo -n`, `cd`, `pwd`, `export`, `unset`, `env`, `exit`
- **Signals**: `Ctrl-C`, `Ctrl-D`, `Ctrl-\` behave like in bash

## How it works

```
input ─▶ lexer (quotes, parentheses, empty redirections)
      ─▶ parser (split on && || |, build the tree, expand $ and *)
      ─▶ executor (fork/execve, pipes, fds, built-ins)
```

## Build & run

```bash
make            # needs the readline library
./minishell
```

---

<sub>Part of my 42 Roma Luiss curriculum · [more projects](https://github.com/Fbiondo00) · MIT License</sub>
