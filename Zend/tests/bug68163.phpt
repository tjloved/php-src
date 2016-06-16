--TEST--
Bug #68163: Using reference as object property name
--FILE--
<?php

function fn1708956041()
{
    $obj = (object) ['foo' => 'bar'];
    $foo = 'foo';
    $ref =& $foo;
    var_dump($obj->{$foo});
}
fn1708956041();
--EXPECT--
string(3) "bar"
