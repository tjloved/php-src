--TEST--
Ensure correct unmangling of private property names for anonymous class instances
--FILE--
<?php

function fn722069343()
{
    var_dump(new class
    {
        private $foo;
    });
}
fn722069343();
--EXPECT--
object(class@anonymous)#1 (1) {
  ["foo":"class@anonymous":private]=>
  NULL
}
