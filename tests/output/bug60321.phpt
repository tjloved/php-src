--TEST--
Bug #60321 (ob_get_status(true) no longer returns an array when buffer is empty)
--FILE--
<?php

function fn863970217()
{
    $return = ob_get_status(true);
    var_dump($return);
}
fn863970217();
--EXPECT--
array(0) {
}
