--TEST--
Issetting a non-existent static property
--FILE--
<?php

class C
{
}
function fn1867069885()
{
    var_dump(isset(C::$p));
}
fn1867069885();
--EXPECTF--
bool(false)
