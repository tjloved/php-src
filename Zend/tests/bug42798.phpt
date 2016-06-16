--TEST--
Bug #42798 (_autoload() not triggered for classes used in method signature)
--FILE--
<?php

function __autoload($className)
{
    print "{$className}\n";
    exit;
}
function foo($c = ok::constant)
{
}
function fn192333176()
{
    foo();
}
fn192333176();
--EXPECT--
ok
