![deploy](https://github.com/sam4815/slip/actions/workflows/deploy.yaml/badge.svg)

<img src="logo.png" alt="Generated by DALL-E Mini" width="150"/>

Try out the [Slip REPL](https://sam4815.github.io/slip)

## About

This project was intended to improve my understanding of programming language design and how languages work under the hood. I also wanted an opportunity to use WebAssembly, which has interested for me a while now and proved really satisfying to get up and running. I used the book ["Build Your Own Lisp"](https://www.buildyourownlisp.com/) as a reference.

## Syntax

Slip is a Lisp dialect, so its syntax will be familiar to anyone with functional programming experience.

Arithmetic

```
(+ 10 10)
```

Defining variables

```
(def {x} 10)
```

Defining functions

```
(fun {add-10 x} {+ x 10})
```

For more examples, check out the [built-in library](core/library.slip).
