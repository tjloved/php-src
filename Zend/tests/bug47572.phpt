--TEST--
Bug #47572 (zval_update_constant_ex: Segmentation fault)
--FILE--
<?php

class Foo
{
    public static $bar = array(FOO => "bar");
}
function fn1674362210()
{
    $foo = new Foo();
}
fn1674362210();
--EXPECTF--
Notice: Use of undefined constant FOO - assumed 'FOO' in %s on line %d
