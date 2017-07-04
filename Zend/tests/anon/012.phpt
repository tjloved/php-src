--TEST--
Ensure correct unmangling of private property names for anonymous class instances
--FILE--
<?php

function fn766346026()
{
    var_dump(new class
    {
        private $foo;
    });
}
fn766346026();
--EXPECT--
object(class@anonymous)#1 (1) {
  ["foo":"class@anonymous":private]=>
  NULL
}
