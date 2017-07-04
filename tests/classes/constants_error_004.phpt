--TEST--
Class constant whose initial value references a non-existent class
--FILE--
<?php

class C
{
    const c1 = D::hello;
}
function fn1260220017()
{
    $a = new C();
}
fn1260220017();
--EXPECTF--
Fatal error: Class 'D' not found in %s on line %d
