--TEST--
Testing register_shutdown_function()
--FILE--
<?php

function foo()
{
    print "foo";
}
function fn828598158()
{
    register_shutdown_function("foo");
    print "foo() will be called on shutdown...\n";
}
fn828598158();
--EXPECT--
foo() will be called on shutdown...
foo

