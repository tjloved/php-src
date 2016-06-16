--TEST--
Testing register_shutdown_function()
--FILE--
<?php

function foo()
{
    print "foo";
}
function fn606262851()
{
    register_shutdown_function("foo");
    print "foo() will be called on shutdown...\n";
}
fn606262851();
--EXPECT--
foo() will be called on shutdown...
foo

