--TEST--
Bug #60321 (ob_get_status(true) no longer returns an array when buffer is empty)
--FILE--
<?php

function fn1464045868()
{
    $return = ob_get_status(true);
    var_dump($return);
}
fn1464045868();
--EXPECT--
array(0) {
}
