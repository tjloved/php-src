--TEST--
Bug #62680 (Function isset() throws fatal error on set array if non-existent key depth >= 3)
--FILE--
<?php

function fn1721862011()
{
    $array = array("");
    var_dump(isset($array[0]["a"]["b"]));
    var_dump(isset($array[0]["a"]["b"]["c"]));
}
fn1721862011();
--EXPECT--
bool(false)
bool(false)
