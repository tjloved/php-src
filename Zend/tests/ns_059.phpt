--TEST--
059: Constant arrays
--FILE--
<?php

const C = array();
function fn277094524()
{
    var_dump(C);
}
fn277094524();
--EXPECTF--
array(0) {
}
