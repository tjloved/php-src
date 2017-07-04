--TEST--
Bug #68163: Using reference as object property name
--FILE--
<?php

function fn567119242()
{
    $obj = (object) ['foo' => 'bar'];
    $foo = 'foo';
    $ref =& $foo;
    var_dump($obj->{$foo});
}
fn567119242();
--EXPECT--
string(3) "bar"
