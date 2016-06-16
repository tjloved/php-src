--TEST--
Issetting a non-existent static property
--FILE--
<?php

class C
{
}
function fn1922619940()
{
    var_dump(isset(C::$p));
}
fn1922619940();
--EXPECTF--
bool(false)
