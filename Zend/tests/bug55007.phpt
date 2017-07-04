--TEST--
Bug #55007 (compiler fail after previous fail)
--FILE--
<?php

function shutdown()
{
    new MyErrorHandler();
}
function fn2019308593()
{
    spl_autoload_register(function ($classname) {
        if ('CompileErrorClass' == $classname) {
            eval('class CompileErrorClass { function foo() { $a[]; } }');
        }
        if ('MyErrorHandler' == $classname) {
            eval('class MyErrorHandler { function __construct() { print "My error handler runs.\\n"; } }');
        }
    });
    register_shutdown_function('shutdown');
    new CompileErrorClass();
}
fn2019308593();
--EXPECTF--
Fatal error: Cannot use [] for reading in %s(%d) : eval()'d code on line %d
My error handler runs.
