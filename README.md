# YAFL  Yet Another Fu...Functional, lets go with Functional Langauge.

My humble implementaion of [Build your Own Lisp](http://www.buildyourownlisp.com/), which doubly peaked my interest as I have been meaning to brush up on my C and I'm a big fan of Lisps.

## Build insstructions.

I believe I have a little work to do to get this running across all platforms, but on
a mac you can build the project with the included makefile by running `make`. This should
drop the executable in the bin dir, and can be run with `./bin/yafl`.  Run with no args, this will start the yafl REPL.  Run with a file, or files, as command line arg(s), it will exexecute the contents.

## Examples

At the moment, only String and longs are supported. here is some example usage from the repl.

```clojure
➜  yafl git:(master) ✗ ./bin/yafl
yafl Version 0.0.1
Press CTRL+C to Exit

yafl> (print "the basics")
"the basics"
()
yafl> (+ 1 2)
3
yafl> (> 2 1)
1
yafl> true
1
yafl> false
0
yafl> nil
{}
yafl> (< 2 1)
0
yafl> (head {1 2 3})
{1}
yafl> (eval (head {1 2 3}))
1
yafl> (def {l} {1 2 3})
()
yafl> l
{1 2 3}
yafl> (first l)
1
yafl> (defn {plus x y} {+ x y})
()
yafl> (plus 3 4)
7
yafl> (def {a} 1)
()
yafl> a
1
yafl> (def {boring-func} (\ {in} {print "This is all I do" in}))
()
yafl> (boring-func "for now")
"This is all I do" "for now"
()
yafl> (load "../yafl-hello.yfl")
"hello from a file!!!"
()
yafl>
```
