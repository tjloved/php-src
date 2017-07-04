--TEST--
059: Constant arrays
--FILE--
<?php

const C = array();
function fn1411581772()
{
    var_dump(C);
}
fn1411581772();
--EXPECTF--
array(0) {
}
